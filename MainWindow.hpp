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
};

#endif // MAINWINDOW_HPP
