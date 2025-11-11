//
// Created by douziguo on 2025/11/11.
//

#include "DataFlowModel.h"

DataFlowModel::DataFlowModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
    : DataFlowGraphModel(std::move(registry))
    , m_detachPossible(true)
    , m_nodesLocked(false)
{
}

bool DataFlowModel::deleteConnection(QtNodes::ConnectionId const& connectionId)
{
    if (!m_detachPossible) {
        return false; // 不允许断开连接
    }
    return DataFlowGraphModel::deleteConnection(connectionId);
}

void DataFlowModel::setDetachPossible(bool enabled)
{
    m_detachPossible = enabled;
    // 不需要发射信号，因为 deleteConnection 方法会实时检查
}

QtNodes::NodeFlags DataFlowModel::nodeFlags(QtNodes::NodeId nodeId) const
{
    auto basicFlags = DataFlowGraphModel::nodeFlags(nodeId);

    if (m_nodesLocked) {
        basicFlags |= QtNodes::NodeFlag::Locked;
    }

    return basicFlags;
}

void DataFlowModel::setNodesLocked(bool locked)
{
    if (m_nodesLocked != locked) {
        m_nodesLocked = locked;

        // 通知所有节点标志已更新
        for (auto nodeId : allNodeIds()) {
            Q_EMIT nodeFlagsUpdated(nodeId);
        }
    }
}