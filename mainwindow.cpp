#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString str;
    QStringList tmp;
    // Invoke 'Open File' dialog.
    QString filename = QFileDialog::getOpenFileName(
                        this,
                        tr("Open File"),
                        "./",
                        "All files (*.*)"
                       );
    if(filename.isEmpty()){
        return;
    }
    // Check validability of selected file.
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this, tr("Failed"), "Failed to open selected file!");
        return;
    }
    // Read CSV file.
    // Inverse matrices.
    // Write new CSV file.
    // Set ui display.
    tmp = filename.split("/");
    str = "Selected File:  " + tmp.at(tmp.length()-1);
    ui->file_name->setText(str);
}
