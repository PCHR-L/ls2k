#ifndef MY_SWITCH_H
#define MY_SWITCH_H


#include <QPushButton>

class CustomSwitch : public QPushButton
{
    Q_OBJECT

public:
    explicit CustomSwitch(QWidget *parent = nullptr);
    void setInitialSize(int width, int height);

protected:
    void mousePressEvent(QMouseEvent *event) override; // 重写鼠标按下事件

signals:
    void turn(bool checked);

private:
    bool isChecked = false; // 跟踪开关状态
};

#endif
