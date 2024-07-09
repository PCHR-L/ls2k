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
#include <control.h>


MyModbus::MyModbus(QWidget *parent) :QWidget(parent)
{
    QPushButton *btnDevice1_open = new QPushButton("开",this);
    QPushButton *btnDevice1_close = new QPushButton("关",this);
    QPushButton *btnDevice2_open = new QPushButton("开",this);
    QPushButton *btnDevice2_close = new QPushButton("关",this);
    QPushButton *btnDevice3_open = new QPushButton("开",this);
    QPushButton *btnDevice3_close = new QPushButton("关",this);
    QPushButton *btnDevice4_open = new QPushButton("开",this);
    QPushButton *btnDevice4_close = new QPushButton("关",this);

    QLabel *deviceQLabel1 = new QLabel("设备1:",this);
    QLabel *deviceQLabel2 = new QLabel("设备2:",this);
    QLabel *deviceQLabel3 = new QLabel("设备3:",this);
    QLabel *deviceQLabel4 = new QLabel("设备4:",this);

    QVBoxLayout *deviceLayout1 = new QVBoxLayout(this);
    deviceLayout1->addWidget(deviceQLabel1);
    deviceLayout1->addWidget(btnDevice1_open);
    deviceLayout1->addWidget(btnDevice1_close);

    QVBoxLayout *deviceLayout2 = new QVBoxLayout(this);
    deviceLayout2->addWidget(deviceQLabel2);
    deviceLayout2->addWidget(btnDevice2_open);
    deviceLayout2->addWidget(btnDevice2_close);

    QVBoxLayout *deviceLayout3 = new QVBoxLayout(this);
    deviceLayout3->addWidget(deviceQLabel3);
    deviceLayout3->addWidget(btnDevice3_open);
    deviceLayout3->addWidget(btnDevice3_close);


    QVBoxLayout *deviceLayout4 = new QVBoxLayout(this);
    deviceLayout4->addWidget(deviceQLabel4);
    deviceLayout4->addWidget(btnDevice4_open);
    deviceLayout4->addWidget(btnDevice4_close);

    btnRead = new QPushButton("读取一次",this);
    pushButton = new QPushButton("读取",this);
    pushButton_stop = new QPushButton("停止读取",this);
    btnRead->resize(180,60);
    pushButton->resize(180,60);
    pushButton_stop->resize(180,60);
    QVBoxLayout *gLayout = new QVBoxLayout(this);
    gLayout->addWidget(btnRead);
    gLayout->addWidget(pushButton);
    gLayout->addWidget(pushButton_stop);

    btnToset = new QPushButton("设置报警阈值",this);
    btnToSecond = new QPushButton("视频监控",this);
    btnToset->resize(180,60);
    btnToSecond->resize(180,60);
    QVBoxLayout *fLayout = new QVBoxLayout(this);
    fLayout->addWidget(btnToset);
    fLayout->addWidget(btnToSecond);

    tempLineEdit = new QLineEdit(this);
    humLineEdit = new QLineEdit(this);
    temp = new QLabel("温度：",this);
    hum = new QLabel("湿度：",this);

    m_axisX = new QDateTimeAxis();
    m_axisY = new QValueAxis();
    m_axisX->setFormat("HH:mm:ss"); // 设置x轴的日期时间格式
    m_axisX->setTitleText("时间");
    m_axisY->setRange(-10, AXIS_MAX_Y); // 确保使用setRange
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

    QHBoxLayout *tempLayout = new QHBoxLayout;
    tempLayout->addWidget(temp);
    tempLayout->addWidget(tempLineEdit);
    QHBoxLayout *humLayout = new QHBoxLayout;
    humLayout->addWidget(hum);
    humLayout->addWidget(humLineEdit);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addLayout(deviceLayout1,0,0);
    gridLayout->addLayout(deviceLayout2,0,1);
    gridLayout->addLayout(deviceLayout3,0,2);
    gridLayout->addLayout(deviceLayout4,0,3);
    gridLayout->addWidget(chartView, 1, 0, 2, 3);
    gridLayout->addLayout(gLayout,2,0);
    gridLayout->addLayout(fLayout,3,0);


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

    connect(btnDevice1_open,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice1_open);
    connect(btnDevice1_close,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice1_close);
    connect(btnDevice2_open,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice2_open);
    connect(btnDevice2_close,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice2_close);
    connect(btnDevice3_open,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice3_open);
    connect(btnDevice3_close,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice3_close);
    connect(btnDevice4_open,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice4_open);
    connect(btnDevice4_close,&QPushButton::clicked,this,&MyModbus::on_btnControlDevice4_close);

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
        tempLineEdit->setText(temperature_str);
        humLineEdit->setText(humidity_str);

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
    // 0x0001是要读取数据的起始寄存器起始地址（不是modbus设备地址）
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

void MyModbus::on_btnControlDevice1_open()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "53" << "1");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice1_close()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "53" << "0");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice2_open()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "54" << "1");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice2_close()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "54" << "0");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice3_open()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "55" << "1");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice3_close()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "55" << "0");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice4_open()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "56" << "1");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
void MyModbus::on_btnControlDevice4_close()
{
    QProcess *mp = new QProcess(this);
    mp->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "56" << "0");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(mp->state() == QProcess::Running) {
        mp->terminate();  // 终止子进程执行
        mp->waitForFinished();  // 等待子进程完成
        if (mp->state() == QProcess::Running) {
            mp->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    mp->deleteLater();
}
