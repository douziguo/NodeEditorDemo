//
// Created by douziguo on 2025/11/12.
//

#include "BasicNodes.h"

// 开始节点实现
StartNodeModel::StartNodeModel()
{
    m_label = new QLabel("开始");
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("QLabel { background-color: #4CAF50; color: white; border: 2px solid #388E3C; border-radius: 10px; padding: 8px; font-weight: bold; }");
    m_label->setMinimumSize(80, 40);
}

unsigned int StartNodeModel::nPorts(PortType portType) const
{
    return (portType == PortType::Out) ? 1 : 0; // 只有输出端口
}

NodeDataType StartNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    return FlowData().type();
}

std::shared_ptr<NodeData> StartNodeModel::outData(PortIndex port)
{
    return std::make_shared<FlowData>();
}

void StartNodeModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    // 开始节点没有输入端口
}

QWidget* StartNodeModel::embeddedWidget()
{
    return m_label;
}

QJsonObject StartNodeModel::save() const
{
    return QJsonObject{};
}

void StartNodeModel::load(QJsonObject const& json)
{
}

// 结束节点实现
EndNodeModel::EndNodeModel()
{
    m_label = new QLabel("结束");
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("QLabel { background-color: #F44336; color: white; border: 2px solid #D32F2F; border-radius: 10px; padding: 8px; font-weight: bold; }");
    m_label->setMinimumSize(80, 40);
}

unsigned int EndNodeModel::nPorts(PortType portType) const
{
    return (portType == PortType::In) ? 1 : 0; // 只有输入端口
}

NodeDataType EndNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    return FlowData().type();
}

std::shared_ptr<NodeData> EndNodeModel::outData(PortIndex port)
{
    return nullptr; // 结束节点没有输出端口
}

void EndNodeModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    m_hasInput = (data != nullptr);
    if (m_hasInput) {
        m_label->setText("完成");
    } else {
        m_label->setText("结束");
    }
}

QWidget* EndNodeModel::embeddedWidget()
{
    return m_label;
}

QJsonObject EndNodeModel::save() const
{
    return QJsonObject{};
}

void EndNodeModel::load(QJsonObject const& json)
{
}