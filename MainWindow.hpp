// Files Renamer., a program to sort and rename picture files
// Copyright (C) 2016 Martial Demolins AKA Folco

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "NameDecoration.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonSource_clicked();
    void on_buttonDestination_clicked();
    void on_buttonSelectAll_clicked();
    void on_buttonUnselectAll_clicked();
    void on_checkDestination_stateChanged(int state);
    void on_buttonRename_clicked();
    void on_buttonDetectAssociated_clicked();

private:
    Ui::MainWindow *ui;
    NameDecoration decorator;

    QString realDestination();
    void updateCurrentFiles();
    void updateNewFileNames();
    void updateDecoratorPath();
    void openPreview(int row, int);
    void updateAutoDetectButton();
};

#endif // MAINWINDOW_HPP
