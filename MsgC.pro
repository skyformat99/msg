QT += core
QT -= gui

CONFIG += c++11

TARGET = MsgC
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    msg.c \
HEADERS += \
    globalmacros.h \
    msg.h
