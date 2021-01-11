#include <canvas.h>

//https://github.com/cslarsen/mandelbrot-js
//to keep aspect ratio constant, maybe aspect ratio of image has to be equal to aspect ratio of complex plane? - https://stackoverflow.com/a/319818


//in order to avoid image stretching, complex plane aspect ratio must be equal to screen aspect ratio


void Canvas::createColorPalette()
{
    QColor startingColor(43, 81, 236);
    QColor endingColor(132, 91, 187);
    double step = 1.0/numberOfColors;
    mapping = new QColor[numberOfColors];

    for(int i = 0; i < numberOfColors; i++)
    {
        double r,g,b;
        r = startingColor.red() + (double)(endingColor.red() - startingColor.red())*i*step;
        g = startingColor.green() + (double)(endingColor.green() - startingColor.green())*i*step;
        b = startingColor.blue() + (double)(endingColor.blue() - startingColor.blue())*i*step;
        mapping[i] = QColor(r, g, b);
    }
}


void Canvas::reset()
{
    numberOfIterations = 500;
    upperLeftX = -2;
    upperLeftY = 1;
    downRightX = 1;
    downRightY = -1;

    //aspectRatio = abs ((upperLeftX - downRightX) / (upperLeftY - downRightY));
    incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    incrementY = (upperLeftY - downRightY) / (double)canvasHeight;

    emit(changeIterationSpinBox(numberOfIterations));
    calculateCPUParallel();
}

//void Canvas::computationDoneNotifier(unsigned short numOfRows, unsigned short rowForThread)
void Canvas::computationDoneNotifier()
{
    /*if(++threadsDone == numberOfWorkers)
    {
        threadsDone = 0;
        draw();
    }*/
   /* for(unsigned short y = rowForThread; y < rowForThread+numOfRows; y++)
        for(unsigned short x = 0; x < canvasWidth; x++)
            image.setPixelColor(x, y, screenColors[y][x]);




    update();*/
    draw();

    qInfo() << "thread done";
}



Canvas::Canvas(QWidget *parent) : QWidget(parent)
{

    //check this: http://www.stefanbion.de/fraktal-generator/colormapping/index.htm
    numberOfColors = 16;
    mapping = new QColor[numberOfColors];
    mapping[0].setRgb(66, 30, 15);
    mapping[1].setRgb(25, 7, 26);
    mapping[2].setRgb(9, 1, 47);
    mapping[3].setRgb(4, 4, 73);
    mapping[4].setRgb(0, 7, 100);
    mapping[5].setRgb(12, 44, 138);
    mapping[6].setRgb(24, 82, 177);
    mapping[7].setRgb(57, 125, 209);
    mapping[8].setRgb(134, 181, 229);
    mapping[9].setRgb(211, 236, 248);
    mapping[10].setRgb(241, 233, 191);
    mapping[11].setRgb(248, 201, 95);
    mapping[12].setRgb(255, 170, 0);
    mapping[13].setRgb(204, 128, 0);
    mapping[14].setRgb(153, 87, 0);
    mapping[15].setRgb(106, 52, 3);


    /*numberOfColors = 256;
    createColorPalette();*/



    canvasWidth = 1024;
    canvasHeight = 768;

    screen = (unsigned int*)calloc(canvasHeight*canvasWidth, sizeof(unsigned int));


    mandelbrotter = new MandelbrotCalculator(screen, canvasWidth, canvasHeight);

    leftClickHold = false;

    this->setMinimumSize(canvasWidth, canvasHeight);
    this->setMaximumSize(canvasWidth, canvasHeight);


    selectionRectangleX = selectionRectangleY = -1;
    numberOfIterations = 500;
    upperLeftX = -2;
    upperLeftY = 1;
    downRightX = 1;
    downRightY = -1;
    //aspectRatio = abs ((upperLeftX - downRightX) / (upperLeftY - downRightY));
    incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    incrementY = (upperLeftY - downRightY) / (double)canvasHeight;
    //aspectRatio = canvasWidth/canvasHeight;


    screenColors = new QColor*[canvasHeight];
    for(int i = 0; i < canvasHeight; i++)
        screenColors[i] = new QColor[canvasWidth];
    QImage tempImage(canvasWidth, canvasHeight, QImage::Format_RGB32); //might be better to allocate on heap so as to not waste the time on copy ctor in the next line
    image = tempImage;
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QRect rectangle = this->rect();


    //paint.fillRect(rectangle,Qt::red);

    paint.drawImage(rectangle, image);
    if(selectionRectangleX != -1) //-1 indicates that no rectangle should be drawn.This method is called whenever the user is drawing a selection rectangle
    {
        QPen pen;
        pen.setWidth(2);
        pen.setColor(Qt::red);
        paint.setPen(pen);
        paint.drawRect(QRect(selectionRectangleX, selectionRectangleY, selectionRectangleWidth, selectionRectangleHeight));
    }


}

void Canvas::changeNumOfIterations(int n)
{
    numberOfIterations = n;
    draw();
}

//unsigned int isMandelbrotNumber(double real, double imaginary, unsigned short numberOfIterations)
unsigned int Canvas::isMandelbrotNumber(double real, double imaginary)

{
    //coordinates are to be used in multithreaded implementation for determining which pixel should be coloured, thread safe container will receive a tuple (convergenceSpeed, coordinateX, coordinateY)
    double secondaryReal = real;
    double secondaryImaginary = imaginary;

    //qInfo() << "(r, i) = " << real << ", " << imaginary;
    //qInfo() << "real, imaginary:" << real << imaginary;
    for (unsigned int i = 0; i < numberOfIterations; i++)
    {

        //(a+ib)^2 = a^2 - b^2 + i2ab
        double tempReal = secondaryReal * secondaryReal - secondaryImaginary * secondaryImaginary + real;
        double tempImaginary = 2 * secondaryReal * secondaryImaginary + imaginary;

        if ((tempReal * tempReal + tempImaginary * tempImaginary) > 4)
        {
            //screen[coordinateY][coordinateX] = 1;
            //image.setPixelColor(coordinateX, coordinateY, Qt::white);
            return i;
        }



        secondaryReal = tempReal;
        secondaryImaginary = tempImaginary;
    }

    //screen[coordinateY][coordinateX] = 0;
    //image.setPixelColor(coordinateX, coordinateY, Qt::black);
    return 0;
}

void Canvas::calculateCoords()
{
    if ((releasedMovedX == clickedX) || (releasedMovedY == clickedY))
        return;

    double clickedNumX, clickedNumY, releasedNumX, releasedNumY;

    /*double incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    double incrementY = (upperLeftY - downRightY) / (double)canvasHeight;*/

    //clickedX * (imageMax - imageMin)/width

    clickedNumX = (double)clickedX*incrementX + upperLeftX;
    clickedNumY = upperLeftY - (double)clickedY*incrementY;
    releasedNumX = (double)releasedMovedX*incrementX + upperLeftX;
    releasedNumY = upperLeftY - (double)releasedMovedY*incrementY;

    //adjust aspect ratio
    double screenRatio = (double)canvasWidth / canvasHeight;
    //double planeRatio = abs((clickedNumX - releasedNumX) / (clickedNumY - releasedNumY));
    double planeWidth = abs(clickedNumX - releasedNumX);
    double planeHeight = abs(clickedNumY - releasedNumY);
    double expectedXdifference = 0;
    double expectedYdifference = 0;

    qInfo() << "screenRatio: " << screenRatio;
    qInfo() << "before:";
    qInfo() << "planeWidth: " << planeWidth;
    qInfo() << "planeHeight: " << planeHeight;
    qInfo() << "planeRatio: " << planeWidth / planeHeight;

    if(planeWidth < planeHeight) //image is too narrow
        expectedXdifference = ((screenRatio * abs(clickedNumY - releasedNumY)) - planeWidth) / 2.0;
    else                        //image is too wide
        expectedYdifference = (((1.0/screenRatio) * abs(clickedNumX - releasedNumX)) - planeHeight) / 2.0;
    //a half of expected differences will be added and subtracted from corresponding coordinates (thus dividing by 2), existing widths and heights have to be accounted
        //so planewidth and planeheight are subtracted

    qInfo() << "expectedXdifference: " << expectedXdifference;
    qInfo() << "expectedYdifference" << expectedYdifference;

    qInfo() << qSetRealNumberPrecision( 40 )  << "clickedNumX, clickedNumY,releasedNumX, releasedNumY" <<  clickedNumX << "," << clickedNumY<< "," << releasedNumX<< "," <<  releasedNumY;

    /*upperLeftY = std::max(clickedNumY, releasedNumY);
    downRightY = std::min(clickedNumY, releasedNumY);

    upperLeftX = std::min(clickedNumX, releasedNumX);
    downRightX = std::max(clickedNumX, releasedNumX);*/

    if (releasedMovedY > clickedY)
    {
        upperLeftY = clickedNumY;
        downRightY = releasedNumY;
    }
    else
    {
        upperLeftY = releasedNumY;
        downRightY = clickedNumY;
    }


    if (releasedMovedX > clickedX)
    {
        upperLeftX = clickedNumX;
        downRightX = releasedNumX;

    }
    else
    {
        upperLeftX = releasedNumX;
        downRightX = clickedNumX;
    }

    upperLeftX -= expectedXdifference;
    downRightX += expectedXdifference;
    upperLeftY += expectedYdifference;
    downRightY -= expectedYdifference;

    incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    incrementY = (upperLeftY - downRightY) / (double)canvasHeight;

    qInfo() << "after:";
    qInfo() << "planeWidth: " << abs(upperLeftX - downRightX);
    qInfo() << "planeHeight: " << abs(upperLeftY - downRightY);
    qInfo() << "planeRatio: " << abs((upperLeftX - downRightX) / (upperLeftY - downRightY));

}


void Canvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        leftClickHold = true;
        clickedX = event->position().x();
        clickedY = event->position().y();
    }

}

void Canvas::calculateCPUSerial()
{
    timer.start();
    mandelbrotter->calculateCPUSerial(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
    emit(setCPUSerialLabel(timer.elapsed()));
    draw();
}

void Canvas::calculateCPUParallel()
{
    timer.start();
    mandelbrotter->calculateCPUParallel(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
    emit(setCPUParallelLabel(timer.elapsed()));
    draw();
}

void Canvas::calculateGPU()
{
    timer.start();
    mandelbrotter->calculateGPU(numberOfIterations, upperLeftX, upperLeftY, incrementX, incrementY);
    emit(setGPULabel(timer.elapsed()));
    draw();
}

void Canvas::calculateAVXSerial()
{
    timer.start();
    mandelbrotter->calculateAVXSerial(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
    emit(setAVXSerialLabel(timer.elapsed()));
    draw();
}

void Canvas::calculateAVXParallel()
{
    timer.start();
    mandelbrotter->calculateAVXParallel(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
    emit(setAVXParallelLabel(timer.elapsed()));
    draw();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
       leftClickHold = false;
       releasedMovedX = event->position().x();
       releasedMovedY = event->position().y();
       selectionRectangleX = -1;
       //qInfo() << "before:";
       //qInfo() << "clickedX, clickedY, releasedMovedX, releasedMovedY" << clickedX << "," << clickedY<<"," <<  releasedMovedX<<"," <<  releasedMovedY;
       //qInfo() << "upperLeftX, upperLeftY, downRightX, downRightY" << upperLeftX << "," << upperLeftY<< "," << downRightX<< "," << downRightY;
       calculateCoords();
       //qInfo() << "after";
       //qInfo() << "upperLeftX, upperLeftY, downRightX, downRightY" << upperLeftX << "," << upperLeftY<< "," << downRightX<< "," << downRightY;
       //draw();

    }
    emit(resetLabels());
    calculateCPUParallel();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    //is activated only when a button is pressed
    if(leftClickHold)
    {
        int releasedMovedX = event->position().x();
        int releasedMovedY = event->position().y();
        selectionRectangle(clickedX, clickedY, releasedMovedX, releasedMovedY);
    }
}

void Canvas::draw()
{

    //setPixelColor(int x, int y, const QColor &color)

    /*qInfo() << "width: " << canvasWidth;
    qInfo() << "width: " << canvasWidth;

    for(unsigned short y = 0; y < canvasHeight; y++)
    {
        double imaginary = upperLeftY - incrementY * y;

        for(unsigned short x = 0; x < canvasWidth; x++)
        {
            int temp = isMandelbrotNumber(upperLeftX + incrementX * x, imaginary);
            if(temp == 0)
            {
                image.setPixelColor(x, y, Qt::black);
                //screen[y][x] = 0;
            }


            else
            {
                image.setPixelColor(x, y, mapping[temp%16]);
                //screen[y][x] = 1;
            }

        }
    }*/
    //added because of multithreading, should be optimised because only a few rows will be updated each time a thread finishes its job



    for(unsigned int y = 0; y < canvasHeight; y++)
        for(unsigned int x = 0; x < canvasWidth; x++)
        {
            QColor tempColor;
            if(screen[y*canvasWidth + x] == 0)
                tempColor = Qt::black;
            else
                tempColor = mapping[screen[y*canvasWidth + x] % numberOfColors];
            image.setPixelColor(x, y, tempColor);
        }



    update();

}


void Canvas::selectionRectangle(int clickedX, int clickedY, int movedX, int movedY)
{

    selectionRectangleWidth = abs(movedX - clickedX);
    selectionRectangleHeight = abs(movedY - clickedY);

    selectionRectangleX = std::min(clickedX, movedX);
    selectionRectangleY = std::min(clickedY, movedY);

    update();


}