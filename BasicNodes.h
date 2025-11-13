//
// Created by douziguo on 2025/11/12.
//

#ifndef NODEEDITORDEMO_BASICNODES_H
#define NODEEDITORDEMO_BASICNODES_H

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QLabel>

using namespace QtNodes;

// 流程数据类型
class FlowData : public NodeData
{
public:
    NodeDataType type() const override
    {
        return NodeDataType{"flow", "Flow"};
    }
};

// 开始节点
class StartNodeModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    StartNodeModel();

    QString caption() const override { return "开始"; }
    QString name() const override { return "StartNode"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;
    QWidget* embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const& json) override;

private:
    QLabel* m_label = nullptr;
};

// 结束节点
class EndNodeModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    EndNodeModel();

    QString caption() const override { return "结束"; }
    QString name() const override { return "EndNode"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;
    QWidget* embeddedWidget() override;
    QJsonObject save() const override;
    void load(QJsonObject const& json) override;

private:
    QLabel* m_label = nullptr;
    bool m_hasInput = false;
};

#endif // NODEEDITORDEMO_BASICNODES_H