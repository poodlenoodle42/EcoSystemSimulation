#-------------------------------------------------
#
# Project created by QtCreator 2020-04-06T14:07:01
#
#-------------------------------------------------

QT       += core gui charts
CONFIG += c++17
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EcoSystemSimulation
TEMPLATE = app


SOURCES += main.cpp\
    simulationview.cpp \
        simulationwindow.cpp \
    vector2.cpp \
    entity.cpp \
    animal.cpp \
    rabbit.cpp \
    plant.cpp \
    fox.cpp

HEADERS  += simulationwindow.h \
    simulationview.h \
    vector2.h \
    entity.h \
    animal.h \
    rabbit.h \
    plant.h \
    fox.h \
    reproducefunction.h

FORMS    += simulationwindow.ui
