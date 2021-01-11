#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    emit(calculateCPUParallel());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCPUSerialLabel(qint64 time)
{
    QString temp = QString::number(time/1000.0);
    ui->CPUserialLabel->setText(temp);
}

void MainWindow::setCPUParallelLabel(qint64 time)
{
    QString temp = QString::number(time/1000.0);
    ui->CPUparallelLabel->setText(temp);
}

void MainWindow::setGPULabel(qint64 time)
{
    QString temp = QString::number(time/1000.0);
    ui->GPULabel->setText(temp);
}

void MainWindow::setAVXSerialLabel(qint64 time)
{
    QString temp = QString::number(time/1000.0);
    ui->AVXserialLabel->setText(temp);
}

void MainWindow::setAVXParallelLabel(qint64 time)
{
    QString temp = QString::number(time/1000.0);
    ui->AVXparallelLabel->setText(temp);
}



void MainWindow::resetTimeLabels()
{
    ui->CPUserialLabel->setText("");
    ui->CPUparallelLabel->setText("");
    ui->GPULabel->setText("");
    ui->AVXserialLabel->setText("");
    ui->AVXparallelLabel->setText("");
}


void MainWindow::on_CPUserial_clicked()
{
    emit(calculateCPUSerial());
}

void MainWindow::on_CPUparallel_clicked()
{
    emit(calculateCPUParallel());
}

void MainWindow::on_GPU_clicked()
{
    emit(calculateGPU());
}


void MainWindow::on_AVXserial_clicked()
{
    emit(calculateAVXSerial());
}

void MainWindow::on_AVXparallel_clicked()
{
    emit(calculateAVXParallel());
}

