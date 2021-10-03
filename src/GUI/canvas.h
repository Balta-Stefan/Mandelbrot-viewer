#ifndef CANVAS_H
#define CANVAS_H
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QImage>
#include <QMouseEvent>
#include <algorithm>
#include <cmath>
#include <QElapsedTimer>
#include <MandelbrotCalculatorOLD.h>
#include <vector>
#include <fstream>
#include <iomanip>

#include "Mandelbrotters/mandelbrot_cpu_serial.h"
#include "Mandelbrotters/mandelbrot_cpu_parallel.h"
#include "Mandelbrotters/mandelbrot_avx_serial.h"
#include "Mandelbrotters/mandelbrot_avx_parallel.h"
#include "Mandelbrotters/mandelbrot_gpu.h"

//when forming a selection rectangle, redraw the widget (using QImage converted to pixmap) and draw rectangle over it
//multithreading: subclass QObject, create QThread object and move that QObject to QThread (movetothread method)
class Canvas : public QWidget
{
    Q_OBJECT
private:
    Mandelbrot_CPU_Serial* mandelbrotterSerialCPU;
    Mandelbrot_CPU_Parallel* mandelbrotterParallelCPU;
    Mandelbrot_AVX_Serial* mandelbrotterAvxSerial;
    Mandelbrot_AVX_Parallel* mandelbrotterAvxParallel;
    Mandelbrot_GPU* mandelbrotterGPU;

    //MandelbrotCalculatorOLD* mandelbrotter;

    const unsigned short canvasWidth = 1024;
    const unsigned short canvasHeight = 760;

    const char* serializedFileName = "mandelbrot coordinates.bin";
    void serializeCoordinates();
    void deserializeCoordinates();
    //unsigned short threadsDone = 0;
    unsigned short numberOfColors;

    void paintEvent(QPaintEvent *event) override;
    unsigned int numberOfIterations;
    unsigned int isMandelbrotNumber(double real, double imaginary);
    double upperLeftX, upperLeftY, downRightX, downRightY;
    double aspectRatio;
    void calculateCoords();
    double incrementX, incrementY;
    //unsigned short screen[768][1024];
    //multithreading...
    //end of multithreading part
    QColor **screenColors;
    QImage image;
    bool leftClickHold;
    unsigned int clickedX, clickedY, releasedMovedX, releasedMovedY;
    void draw();
    void selectionRectangle(int clickedX, int clickedY, int movedX, int movedY);
    int selectionRectangleX, selectionRectangleY, selectionRectangleWidth, selectionRectangleHeight;
    void mouseReleaseCalculation();
    QColor *mapping;
    void calculatorFunction(unsigned short screenColorRowIndex);
    void createColorPalette();

    unsigned int* screen;

    QElapsedTimer timer;

    std::vector<double> upperLeftXDatabase;
    std::vector<double> upperLeftYDatabase;
    std::vector<double> downRightXDatabase;
    std::vector<double> downRightYDatabase;

    std::vector<double> upperLeftXBenchmark;
    std::vector<double> upperLeftYBenchmark;
    std::vector<double> downRightXBenchmark;
    std::vector<double> downRightYBenchmark;
    size_t benchmarkCounter = 0;

public:
    Canvas(QWidget *parent = nullptr);
    ~Canvas();
private slots:
    void changeNumOfIterations(int n);

public slots:
     void reset();
     void benchmark();
     void calculateCPUSerial();
     void calculateCPUParallel();
     void calculateGPU();
     void calculateAVXSerial();
     void calculateAVXParallel();

signals:
     void changeIterationSpinBox(int numOfIterations);
     void setCPUSerialLabel(qint64);
     void setCPUParallelLabel(qint64);
     void setGPULabel(qint64);
     void setAVXSerialLabel(qint64);
     void setAVXParallelLabel(qint64);
     void resetLabels();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wakeThreads();


};


#endif // CANVAS_H
