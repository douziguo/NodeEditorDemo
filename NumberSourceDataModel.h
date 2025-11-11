//
// Created by douziguo on 2025/11/11.
//

#ifndef NODEEDITORDEMO_NUMBERSOURCEDATAMODEL_H
#define NODEEDITORDEMO_NUMBERSOURCEDATAMODEL_H

#include <QtNodes/NodeDelegateModel>
#include <QSpinBox>
#include "DataTypes.h"

class NumberSourceDataModel : public QtNodes::NodeDelegateModel
{
    Q_OBJECT

public:
    NumberSourceDataModel();
    ~NumberSourceDataModel() override = default;

    // 必须实现的纯虚函数
    QString caption() const override;
    QString name() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex) override;

    QWidget* embeddedWidget() override;

    QJsonObject save() const override;
    void load(QJsonObject const& json) override;

private slots:
    void onNumberChanged(int value);

private:
    int m_number = 42;
    QSpinBox* m_spinBox = nullptr;
};

#endif //NODEEDITORDEMO_NUMBERSOURCEDATAMODEL_H