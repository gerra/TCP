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

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


SOURCES += main.cpp \
    ../tcpconnection.cpp \
    server.cpp

HEADERS += \
    ../tcpconnection.h \
    server.h
