#include "ctrl_modbus_device.h"
#include <QModbusDataUnit>
#include <QDebug>

CtrlModbusDevice::CtrlModbusDevice(QWidget *parent) : QWidget(parent)
{
    modbusClient = new QModbusClient;
}

CtrlModbusDevice::~CtrlModbusDevice()
{

}

