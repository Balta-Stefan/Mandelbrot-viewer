#include "GUI/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Stefan's Mandelbrot set viewer");
    //emit(calculateCPUParallel());
    emit(calculateAVXParallel());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCPUSerialLabel(qint64 time)
{
    QString temp = QString::number(time/1000000.0);
    ui->CPUserialLabel->setText(temp + "[ms]");
}

void MainWindow::setCPUParallelLabel(qint64 time)
{
    QString temp = QString::number(time/1000000.0);
    ui->CPUparallelLabel->setText(temp + "[ms]");
}

void MainWindow::setGPULabel(qint64 time)
{
    QString temp = QString::number(time/1000000.0);
    ui->GPULabel->setText(temp + "[ms]");
}

void MainWindow::setAVXSerialLabel(qint64 time)
{
    QString temp = QString::number(time/1000000.0);
    ui->AVXserialLabel->setText(temp + "[ms]");
}

void MainWindow::setAVXParallelLabel(qint64 time)
{
    QString temp = QString::number(time/1000000.0);
    ui->AVXparallelLabel->setText(temp + "[ms]");
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

