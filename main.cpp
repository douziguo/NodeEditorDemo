#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "NumberSourceDataModel.h"
#include "NumberDisplayDataModel.h"

using namespace QtNodes;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr)
    {
        setupUI();
        setupScene();
    }

private slots:
    void addNumberSource()
    {
        auto nodeId = m_graphModel->addNode("NumberSource");
        m_graphModel->setNodeData(nodeId, NodeRole::Position, getNextNodePosition());
    }

    void addNumberDisplay()
    {
        auto nodeId = m_graphModel->addNode("NumberDisplay");
        m_graphModel->setNodeData(nodeId, NodeRole::Position, getNextNodePosition());
    }

    void clearScene()
    {
        m_scene->clearScene();
        m_nextNodePos = QPointF(50, 50);
    }

private:
    void setupUI()
    {
        auto centralWidget = new QWidget();
        setCentralWidget(centralWidget);

        auto mainLayout = new QVBoxLayout(centralWidget);
        auto buttonLayout = new QHBoxLayout();

        auto addSourceBtn = new QPushButton("Add Number Source");
        auto addDisplayBtn = new QPushButton("Add Number Display");
        auto clearBtn = new QPushButton("Clear Scene");

        buttonLayout->addWidget(addSourceBtn);
        buttonLayout->addWidget(addDisplayBtn);
        buttonLayout->addWidget(clearBtn);
        buttonLayout->addStretch();

        m_graphicsView = new QGraphicsView();

        mainLayout->addLayout(buttonLayout);
        mainLayout->addWidget(m_graphicsView);

        connect(addSourceBtn, &QPushButton::clicked, this, &MainWindow::addNumberSource);
        connect(addDisplayBtn, &QPushButton::clicked, this, &MainWindow::addNumberDisplay);
        connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearScene);

        setWindowTitle("QtNodes Demo");
        resize(1000, 700);
    }

    void setupScene()
    {
        // 创建注册表
        auto registry = std::make_shared<NodeDelegateModelRegistry>();
        registry->registerModel<NumberSourceDataModel>("Sources");
        registry->registerModel<NumberDisplayDataModel>("Displays");

        // 创建数据模型
        m_graphModel = std::make_shared<DataFlowGraphModel>(registry);

        m_scene = new DataFlowGraphicsScene(*m_graphModel);
        m_graphicsView->setScene(m_scene);

        // 添加示例节点
        addNumberSource();
        addNumberDisplay();
    }

    QPointF getNextNodePosition()
    {
        QPointF pos(m_nextNodePos);
        m_nextNodePos += QPointF(200, 0);
        if (m_nextNodePos.x() > 800) {
            m_nextNodePos = QPointF(50, m_nextNodePos.y() + 150);
        }
        return pos;
    }

    QGraphicsView* m_graphicsView = nullptr;
    DataFlowGraphicsScene* m_scene = nullptr;
    std::shared_ptr<DataFlowGraphModel> m_graphModel;
    QPointF m_nextNodePos = QPointF(50, 50);
};

#include <QLibrary>

void checkQtNodesCompatibility()
{
    qDebug() << "=== 检查 QtNodes 兼容性 ===";

    // 检查是否缺少必需的符号
    QLibrary qtNodesLib("QtNodes");
    if (qtNodesLib.load()) {
        qDebug() << "✓ QtNodes 库加载成功";

        // 检查关键函数是否存在
        if (qtNodesLib.resolve("_ZN8QtNodes21NodeDelegateModelC2Ev")) {
            qDebug() << "✓ NodeDelegateModel 构造函数存在";
        } else {
            qDebug() << "✗ NodeDelegateModel 构造函数不存在";
        }

        qtNodesLib.unload();
    } else {
        qDebug() << "✗ QtNodes 库加载失败:" << qtNodesLib.errorString();
    }
}

void testNodeCreation()
{
    qDebug() << "=== 详细节点创建测试 ===";

    // 创建注册表并注册模型
    auto registry = std::make_shared<NodeDelegateModelRegistry>();

    qDebug() << "注册 NumberSourceDataModel...";
    try {
        registry->registerModel<NumberSourceDataModel>("NumberSource");
        qDebug() << "✓ NumberSourceDataModel 注册成功";
    } catch (const std::exception& e) {
        qCritical() << "✗ NumberSourceDataModel 注册失败:" << e.what();
        return;
    }

    qDebug() << "注册 NumberDisplayDataModel...";
    try {
        registry->registerModel<NumberDisplayDataModel>("NumberDisplay");
        qDebug() << "✓ NumberDisplayDataModel 注册成功";
    } catch (const std::exception& e) {
        qCritical() << "✗ NumberDisplayDataModel 注册失败:" << e.what();
        return;
    }

    // 创建数据模型
    auto graphModel = std::make_shared<DataFlowGraphModel>(registry);

    // 测试创建节点
    qDebug() << "测试创建 NumberSource 节点:";
    try {
        auto sourceNodeId = graphModel->addNode("NumberSource");
        if (sourceNodeId != InvalidNodeId) {
            qDebug() << "✓ NumberSource 节点创建成功，ID:" << sourceNodeId;

            // 检查节点数据
            auto nodeType = graphModel->nodeData(sourceNodeId, NodeRole::Type);
            auto nodeCaption = graphModel->nodeData(sourceNodeId, NodeRole::Caption);
            qDebug() << "节点类型:" << nodeType.toString();
            qDebug() << "节点标题:" << nodeCaption.toString();
        } else {
            qDebug() << "✗ NumberSource 节点创建失败";

            // 检查可用的模型名称
            qDebug() << "检查可用的模型名称...";
            // 这里可能需要使用 QtNodes 提供的其他方法来检查
        }
    } catch (const std::exception& e) {
        qCritical() << "创建 NumberSource 节点时异常:" << e.what();
    }

    qDebug() << "测试创建 NumberDisplay 节点:";
    try {
        auto displayNodeId = graphModel->addNode("NumberDisplay");
        if (displayNodeId != InvalidNodeId) {
            qDebug() << "✓ NumberDisplay 节点创建成功，ID:" << displayNodeId;
        } else {
            qDebug() << "✗ NumberDisplay 节点创建失败";
        }
    } catch (const std::exception& e) {
        qCritical() << "创建 NumberDisplay 节点时异常:" << e.what();
    }

    qDebug() << "=== 测试完成 ===";
}

void checkModelNames()
{
    qDebug() << "=== 检查模型名称一致性 ===";

    // 创建模型实例并检查名称
    auto sourceModel = std::make_unique<NumberSourceDataModel>();
    auto displayModel = std::make_unique<NumberDisplayDataModel>();

    qDebug() << "NumberSourceDataModel.name():" << sourceModel->name();
    qDebug() << "NumberDisplayDataModel.name():" << displayModel->name();

    // 检查注册时使用的名称
    qDebug() << "注册时使用的名称: 'NumberSource' 和 'NumberDisplay'";

    // 检查是否匹配
    if (sourceModel->name() == "NumberSource") {
        qDebug() << "✓ NumberSourceDataModel 名称匹配";
    } else {
        qDebug() << "✗ NumberSourceDataModel 名称不匹配";
        qDebug() << "  期望: 'NumberSource'";
        qDebug() << "  实际: '" << sourceModel->name() << "'";
    }

    if (displayModel->name() == "NumberDisplay") {
        qDebug() << "✓ NumberDisplayDataModel 名称匹配";
    } else {
        qDebug() << "✗ NumberDisplayDataModel 名称不匹配";
        qDebug() << "  期望: 'NumberDisplay'";
        qDebug() << "  实际: '" << displayModel->name() << "'";
    }
}

void testDifferentRegistrationNames()
{
    qDebug() << "=== 尝试不同的注册名称 ===";

    const char* testNames[] = {
        "NumberSource",
        "NumberSourceDataModel",
        "number_source",
        "Source",
        "NumberSourceModel",
        "Sources",
        nullptr
    };

    for (int i = 0; testNames[i] != nullptr; i++) {
        qDebug() << "尝试注册名称:" << testNames[i];

        try {
            auto registry = std::make_shared<NodeDelegateModelRegistry>();
            registry->registerModel<NumberSourceDataModel>(testNames[i]);

            auto graphModel = std::make_shared<DataFlowGraphModel>(registry);
            auto nodeId = graphModel->addNode(testNames[i]);

            if (nodeId != InvalidNodeId) {
                qDebug() << "✓ 成功! 使用名称:" << testNames[i];
                return;
            } else {
                qDebug() << "✗ 失败! 使用名称:" << testNames[i];
            }
        } catch (...) {
            qDebug() << "✗ 异常! 使用名称:" << testNames[i];
        }
    }

    qDebug() << "=== 所有名称尝试失败 ===";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== 开始详细测试 ===";

    // 运行所有测试
    checkModelNames();
    testNodeCreation();
    testDifferentRegistrationNames();

    MainWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"