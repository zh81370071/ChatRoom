#-------------------------------------------------
#
# Project created by QtCreator 2021-02-23T14:28:27
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network
QT       += core gui sql



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CCprogram
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwidget.cpp \
    register.cpp \
    widget.cpp \
    databaselink.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    historywidget.cpp

HEADERS += \
        mainwidget.h \
    register.h \
    widget.h \
    databaselink.h \
    tcpserver.h \
    tcpclient.h \
    historywidget.h



RESOURCES += \
    res.qrc

FORMS += \
    register.ui\
   mainwidget.ui \
    widget.ui \
    tcpserver.ui \
    tcpclient.ui \
    historywidget.ui


