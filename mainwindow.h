//
// Created by douziguo on 2025/11/12.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "nodeeditorcore.h"
#include <QMainWindow>
#include <QListWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QCloseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void checkDragDropSettings();

    void testNodeAdding();

    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    // 文件操作
    void newScene();
    void openScene();
    bool saveScene();
    bool saveSceneAs();

    // 编辑操作
    void undo();
    void redo();
    void copy();
    void paste();
    void deleteSelected();

    // 视图操作
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToView();
    void showNodePanel(bool show);

    // 节点操作
    void addStartNode();
    void addEndNode();
    void addMathOperationNode();
    void addTextDisplayNode();

    // 工具操作
    void executeFlow();
    void validateFlow();
    void clearScene();

    // 帮助
    void showAbout();
    void showHelp();

    // 更新界面
    void updateWindowTitle();
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBars();
    void setupDockWidgets();
    void setupStatusBar();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    bool confirmUnsavedChanges();

    void addNodeFromDrag(const QString &nodeType, const QPointF &scenePos);
    QPointF mapToScenePos(const QPoint &windowPos) const;

private:
    NodeEditorCore *m_editorCore;

    // UI 组件
    QDockWidget *m_nodeDock;
    QListWidget *m_nodeList;
    QLabel *m_statusLabel;
    QLabel *m_nodeCountLabel;
    QLabel *m_connectionCountLabel;

    // 动作
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exitAction;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_deleteAction;
    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_resetZoomAction;
    QAction *m_fitToViewAction;
    QAction *m_showNodePanelAction;
    QAction *m_executeAction;
    QAction *m_validateAction;
    QAction *m_clearAction;
    QAction *m_aboutAction;
    QAction *m_helpAction;

    // 状态
    QString m_currentFile;
    bool m_isModified;
};

#endif // MAINWINDOW_H