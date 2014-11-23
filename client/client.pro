#-------------------------------------------------
#
# Project created by QtCreator 2014-11-18T16:53:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = client
CONFIG   += console
CONFIG   -= app_bundle
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TEMPLATE = app


SOURCES += main.cpp \
    ../tcpconnection.cpp \
    client.cpp

HEADERS += \
    ../tcpconnection.h \
    client.h
