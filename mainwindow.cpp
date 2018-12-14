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
    QStringList tmp;
    QString in_fname, filename, filename_new;
    QString run_num, succ_num, fail_num, fail_idx;
    QString output_str;

    // Invoke 'Open File' dialog.
    in_fname = QFileDialog::getOpenFileName(
                        this,
                        tr("Open File"),
                        "./",
                        "All files (*.*)"
                       );
    if(in_fname.isEmpty()){
        return;
    }

    // Acquire file name while set new file name.
    tmp = in_fname.split("/"); // In Linux
    filename = tmp.at(tmp.length()-1);
    tmp = filename.split(".");
    filename_new = tmp.at(0) + "_new." + tmp.at(1);

    // Check validability of selected file.
    if(tmp.at(1) != "csv"){
        QMessageBox::information(this, tr("Failed"), "Please choose a CSV file!");
        return;
    }
    QFile file(in_fname);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this, tr("Failed"), "Failed to open selected file!");
        return;
    }

    // Inverse matrices.

    // Set ui display.
    output_str = "Selected CSV File:  " + filename;
    ui->file_name->setText(output_str);
    output_str = "Output CSV File:  " + filename_new;
    ui->new_file->setText(output_str);
}
