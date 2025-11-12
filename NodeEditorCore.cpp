//
// Created by douziguo on 2025/11/12.
//

#include "nodeeditorcore.h"
#include "BasicNodes.h"
#include <QtNodes/ConnectionStyle>
#include <QtNodes/StyleCollection>
#include <QMessageBox>
#include <QDebug>

// 定义 InvalidConnectionId 常量
static const ConnectionId InvalidConnectionId{InvalidNodeId, 0, InvalidNodeId, 0};

NodeEditorCore::NodeEditorCore(QObject* parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_nodeCounter(0)
{
}

NodeEditorCore::~NodeEditorCore()
{
    // 注意删除顺序：先删除视图，再删除场景
    delete m_view;
    delete m_scene;
}

bool NodeEditorCore::initialize()
{
    try {
        qDebug() << "Initializing NodeEditorCore...";

        // 1. 创建模型注册表
        m_registry = std::make_shared<NodeDelegateModelRegistry>();

        // 2. 注册节点模型
        registerNodeModels();
        qDebug() << "Node models registered successfully";

        // 3. 创建数据流图形模型
        m_graphModel = std::make_shared<DataFlowGraphModel>(m_registry);
        qDebug() << "DataFlowGraphModel created";

        // 4. 创建图形场景
        m_scene = new DataFlowGraphicsScene(*m_graphModel, this);
        qDebug() << "DataFlowGraphicsScene created";

        // 5. 创建视图
        m_view = new GraphicsView(m_scene);
        m_view->setScene(m_scene);
        qDebug() << "GraphicsView created";

        // 6. 设置视图属性以确保正确显示
        m_view->setRenderHint(QPainter::Antialiasing);
        m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_view->setDragMode(QGraphicsView::RubberBandDrag);

        // 设置初始场景大小
        m_scene->setSceneRect(-1000, -1000, 2000, 2000);

        // 7. 设置连接样式
        QtNodes::ConnectionStyle::setConnectionStyle(
            R"(
            {
                "ConnectionStyle": {
                    "ConstructionColor": "gray",
                    "NormalColor": "black",
                    "SelectedColor": "blue",
                    "SelectedHaloColor": "deepskyblue",
                    "HoveredColor": "deepskyblue",
                    "LineWidth": 3.0,
                    "ConstructionLineWidth": 2.0,
                    "PointDiameter": 10.0,
                    "UseDataDefinedColors": false
                }
            }
            )"
        );

        qDebug() << "NodeEditorCore initialized successfully";
        qDebug() << "Scene rect:" << m_scene->sceneRect();
        qDebug() << "View size:" << m_view->size();

        return true;

    } catch (const std::exception& e) {
        qCritical() << "Failed to initialize NodeEditorCore:" << e.what();
        return false;
    }
}

void NodeEditorCore::registerNodeModels()
{
    if (!m_registry) {
        qWarning() << "Registry is null, cannot register node models";
        return;
    }

    try {
        // 注册您的基础节点模型 - 使用新的API
        // 新版本QtNodes使用 registerModel<T>() 而不是 registerModel<T>(category)
        m_registry->registerModel<StartNodeModel>();
        m_registry->registerModel<EndNodeModel>();

        qDebug() << "Registered node models successfully";

        // 新版本API可能没有 registeredModelsCategory 方法
        // 我们可以通过其他方式验证注册是否成功
        qDebug() << "Node models registration completed";

    } catch (const std::exception& e) {
        qCritical() << "Failed to register node models:" << e.what();
    }
}

NodeId NodeEditorCore::addNode(const QString& nodeType)
{
    if (!m_graphModel) {
        qWarning() << "Graph model is null";
        return InvalidNodeId;
    }

    try {
        // 添加节点
        NodeId nodeId = m_graphModel->addNode(nodeType);

        if (nodeId == InvalidNodeId) {
            qWarning() << "Failed to add node of type:" << nodeType;
            return InvalidNodeId;
        }

        // 设置节点位置（自动排列）
        QPointF position = getNextNodePosition();
        m_graphModel->setNodeData(nodeId, NodeRole::Position, position);

        m_nodeCounter++;

        qDebug() << "Added node - Type:" << nodeType << "ID:" << nodeId << "Position:" << position;

        emit nodeAdded(nodeId);
        emit modificationChanged(true);

        return nodeId;

    } catch (const std::exception& e) {
        qCritical() << "Exception while adding node" << nodeType << ":" << e.what();
        return InvalidNodeId;
    }
}

bool NodeEditorCore::removeNode(NodeId nodeId)
{
    if (!m_graphModel || nodeId == InvalidNodeId) {
        return false;
    }

    try {
        // 新版本使用 deleteNode 而不是 removeNode
        bool success = m_graphModel->deleteNode(nodeId);
        if (success) {
            m_nodeCounter--;
            emit nodeRemoved(nodeId);
            emit modificationChanged(true);
            qDebug() << "Removed node ID:" << nodeId;
        }
        return success;
    } catch (const std::exception& e) {
        qCritical() << "Failed to remove node" << nodeId << ":" << e.what();
        return false;
    }
}

ConnectionId NodeEditorCore::addConnection(NodeId sourceNode, PortIndex sourcePort,
                                          NodeId targetNode, PortIndex targetPort)
{
    if (!m_graphModel) {
        qWarning() << "Graph model is null";
        return InvalidConnectionId;
    }

    // 检查参数有效性
    if (sourceNode == InvalidNodeId || targetNode == InvalidNodeId) {
        qWarning() << "Invalid node IDs for connection";
        return InvalidConnectionId;
    }

    // 检查节点是否存在
    auto nodeIds = m_graphModel->allNodeIds();
    if (std::find(nodeIds.begin(), nodeIds.end(), sourceNode) == nodeIds.end() ||
        std::find(nodeIds.begin(), nodeIds.end(), targetNode) == nodeIds.end()) {
        qWarning() << "One or both nodes do not exist";
        return InvalidConnectionId;
    }

    try {
        ConnectionId connectionId{sourceNode, sourcePort, targetNode, targetPort};

        // 检查连接是否已存在
        auto existingConnections = m_graphModel->allConnectionIds(sourceNode);
        if (std::find(existingConnections.begin(), existingConnections.end(), connectionId) != existingConnections.end()) {
            qWarning() << "Connection already exists";
            return InvalidConnectionId;
        }

        // 添加连接（新版本返回void）
        m_graphModel->addConnection(connectionId);

        // 验证连接是否真的被添加了
        auto connectionsAfter = m_graphModel->allConnectionIds(sourceNode);
        bool connectionAddedSuccessfully = std::find(connectionsAfter.begin(), connectionsAfter.end(), connectionId) != connectionsAfter.end();

        if (connectionAddedSuccessfully) {
            emit connectionAdded(connectionId);
            emit modificationChanged(true);
            qDebug() << "Successfully added connection from node" << sourceNode << "port" << sourcePort
                     << "to node" << targetNode << "port" << targetPort;
            return connectionId;
        } else {
            qWarning() << "Failed to add connection (verification failed)";
            return InvalidConnectionId;
        }

    } catch (const std::exception& e) {
        qCritical() << "Exception while adding connection:" << e.what();
        return InvalidConnectionId;
    }
}

bool NodeEditorCore::removeConnection(ConnectionId connectionId)
{
    if (!m_graphModel) {
        return false;
    }

    try {
        // 新版本使用 deleteConnection
        bool success = m_graphModel->deleteConnection(connectionId);
        if (success) {
            emit connectionRemoved(connectionId);
            emit modificationChanged(true);
            qDebug() << "Removed connection";
        }
        return success;
    } catch (const std::exception& e) {
        qCritical() << "Failed to remove connection:" << e.what();
        return false;
    }
}

QJsonObject NodeEditorCore::saveScene() const
{
    if (!m_graphModel) {
        return QJsonObject();
    }

    try {
        QJsonObject sceneData = m_graphModel->save();

        // 移除const限定符来发射信号
        const_cast<NodeEditorCore*>(this)->sceneSaved();

        qDebug() << "Scene saved successfully, nodes:" << nodeCount();
        return sceneData;
    } catch (const std::exception& e) {
        qCritical() << "Failed to save scene:" << e.what();
        return QJsonObject();
    }
}

bool NodeEditorCore::loadScene(const QJsonObject& json)
{
    if (!m_graphModel) {
        return false;
    }

    if (json.isEmpty()) {
        qWarning() << "Empty JSON provided for scene loading";
        return false;
    }

    try {
        // 清除现有场景 - 新版本使用 allNodeIds() 和 deleteNode()
        auto nodeIds = m_graphModel->allNodeIds();
        for (auto nodeId : nodeIds) {
            m_graphModel->deleteNode(nodeId);
        }

        // 加载新场景
        m_graphModel->load(json);
        m_nodeCounter = static_cast<int>(m_graphModel->allNodeIds().size());

        emit sceneLoaded();
        emit modificationChanged(false);

        qDebug() << "Scene loaded successfully, nodes:" << nodeCount();
        return true;

    } catch (const std::exception& e) {
        qCritical() << "Failed to load scene:" << e.what();
        return false;
    }
}

void NodeEditorCore::setNodePosition(NodeId nodeId, const QPointF& position)
{
    if (m_graphModel && nodeId != InvalidNodeId) {
        m_graphModel->setNodeData(nodeId, NodeRole::Position, position);
    }
}

int NodeEditorCore::nodeCount() const
{
    if (!m_graphModel) return 0;
    auto nodes = m_graphModel->allNodeIds();
    return static_cast<int>(nodes.size());
}

int NodeEditorCore::connectionCount() const
{
    if (!m_graphModel) return 0;

    size_t count = 0;
    auto nodeIds = m_graphModel->allNodeIds();
    for (auto nodeId : nodeIds) {
        auto connections = m_graphModel->allConnectionIds(nodeId);
        count += connections.size();
    }
    // 每个连接被计算两次（出入节点各一次），所以需要除以2
    return static_cast<int>(count / 2);
}

QPointF NodeEditorCore::getNextNodePosition()
{
    // 简单的位置排列策略：按添加顺序排列
    int row = m_nodeCounter / 3; // 每行最多3个节点
    int col = m_nodeCounter % 3;
    return QPointF(col * 250, row * 200);
}