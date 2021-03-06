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
#include <mandelbrotCalculator.h>
#include <vector>
#include <fstream>
#include <iomanip>




//when forming a selection rectangle, redraw the widget (using QImage converted to pixmap) and draw rectangle over it
//multithreading: subclass QObject, create QThread object and move that QObject to QThread (movetothread method)
class Canvas : public QWidget
{
    Q_OBJECT
private:
    const char* serializedFileName = "mandelbrot coordinates.bin";
    void serializeCoordinates();
    void deserializeCoordinates();
    //unsigned short threadsDone = 0;
    unsigned short numberOfColors;
    unsigned short canvasWidth, canvasHeight;
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

    MandelbrotCalculator* mandelbrotter;
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
