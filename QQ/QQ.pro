#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T21:39:25
#
#-------------------------------------------------

QT       += core gui

##########################
QT += network
###########################

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QQ
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    menupushbutton.cpp

HEADERS  += widget.h \
    tcpserver.h \
    tcpclient.h \
    menubutton.h

FORMS    += widget.ui \
    tcpserver.ui \
    tcpclient.ui

RESOURCES += \
    images.qrc

RC_FILE += myico.rc //添加图标
