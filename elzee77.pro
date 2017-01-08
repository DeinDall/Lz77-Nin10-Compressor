CONFIG += c++11

QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = lz77

SOURCES += \
    main.cpp \
    stk/lz77.cpp \
    qstdio.cpp

HEADERS += \
    stk/lz77.h \
    qstdio.h
