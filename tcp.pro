#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T18:01:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = tcp
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


SOURCES += main.cpp \
    tcpconnection.cpp

HEADERS += \
    tcpconnection.h
