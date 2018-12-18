#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

#define COL 4
#define ROW 4

struct in_para{
    QString path;
    QString fname;
    QString f_newname;
    int run_num;
    int succ_num;
    int fail_num;
    QString fail_idx;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    int inverse(struct in_para *p);
    int run(double a[ROW][COL], double b[ROW][COL], double c[ROW][COL]);
    double det(double a[ROW][COL]);
    void adj(double a[ROW][COL], double b[ROW][COL]);
};

#endif // MAINWINDOW_H
