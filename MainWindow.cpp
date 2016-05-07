#include "MainWindow.hpp"
#include "ui_MainWindow.h"

#include <QDir>
#include <QChar>
#include <QString>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QByteArray>
#include <QStringList>
#include <QFileDialog>
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
    connect(&decorator, &NameDecoration::maxFolderChanged, ui->spinParentFolder, &QSpinBox::setMaximum);
    connect(ui->editSource, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);
    connect(ui->editDestination, &QLineEdit::textChanged, this, &MainWindow::updateDecoratorPath);
    connect(&decorator, &NameDecoration::decorationChanged, this, &MainWindow::updateNewFileNames);
    connect(ui->checkOverwrite, &QCheckBox::toggled, this, &MainWindow::updateNewFileNames);
    connect(ui->tableFiles, &QTableWidget::cellChanged, this, &MainWindow::updateNewFileNames);
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
        updateNewFileNames();
    }
}

void MainWindow::on_buttonClear_clicked()
{
    ui->editDestination->clear();
}

//
//  Destination logic
//

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

    // Early return if the table is empty
    if (ui->tableFiles->rowCount() == 0)
        return;

    // Retrieve the existing files in the destination folder, to be able to avoid overwriting
    QStringList existingfiles;
    if (!ui->checkOverwrite->isChecked())
        existingfiles = QDir(realDestination()).entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    // Set the first digit and the number of digits used in the name
    int num = 1;
    int numlength = 1 + log10(existingfiles.count() + ui->tableFiles->rowCount());

    // Create the names and fill the table
    for (int i = 0; i < ui->tableFiles->rowCount(); i++) {
        // Remove existing file name
        delete ui->tableFiles->takeItem(i, 1);

        // Process only selected files
        if (ui->tableFiles->item(i, 0)->checkState() == Qt::Unchecked)
            continue;

        // Retrieve the extension of the original file
        QString extension(QFileInfo(ui->tableFiles->item(i, 0)->text()).suffix());

        // Generate a new name until an unused one is found
        QString newfilename;
        do {
            newfilename = decorator.decoration() + QString("%1.%2").arg(num, numlength, 10, QChar('0')).arg(extension);
            num++;
        }
        while (existingfiles.contains(newfilename));

        // Create and set the new item
        QTableWidgetItem* item = new QTableWidgetItem(newfilename);
        ui->tableFiles->setItem(i, 1, item);
    }

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
// Name decoration
//

void MainWindow::updateDecoratorPath()
{
    decorator.setPath(realDestination());
}