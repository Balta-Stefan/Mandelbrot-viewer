#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setCPUSerialLabel(qint64);
    void setCPUParallelLabel(qint64);
    void setGPULabel(qint64);
    void setAVXSerialLabel(qint64);
    void setAVXParallelLabel(qint64);
    void resetTimeLabels();

signals:
    void calculateCPUSerial();
    void calculateCPUParallel();
    void calculateGPU();
    void calculateAVXSerial();
    void calculateAVXParallel();
    void benchmark();

private slots:
    void on_CPUserial_clicked();

    void on_CPUparallel_clicked();

    void on_AVXserial_clicked();

    void on_AVXparallel_clicked();

    void on_GPU_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
