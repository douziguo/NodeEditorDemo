//
// Created by douziguo on 2025/11/12.
//

#include "NodeEditorCore.h"
#include "BasicNodes.h"
#include <QtNodes/ConnectionStyle>
#include <QtNodes/StyleCollection>
#include <QStack>
#include <QSet>
#include <QDebug>

NodeEditorCore::NodeEditorCore(QObject* parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_nodeCounter(0)
    , m_isModified(false)
{
}

NodeEditorCore::~NodeEditorCore()
{
    delete m_view;
    delete m_scene;
}

bool NodeEditorCore::initialize()
{
    try {
        qDebug() << "初始化 NodeEditorCore";

        m_registry = std::make_shared<NodeDelegateModelRegistry>();
        registerNodeModels();

        m_graphModel = std::make_shared<DataFlowGraphModel>(m_registry);

        m_scene = new DataFlowGraphicsScene(*m_graphModel, this);

        m_view = new GraphicsView(m_scene);
        m_view->setScene(m_scene);
        m_view->setRenderHint(QPainter::Antialiasing);
        m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        m_view->setDragMode(QGraphicsView::RubberBandDrag);

        QtNodes::ConnectionStyle::setConnectionStyle(R"({
            "ConnectionStyle": {
                "ConstructionColor": "gray",
                "NormalColor": "black",
                "SelectedColor": "gray",
                "SelectedHaloColor": "deepskyblue",
                "HoveredColor": "deepskyblue",
                "LineWidth": 3.0,
                "ConstructionLineWidth": 2.0,
                "PointDiameter": 10.0
            }
        })");

        registerNodeExecutors();

        setupConnections();

        m_scene->setSceneRect(-1000, -1000, 2000, 2000);

        qDebug() << "NodeEditorCore 初始化完成";
        return true;

    } catch (const std::exception& e) {
        qCritical() << "初始化失败:" << e.what();
        return false;
    }
}

void NodeEditorCore::registerNodeModels()
{
    if (!m_registry) {
        qWarning() << "注册表未初始化";
        return;
    }

    try {
        m_registry->registerModel<StartNodeModel>();
        m_registry->registerModel<EndNodeModel>();

        qDebug() << "注册节点模型: StartNode, EndNode";

    } catch (const std::exception& e) {
        qCritical() << "注册节点模型失败:" << e.what();
    }
}

void NodeEditorCore::registerNodeExecutors()
{
    registerNodeExecutor("StartNode", [this](NodeId nodeId, const QVariantMap& inputs) {
        qDebug() << "执行开始节点" << nodeId;
        return QVariant("flow_started");
    });

    registerNodeExecutor("EndNode", [this](NodeId nodeId, const QVariantMap& inputs) {
        qDebug() << "执行结束节点" << nodeId;

        if (inputs.contains("input0")) {
            qDebug() << "结束节点接收到输入:" << inputs["input0"];
            return QVariant("flow_completed_with_input");
        } else {
            qDebug() << "结束节点无输入";
            return QVariant("flow_completed_no_input");
        }
    });

    qDebug() << "注册节点执行器: StartNode, EndNode";
}

void NodeEditorCore::setupConnections()
{
    if (!m_graphModel) return;

    connect(m_graphModel.get(), &DataFlowGraphModel::nodeCreated,
            this, [this](NodeId nodeId) {
        qDebug() << "节点创建:" << nodeId;
        emit nodeAdded(nodeId);
        setModified(true);
    });

    connect(m_graphModel.get(), &DataFlowGraphModel::nodeDeleted,
            this, [this](NodeId nodeId) {
        qDebug() << "节点删除:" << nodeId;
        emit nodeRemoved(nodeId);
        setModified(true);
    });

    connect(m_graphModel.get(), &DataFlowGraphModel::connectionCreated,
            this, [this](ConnectionId const& connectionId) {
        qDebug() << "连接创建:" << connectionIdToString(connectionId);
        emit connectionAdded(connectionId);
        setModified(true);
    });

    connect(m_graphModel.get(), &DataFlowGraphModel::connectionDeleted,
            this, [this](ConnectionId const& connectionId) {
        qDebug() << "连接删除:" << connectionIdToString(connectionId);
        emit connectionRemoved(connectionId);
        setModified(true);
    });
}

NodeId NodeEditorCore::addNode(const QString& nodeType, const QPointF& position)
{
    if (!m_graphModel) {
        qWarning() << "图形模型未初始化";
        return InvalidNodeId;
    }

    try {
        NodeId nodeId = m_graphModel->addNode(nodeType);
        if (nodeId == InvalidNodeId) {
            qWarning() << "添加节点失败，类型:" << nodeType;
            return InvalidNodeId;
        }

        QPointF finalPosition = position.isNull() ? getNextNodePosition() : position;
        m_graphModel->setNodeData(nodeId, NodeRole::Position, finalPosition);

        m_nodeCounter++;
        qDebug() << "添加节点成功 - 类型:" << nodeType << "ID:" << nodeId << "位置:" << finalPosition;

        return nodeId;

    } catch (const std::exception& e) {
        qCritical() << "添加节点异常:" << e.what();
        return InvalidNodeId;
    }
}

bool NodeEditorCore::removeNode(NodeId nodeId)
{
    if (!m_graphModel || nodeId == InvalidNodeId) {
        return false;
    }

    try {
        bool success = m_graphModel->deleteNode(nodeId);
        if (success) {
            m_nodeCounter--;
            qDebug() << "删除节点成功，ID:" << nodeId;
        } else {
            qWarning() << "删除节点失败，ID:" << nodeId;
        }
        return success;

    } catch (const std::exception& e) {
        qCritical() << "删除节点异常:" << e.what();
        return false;
    }
}

void NodeEditorCore::setNodePosition(NodeId nodeId, const QPointF& position)
{
    if (m_graphModel && nodeId != InvalidNodeId) {
        m_graphModel->setNodeData(nodeId, NodeRole::Position, position);
        qDebug() << "设置节点位置 - ID:" << nodeId << "位置:" << position;
    }
}

ConnectionId NodeEditorCore::addConnection(NodeId sourceNode, PortIndex sourcePort,
                                           NodeId targetNode, PortIndex targetPort)
{
    if (!m_graphModel) {
        return InvalidConnectionId;
    }

    try {
        ConnectionId connectionId{sourceNode, sourcePort, targetNode, targetPort};

        // 修复：addConnection 返回 void，不返回 bool
        m_graphModel->addConnection(connectionId);

        qDebug() << "添加连接成功 -" << connectionIdToString(connectionId);
        return connectionId;

    } catch (const std::exception& e) {
        qCritical() << "添加连接异常:" << e.what();
        return InvalidConnectionId;
    }
}

bool NodeEditorCore::removeConnection(ConnectionId connectionId)
{
    if (!m_graphModel) {
        return false;
    }

    if (connectionId == InvalidConnectionId) {
        return false;
    }

    try {
        // 修复：deleteConnection 返回 void，不返回 bool
        m_graphModel->deleteConnection(connectionId);
        qDebug() << "删除连接成功 -" << connectionIdToString(connectionId);
        return true;

    } catch (const std::exception& e) {
        qCritical() << "删除连接异常:" << e.what();
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
        qDebug() << "保存场景成功，节点数:" << nodeCount() << "连接数:" << connectionCount();

        // 修复：移除 const 限定符来发射信号
        const_cast<NodeEditorCore*>(this)->emit sceneSaved();
        return sceneData;

    } catch (const std::exception& e) {
        qCritical() << "保存场景失败:" << e.what();
        return QJsonObject();
    }
}

bool NodeEditorCore::loadScene(const QJsonObject& json)
{
    if (!m_graphModel) {
        return false;
    }

    if (json.isEmpty()) {
        qWarning() << "加载场景失败: JSON 数据为空";
        return false;
    }

    try {
        clearScene();

        m_graphModel->load(json);

        // 修复：size_t 到 int 的转换
        auto nodeIds = m_graphModel->allNodeIds();
        m_nodeCounter = static_cast<int>(nodeIds.size());

        qDebug() << "加载场景成功，节点数:" << nodeCount() << "连接数:" << connectionCount();

        emit sceneLoaded();
        setModified(false);
        return true;

    } catch (const std::exception& e) {
        qCritical() << "加载场景失败:" << e.what();
        return false;
    }
}

void NodeEditorCore::clearScene()
{
    if (!m_graphModel) return;

    try {
        auto nodeIds = m_graphModel->allNodeIds();
        for (auto nodeId : nodeIds) {
            m_graphModel->deleteNode(nodeId);
        }
        m_nodeCounter = 0;

        qDebug() << "清空场景完成";
        setModified(true);

    } catch (const std::exception& e) {
        qCritical() << "清空场景失败:" << e.what();
    }
}

bool NodeEditorCore::executeFlow()
{
    if (!m_graphModel) {
        qWarning() << "图形模型未初始化";
        return false;
    }

    qDebug() << "开始执行数据流...";
    emit executionStarted();
    m_executionResults.clear();

    QList<NodeId> executionOrder = getExecutionOrder();
    if (executionOrder.isEmpty()) {
        qWarning() << "无法确定执行顺序，可能为空场景或循环依赖";
        emit executionFinished(false);
        return false;
    }

    qDebug() << "执行顺序:" << executionOrder;

    for (NodeId nodeId : executionOrder) {
        try {
            QVariant result = executeNode(nodeId);
            m_executionResults[nodeId] = result;
            emit nodeExecuted(nodeId, result);

            qDebug() << "执行节点" << nodeId << "结果:" << result;
        } catch (const std::exception& e) {
            qCritical() << "执行节点" << nodeId << "失败:" << e.what();
            emit executionFinished(false);
            return false;
        }
    }

    qDebug() << "数据流执行完成";
    emit executionFinished(true);
    return true;
}

QVariant NodeEditorCore::executeNode(NodeId nodeId)
{
    if (!m_graphModel) {
        throw std::runtime_error("图形模型未初始化");
    }

    QString nodeType = m_graphModel->nodeData(nodeId, NodeRole::Type).toString();
    qDebug() << "执行节点:" << nodeId << "类型:" << nodeType;

    QVariantMap inputs;
    auto connections = m_graphModel->allConnectionIds(nodeId);
    for (const auto& conn : connections) {
        if (conn.inNodeId == nodeId) {
            QString inputKey = QString("input%1").arg(conn.inPortIndex);

            // 使用 QMap<NodeId, QVariant> 可以直接用 NodeId 作为键
            if (m_executionResults.contains(conn.outNodeId)) {
                inputs[inputKey] = m_executionResults[conn.outNodeId];
                qDebug() << "输入" << inputKey << "来自节点" << conn.outNodeId
                         << "值:" << m_executionResults[conn.outNodeId];
            }
        }
    }

    if (m_nodeExecutors.contains(nodeType)) {
        return m_nodeExecutors[nodeType](nodeId, inputs);
    } else {
        throw std::runtime_error(QString("未注册的执行器: %1").arg(nodeType).toStdString());
    }
}

QVariantMap NodeEditorCore::getExecutionResults() const
{
    // 如果需要返回 QVariantMap，可以转换一下
    QVariantMap result;
    for (auto it = m_executionResults.begin(); it != m_executionResults.end(); ++it) {
        result[QString::number(it.key())] = it.value();
    }
    return result;
}

QList<NodeId> NodeEditorCore::getExecutionOrder() const
{
    if (!m_graphModel) {
        return QList<NodeId>();
    }

    QList<NodeId> executionOrder;
    QSet<NodeId> visited;
    QSet<NodeId> tempMark;

    auto nodeIds = m_graphModel->allNodeIds();

    std::function<bool(NodeId)> dfs = [&](NodeId nodeId) {
        if (tempMark.contains(nodeId)) {
            qWarning() << "发现循环依赖，节点:" << nodeId;
            return false;
        }
        if (visited.contains(nodeId)) {
            return true;
        }

        tempMark.insert(nodeId);

        auto connections = m_graphModel->allConnectionIds(nodeId);
        for (const auto& connection : connections) {
            if (connection.inNodeId == nodeId) {
                if (!dfs(connection.outNodeId)) {
                    return false;
                }
            }
        }

        tempMark.remove(nodeId);
        visited.insert(nodeId);
        executionOrder.prepend(nodeId);

        return true;
    };

    for (NodeId nodeId : nodeIds) {
        if (!visited.contains(nodeId)) {
            if (!dfs(nodeId)) {
                qWarning() << "发现循环依赖，无法确定执行顺序";
                return QList<NodeId>();
            }
        }
    }

    return executionOrder;
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

    // 最简单的方法：每个连接会被计算两次，所以除以2
    int totalConnections = 0;
    auto nodeIds = m_graphModel->allNodeIds();

    for (auto nodeId : nodeIds) {
        auto connections = m_graphModel->allConnectionIds(nodeId);
        totalConnections += connections.size();
    }

    // 每个连接在出节点和入节点各被计算一次，所以总数要除以2
    return totalConnections / 2;
}

void NodeEditorCore::registerNodeExecutor(const QString& nodeType, NodeExecutor executor)
{
    m_nodeExecutors[nodeType] = executor;
    qDebug() << "注册节点执行器:" << nodeType;
}

void NodeEditorCore::setModified(bool modified)
{
    if (m_isModified != modified) {
        m_isModified = modified;
        emit modificationChanged(modified);
        qDebug() << (modified ? "场景已修改" : "场景已保存");
    }
}

QPointF NodeEditorCore::getNextNodePosition()
{
    int row = m_nodeCounter / 3;
    int col = m_nodeCounter % 3;
    return QPointF(col * 250, row * 200);
}