//
// Created by douziguo on 2025/11/11.
//

#include "NumberSourceDataModel.h"
#include <QDebug>


NumberSourceDataModel::NumberSourceDataModel()
{
    qDebug() << "NumberSourceDataModel 构造函数";
}

QString NumberSourceDataModel::caption() const
{
    return QString("Number Source");
}

QString NumberSourceDataModel::name() const
{
    // 这个名称必须与注册时使用的名称完全一致
    return QString("NumberSource");
}

unsigned int NumberSourceDataModel::nPorts(QtNodes::PortType portType) const
{
    // 只有输出端口，没有输入端口
    return (portType == QtNodes::PortType::Out) ? 1 : 0;
}

QtNodes::NodeDataType NumberSourceDataModel::dataType(QtNodes::PortType portType,
                                                     QtNodes::PortIndex portIndex) const
{
    Q_UNUSED(portType)
    Q_UNUSED(portIndex)
    return NumberData().type();
}

std::shared_ptr<QtNodes::NodeData> NumberSourceDataModel::outData(QtNodes::PortIndex port)
{
    Q_UNUSED(port)
    return std::make_shared<NumberData>(m_number);
}

void NumberSourceDataModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                    QtNodes::PortIndex portIndex)
{
    Q_UNUSED(data)
    Q_UNUSED(portIndex)
    // 只有输出端口，不需要处理输入
}

QWidget* NumberSourceDataModel::embeddedWidget()
{
    if (!m_spinBox) {
        m_spinBox = new QSpinBox();
        m_spinBox->setRange(0, 1000);
        m_spinBox->setValue(m_number);
        connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &NumberSourceDataModel::onNumberChanged);
    }
    return m_spinBox;
}

QJsonObject NumberSourceDataModel::save() const
{
    QJsonObject modelJson;
    modelJson["name"] = name();
    modelJson["number"] = m_number;
    return modelJson;
}

void NumberSourceDataModel::load(QJsonObject const& json)
{
    if (json.contains("number")) {
        m_number = json["number"].toInt();
        if (m_spinBox) {
            m_spinBox->setValue(m_number);
        }
    }
}

void NumberSourceDataModel::onNumberChanged(int value)
{
    m_number = value;
    Q_EMIT dataUpdated(0);
}