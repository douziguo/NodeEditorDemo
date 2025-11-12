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
#include <memory>

using namespace QtNodes;

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

    NodeId addNode(const QString& nodeType);
    bool removeNode(NodeId nodeId);

    ConnectionId addConnection(NodeId sourceNode, PortIndex sourcePort,
                               NodeId targetNode, PortIndex targetPort);
    bool removeConnection(ConnectionId connectionId);

    QJsonObject saveScene() const;
    bool loadScene(const QJsonObject& json);

    // 新增方法：设置节点位置
    void setNodePosition(NodeId nodeId, const QPointF& position);

    // 新增方法：获取场景信息
    int nodeCount() const;
    int connectionCount() const;

signals:
    void sceneLoaded();
    void sceneSaved();
    void nodeAdded(NodeId nodeId);
    void nodeRemoved(NodeId nodeId);
    void connectionAdded(ConnectionId connectionId);
    void connectionRemoved(ConnectionId connectionId);
    void modificationChanged(bool modified);

private:
    void registerNodeModels();
    QPointF getNextNodePosition();

private:
    std::shared_ptr<NodeDelegateModelRegistry> m_registry;
    std::shared_ptr<DataFlowGraphModel> m_graphModel;
    DataFlowGraphicsScene* m_scene;
    GraphicsView* m_view;

    int m_nodeCounter = 0; // 用于自动排列节点位置
};

#endif // NODEEDITORDEMO_NODEEDITORCORE_H