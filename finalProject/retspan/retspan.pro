#-------------------------------------------------
#
# Project created by QtCreator 2013-11-20T18:18:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = retspan
TEMPLATE = app
QT += network
CONFIG += crypto
CONFIG -= x86_64


SOURCES += main.cpp\
        mainwindow.cpp \
    chordnode.cpp \
    networkmanager.cpp \
    node.cpp \
    util.cpp \
    manager.cpp \
    fingerentry.cpp \
    filemanager.cpp \
    file.cpp \
    keyvaluestore.cpp

HEADERS  += mainwindow.h \
    chordnode.h \
    networkmanager.h \
    node.h \
    util.h \
    manager.h \
    filemanager.h \
    file.h \
    keyvaluestore.h

FORMS    += mainwindow.ui
