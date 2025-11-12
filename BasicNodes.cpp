//
// Created by douziguo on 2025/11/12.
//

#include "BasicNodes.h"
#include <QVBoxLayout>
#include <QFrame>

StartNodeModel::StartNodeModel()
{
    m_label = new QLabel("开始");
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet(R"(
        QLabel {
            background-color: #4CAF50;
            color: white;
            border: 2px solid #388E3C;
            border-radius: 10px;
            padding: 8px;
            font-weight: bold;
            font-size: 12px;
        }
    )");
    m_label->setMinimumSize(80, 40);
}

unsigned int StartNodeModel::nPorts(PortType portType) const
{
    return (portType == PortType::Out) ? 1 : 0;
}

NodeDataType StartNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    return NumberData().type();
}

std::shared_ptr<NodeData> StartNodeModel::outData(PortIndex port)
{
    return std::make_shared<NumberData>(m_number);
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
    QJsonObject modelJson;
    modelJson["number"] = m_number;
    return modelJson;
}

void StartNodeModel::load(QJsonObject const& json)
{
    if (json.contains("number")) {
        m_number = json["number"].toDouble();
        if (m_label) {
            m_label->setToolTip(QString("输出值: %1").arg(m_number));
        }
    }
}

EndNodeModel::EndNodeModel()
{
    m_label = new QLabel("结束");
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet(R"(
        QLabel {
            background-color: #F44336;
            color: white;
            border: 2px solid #D32F2F;
            border-radius: 10px;
            padding: 8px;
            font-weight: bold;
            font-size: 12px;
        }
    )");
    m_label->setMinimumSize(80, 40);
}

unsigned int EndNodeModel::nPorts(PortType portType) const
{
    return (portType == PortType::In) ? 1 : 0;
}

NodeDataType EndNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    return NumberData().type();
}

std::shared_ptr<NodeData> EndNodeModel::outData(PortIndex port)
{
    return nullptr; // 结束节点没有输出端口
}

void EndNodeModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    if (auto numberData = std::dynamic_pointer_cast<NumberData>(data)) {
        m_number = numberData->number;
        if (m_label) {
            m_label->setText(QString("结果: %1").arg(m_number));
            m_label->setToolTip(QString("输入值: %1").arg(m_number));
        }
    } else {
        // 如果没有输入数据，显示默认文本
        if (m_label) {
            m_label->setText("结束");
            m_label->setToolTip("等待输入数据");
        }
    }
}

QWidget* EndNodeModel::embeddedWidget()
{
    return m_label;
}

QJsonObject EndNodeModel::save() const
{
    QJsonObject modelJson;
    modelJson["number"] = m_number;
    return modelJson;
}

void EndNodeModel::load(QJsonObject const& json)
{
    if (json.contains("number")) {
        m_number = json["number"].toDouble();
        if (m_label) {
            m_label->setText(QString("结果: %1").arg(m_number));
        }
    }
}