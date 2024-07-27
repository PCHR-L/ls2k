#include "my_switch.h"
#include <QMouseEvent>

CustomSwitch::CustomSwitch(QWidget *parent) : QPushButton(parent), isChecked(false)
{
    // 设置初始样式
    setStyleSheet("QPushButton { "
                  "  border: 1px solid black; "
                  "  border-radius: 10px; "
                  "  background-color: red; }");
    // 调用函数设置初始大小
    setInitialSize(50, 20); // 宽度为100px，高度为50px
}

void CustomSwitch::setInitialSize(int width, int height)
{
    setFixedSize(width, height);
}

void CustomSwitch::mousePressEvent(QMouseEvent *event)
{
    // 切换状态
    isChecked = !isChecked;
    // 根据状态更新样式
    setStyleSheet(isChecked ? "QPushButton { background-color: green; }" : "QPushButton { background-color: red; }");
    // 发出信号
    emit turn(isChecked);
    // 调用基类的事件处理
    QPushButton::mousePressEvent(event);
}
