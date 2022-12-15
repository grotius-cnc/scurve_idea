#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <thread>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    myOpenGl=new opengl();
    ui->gridLayout_opengl->addWidget(myOpenGl);

    myJog=new jog();

    thread = new std::thread(&MainWindow::update,this);
    thread->detach(); //! Execute the thread independent from other programs.

    on_lineEdit_vel_max_textChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    while(1){
        //! Use timer delay ..
        usleep(1000000*ui->lineEdit_interval_time->text().toDouble());

        //! std::cout<<"update"<<std::endl;
        myJog->set(ui->lineEdit_vel_max->text().toDouble(),
                   ui->lineEdit_acc_max->text().toDouble(),
                   ui->lineEdit_acc_interval->text().toDouble(),
                   ui->lineEdit_interval_time->text().toDouble());
        myJog->update();

        ui->label_period->setText(QString::number(myJog->Period(),'f',1));
        ui->label_vel->setText(QString::number(myJog->Velocity(),'f',3));
        ui->label_acc->setText(QString::number(myJog->Acceleration(),'f',3));
        ui->label_pos->setText(QString::number(myJog->Position(),'f',3));
        ui->label_time->setText(QString::number(myJog->Time(),'f',3));

        myOpenGl->setMaxValues(ui->lineEdit_vel_max->text().toDouble(),ui->lineEdit_acc_max->text().toDouble());
        myOpenGl->setValues(myJog->Velocity(),myJog->Acceleration());
       // myOpenGl->update();
    }
}

void MainWindow::on_pushButton_jog_x_plus_pressed()
{
    myJog->jog_plus_start();
}

void MainWindow::on_pushButton_jog_x_plus_released()
{
    myJog->jog_plus_stop();
}

void MainWindow::on_pushButton_jog_x_min_pressed()
{
    myJog->jog_min_start();
}

void MainWindow::on_pushButton_jog_x_min_released()
{
    myJog->jog_min_stop();
}

void MainWindow::on_lineEdit_vel_max_textChanged(const QString &arg1)
{
    //! mm/s to mm/min.
    ui->label_vel_max_mm_min->setText(QString::number(ui->lineEdit_vel_max->text().toDouble()*60));
}

void MainWindow::on_pushButton_reset_pressed()
{
    myJog->reset();
    myOpenGl->reset();
}
