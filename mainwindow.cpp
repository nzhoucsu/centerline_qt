#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    int i;
    QStringList tmp;
    QString in_fname, path, filename, filename_new;
    QString output_str;
    struct in_para in_struct, *p;

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

    // Acquire file name while set name of output file.
    i = in_fname.lastIndexOf("/"); // In Linux
    path = in_fname.mid(0, i);
    tmp = in_fname.split("/"); // In Linux
    filename = tmp.at(tmp.length()-1);
    tmp = filename.split(".");
    filename_new = tmp.at(0) + "_new." + tmp.at(1);

    // Check validability of selected file.
    if(tmp.at(1) != "csv"){
        QMessageBox::information(this, tr("Failed"), "Please choose a CSV file!");
        return;
    }

    // Inverse matrices.
    in_struct.path = path;
    in_struct.fname = filename;
    in_struct.f_newname = filename_new;
    in_struct.run_num = 0;
    in_struct.succ_num = 0;
    in_struct.fail_num = 0;
    in_struct.fail_idx = "";
    p = &in_struct;
    if(inverse(p) == -1){
        return;
    }
    // Set ui display.
    output_str = "Selected CSV File:  " + p->fname;
    ui->file_name->setText(output_str);
    ui->rslt_tag->setText("----- RESULT -----");
    output_str = "Record Num: " + QString::number(in_struct.run_num);
    ui->run_num->setText(output_str);
    output_str = "Success Num: " + QString::number(in_struct.succ_num);
    ui->succ_num->setText(output_str);
    output_str = "Failure Num: " + QString::number(in_struct.fail_num);
    ui->fail_num->setText(output_str);
    output_str = "Failure Info: " + in_struct.fail_idx;
    ui->fail_idx->setText(output_str);
    output_str = "Output CSV File:  " + in_struct.f_newname;
    ui->new_file->setText(output_str);
}

int MainWindow::inverse(struct in_para *p){
    int i, j, line_num = 0, tmp;
    QString line;
    QStringList values;
    QString f_name = p->path + "/" + p->fname; // "/" for Linux
    QString fnw_name = p->path + "/" + p->f_newname; // "/" for Linux
    QFile file(f_name);
    QFile file_new(fnw_name);
    double a[ROW][COL], b[ROW][COL], c[ROW][COL];
    bool isnum = true;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::information(this, tr("Failed"), "Failed to read selected file!");
        return -1;
    }

    if(!file_new.open(QIODevice::ReadWrite | QIODevice::Text)){
        QMessageBox::information(this, tr("Failed"), "Failed to create new file!");
        return -1;
    }

    QTextStream xin(&file);
    QTextStream xout(&file_new);
    while(!xin.atEnd()){
        p->run_num ++;
        line_num ++;
        line = xin.readLine();
        values = line.split(",");
        for(i=0; i<COL; i++){
            for(j=0; j<ROW; j++){
                a[j][i] = QString(values[i*COL+j]).toDouble(&isnum);
                if (!isnum){
                    p->fail_num ++;
                    p->fail_idx = p->fail_idx + QString::number(line_num) + ", ";
                    break;
                }
            }
            if(!isnum){
                break;
            }
        }
        if(isnum){
            tmp = run(a, b, c);
            if(tmp == -1){
                p->fail_num ++;
                p->fail_idx = p->fail_idx + QString::number(line_num) + ", ";
            }
            else{
                verify(a, c);
                p->succ_num ++;
                xout<<c[0][0]<<","<<c[1][0]<<","<<c[2][0]<<","<<c[3][0]<<",";
                xout<<c[0][1]<<","<<c[1][1]<<","<<c[2][1]<<","<<c[3][1]<<",";
                xout<<c[0][2]<<","<<c[1][2]<<","<<c[2][2]<<","<<c[3][2]<<",";
                xout<<c[0][3]<<","<<c[1][3]<<","<<c[2][3]<<","<<c[3][3]<<"\n";
            }
        }
    }
    file.close();
    file_new.close();
    p->fail_idx = p->fail_idx.mid(0, p->fail_idx.length()-2);
    return 0;
}


void MainWindow::verify(double a[ROW][COL], double c[ROW][COL]){
    int i, j, k;
    double d[ROW][COL];
    QString s = "";

    for(i=0; i<ROW; i++){
        for(j=0; j<COL; j++){
            d[i][j] = 0;
        }
    }
    for(i=0; i<ROW; i++){
        for(j=0; j<COL; j++){
            for(k=0; k<COL; k++){
                d[i][j] += a[i][k]*c[k][j];
            }
        }
    }

    s = QString::number(d[0][0]) + ", " + QString::number(d[0][1]) + ", " + QString::number(d[0][2]) + ", " + QString::number(d[0][3]) + "\n";
    s += QString::number(d[1][0]) + ", " + QString::number(d[1][1]) + ", " + QString::number(d[1][2]) + ", " + QString::number(d[1][3]) + "\n";
    s += QString::number(d[2][0]) + ", " + QString::number(d[2][1]) + ", " + QString::number(d[2][2]) + ", " + QString::number(d[2][3]) + "\n";
    s += QString::number(d[3][0]) + ", " + QString::number(d[3][1]) + ", " + QString::number(d[3][2]) + ", " + QString::number(d[3][3]) + "\n";
    QMessageBox::information(this, tr("Result"), s);
}


int MainWindow::run(double a[ROW][COL], double b[ROW][COL], double c[ROW][COL]){
    int i, j;
    double det_a = det(a);
    if(det_a == 0){
        return -1;
    }
    adj(a, b);
    for (i=0; i<ROW; i++){
        for(j=0; j<COL; j++){
            c[i][j] = b[i][j] / det_a;
        }
    }
    return 0;
}

double MainWindow::det(double a[ROW][COL]){
    return (a[0][0]*a[1][1]*a[2][2]*a[3][3]+
            a[0][0]*a[1][2]*a[2][3]*a[3][1]+
            a[0][0]*a[1][3]*a[2][1]*a[3][2]-
            a[0][0]*a[1][3]*a[2][2]*a[3][1]-
            a[0][0]*a[1][2]*a[2][1]*a[3][3]-
            a[0][0]*a[1][1]*a[2][3]*a[3][2]-
            a[0][1]*a[1][0]*a[2][2]*a[3][3]-
            a[0][2]*a[1][0]*a[2][3]*a[3][1]-
            a[0][3]*a[1][0]*a[2][1]*a[3][2]+
            a[0][3]*a[1][0]*a[2][2]*a[3][1]+
            a[0][2]*a[1][0]*a[2][1]*a[3][3]+
            a[0][1]*a[1][0]*a[2][3]*a[3][2]+
            a[0][1]*a[1][2]*a[2][0]*a[3][3]+
            a[0][2]*a[1][3]*a[2][0]*a[3][1]+
            a[0][3]*a[1][1]*a[2][0]*a[3][2]-
            a[0][3]*a[1][2]*a[2][0]*a[3][1]-
            a[0][2]*a[1][1]*a[2][0]*a[3][3]-
            a[0][1]*a[1][3]*a[2][0]*a[3][2]-
            a[0][1]*a[1][2]*a[2][3]*a[3][0]-
            a[0][2]*a[1][3]*a[2][1]*a[3][0]-
            a[0][3]*a[1][1]*a[2][2]*a[3][0]+
            a[0][3]*a[1][2]*a[2][1]*a[3][0]+
            a[0][2]*a[1][1]*a[2][3]*a[3][0]+
            a[0][1]*a[1][3]*a[2][2]*a[3][0]);
}

void MainWindow::adj(double a[ROW][COL], double b[ROW][COL]){
    b[0][0] = (a[1][1]*a[2][2]*a[3][3]+
               a[1][2]*a[2][3]*a[3][1]+
               a[1][3]*a[2][1]*a[3][2]-
               a[1][3]*a[2][2]*a[3][1]-
               a[1][2]*a[2][1]*a[3][3]-
               a[1][1]*a[2][3]*a[3][2]
              );
    b[0][1] = (a[1][0]*a[2][2]*a[3][3]+
               a[1][2]*a[2][3]*a[3][0]+
               a[1][3]*a[2][0]*a[3][2]-
               a[1][3]*a[2][2]*a[3][0]-
               a[1][2]*a[2][0]*a[3][3]-
               a[1][0]*a[2][3]*a[3][2]
              );
    b[0][2] = (a[1][0]*a[2][1]*a[3][3]+
               a[1][1]*a[2][3]*a[3][0]+
               a[1][3]*a[2][0]*a[3][1]-
               a[1][3]*a[2][1]*a[3][0]-
               a[1][1]*a[2][0]*a[3][3]-
               a[1][0]*a[2][3]*a[3][1]
              );
    b[0][3] = (a[1][0]*a[2][1]*a[3][2]+
               a[1][1]*a[2][2]*a[3][0]+
               a[1][2]*a[2][0]*a[3][1]-
               a[1][2]*a[2][1]*a[3][0]-
               a[1][1]*a[2][0]*a[3][2]-
               a[1][0]*a[2][2]*a[3][1]
              );
    b[1][0] = (a[1][0]*a[2][2]*a[3][3]+
               a[0][2]*a[2][3]*a[3][1]+
               a[0][3]*a[2][1]*a[3][2]-
               a[0][3]*a[2][2]*a[3][1]-
               a[0][2]*a[2][1]*a[3][3]-
               a[0][1]*a[2][3]*a[3][2]
              );
    b[1][1] = (a[0][0]*a[2][2]*a[3][3]+
               a[0][2]*a[2][3]*a[3][0]+
               a[0][3]*a[2][0]*a[3][2]-
               a[0][3]*a[2][2]*a[3][0]-
               a[0][2]*a[2][0]*a[3][3]-
               a[0][0]*a[2][3]*a[3][2]
              );
    b[1][2] = (a[0][0]*a[2][1]*a[3][3]+
               a[0][1]*a[2][3]*a[3][0]+
               a[0][3]*a[2][0]*a[3][1]-
               a[0][3]*a[2][1]*a[3][0]-
               a[0][1]*a[2][0]*a[3][3]-
               a[0][0]*a[2][3]*a[3][1]
              );
    b[1][3] = (a[0][0]*a[2][1]*a[3][2]+
               a[0][1]*a[2][2]*a[3][0]+
               a[0][2]*a[2][0]*a[3][1]-
               a[0][2]*a[2][1]*a[3][0]-
               a[0][1]*a[2][0]*a[3][2]-
               a[0][0]*a[2][2]*a[3][1]
              );
    b[2][0] = (a[0][1]*a[1][2]*a[3][3]+
               a[0][2]*a[1][3]*a[3][1]+
               a[0][3]*a[1][1]*a[3][2]-
               a[0][3]*a[1][2]*a[3][1]-
               a[0][2]*a[1][1]*a[3][3]-
               a[0][1]*a[1][3]*a[3][2]
              );
    b[2][1] = (a[0][0]*a[1][2]*a[3][3]+
               a[0][2]*a[1][3]*a[3][0]+
               a[0][3]*a[1][0]*a[3][2]-
               a[0][3]*a[1][2]*a[3][0]-
               a[0][2]*a[1][0]*a[3][3]-
               a[0][0]*a[1][3]*a[3][2]
              );
    b[2][2] = (a[0][0]*a[1][1]*a[3][3]+
               a[0][1]*a[1][3]*a[3][0]+
               a[0][3]*a[1][0]*a[3][1]-
               a[0][3]*a[1][1]*a[3][0]-
               a[0][1]*a[1][0]*a[3][3]-
               a[0][0]*a[1][3]*a[3][1]
              );
    b[2][3] = (a[0][0]*a[1][1]*a[3][2]+
               a[0][1]*a[1][2]*a[3][0]+
               a[0][2]*a[1][0]*a[3][1]-
               a[0][2]*a[1][1]*a[3][0]-
               a[0][1]*a[1][0]*a[3][2]-
               a[0][0]*a[1][2]*a[3][1]
              );
    b[3][0] = (a[0][1]*a[1][2]*a[2][3]+
               a[0][2]*a[1][3]*a[2][1]+
               a[0][3]*a[1][1]*a[2][2]-
               a[0][3]*a[1][2]*a[2][1]-
               a[0][2]*a[1][1]*a[2][3]-
               a[0][1]*a[1][3]*a[2][2]
              );
    b[3][1] = (a[0][0]*a[1][2]*a[2][3]+
               a[0][2]*a[1][3]*a[2][0]+
               a[0][3]*a[1][0]*a[2][2]-
               a[0][3]*a[1][2]*a[2][0]-
               a[0][2]*a[1][0]*a[2][3]-
               a[0][0]*a[1][3]*a[2][2]
              );
    b[3][2] = (a[0][0]*a[1][1]*a[2][3]+
               a[0][1]*a[1][3]*a[2][0]+
               a[0][3]*a[1][0]*a[2][1]-
               a[0][3]*a[1][1]*a[2][0]-
               a[0][1]*a[1][0]*a[2][3]-
               a[0][0]*a[1][3]*a[2][1]
              );
    b[3][3] = (a[0][0]*a[1][1]*a[2][2]+
               a[0][1]*a[1][2]*a[2][0]+
               a[0][2]*a[1][0]*a[2][1]-
               a[0][2]*a[1][1]*a[2][0]-
               a[0][1]*a[1][0]*a[2][2]-
               a[0][0]*a[1][2]*a[2][1]
              );
}











































