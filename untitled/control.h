#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QProcess>


class Control : public QObject
{
public:
    Control();
    ~Control();
public slots:
    void setGPIO(QString port, QString value);

};



#endif // CONTROL_H
