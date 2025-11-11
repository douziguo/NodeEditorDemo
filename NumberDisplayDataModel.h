//
// Created by douziguo on 2025/11/11.
//

#ifndef NODEEDITORDEMO_NUMBERDISPLAYDATAMODEL_H
#define NODEEDITORDEMO_NUMBERDISPLAYDATAMODEL_H

#include <QtNodes/NodeDelegateModel>
#include <QLabel>
#include "DataTypes.h"

class NumberDisplayDataModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    NumberDisplayDataModel();

    QString caption() const override { return "Number Display"; }
    QString name() const override { return "NumberDisplay"; }

    unsigned int nPorts(QtNodes::PortType portType) const override
    {
        return (portType == QtNodes::PortType::In) ? 1 : 0;
    }

    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex portIndex) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex) override;

    QWidget* embeddedWidget() override;

    QJsonObject save() const override { return {}; }
    void load(QJsonObject const&) override {}

private:
    QLabel* m_label = nullptr;
    double m_number = 0.0;
};

#endif //NODEEDITORDEMO_NUMBERDISPLAYDATAMODEL_H