//
// Created by douziguo on 2025/11/11.
//

#ifndef NODEEDITORDEMO_DATATYPES_H
#define NODEEDITORDEMO_DATATYPES_H

#include <QtNodes/NodeData>

class NumberData : public QtNodes::NodeData
{
public:
    NumberData() : number(0.0) {}
    NumberData(double n) : number(n) {}

    QtNodes::NodeDataType type() const override
    {
        return QtNodes::NodeDataType{"number", "Number"};
    }

    double number = 0.0;
};

#endif //NODEEDITORDEMO_DATATYPES_H