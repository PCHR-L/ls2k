#include "mainwindow.h"
#include "mymodbus.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),mytimer(new QTimer(this))
{

    mqtt_client_t *client = NULL;
    mqtt_log_init();        //应该用于初始化MQTT客户端的日志系统
    client = mqtt_lease();  //申请一个MQTT客户端
    mqtt_set_port(client, const_cast<char*>("1883"));                           //设置要连接的MQTT服务器端口号
    mqtt_set_host(client, const_cast<char*>("aanhlrj.iot.gz.baidubce.com"));    //设置要连接的MQsuTT服务器地址，参数：域名 / 点分十进制的IP地址
    mqtt_set_client_id(client,const_cast<char*>("device11"));                       //设置客户端的ID
    mqtt_set_user_name(client, const_cast<char*>("thingidp@aanhlrj|device11|0|MD5"));                   //设置客户端的用户名
    mqtt_set_password(client, const_cast<char*>("f0e0d17c95f4c26c5070af0ae3cdb5cc"));           //设置客户端的密码
    mqtt_set_clean_session(client, 1);                                          //设置在断开连接后清除会话
    if(!mqtt_connect(client))
        qDebug() << "连接服务器成功";                                                       //与服务器建立连接，有返回值
    if(!mqtt_subscribe(client, "fortest", QOS0, topic1_handler))
        qDebug() << "主题订阅成功";                       //订阅主题，参数：主题名字、服务质量、指定当收到主题数据时的处理函数。

    stackedWidget = new QStackedWidget(this);
    mymodbus = new MyModbus(this);
    mycamera = new Camera(this);
    thw = new TemperatureHumidityMonitor(mymodbus,this);
    datasend = new DataSender(this,mymodbus,client,"fortest");
    datasend->setParameters(mymodbus->temperature,mymodbus->humidity);
    stackedWidget->addWidget(mymodbus);
    stackedWidget->addWidget(mycamera);
    stackedWidget->addWidget(thw);
    stackedWidget->setCurrentIndex(0);
    setCentralWidget(stackedWidget); // 将stackedWidget设置为中央部件
    mytimer->start(10000);

    connect(mymodbus, &MyModbus::goToCameraPage, this, &MainWindow::handleGoToCameraPage);
    connect(mymodbus,&MyModbus::goToSetPage,this,&MainWindow::handleGoToSetpage);
    connect(mytimer,&QTimer::timeout,datasend,&DataSender::mqttPublishJson);
    connect(mycamera, &Camera::returnFirstPage, this, &MainWindow::handleReturnFirstPage);
    connect(thw, &TemperatureHumidityMonitor::returnFirstPage, this, &MainWindow::handleReturnFirstPage);

}

MainWindow::~MainWindow()
{
}

void MainWindow::handleGoToCameraPage() {
    stackedWidget->setCurrentIndex(1); //  Camera 页面
}

void MainWindow::handleReturnFirstPage(){
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::handleGoToSetpage(){
    stackedWidget->setCurrentIndex(2);
}


void MainWindow::topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}
