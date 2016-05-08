# Files Renamer., a program to sort and rename picture files
# Copyright (C) 2016 Martial Demolins AKA Folco

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

DISTFILES += \
    LICENSE
