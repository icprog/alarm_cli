#-------------------------------------------------
#
# Project created by QtCreator 2015-11-06T09:25:37
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = alarm_cli
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    alarmclient.cpp

HEADERS  += mainwindow.h \
    alarmclient.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

#add ico to windows application
RC_FILE = myapp.rc
