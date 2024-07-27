#ifndef CTRL_MODBUS_DEVICE_H
#define CTRL_MODBUS_DEVICE_H

#include <QWidget>
#include <QModbusClient>
#include <QPushButton>


class CtrlModbusDevice : public QWidget
{
    Q_OBJECT
public:
    CtrlModbusDevice(QWidget *parent = nullptr);
    ~CtrlModbusDevice();

private:
    QModbusClient *modbusClient = nullptr;
};


#endif // CTRL_MODBUS_DEVICE_H
