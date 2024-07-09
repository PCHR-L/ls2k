#include "tempHumMonitor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDoubleValidator>
#include "mymodbus.h"
#include <QTimer>

TemperatureHumidityMonitor::TemperatureHumidityMonitor(MyModbus *modbus,QWidget *parent)
    :QWidget(parent)
{
    // 初始化UI和变量
    initUI(modbus);
}

TemperatureHumidityMonitor::~TemperatureHumidityMonitor()
{

}

void TemperatureHumidityMonitor::initUI(MyModbus *modbus)
{

    QLabel *labelTemperatureMax = new QLabel("温度最大值：", this);
    QLabel *labelTemperatureMin = new QLabel("温度最小值：", this);
    QLabel *labelHumidityMax = new QLabel("湿度最大值：", this);
    QLabel *labelHumidityMin = new QLabel("湿度最小值：", this);

    editTemperatureMaxThreshold = new QLineEdit(this);
    editTemperatureMaxThreshold->resize(120,60);
    editTemperatureMinThreshold = new QLineEdit(this);
    editTemperatureMinThreshold->resize(120,60);
    editHumidityMaxThreshold = new QLineEdit(this);
    editHumidityMaxThreshold->resize(120,60);
    editHumidityMinThreshold = new QLineEdit(this);
    editHumidityMinThreshold->resize(120,60);

    QFormLayout *thresholdLayout = new QFormLayout;
    thresholdLayout->addRow(labelTemperatureMax, editTemperatureMaxThreshold);
    thresholdLayout->addRow(labelTemperatureMin, editTemperatureMinThreshold);
    thresholdLayout->addRow(labelHumidityMax, editHumidityMaxThreshold);
    thresholdLayout->addRow(labelHumidityMin, editHumidityMinThreshold);

    labelTemperature = new QLabel("温度: --°C", this);
    labelTemperature->resize(120,120);
    labelHumidity = new QLabel("湿度: --%", this);
    labelHumidity->resize(120,120);

    // 水平布局来排列按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    btnSetThresholds = new QPushButton("设置报警阈值", this);
    btnControlExternalModule = new QPushButton("关闭报警", this);
    returnFirst = new QPushButton("返回",this);
    buttonLayout->addStretch(1); // 在按钮之间添加空间
    buttonLayout->addWidget(btnSetThresholds);
    buttonLayout->addWidget(btnControlExternalModule);
    buttonLayout->addStretch(1); // 在按钮之间添加空间
    buttonLayout->addWidget(returnFirst);
    buttonLayout->addStretch(1);

    // 使用垂直布局来组合所有控件
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(labelTemperature);
    mainLayout->addWidget(labelHumidity);
    mainLayout->addStretch(1); // 在控件之间添加空间
    mainLayout->addLayout(thresholdLayout);
    mainLayout->addLayout(buttonLayout); // 添加按钮布局
    mainLayout->addStretch(1); // 在底部添加空间
    setLayout(mainLayout); // 应用主布局

    // 设置输入验证器
    QDoubleValidator *validator = new QDoubleValidator(this);
    editTemperatureMaxThreshold->setValidator(validator);
    editTemperatureMinThreshold->setValidator(validator);
    editHumidityMaxThreshold->setValidator(validator);
    editHumidityMinThreshold->setValidator(validator);

    // 连接信号和槽
    connect(btnSetThresholds, &QPushButton::clicked, this, &TemperatureHumidityMonitor::onSetThresholdsClicked);
    connect(btnControlExternalModule, &QPushButton::clicked, this, &TemperatureHumidityMonitor::onControlExternalModuleClicked);
    connect(returnFirst,&QPushButton::clicked,this,&TemperatureHumidityMonitor::on_btnGoBack_clicked);
    connect(modbus, &MyModbus::temperatureHumidityUpdated,this, &TemperatureHumidityMonitor::onTemperatureHumidityUpdated);

}

void TemperatureHumidityMonitor::onTemperatureHumidityUpdated(qreal temperature, qreal humidity)
{
    labelTemperature->setText(QString("温度: %1°C").arg(temperature));
    labelHumidity->setText(QString("湿度: %1%").arg(humidity));
    qDebug() << "准备比较温湿度阈值";
    // 检查温度是否超出最大值或最小值
    if (temperature > temperatureMaxThreshold || temperature < temperatureMinThreshold) {
        // 触发湿度报警
        alarm();
        alert("Warning: 温度超出设定范围！");
    }
    // 检查湿度是否超出最大值或最小值
    if (humidity > humidityMaxThreshold || humidity < humidityMinThreshold) {
        // 触发湿度报警
        alarm();
        alert("Warning: 湿度超出设定范围！");
    }
}

void TemperatureHumidityMonitor::onSetThresholdsClicked()
{
    bool ok;
    temperatureMaxThreshold = editTemperatureMaxThreshold->text().toDouble(&ok);
    temperatureMinThreshold = editTemperatureMinThreshold->text().toDouble(&ok);
    humidityMaxThreshold = editHumidityMaxThreshold->text().toDouble(&ok);
    humidityMinThreshold = editHumidityMinThreshold->text().toDouble(&ok);
    // 确保设置的阈值是合理的
    if (temperatureMaxThreshold <= temperatureMinThreshold ||
            humidityMaxThreshold <= humidityMinThreshold) {
        alert("阈值设置不合理，请确保最大值大于最小值。");
        return;
    }
    // 阈值设置成功
    alert("阈值设置成功。");
}

void TemperatureHumidityMonitor::onControlExternalModuleClicked()
{
    no_alarm();
}

void TemperatureHumidityMonitor::alert(const QString &message) {
    // 简单的报警函数，弹出消息框
    QMessageBox::warning(this, "警告", message);
}

void TemperatureHumidityMonitor::alarm()
{
    QProcess *process = new QProcess(this);
    process->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "57" <<"0");
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(process->state() == QProcess::Running) {
        process->terminate();  // 终止子进程执行
        process->waitForFinished();  // 等待子进程完成
        if (process->state() == QProcess::Running) {
            process->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    process->deleteLater();
}
void TemperatureHumidityMonitor::no_alarm()
{
    // 使用QProcess执行外部程序
    QProcess *process = new QProcess(this);
    process->start("/home/pchrl/myLoongson/gpio_out", QStringList() << "57" <<"1");
    if (!process->waitForStarted()) {
        alert("关闭报警失败");
    } else {
        alert("已关闭报警");
    }
    // 在销毁 QProcess 对象之前，等待子进程完成
    if(process->state() == QProcess::Running) {
        process->terminate();  // 终止子进程执行
        process->waitForFinished();  // 等待子进程完成
        if (process->state() == QProcess::Running) {
            process->kill();  // 如果进程没有响应 terminate，使用 kill
        }
    }
    process->deleteLater();
}
void TemperatureHumidityMonitor::on_btnGoBack_clicked()
{
    emit returnFirstPage();
}

