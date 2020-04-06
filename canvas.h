#ifndef CANVAS_H
#define CANVAS_H
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QImage>
#include <QMouseEvent>
#include <algorithm>
#include <cmath>
#include <vector>
#include <QPair>






unsigned int isMandelbrotNumber(double real, double imaginary, unsigned short numberOfIterations);

//when forming a selection rectangle, redraw the widget (using QImage converted to pixmap) and draw rectangle over it
//multithreading: subclass QObject, create QThread object and move that QObject to QThread (movetothread method)
class Canvas : public QWidget
{
    Q_OBJECT
private:
    unsigned short canvasWidth, canvasHeight;
    void paintEvent(QPaintEvent *event) override;
    unsigned short numberOfWorkers;
    unsigned short numberOfIterations;
    unsigned int isMandelbrotNumber(double real, double imaginary);
    double upperLeftX, upperLeftY, downRightX, downRightY, aspectRatio;
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
    QColor mapping[16];
    void calculatorFunction(unsigned short screenColorRowIndex);

    class Worker : public QObject
    {
        Q_OBJECT

    public slots:
        void doWork();
    signals:
        void resultReady();
    private:
        unsigned short numOfRows;

    };



public:
    Canvas(QWidget *parent = nullptr);
private slots:
    void changeNumOfIterations(int n);
    void changeNumOfWorkers(int n);
    void reset();
    void computationDoneNotifier();


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


};

#endif // CANVAS_H
