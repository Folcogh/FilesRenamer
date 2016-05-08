#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QDir>
#include <QFile>
#include <QChar>
#include <QString>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QByteArray>
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

    // Connections for some simple widgets
    connect(ui->editPrefix, &QLineEdit::textChanged, &decorator, &NameDecoration::setPrefix);
    connect(ui->editSeparator, &QLineEdit::textChanged, &decorator, &NameDecoration::setSeparator);
    connect(ui->spinParentFolder, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), &decorator, &NameDecoration::setFolderNum);
    connect(ui->editSource, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);
    connect(ui->editDestination, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);
    connect(ui->checkOverwrite, &QCheckBox::toggled, this, &MainWindow::updateNewFileNames);
    connect(ui->tableFiles, &QTableWidget::cellChanged, this, &MainWindow::updateNewFileNames);
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

void MainWindow::on_buttonSource_clicked()
{
    // Set the initial folder of the dialog
    QString initfolder;
    if (ui->editSource->text().isEmpty())
        initfolder = QDir::homePath();
    else
        initfolder = ui->editSource->text();

    QString srcfolder = QFileDialog::getExistingDirectory(nullptr, tr("Source folder"), initfolder, QFileDialog::DontUseNativeDialog);
    if (!srcfolder.isEmpty() && (srcfolder != ui->editSource->text())) {
        ui->editSource->setText(srcfolder);
        updateCurrentFiles();
    }
}

void MainWindow::on_buttonDestination_clicked()
{
    // Set the initial folder of the dialog
    QString initfolder;
    if (ui->editDestination->text().isEmpty())
        initfolder = QDir::homePath();
    else
        initfolder = ui->editDestination->text();

    QString destfolder = QFileDialog::getExistingDirectory(nullptr, tr("Destination folder"), initfolder, QFileDialog::DontUseNativeDialog);
    if (!destfolder.isEmpty() && (destfolder != ui->editDestination->text())) {
        ui->editDestination->setText(destfolder);
        ui->checkDestination->setCheckState(Qt::Checked);
        updateNewFileNames();
    }
}

void MainWindow::on_checkDestination_stateChanged(int state)
{
    if (state == Qt::Unchecked)
        ui->editDestination->clear();
    else if (ui->editDestination->text().isEmpty())
        ui->checkDestination->setCheckState(Qt::Unchecked);
}

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

void MainWindow::updateDecoratorPath()
{
    decorator.setPath(realDestination());
}

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

    ui->tableFiles->blockSignals(false);
    updateNewFileNames();
}

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

    if (checkedcount == 1)
        ui->buttonDetectAssociated->setEnabled(true);
    else
        ui->buttonDetectAssociated->setDisabled(true);

    if (checkedcount == ui->tableFiles->rowCount())
        ui->buttonSelectAll->setDisabled(true);
    else
        ui->buttonSelectAll->setEnabled(true);

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

//
// Rename
//

void MainWindow::on_buttonRename_clicked()
{
    for (int i = 0; i < ui->tableFiles->rowCount(); i++) {
        if (ui->tableFiles->item(i, 0)->checkState() == Qt::Unchecked)
            continue;

        QFile srcfile(QString("%1/%2").arg(ui->editSource->text()).arg(ui->tableFiles->item(i, 0)->text()));
        QFile destfile(QString("%1/%2").arg(realDestination()).arg(ui->tableFiles->item(i, 1)->text()));

        if (destfile.exists())
            destfile.remove();

        if (!srcfile.copy(destfile.fileName())) {
            if (QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("Error while renaming the file %1. Do you want to continue the renaming process ?").arg(srcfile.fileName()),
                                      QMessageBox::Yes,
                                      QMessageBox::Abort)
                    == QMessageBox::Abort)
                break;
            continue;
        }

        if (ui->checkDeleteSource->isChecked())
            srcfile.remove();
    }
    updateNewFileNames();
}
