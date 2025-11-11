//
// Created by douziguo on 2025/11/11.
//

#ifndef NODEEDITORDEMO_DATAFLOWMODEL_H
#define NODEEDITORDEMO_DATAFLOWMODEL_H

#include <QtNodes/DataFlowGraphModel>
#include <memory>

class DataFlowModel : public QtNodes::DataFlowGraphModel
{
    Q_OBJECT

public:
    DataFlowModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);

    bool deleteConnection(QtNodes::ConnectionId const& connectionId);

    void setDetachPossible(bool enabled = true);
    void setNodesLocked(bool locked = true);
    QtNodes::NodeFlags nodeFlags(QtNodes::NodeId nodeId) const override;

private:
    bool m_detachPossible = true;
    bool m_nodesLocked = false;
};

#endif //NODEEDITORDEMO_DATAFLOWMODEL_H