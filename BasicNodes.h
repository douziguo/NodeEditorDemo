//
// Created by douziguo on 2025/11/12.
//

#ifndef NODEEDITORDEMO_BASICNODES_H
#define NODEEDITORDEMO_BASICNODES_H

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeData>
#include <QLabel>

// 使用 QtNodes 中定义的 NodeData 基类
using namespace QtNodes;

// 数字数据类型
class NumberData : public NodeData
{
public:
    NumberData() : number(0.0) {}
    NumberData(double n) : number(n) {}

    NodeDataType type() const override
    {
        return NodeDataType{"number", "Number"};
    }

    double number = 0.0;
};

// 开始节点模型
class StartNodeModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    StartNodeModel();

    QString caption() const override { return "开始节点"; }
    QString name() const override { return "StartNode"; }
    //QString descriptions() const override { return "流程开始节点"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget* embeddedWidget() override;

    QJsonObject save() const override;
    void load(QJsonObject const& json) override;

private:
    double m_number = 42.0;
    QLabel* m_label = nullptr;
};

// 结束节点模型
class EndNodeModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    EndNodeModel();

    QString caption() const override { return "结束节点"; }
    QString name() const override { return "EndNode"; }
    //QString descriptions() const override { return "流程结束节点"; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget* embeddedWidget() override;

    QJsonObject save() const override;
    void load(QJsonObject const& json) override;

private:
    double m_number = 0.0;
    QLabel* m_label = nullptr;
};

#endif //NODEEDITORDEMO_BASICNODES_H