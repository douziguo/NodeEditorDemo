//
// Created by douziguo on 2025/11/12.
//

#ifndef NODEEDITORDEMO_NODEEDITORCORE_H
#define NODEEDITORDEMO_NODEEDITORCORE_H

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QObject>
#include <QJsonObject>
#include <QVariantMap>
#include <functional>
#include <memory>

using namespace QtNodes;

// 定义 InvalidConnectionId 常量
static const ConnectionId InvalidConnectionId{InvalidNodeId, 0, InvalidNodeId, 0};

// ConnectionId 转换为字符串的辅助函数
inline QString connectionIdToString(const ConnectionId& conn)
{
    return QString("Connection(outNode=%1, outPort=%2, inNode=%3, inPort=%4)")
        .arg(conn.outNodeId)
        .arg(conn.outPortIndex)
        .arg(conn.inNodeId)
        .arg(conn.inPortIndex);
}

class NodeEditorCore : public QObject
{
    Q_OBJECT

public:
    explicit NodeEditorCore(QObject* parent = nullptr);
    ~NodeEditorCore();

    bool initialize();

    DataFlowGraphicsScene* scene() const { return m_scene; }
    GraphicsView* view() const { return m_view; }
    std::shared_ptr<DataFlowGraphModel> graphModel() const { return m_graphModel; }

    NodeId addNode(const QString& nodeType, const QPointF& position = QPointF(0, 0));
    bool removeNode(NodeId nodeId);
    void setNodePosition(NodeId nodeId, const QPointF& position);

    ConnectionId addConnection(NodeId sourceNode, PortIndex sourcePort,
                               NodeId targetNode, PortIndex targetPort);
    bool removeConnection(ConnectionId connectionId);

    QJsonObject saveScene() const;
    bool loadScene(const QJsonObject& json);
    void clearScene();

    bool executeFlow();
    QVariantMap getExecutionResults() const;
    int nodeCount() const;
    int connectionCount() const;

    using NodeExecutor = std::function<QVariant(NodeId, const QVariantMap&)>;
    void registerNodeExecutor(const QString& nodeType, NodeExecutor executor);

    bool hasUnsavedChanges() const { return m_isModified; }
    void setModified(bool modified);

signals:
    void sceneLoaded();
    void sceneSaved();
    void nodeAdded(NodeId nodeId);
    void nodeRemoved(NodeId nodeId);
    void connectionAdded(ConnectionId connectionId);
    void connectionRemoved(ConnectionId connectionId);
    void modificationChanged(bool modified);
    void executionStarted();
    void executionFinished(bool success);
    void nodeExecuted(NodeId nodeId, QVariant result);

private:
    void registerNodeModels();
    void registerNodeExecutors();
    void setupConnections();
    QPointF getNextNodePosition();
    QList<NodeId> getExecutionOrder() const;
    QVariant executeNode(NodeId nodeId);

private:
    std::shared_ptr<NodeDelegateModelRegistry> m_registry;
    std::shared_ptr<DataFlowGraphModel> m_graphModel;
    DataFlowGraphicsScene* m_scene;
    GraphicsView* m_view;

    QMap<NodeId, QVariant> m_executionResults;
    QMap<QString, NodeExecutor> m_nodeExecutors;
    int m_nodeCounter = 0;
    bool m_isModified = false;
};

#endif // NODEEDITORDEMO_NODEEDITORCORE_H