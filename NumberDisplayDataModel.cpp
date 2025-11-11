//
// Created by douziguo on 2025/11/11.
//

#include "NumberDisplayDataModel.h"

NumberDisplayDataModel::NumberDisplayDataModel()
{
}

QtNodes::NodeDataType NumberDisplayDataModel::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
    Q_UNUSED(portType)
    Q_UNUSED(portIndex)
    return NumberData().type();
}

std::shared_ptr<QtNodes::NodeData> NumberDisplayDataModel::outData(QtNodes::PortIndex portIndex)
{
    Q_UNUSED(portIndex)
    return nullptr; // 只有输入端口，没有输出
}

void NumberDisplayDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex)
{
    Q_UNUSED(portIndex)

    if (auto numberData = std::dynamic_pointer_cast<NumberData>(data)) {
        m_number = numberData->number;
        if (m_label) {
            m_label->setText(QString::number(m_number));
            m_label->setStyleSheet("QLabel { background: lightgreen; padding: 5px; border: 1px solid gray; }");
        }
    } else {
        m_number = 0.0;
        if (m_label) {
            m_label->setText("No input");
            m_label->setStyleSheet("");
        }
    }
}

QWidget* NumberDisplayDataModel::embeddedWidget()
{
    if (!m_label) {
        m_label = new QLabel("No input");
        m_label->setMinimumWidth(80);
        m_label->setAlignment(Qt::AlignCenter);
    }
    return m_label;
}