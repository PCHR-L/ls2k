QT       += core gui serialbus serialport charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SaveImageThread.cpp \
    camera.cpp \
    camerathread.cpp \
    datasend.cpp \
    httpserver.cpp \
    main.cpp \
    mainwindow.cpp \
    myCan.cpp \
    my_switch.cpp \
    mymodbus.cpp \
    tempHumMonitor.cpp \
    v4l2_c-master/v4l2.c

HEADERS += \
    SaveImageThread.h \
    camera.h \
    camerathread.h \
    datasend.h \
    httpserver.h \
    mainwindow.h \
    myCan.h \
    my_switch.h \
    mymodbus.h \
    tempHumMonitor.h \
    v4l2_c-master/v4l2.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../../../../../usr/local/ls2k_opencv_include
DEPENDPATH += $$PWD/../../../../../usr/local/ls2k_opencv_include
LIBS += -L$$PWD/../../../../../usr/lib/ls2k_opencv_lib/ -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_videoio \
-lopencv_imgcodecs



unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/libmqttclient/lib/ -lmqttclient

INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/mqtt
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/common
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/common/log
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/config
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/mbedtls
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/mbedtls/wrapper
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/network
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/platform/linux
INCLUDEPATH += $$PWD/../../../../../usr/lib/libmqttclient/include/mqttclient
DEPENDPATH += $$PWD/../../../../../usr/lib/libmqttclient/include

DISTFILES += \
    index.html
