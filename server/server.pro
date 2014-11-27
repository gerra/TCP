#-------------------------------------------------
#
# Project created by QtCreator 2014-11-18T16:53:24
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


SOURCES += main.cpp \
    ../tcpconnection.cpp \
    server.cpp \
    ../epollhandler.cpp \
    ../tcpsocket.cpp \
    ../tcpexception.cpp

HEADERS += \
    ../tcpconnection.h \
    server.h \
    ../epollhandler.h \
    ../tcpsocket.h \
    ../tcpexception.h
