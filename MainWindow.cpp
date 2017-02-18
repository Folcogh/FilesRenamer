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

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QDir>
#include <QFile>
#include <QChar>
#include <QLabel>
#include <QString>
#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QDateTime>
#include <QFileInfo>
#include <QByteArray>
#include <QVBoxLayout>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QTableWidget>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    adjustSize();

    decorator.setFolderNum(ui->spinParentFolder->value());
    decorator.setPrefix(ui->editPrefix->text());
    decorator.setSeparator(ui->editSeparator->text());

    // Connections which make the decoration to change
    connect(ui->editPrefix, &QLineEdit::textChanged, &decorator, &NameDecoration::setPrefix);
    connect(ui->editSeparator, &QLineEdit::textChanged, &decorator, &NameDecoration::setSeparator);
    connect(ui->spinParentFolder, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &decorator, &NameDecoration::setFolderNum);
    connect(ui->checkOverwrite, &QCheckBox::toggled, this, &MainWindow::updateNewFileNames);

    // Source/destination definitions
    connect(ui->editSource, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);
    connect(ui->editDestination, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);

    // Table connections to handle box checking, preview and selection
    connect(ui->tableFiles, &QTableWidget::cellChanged, this, &MainWindow::updateNewFileNames);
    connect(ui->tableFiles, &QTableWidget::cellDoubleClicked, this, &MainWindow::openPreview);
    connect(ui->tableFiles, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateAutoDetectButton);

    // Signals thrown by the decorator when the ui must be updated
    connect(&decorator, &NameDecoration::decorationChanged, this, &MainWindow::updateNewFileNames);
    connect(&decorator, &NameDecoration::maxFolderChanged, ui->spinParentFolder, &QSpinBox::setMaximum);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
//  Source and destination
//

// Source definition. It can't be redefined to a null path after its initial definition
void MainWindow::on_buttonSource_clicked()
{
    // Set the initial folder of the dialog
    QString initfolder;
    if (ui->editSource->text().isEmpty())
        initfolder = QDir::homePath();
    else
        initfolder = ui->editSource->text();

    QString srcfolder = QFileDialog::getExistingDirectory(nullptr, tr("Source folder"), initfolder);
    if (!srcfolder.isEmpty() && (srcfolder != ui->editSource->text())) {
        ui->editSource->setText(srcfolder);
        updateCurrentFiles();
    }
}

// Destination definition. It can be reset to nothing if its checkbox is unchecked
void MainWindow::on_buttonDestination_clicked()
{
    // Set the initial folder of the dialog
    QString initfolder;
    if (ui->editDestination->text().isEmpty())
        initfolder = QDir::homePath();
    else
        initfolder = ui->editDestination->text();

    QString destfolder = QFileDialog::getExistingDirectory(nullptr, tr("Destination folder"), initfolder);
    if (!destfolder.isEmpty() && (destfolder != ui->editDestination->text())) {
        ui->editDestination->setText(destfolder);
        ui->checkDestination->setCheckState(Qt::Checked);
        updateNewFileNames();
    }
}

// The destination checkbox is used to clear the destination if one has been defined.
// If the field is empty, the checkbox is unchecked and have no effect
void MainWindow::on_checkDestination_stateChanged(int state)
{
    ui->editDestination->blockSignals(true);
    ui->checkDestination->blockSignals(true);

    if (state == Qt::Unchecked)
        ui->editDestination->clear();
    else if (ui->editDestination->text().isEmpty())
        ui->checkDestination->setCheckState(Qt::Unchecked);

    ui->checkDestination->blockSignals(false);
    ui->editDestination->blockSignals(false);
}

// The destination is the source folder if no destination is specified
QString MainWindow::realDestination()
{
    QString destination;
    if (ui->editDestination->text().isEmpty())
        destination = ui->editSource->text();
    else
        destination = ui->editDestination->text();

    return destination;
}

//
//  File list
//

// Called when the source or the destination path have changed
// The decorator will decide if the decoration pattern has really changed
void MainWindow::updateDecoratorPath()
{
    decorator.setPath(realDestination());
}

// Clear and recreate the source file list
void MainWindow::updateCurrentFiles()
{
    // Disable the "cellChanged" signal
    ui->tableFiles->blockSignals(true);

    // Build the extension filter
    QStringList filters;
    QList<QByteArray> extlist = QImageReader::supportedImageFormats();
    foreach (QByteArray ext, extlist)
        filters << "*." + ext;

    // Retrieve files
    QStringList files = QDir(ui->editSource->text()).entryList(filters, QDir::Files, QDir::Name);

    // Add an item for each file
    ui->tableFiles->clearContents();
    ui->tableFiles->setRowCount(0);
    QTableWidgetItem* item;
    for (int i = 0; i < files.count(); i ++) {
        item = new QTableWidgetItem(files.at(i));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Checked);
        ui->tableFiles->setRowCount(ui->tableFiles->rowCount() + 1);
        ui->tableFiles->setItem(i, 0, item);
    }

    ui->tableFiles->resizeColumnToContents(0);
    ui->tableFiles->blockSignals(false);
    updateNewFileNames();
}

// Clear and recreate the destination file list
// It can be triggered by most of the ui changes
void MainWindow::updateNewFileNames()
{
    // Disable the "cellChanged" signal
    ui->tableFiles->blockSignals(true);

    // Retrieve the existing files in the destination folder, to be able to avoid overwriting
    QStringList existingfiles;
    if (!ui->checkOverwrite->isChecked())
        existingfiles = QDir(realDestination()).entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    // Set the first digit and the number of digits used in the name
    int num = 1;
    int numlength = 1 + log10(existingfiles.count() + ui->tableFiles->rowCount());

    // Create the names and fill the table. checkedcount is used to enable/disable the buttons under the table
    int checkedcount = 0;

    for (int i = 0; i < ui->tableFiles->rowCount(); i++) {
        // Remove existing file name
        delete ui->tableFiles->takeItem(i, 1);

        // Process only selected files
        if (ui->tableFiles->item(i, 0)->checkState() == Qt::Unchecked)
            continue;

        // Generate a new name until an unused one is found
        QString extension(QFileInfo(ui->tableFiles->item(i, 0)->text()).suffix());
        QString newfilename;
        do {
            newfilename = decorator.decoration() + QString("%1.%2").arg(num, numlength, 10, QChar('0')).arg(extension);
            num++;
        } while (existingfiles.contains(newfilename));
        checkedcount++;

        // Create and set the new item
        QTableWidgetItem* item = new QTableWidgetItem(newfilename);
        ui->tableFiles->setItem(i, 1, item);
    }

    // Update buttons
    if (checkedcount == 0) {
        ui->buttonUnselectAll->setDisabled(true);
        ui->buttonRename->setDisabled(true);
    }
    else {
        ui->buttonUnselectAll->setEnabled(true);
        ui->buttonRename->setEnabled(true);
    }

    if (checkedcount == ui->tableFiles->rowCount())
        ui->buttonSelectAll->setDisabled(true);
    else
        ui->buttonSelectAll->setEnabled(true);

    updateAutoDetectButton();

    // Finnaly, re-enable the signals
    ui->tableFiles->blockSignals(false);
}

//
//  File selection
//

void MainWindow::on_buttonSelectAll_clicked()
{
    for (int i = 0; i < ui->tableFiles->rowCount(); i++)
        ui->tableFiles->item(i, 0)->setCheckState(Qt::Checked);
}

void MainWindow::on_buttonUnselectAll_clicked()
{
    for (int i = 0; i < ui->tableFiles->rowCount(); i++)
        ui->tableFiles->item(i, 0)->setCheckState(Qt::Unchecked);
}

void MainWindow::updateAutoDetectButton()
{
    if (ui->tableFiles->selectedItems().size() == 0)
        ui->buttonDetectAssociated->setDisabled(true);
    else
        ui->buttonDetectAssociated->setEnabled(true);
}

// The associated files are the ones created 10 seconds before or after the currently selected file
void MainWindow::on_buttonDetectAssociated_clicked()
{
    // Get the creation date of the selected file
    QList<QTableWidgetItem*> lst = ui->tableFiles->selectedItems();
    QTableWidgetItem* item = (lst.at(0)->column() == 0 ? lst.at(0) : lst.at(1));
    QString filename = QString("%1/%2").arg(ui->editSource->text()).arg(item->text());
    QDateTime refdate = QFileInfo(filename).created();

    // Select closely created files
    for (int i = 0; i < ui->tableFiles->rowCount(); i++) {
        QString filename = QString("%1/%2").arg(ui->editSource->text()).arg(ui->tableFiles->item(i, 0)->text());
        QDateTime date = QFileInfo(filename).created();
        if ((refdate.secsTo(date) < 10) && (refdate.secsTo(date) > -10))
            ui->tableFiles->item(i, 0)->setCheckState(Qt::Checked);
    }
}

//
// Renaming
//

// 1. Delete the destination file if one exists
// 2. Copy and rename the selected file
// 3. Delete the source file if requested
void MainWindow::on_buttonRename_clicked()
{
    for (int i = 0; i < ui->tableFiles->rowCount(); i++) {
        if (ui->tableFiles->item(i, 0)->checkState() == Qt::Unchecked)
            continue;

        QFile srcfile(QString("%1/%2").arg(ui->editSource->text()).arg(ui->tableFiles->item(i, 0)->text()));
        QFile destfile(QString("%1/%2").arg(realDestination()).arg(ui->tableFiles->item(i, 1)->text()));

        // Avoid to delete the file if the source and the destination files are identical
        if (srcfile.fileName() == destfile.fileName())
            continue;

        if (destfile.exists())
            destfile.remove();

        if (!srcfile.copy(destfile.fileName())) {
            if (QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("Error while renaming the file %1. Do you want to continue the renaming process ?").arg(srcfile.fileName()),
                                      QMessageBox::Yes,
                                      QMessageBox::Abort
                                      ) == QMessageBox::Abort)
                break;
            continue;
        }

        if (ui->checkDeleteSource->isChecked())
            srcfile.remove();
    }

    // Will remove the files which have been moved
    updateCurrentFiles();
}

//
//  Preview
//

// Open a maximized window containing the image of the double-clicked file
void MainWindow::openPreview(int row, int)
{
    QWidget* win = new QWidget;
    new QVBoxLayout(win);
    QLabel* preview = new QLabel;

    preview->setPixmap(QPixmap(QString("%1/%2").arg(ui->editSource->text()).arg(ui->tableFiles->item(row, 0)->text())));
    preview->setScaledContents(true);
    win->layout()->addWidget(preview);
    win->showMaximized();
    delete win;
}
