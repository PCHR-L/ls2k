#include "mymodbus.h"
#include <QVBoxLayout>
#include <QModbusRtuSerialMaster>  //QModbus通信的客户端类
#include <QModbusDataUnit>         //QModbus数据单元
#include <QModbusReply>             //QModbusReply这个类存储了来自Client的数据
#include <QVariant>
#include <QSerialPort>
#include <QDebug>
#include "mainwindow.h"
#include <QtCharts/QDateTimeAxis>
#include <QFont>


MyModbus::MyModbus(QWidget *parent) :QWidget(parent)
{
    btnRead = new QPushButton("读取一次",this);
    btnRead->setStyleSheet("font-size: 18px;");
    btnRead->resize(100,50);
    pushButton = new QPushButton("开始读取",this);
    pushButton->setStyleSheet("font-size: 18px;");
    pushButton->resize(100,50);
    pushButton_stop = new QPushButton("停止读取",this);
    pushButton_stop->setStyleSheet("font-size: 18px;");
    pushButton_stop->resize(100,50);

    btnToset = new QPushButton("设置报警阈值",this);
    btnToset->setStyleSheet("font-size: 18px;");
    btnToset->resize(100,50);
    btnToSecond = new QPushButton("视频监控",this);
    btnToSecond->setStyleSheet("font-size: 18px;");
    btnToSecond->resize(100,50);
    btnToCan = new QPushButton("Can设备");
    btnToCan->setStyleSheet("font-size: 18px;");
    btnToCan->resize(100,50);

    QLabel  *d1 = new QLabel("设备1:");
    d1->setStyleSheet("font-size: 24px;");
    QLabel  *d2 = new QLabel("设备2:");
    d2->setStyleSheet("font-size: 24px;");
    QLabel  *d3 = new QLabel("设备3:");
    d3->setStyleSheet("font-size: 24px;");
    QLabel  *d4 = new QLabel("设备4:");
    d4->setStyleSheet("font-size: 24px;");
    QLabel  *d5 = new QLabel("设备5:");
    d5->setStyleSheet("font-size: 24px;");
    QLabel  *d6 = new QLabel("设备6:");
    d6->setStyleSheet("font-size: 24px;");
    QLabel  *d7 = new QLabel("设备7:");
    d7->setStyleSheet("font-size: 24px;");
    QLabel  *d8 = new QLabel("设备8:");
    d8->setStyleSheet("font-size: 24px;");
    switchButton1 = new CustomSwitch(this);
    switchButton2 = new CustomSwitch(this);
    switchButton3 = new CustomSwitch(this);
    switchButton4 = new CustomSwitch(this);
    switchButton5 = new CustomSwitch(this);
    switchButton6 = new CustomSwitch(this);
    switchButton7 = new CustomSwitch(this);
    switchButton8 = new CustomSwitch(this);

    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(d1);
    l1->addWidget(switchButton1);
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(d2);
    l2->addWidget(switchButton2);
    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(d3);
    l3->addWidget(switchButton3);
    QHBoxLayout *l4 = new QHBoxLayout;
    l4->addWidget(d4);
    l4->addWidget(switchButton4);
    QHBoxLayout *l5 = new QHBoxLayout;
    l5->addWidget(d5);
    l5->addWidget(switchButton5);
    QHBoxLayout *l6 = new QHBoxLayout;
    l6->addWidget(d6);
    l6->addWidget(switchButton6);
    QHBoxLayout *l7 = new QHBoxLayout;
    l7->addWidget(d7);
    l7->addWidget(switchButton7);
    QHBoxLayout *l8 = new QHBoxLayout;
    l8->addWidget(d8);
    l8->addWidget(switchButton8);

    temp = new QLabel("温度: --°C",this);
    temp->setStyleSheet("font-size: 32px;");
    temp->resize(100,50);
    hum = new QLabel("湿度: --%",this);
    hum->setStyleSheet("font-size: 32px;");
    hum->resize(100,50);
    m_axisX = new QDateTimeAxis();
    m_axisY = new QValueAxis();

    m_axisX->setFormat("HH:mm:ss"); // 设置x轴的日期时间格式
    m_axisX->setTitleText("时间");
    m_axisY->setRange(-10, AXIS_MAX_Y); // 确保使用setRange
    QFont axisFont;
    axisFont.setPointSize(12); // 设置为12点
    m_axisY->setLabelsFont(axisFont);
    m_axisX->setLabelsFont(axisFont);

    temp_lineSeries = new QLineSeries();               //曲线对象
    hum_lineSeries = new QLineSeries();
    temp_lineSeries->setPointsVisible(true);           //设置数据点可见
    temp_lineSeries->setName("温度");
    hum_lineSeries->setPointsVisible(true);           //设置数据点可见
    hum_lineSeries->setName("湿度");

    m_chart = new QChart();
    m_chart->resize(400,400);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);       //加入坐标轴
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addSeries(temp_lineSeries);
    m_chart->addSeries(hum_lineSeries);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);         //加入动画
    temp_lineSeries->attachAxis(m_axisX);              //曲线对象关联上坐标轴，此步骤必须在m_chart->addSeries之后
    temp_lineSeries->attachAxis(m_axisY);
    hum_lineSeries->attachAxis(m_axisX);              //曲线对象关联上坐标轴，此步骤必须在m_chart->addSeries之后
    hum_lineSeries->attachAxis(m_axisY);
    QChartView *chartView = new QChartView(m_chart);
    chartView->setStyleSheet("border: 1px solid black;");

    QLabel *title = new QLabel("控制设备(红色关，绿色开):");
    title->setStyleSheet("font-size: 24px;");

    gridLayout = new QGridLayout(this);
    gridLayout->addWidget(title,5,3,1,8);
    gridLayout->addLayout(l1,1,5);
    gridLayout->addLayout(l2,1,6);
    gridLayout->addLayout(l3,1,7);
    gridLayout->addLayout(l4,1,8);
    gridLayout->addLayout(l5,2,5);
    gridLayout->addLayout(l6,2,6);
    gridLayout->addLayout(l7,2,7);
    gridLayout->addLayout(l8,2,8);
    gridLayout->addWidget(chartView, 0, 0, 4, 5);
    gridLayout->addWidget(temp,3,5,1,2);
    gridLayout->addWidget(hum,3,6,1,2);
    gridLayout->addWidget(btnRead,4,4);
    gridLayout->addWidget(pushButton, 4,5);      // 开始按钮
    gridLayout->addWidget(pushButton_stop, 4, 6); // 停止按钮
    gridLayout->addWidget(btnToset,5,0);
    gridLayout->addWidget(btnToSecond,5,4);
    gridLayout->addWidget(btnToCan,5,8);
    setLayout(gridLayout);

    m_timer = new QTimer(this);
    m_timer->setInterval(10000);

    connect(m_timer,&QTimer::timeout,this,&MyModbus::on_timerTimeout);

    //创建一个ModbusRTU通信的主机对象
    modbusManager = new QModbusRtuSerialMaster;

    //连接
    if(modbusManager->state() == QModbusDevice::ConnectedState){
        modbusManager->disconnect();
    }
    modbusManager->setConnectionParameter(QModbusDevice::SerialPortNameParameter,QVariant("/dev/ttyUSB0"));
    modbusManager->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,QSerialPort::Baud4800);
    modbusManager->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,QSerialPort::Data8);
    modbusManager->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
    modbusManager->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);

    modbusManager->setTimeout(1000);
    modbusManager->setNumberOfRetries(3);

    //连接modbus设备
    bool ok = modbusManager->connectDevice();
    if(ok){
        qDebug() << "Modbus设备连接成功";
    }else{
        qDebug() << "Modbus连接失败";
    }

    connect(btnToSecond,&QPushButton::clicked,this,&MyModbus::on_btnToSecond_clicked);
    connect(btnRead,&QPushButton::clicked,this,&MyModbus::on_btnRead_clicked);
    connect(pushButton,&QPushButton::clicked,this,&MyModbus::on_pushButton_clicked);
    connect(pushButton_stop,&QPushButton::clicked,this,&MyModbus::on_pushButton_stop_clicked);
    connect(btnToset,&QPushButton::clicked,this,&MyModbus::on_btnToSet_clicked);
    connect(btnToCan,&QPushButton::clicked,this,&MyModbus::on_btnToCan_clicked);

    connect(switchButton1,&CustomSwitch::turn,this,&MyModbus::on_switch1);
    connect(switchButton2,&CustomSwitch::turn,this,&MyModbus::on_switch2);
    connect(switchButton3,&CustomSwitch::turn,this,&MyModbus::on_switch3);
    connect(switchButton4,&CustomSwitch::turn,this,&MyModbus::on_switch4);
    connect(switchButton5,&CustomSwitch::turn,this,&MyModbus::on_switch5);
    connect(switchButton6,&CustomSwitch::turn,this,&MyModbus::on_switch6);
    connect(switchButton7,&CustomSwitch::turn,this,&MyModbus::on_switch7);
    connect(switchButton8,&CustomSwitch::turn,this,&MyModbus::on_switch8);
}

MyModbus::~MyModbus()
{
    if(modbusManager)
    {
        modbusManager->disconnectDevice();
    }
    delete modbusManager;
}

void MyModbus::readyRead()
{
    QMutexLocker locker(&m_mutex); // 使用互斥锁保护临界区
    //QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
    auto reply = qobject_cast<QModbusReply*>(sender());
    if(reply == nullptr){
        qDebug() << "没有响应";
        return;
    }
    if(reply->error() == QModbusDevice::NoError){
        const QModbusDataUnit rev_data = reply->result();
        qDebug() << rev_data.values();     //返回的是qvector,其实三quint16数组
        //        vector.append(rev_data.value(0));   //这个vector在头文件中定义为全局变量.因为我们每次只读取一个寄存器

        //        qDebug() << "摘录出来的vector是" << vector;
        quint16 humidityData = rev_data.value(0);
        quint16 temperatureData = rev_data.value(1);


        temperature = temperatureData /10.0;
        humidity = humidityData /10.0;

        temperature_str = QString("%1").arg(temperature);      //把任何类型数据转换为字符串
        humidity_str = QString::number(humidity,'g',4);     //把任何类型数据转换为字符串
        temp->setText(QString("温度: %1°C").arg(temperature));
        hum->setText(QString("湿度: %1%").arg(humidity));

        emit temperatureHumidityUpdated(temperature,humidity);

        // 获取当前时间作为x轴的值
        QDateTime currentTime = QDateTime::currentDateTime();
        qint64 timeMillis = currentTime.toMSecsSinceEpoch();

        // 假设您希望x轴显示最近几分钟的数据
        int minutesToShow = 10;
        qint64 rangeMillis = minutesToShow * 60 * 1000;
        m_axisX->setRange(currentTime.addMSecs(-rangeMillis), currentTime);

        temp_lineSeries->append(timeMillis,temperature);
        hum_lineSeries->append(timeMillis,humidity);

        // 根据数据动态调整坐标轴范围cd
        if (temperature > m_axisY->max())
            m_axisY->setMax(temperature * 1.2);
        if (humidity > m_axisY->max())
            m_axisY->setMax(humidity * 1.2);

        if(temperature > AXIS_MAX_Y * 0.8){
            m_axisY->setMax(temperature*1.2);
        }

        if(humidity > AXIS_MAX_Y * 0.8){
            m_axisY->setMax(humidity*1.2);
        }
    }
    else if (reply->error() == QModbusDevice::ProtocolError){
        qDebug() << "读取数据协议错误" << reply->errorString();
    }else{
        qDebug() << "未知错误" << reply->errorString();
    }
    reply->deleteLater();
}

void MyModbus::on_btnRead_clicked()
{
    QModbusDataUnit data(QModbusDataUnit::InputRegisters,0x0000,2);         //准备Qt的modbus指令数据单元
    // 0x0000是要读取数据的起始寄存器起始地址（不是modbus设备地址）
    //2表示读取2个寄存器里面的数据
    //如果DataUnit中，要读取的起始寄存器设置不正确，运行时会输出“读取数据协议错误iu“Modbus Exception Response.””
    QModbusReply *reply = modbusManager->sendReadRequest(data, 0x01);     //参数0x0001为modbus设备地址，无需功能码，Qt已经自动处理4
    if(reply == nullptr){
        //如果sendReadRequest(),modbus设备地址设置不正确，运行时会输出 “qt.modbus: (Client) Device is not connected”
        qDebug() << "读取数据失败" << modbusManager->errorString();
    }
    if(!reply->isFinished()){
        connect(reply,&QModbusReply::finished,this,&MyModbus::readyRead);
    }
}

void MyModbus::on_switch1(bool checked)
{
    int v = checked;

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0000, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch2(bool checked)
{
    int v = checked;

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0001, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch3(bool checked)
{
    int v = checked;

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0002, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch4(bool checked)
{
    int v = checked;

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0003, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch5(bool checked)
{
    int v = checked;
    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0004, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch6(bool checked)
{
    int v = checked;
    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0005, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);

}
void MyModbus::on_switch7(bool checked)
{
    int v = checked;
    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0006, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);
}
void MyModbus::on_switch8(bool checked)
{
    int v = checked;
    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0x0007, 1);
    QVector<quint16> values;
    values.append(v);
    dataUnit.setValues(values);
    modbusManager->sendWriteRequest(dataUnit, 0x02);
}

void MyModbus::on_pushButton_clicked()
{
    m_timer->start();
}

void MyModbus::on_timerTimeout()
{
    QModbusDataUnit data(QModbusDataUnit::InputRegisters,0x0000,2);
    QModbusReply *reply = modbusManager->sendReadRequest(data,0x01);

    if(reply == nullptr){
        qDebug() << "读取数据失败" << modbusManager->errorString();
    }
    if(!reply->isFinished()){
        connect(reply,&QModbusReply::finished,this,&MyModbus::readyRead);
    }

}

void MyModbus::on_pushButton_stop_clicked()
{
    m_timer->stop();
}

void MyModbus::on_btnToSecond_clicked() {
    emit goToCameraPage();
}

void MyModbus::on_btnToSet_clicked(){
    emit goToSetPage();
}

void MyModbus::on_btnToCan_clicked(){
    emit goToCan();
}
