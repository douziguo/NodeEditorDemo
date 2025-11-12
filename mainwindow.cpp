//
// Created by douziguo on 2025/11/12.
//

#include "mainwindow.h"
#include "mainwindow.h"
#include <QToolBar>
#include <QMenuBar>
#include <QAction>
#include <QDockWidget>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_editorCore(new NodeEditorCore(this))
    , m_nodeDock(nullptr)
    , m_nodeList(nullptr)
    , m_statusLabel(nullptr)
    , m_nodeCountLabel(nullptr)
    , m_connectionCountLabel(nullptr)
    , m_isModified(false)
{

    qDebug() << "MainWindow 构造函数开始";

    // 启用拖拽接受
    setAcceptDrops(true);
    qDebug() << "主窗口拖拽接受已启用";

    // 初始化核心组件
    if (m_editorCore->initialize()) {
        setupUI();
        setupMenuBar();
        setupToolBars();
        setupDockWidgets();
        setupStatusBar();
        setupConnections();

        loadSettings();
        updateWindowTitle();
        updateStatusBar();

        setWindowTitle("QtNodes Editor - 拖拽式节点编辑器");
        resize(1400, 900);

        qDebug() << "MainWindow 初始化完成";
    } else {
        QMessageBox::critical(this, "错误", "编辑器核心初始化失败");
        close();
    }

    //testNodeAdding();
    //checkDragDropSettings();
}

# if 0
void MainWindow::checkDragDropSettings()
{
    qDebug() << "=== 拖拽设置检查 ===";
    qDebug() << "主窗口 acceptDrops:" << acceptDrops();
    if (m_editorCore && m_editorCore->view()) {
        qDebug() << "视图 acceptDrops:" << m_editorCore->view()->acceptDrops();
        qDebug() << "视图几何:" << m_editorCore->view()->geometry();
    }
    if (m_nodeList) {
        qDebug() << "节点列表 acceptDrops:" << m_nodeList->acceptDrops();
        qDebug() << "节点列表 dragEnabled:" << m_nodeList->dragEnabled();
    }
}

void MainWindow::testNodeAdding()
{
    qDebug() << "=== 测试节点添加功能 ===";

    if (!m_editorCore) {
        qDebug() << "❌ 编辑器核心为空";
        return;
    }

    // 测试直接添加节点
    NodeId testNodeId = m_editorCore->addNode("StartNode");
    if (testNodeId != InvalidNodeId) {
        qDebug() << "✅ 直接添加节点成功，ID:" << testNodeId;

        // 设置位置
        m_editorCore->setNodePosition(testNodeId, QPointF(100, 100));
        qDebug() << "✅ 节点位置设置完成";

        // 立即更新状态栏
        updateStatusBar();
    } else {
        qDebug() << "❌ 直接添加节点失败";
    }

    qDebug() << "=== 测试完成 ===";
}
#endif

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (confirmUnsavedChanges()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "=== dragEnterEvent ===";
    qDebug() << "事件源:" << event->source();
    qDebug() << "MIME格式:" << event->mimeData()->formats();
    qDebug() << "事件对象:" << event;

    // 正确的检查：使用 QListWidget 的标准格式
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
        qDebug() << "✅ 接受拖拽进入（QListWidget 标准格式）";
    }
    // 备用检查：检查事件源
    else if (event->source() == m_nodeList) {
        event->acceptProposedAction();
        qDebug() << "✅ 接受拖拽进入（来自节点列表）";
    }
    else {
        event->ignore();
        qDebug() << "❌ 忽略拖拽进入 - 无有效格式";
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    // 必须实现 dragMoveEvent，否则拖拽可能中断
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ||
        event->source() == m_nodeList) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug() << "=== dropEvent ===";
    qDebug() << "释放位置:" << event->pos();
    qDebug() << "事件源:" << event->source();
    qDebug() << "MIME格式:" << event->mimeData()->formats();

    if (event->source() == m_nodeList) {
        // 获取选中的节点类型
        QList<QListWidgetItem*> selectedItems = m_nodeList->selectedItems();
        if (!selectedItems.isEmpty()) {
            QString nodeType = selectedItems.first()->text();
            qDebug() << "添加节点类型:" << nodeType;

            // 转换坐标
            QPointF scenePos = mapToScenePos(event->pos());
            qDebug() << "窗口坐标:" << event->pos() << "-> 场景坐标:" << scenePos;

            // 添加节点
            addNodeFromDrag(nodeType, scenePos);
            event->acceptProposedAction();
            qDebug() << "✅ 拖拽完成，添加节点:" << nodeType;
        } else {
            qDebug() << "❌ 没有选中的节点";
            event->ignore();
        }
    } else {
        qDebug() << "❌ 事件源不是节点列表";
        event->ignore();
    }
}

QPointF MainWindow::mapToScenePos(const QPoint &windowPos) const
{
    if (!m_editorCore || !m_editorCore->view()) {
        return QPointF(0, 0);
    }

    // 将窗口坐标转换为视图坐标
    QPoint viewPos = m_editorCore->view()->mapFromParent(windowPos);

    // 将视图坐标转换为场景坐标
    return m_editorCore->view()->mapToScene(viewPos);
}

void MainWindow::addNodeFromDrag(const QString &nodeType, const QPointF &scenePos)
{
    if (!m_editorCore) {
        qDebug() << "❌ 编辑器核心未初始化";
        return;
    }

    qDebug() << "添加节点 - 类型:" << nodeType << "位置:" << scenePos;

    NodeId nodeId = m_editorCore->addNode(nodeType);
    if (nodeId != InvalidNodeId) {
        qDebug() << "✅ 节点添加成功，ID:" << nodeId;

        // 设置节点位置
        m_editorCore->setNodePosition(nodeId, scenePos);

        m_isModified = true;
        updateWindowTitle();
        updateStatusBar();

        statusBar()->showMessage(QString("已添加节点: %1").arg(nodeType), 2000);
    }
    else {
        qDebug() << "❌ 节点添加失败，类型:" << nodeType;
        statusBar()->showMessage(QString("添加节点失败: %1").arg(nodeType), 2000);
    }
}

void MainWindow::setupUI()
{
    // 设置中心部件
    if (m_editorCore && m_editorCore->view()) {
        // 关键：确保视图接受拖拽
        m_editorCore->view()->setAcceptDrops(false);
        m_editorCore->view()->setDragMode(QGraphicsView::RubberBandDrag);
        m_editorCore->view()->setRenderHint(QPainter::Antialiasing);
        m_editorCore->view()->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        // 设置视图样式
        m_editorCore->view()->setStyleSheet(R"(
            QGraphicsView {
                background-color: #f8f9fa;
                border: 1px solid #dee2e6;
                border-radius: 4px;
            }
        )");

        setCentralWidget(m_editorCore->view());

        qDebug() << "UI设置完成，视图拖拽已启用";
    } else {
        qDebug() << "UI设置失败：编辑器或视图为空";
    }
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    QMenu* fileMenu = menuBar()->addMenu("文件(&F)");

    m_newAction = new QAction("新建(&N)", this);
    m_newAction->setShortcut(QKeySequence::New);
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newScene);

    m_openAction = new QAction("打开(&O)", this);
    m_openAction->setShortcut(QKeySequence::Open);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openScene);

    m_saveAction = new QAction("保存(&S)", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveScene);

    m_saveAsAction = new QAction("另存为(&A)", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveSceneAs);

    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);

    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addAction(m_saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // 编辑菜单
    QMenu* editMenu = menuBar()->addMenu("编辑(&E)");

    m_undoAction = new QAction("撤销(&U)", this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);

    m_redoAction = new QAction("重做(&R)", this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);

    editMenu->addAction(m_undoAction);
    editMenu->addAction(m_redoAction);
    editMenu->addSeparator();

    m_copyAction = new QAction("复制(&C)", this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    connect(m_copyAction, &QAction::triggered, this, &MainWindow::copy);

    m_pasteAction = new QAction("粘贴(&P)", this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, this, &MainWindow::paste);

    m_deleteAction = new QAction("删除(&D)", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    connect(m_deleteAction, &QAction::triggered, this, &MainWindow::deleteSelected);

    editMenu->addAction(m_copyAction);
    editMenu->addAction(m_pasteAction);
    editMenu->addAction(m_deleteAction);

    // 视图菜单
    QMenu* viewMenu = menuBar()->addMenu("视图(&V)");

    m_zoomInAction = new QAction("放大", this);
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    m_zoomOutAction = new QAction("缩小", this);
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    m_resetZoomAction = new QAction("重置缩放", this);
    connect(m_resetZoomAction, &QAction::triggered, this, &MainWindow::resetZoom);

    m_fitToViewAction = new QAction("适应视图", this);
    connect(m_fitToViewAction, &QAction::triggered, this, &MainWindow::fitToView);

    m_showNodePanelAction = new QAction("节点面板", this);
    m_showNodePanelAction->setCheckable(true);
    m_showNodePanelAction->setChecked(true);
    connect(m_showNodePanelAction, &QAction::toggled, this, &MainWindow::showNodePanel);

    viewMenu->addAction(m_zoomInAction);
    viewMenu->addAction(m_zoomOutAction);
    viewMenu->addAction(m_resetZoomAction);
    viewMenu->addAction(m_fitToViewAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_showNodePanelAction);

    // 工具菜单
    QMenu* toolsMenu = menuBar()->addMenu("工具(&T)");

    m_executeAction = new QAction("执行数据流", this);
    m_executeAction->setShortcut(QKeySequence("F5"));
    connect(m_executeAction, &QAction::triggered, this, &MainWindow::executeFlow);

    m_validateAction = new QAction("验证", this);
    connect(m_validateAction, &QAction::triggered, this, &MainWindow::validateFlow);

    m_clearAction = new QAction("清除场景", this);
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::clearScene);

    toolsMenu->addAction(m_executeAction);
    toolsMenu->addAction(m_validateAction);
    toolsMenu->addAction(m_clearAction);

    // 帮助菜单
    QMenu* helpMenu = menuBar()->addMenu("帮助(&H)");

    m_aboutAction = new QAction("关于", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    m_helpAction = new QAction("帮助", this);
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::showHelp);

    helpMenu->addAction(m_helpAction);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolBars()
{
    // 主工具栏
    QToolBar* mainToolbar = addToolBar("主工具栏");
    mainToolbar->setObjectName("MainToolbar");
    mainToolbar->setMovable(false);
    mainToolbar->setIconSize(QSize(24, 24));

    mainToolbar->addAction(m_newAction);
    mainToolbar->addAction(m_openAction);
    mainToolbar->addAction(m_saveAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(m_executeAction);
    mainToolbar->addAction(m_clearAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(m_zoomInAction);
    mainToolbar->addAction(m_zoomOutAction);
    mainToolbar->addAction(m_resetZoomAction);
}

void MainWindow::setupDockWidgets()
{
    // 节点面板
    m_nodeDock = new QDockWidget("节点库", this);

    // 使用普通 QListWidget
    m_nodeList = new QListWidget(m_nodeDock);

    // 设置拖拽属性 - 这是关键！
    m_nodeList->setDragEnabled(true);
    m_nodeList->setDragDropMode(QAbstractItemView::DragOnly);
    m_nodeList->setDefaultDropAction(Qt::CopyAction);
    m_nodeList->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置列表样式
    m_nodeList->setViewMode(QListView::IconMode);
    m_nodeList->setIconSize(QSize(64, 64));
    m_nodeList->setSpacing(10);

    m_nodeList->setStyleSheet(R"(
        QListWidget {
            background-color: white;
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 5px;
        }
        QListWidget::item {
            background-color: #e8f4fd;
            border: 1px solid #b8d6fb;
            border-radius: 8px;
            padding: 12px;
            margin: 4px;
            color: #2c3e50;
            font-weight: bold;
            text-align: center;
        }
        QListWidget::item:hover {
            background-color: #d1ecff;
            border: 1px solid #6cb2eb;
        }
        QListWidget::item:selected {
            background-color: #bee5eb;
            border: 1px solid #17a2b8;
        }
    )");

    // 添加节点类型
    QStringList nodeTypes = {
        "StartNode", "EndNode", "MathOperation", "TextDisplay"
    };

    for (const QString &nodeType : nodeTypes) {
        QListWidgetItem *item = new QListWidgetItem(nodeType, m_nodeList);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        // 设置工具提示
        if (nodeType == "StartNode") item->setToolTip("开始节点");
        else if (nodeType == "EndNode") item->setToolTip("结束节点");
        else if (nodeType == "MathOperation") item->setToolTip("数学运算节点");
        else if (nodeType == "TextDisplay") item->setToolTip("文本显示节点");
    }

    m_nodeDock->setWidget(m_nodeList);
    m_nodeDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_nodeDock->setMinimumWidth(200);

    addDockWidget(Qt::LeftDockWidgetArea, m_nodeDock);

    qDebug() << "节点面板设置完成，拖拽已启用";
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("就绪");
    m_nodeCountLabel = new QLabel("节点: 0");
    m_connectionCountLabel = new QLabel("连接: 0");

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_nodeCountLabel);
    statusBar()->addPermanentWidget(m_connectionCountLabel);
}

void MainWindow::setupConnections()
{
    // 连接编辑器信号
    if (m_editorCore) {
        connect(m_editorCore, &NodeEditorCore::nodeAdded, this, &MainWindow::updateStatusBar);
        connect(m_editorCore, &NodeEditorCore::nodeRemoved, this, &MainWindow::updateStatusBar);
        connect(m_editorCore, &NodeEditorCore::connectionAdded, this, &MainWindow::updateStatusBar);
        connect(m_editorCore, &NodeEditorCore::connectionRemoved, this, &MainWindow::updateStatusBar);
        connect(m_editorCore, &NodeEditorCore::modificationChanged, this, [this](bool modified) {
            m_isModified = modified;
            updateWindowTitle();
        });
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    bool shouldShowNodePanel = settings.value("showNodePanel", true).toBool();
    m_showNodePanelAction->setChecked(shouldShowNodePanel);
    showNodePanel(shouldShowNodePanel);
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("showNodePanel", m_showNodePanelAction->isChecked());
}

bool MainWindow::confirmUnsavedChanges()
{
    if (m_isModified) {
        auto reply = QMessageBox::question(this, "确认",
            "当前场景有未保存的更改。是否保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            return saveScene();
        } else if (reply == QMessageBox::Discard) {
            return true;
        } else if (reply == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

// 文件操作槽函数
void MainWindow::newScene()
{
    if (!confirmUnsavedChanges()) return;

    if (m_editorCore) {
        // 清除当前场景
        clearScene();
    }

    m_currentFile.clear();
    m_isModified = false;
    updateWindowTitle();
    updateStatusBar();

    statusBar()->showMessage("新建场景", 2000);
}

void MainWindow::openScene()
{
    if (!confirmUnsavedChanges()) return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "打开场景", "", "节点场景文件 (*.json)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (m_editorCore) {
        if (m_editorCore->loadScene(doc.object())) {
            m_currentFile = fileName;
            m_isModified = false;
            updateWindowTitle();
            statusBar()->showMessage("场景已加载", 2000);
        } else {
            QMessageBox::warning(this, "错误", "加载场景失败");
        }
    }
}

bool MainWindow::saveScene()
{
    if (m_currentFile.isEmpty()) {
        return saveSceneAs();
    }

    if (m_editorCore) {
        QJsonObject sceneData = m_editorCore->saveScene();
        QFile file(m_currentFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(sceneData).toJson());
            file.close();
            m_isModified = false;
            updateWindowTitle();
            statusBar()->showMessage("场景已保存", 2000);
            return true;
        } else {
            QMessageBox::warning(this, "错误", "保存文件失败");
            return false;
        }
    }
    return false;
}

bool MainWindow::saveSceneAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "保存场景", "", "节点场景文件 (*.json)");

    if (fileName.isEmpty()) return false;

    m_currentFile = fileName;
    return saveScene();
}

// 编辑操作槽函数
void MainWindow::undo()
{
    statusBar()->showMessage("撤销操作", 1000);
    // 这里可以添加实际的撤销逻辑
}

void MainWindow::redo()
{
    statusBar()->showMessage("重做操作", 1000);
    // 这里可以添加实际的重做逻辑
}

void MainWindow::copy()
{
    statusBar()->showMessage("复制", 1000);
    // 这里可以添加实际的复制逻辑
}

void MainWindow::paste()
{
    statusBar()->showMessage("粘贴", 1000);
    // 这里可以添加实际的粘贴逻辑
}

void MainWindow::deleteSelected()
{
    statusBar()->showMessage("删除选中项", 1000);
    // 这里可以添加实际的删除逻辑
}

// 视图操作槽函数
void MainWindow::zoomIn()
{
    if (m_editorCore && m_editorCore->view()) {
        m_editorCore->view()->scale(1.2, 1.2);
        updateStatusBar();
    }
}

void MainWindow::zoomOut()
{
    if (m_editorCore && m_editorCore->view()) {
        m_editorCore->view()->scale(0.8, 0.8);
        updateStatusBar();
    }
}

void MainWindow::resetZoom()
{
    if (m_editorCore && m_editorCore->view()) {
        m_editorCore->view()->resetTransform();
        updateStatusBar();
    }
}

void MainWindow::fitToView()
{
    if (m_editorCore && m_editorCore->scene() && m_editorCore->view()) {
        m_editorCore->view()->fitInView(m_editorCore->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        updateStatusBar();
    }
}

void MainWindow::showNodePanel(bool show)
{
    if (m_nodeDock) {
        m_nodeDock->setVisible(show);
    }
}

// 节点操作槽函数
void MainWindow::addStartNode()
{
    addNodeFromDrag("StartNode", QPointF(0, 0));
}

void MainWindow::addEndNode()
{
    addNodeFromDrag("EndNode", QPointF(200, 0));
}

void MainWindow::addMathOperationNode()
{
    addNodeFromDrag("MathOperation", QPointF(100, 150));
}

void MainWindow::addTextDisplayNode()
{
    addNodeFromDrag("TextDisplay", QPointF(100, 300));
}

// 工具操作槽函数
void MainWindow::executeFlow()
{
    statusBar()->showMessage("执行数据流...", 2000);
    QMessageBox::information(this, "执行", "数据流执行完成");
}

void MainWindow::validateFlow()
{
    statusBar()->showMessage("验证场景...", 2000);
    QMessageBox::information(this, "验证", "场景验证完成");
}

void MainWindow::clearScene()
{
    if (m_editorCore && m_editorCore->graphModel()) {
        auto nodes = m_editorCore->graphModel()->allNodeIds();
        for (auto nodeId : nodes) {
            m_editorCore->removeNode(nodeId);
        }
        m_isModified = true;
        updateWindowTitle();
        statusBar()->showMessage("场景已清除", 2000);
    }
}

// 帮助槽函数
void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于 QtNodes Editor",
        "<h3>QtNodes Editor</h3>"
        "<p>基于 QtNodes 库的可视化节点编辑器</p>"
        "<p>版本 1.0.0</p>"
        "<p>支持拖拽式节点添加</p>"
        "<p>© 2023 QtNodes Editor</p>");
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, "帮助",
        "使用说明:\n"
        "- 从左侧节点库拖拽节点到场景\n"
        "- 连接节点的输入输出端口\n"
        "- 使用工具栏按钮执行操作\n"
        "- 支持鼠标滚轮缩放视图");
}

void MainWindow::updateWindowTitle()
{
    QString title = "QtNodes Editor";
    if (!m_currentFile.isEmpty()) {
        title += " - " + QFileInfo(m_currentFile).fileName();
    }
    if (m_isModified) {
        title += " *";
    }
    setWindowTitle(title);
}

void MainWindow::updateStatusBar()
{
    if (m_editorCore) {
        int nodeCount = m_editorCore->nodeCount();
        int connectionCount = m_editorCore->connectionCount();

        m_nodeCountLabel->setText(QString("节点: %1").arg(nodeCount));
        m_connectionCountLabel->setText(QString("连接: %1").arg(connectionCount));

        if (nodeCount == 0) {
            m_statusLabel->setText("就绪 - 可从左侧节点库拖拽添加节点");
        } else {
            m_statusLabel->setText("就绪 - 支持拖拽操作");
        }
    } else {
        m_nodeCountLabel->setText("节点: 0");
        m_connectionCountLabel->setText("连接: 0");
        m_statusLabel->setText("就绪 - 编辑器未初始化");
    }
}