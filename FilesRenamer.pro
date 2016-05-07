#-------------------------------------------------
#
# Project created by QtCreator 2016-04-25T21:19:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FilesRenamer
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra
SOURCES += main.cpp\
        MainWindow.cpp \
    NameDecoration.cpp

HEADERS  += MainWindow.hpp \
    NameDecoration.hpp

FORMS    += MainWindow.ui
