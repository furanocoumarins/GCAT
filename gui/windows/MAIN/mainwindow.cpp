#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_pressed()
{
}

void MainWindow::on_pushButton_released()
{
}

void MainWindow::is_endposValid(int value){
    int start = ui->startPOSITION->value();
    value = value < start ? ++start : value;
    ui->endPOSITION->setValue(value);
}

void MainWindow::on_endPOSITION_valueChanged(int value)
{
    is_endposValid(value);
}

void MainWindow::on_startPOSITION_valueChanged(int value)
{
    is_endposValid(value);
}
