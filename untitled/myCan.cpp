#include "myCan.h"
#include <QGuiApplication>
#include <QScreen>

MyCan::MyCan(QWidget *parent)
    :QWidget(parent)
{
    /* 使用系统指令比特率初始化 CAN,默认为 1000000bits/s */
    system("ifconfig can0 down");
    system("ip link set up can0 type can bitrate 1000000 restart-ms 100");

    layoutInit();       //布局初始化
    pluginItemInit();   //可用插件初始化
    pluginChanged(comboBox[0]->currentIndex()); //可用接口项初始化
    bitrateItemInit();  //比特率项初始化

}

MyCan::~MyCan()
{
}

static QString frameFlags(const QCanBusFrame &frame)
{
    /* 格式化接收到的消息 */
    QString result = QLatin1String(" --- ");
    if (frame.hasBitrateSwitch())
        result[1] = QLatin1Char('B');
    if (frame.hasErrorStateIndicator())
        result[2] = QLatin1Char('E');
    if (frame.hasLocalEcho())
        result[3] = QLatin1Char('L');
    return result;
}

void MyCan::sendFrame()
{
    if(!canDevice)
        return;
    QString str = lineEdit->text();
    QByteArray data = 0;
    QString strTemp = nullptr;
    /* 以空格分隔 lineEdit 的内容，并存储到字符串链表中 */
    QStringList strlist = str.split(' ');
    for (int i = 1; i < strlist.count(); i++) {
        strTemp = strTemp + strlist[i];
    }
    /* 将字符串的内容转为 QByteArray 类型 */
    data = QByteArray::fromHex(strTemp.toLatin1());

    bool ok;
    /* 以 16 进制读取要发送的帧内容里第一个数据，并作为帧 ID */
    int framId = strlist[0].toInt(&ok, 16);
    QCanBusFrame frame = QCanBusFrame(framId, data);
    /* 写入帧 */
    canDevice->writeFrame(frame);
}

void MyCan::receivedFrames()
{
    if (!canDevice)
        return;
    /* 读取帧 */
    while (canDevice->framesAvailable()) {
        const QCanBusFrame frame = canDevice->readFrame();
        QString view;
        if (frame.frameType() == QCanBusFrame::ErrorFrame)
            view = canDevice->interpretErrorFrame(frame);
        else
            view = frame.toString();
        const QString time = QString::fromLatin1("%1.%2 ")
                .arg(frame.timeStamp().seconds(), 10, 10, QLatin1Char(' '))
                .arg(frame.timeStamp().microSeconds() / 100, 4, 10, QLatin1Char('0'));
        const QString flags = frameFlags(frame);
        textBrowser->insertPlainText(time + flags + view + "\n");
    }
}

void MyCan::layoutInit()
{
    /* 获取屏幕的分辨率，Qt 官方建议使用这
     * 种方法获取屏幕分辨率，防上多屏设备导致对应不上
     * 注意，这是获取整个桌面系统的分辨率
     */
    //QList <QScreen *> list_screen = QGuiApplication::screens();

    textBrowser = new QTextBrowser();
    lineEdit = new QLineEdit();
    vboxLayout = new QVBoxLayout();
    funcWidget = new QWidget();
    gridLayout = new QGridLayout();

    btnGoBack = new QPushButton();

    QList <QString> list1;
    list1<<"插件类型:"<<"可用接口:"<<"比特率 bits/sec:";

    for (int i = 0; i < 3; i++) {
        label[i] = new QLabel(list1[i]);
        /* 设置最小宽度与高度 */
        label[i]->setMinimumSize(120, 30);
        label[i]->setMaximumHeight(50);
        /* 自动调整 label 的大小 */
        label[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        /* 将 label[i]添加至网格的坐标(0, i) */
        gridLayout->addWidget(label[i], 0, i);
    }
    label[3] = new QLabel();
    label[3]->setMaximumHeight(30);

    for (int i = 0; i < 3; i++) {
        comboBox[i] = new QComboBox();
        comboBox[i]->setMinimumSize(120, 30);
        comboBox[i]->setMaximumHeight(50);
        /* 自动调整 label 的大小 */
        comboBox[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        /* 将 comboBox[i]添加至网格的坐标(1, i) */
        gridLayout->addWidget(comboBox[i], 1, i);
    }

    /* QList 链表，字符串类型 */
    QList <QString> list2;
    list2<<"发送"<<"连接 CAN";

    for (int i = 0; i < 2; i++) {
        pushButton[i] = new QPushButton(list2[i]);
        pushButton[i]->setMinimumSize(120, 30);
        pushButton[i]->setMaximumHeight(50);
        /* 自动调整 label 的大小 */
        pushButton[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        /* 将 pushButton[0]添加至网格的坐标(i, 3) */
        gridLayout->addWidget(pushButton[i], i, 3);
    }
    pushButton[0]->setEnabled(false);
    gridLayout->addWidget(btnGoBack, 2,4);
    vboxLayout->addWidget(textBrowser);
    vboxLayout->addWidget(lineEdit);
    funcWidget->setLayout(gridLayout);
    vboxLayout->addWidget(funcWidget);
    vboxLayout->addWidget(label[3]);
    setLayout(vboxLayout);

    /* 设置文本 */
    textBrowser->setPlaceholderText("系统时间 帧 ID 长度 数据");
    lineEdit->setText("123 aa 77 66 55 44 33 22 11");
    label[3]->setText(tr("未连接！"));

    connect(pushButton[1], SIGNAL(clicked()),this, SLOT(connectDevice()));
    connect(pushButton[0], SIGNAL(clicked()),this, SLOT(sendFrame()));
    connect(btnGoBack,&QPushButton::clicked,this,&MyCan::on_btnGoBack_clicked);
}

/* 从系统中读取可用的插件，并显示到 comboBox[0] */
void MyCan::pluginItemInit()
{
    comboBox[0]->addItems(QCanBus::instance()->plugins());
    for (int i = 0; i < QCanBus::instance()->plugins().count(); i++) {
        if (QCanBus::instance()->plugins().at(i) == "socketcan")
            comboBox[0]->setCurrentIndex(i);
    }

    connect(comboBox[0], SIGNAL(currentIndexChanged(int)),this, SLOT(pluginChanged(int)));
}

/* 插件类型改变 */
void MyCan::pluginChanged(int)
{
    QList<QCanBusDeviceInfo> interfaces;
    comboBox[1]->clear();
    /* 当我们改变插件时，我们同时需要将可用接口，从插件类型中读取出来 */
    interfaces = QCanBus::instance()->availableDevices(comboBox[0]->currentText());
    for (const QCanBusDeviceInfo &info : qAsConst(interfaces)) {
        comboBox[1]->addItem(info.name());
    }
}

/* 初始化一些常用的比特率，can 的比特率不是随便设置的，有相应的计算公式 */
void MyCan::bitrateItemInit()
{
    const QList<int> rates = {
        10000, 20000, 50000, 100000, 125000,
        250000, 500000, 800000, 1000000
    };

    for (int rate : rates)
        comboBox[2]->addItem(QString::number(rate), rate);
    /* 默认初始化以 1000000 比特率 */
    comboBox[2]->setCurrentIndex(8);
}

/* 连接或断开 CAN */
void MyCan::connectDevice()
{
    if (pushButton[1]->text() == "连接 CAN") {
        /* Qt 中的 QCanBusDevice::BitRateKey 不能设置比特率 */
        QString cmd1 = tr("ifconfig %1 down").arg(comboBox[1]->currentText());
        QString cmd2 = tr("ip link set up %1 type can bitrate %2 restart-ms 100")
                .arg(comboBox[1]->currentText())
                .arg(comboBox[2]->currentText());
        /* 使用系统指令以设置的比特率初始化 CAN */
        system(cmd1.toStdString().c_str());
        system(cmd2.toStdString().c_str());

        QString errorString;
        /* 以设置的插件名与接口实例化 canDevice */
        canDevice = QCanBus::instance()->createDevice(comboBox[0]->currentText(),comboBox[1]->currentText(),&errorString);
        if (!canDevice) {
            label[3]->setText(tr("Error creating device '%1', reason: '%2'").arg(comboBox[0]->currentText()).arg(errorString));
            return;
        }

        /* 连接 CAN */
        if (!canDevice->connectDevice()) {
            label[3]->setText(tr("Connection error: %1").arg(canDevice->errorString()));
            delete canDevice;
            canDevice = nullptr;
            return;
        }

        connect(canDevice, SIGNAL(framesReceived()),this, SLOT(receivedFrames()));
        connect(canDevice,SIGNAL(errorOccurred(QCanBusDevice::CanBusError)),this,SLOT(canDeviceErrors(QCanBusDevice::CanBusError)));
        /* 将连接信息插入到 label */
        label[3]->setText(tr("插件类型为: %1, 已连接到 %2, 比特率为 %3 kBit/s").arg(comboBox[0]->currentText()).arg(comboBox[1]->currentText())
                .arg(comboBox[2]->currentText().toInt() / 1000));
        pushButton[1]->setText("断开 CAN");
        /* 使能/失能 */
        pushButton[0]->setEnabled(true);
        comboBox[0]->setEnabled(false);
        comboBox[1]->setEnabled(false);
        comboBox[2]->setEnabled(false);
    }else{
        if (!canDevice)
            return;
        /* 断开连接 */
        canDevice->disconnectDevice();
        delete canDevice;
        canDevice = nullptr;
        pushButton[1]->setText("连接 CAN");
        pushButton[0]->setEnabled(false);
        label[3]->setText(tr("未连接！"));
        comboBox[0]->setEnabled(true);
        comboBox[1]->setEnabled(true);
        comboBox[2]->setEnabled(true);
    }
}

void MyCan::canDeviceErrors(QCanBusDevice::CanBusError error) const
{
    /* 错误处理 */
    switch (error) {
    case QCanBusDevice::ReadError:
    case QCanBusDevice::WriteError:
    case QCanBusDevice::ConnectionError:
    case QCanBusDevice::ConfigurationError:
    case QCanBusDevice::UnknownError:
        label[3]->setText(canDevice->errorString());
        break;
    default:
        break;
    }
}

void MyCan::on_btnGoBack_clicked()
{
    emit returnFirstPage();
}
