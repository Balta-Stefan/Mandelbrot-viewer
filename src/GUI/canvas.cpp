#include <GUI/canvas.h>

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
    emit(resetLabels());
    calculateAVXParallel();
}

void Canvas::benchmark()
{
    /*std::ofstream outputFile("benchmark results.txt", std::ios::out);

    size_t size = upperLeftXBenchmark.size();
    QElapsedTimer timer;

    for(size_t i = 0; i < size; i++)
    {
        upperLeftX = upperLeftXBenchmark[i];
        upperLeftY = upperLeftYBenchmark[i];
        downRightX = downRightXBenchmark[i];
        downRightY = downRightYBenchmark[i];

        incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
        incrementY = (upperLeftY - downRightY) / (double)canvasHeight;

        timer.start();
        mandelbrotter->calculateCPUSerial(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
        double elapsedCPUSerial = timer.nsecsElapsed()/1000000.0;

        timer.start();
        mandelbrotter->calculateCPUParallel(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
        double elapsedCPUParallel = timer.nsecsElapsed()/1000000.0;

        timer.start();
        mandelbrotter->calculateGPU(numberOfIterations, upperLeftX, upperLeftY, incrementX, incrementY);
        double elapsedGPU = timer.nsecsElapsed()/1000000.0;

        timer.start();
        mandelbrotter->calculateAVXSerial(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
        double elapsedAVXSerial = timer.nsecsElapsed()/1000000.0;

        timer.start();
        mandelbrotter->calculateAVXParallel(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY, canvasWidth, canvasHeight);
        double elapsedAVXParallel = timer.nsecsElapsed()/1000000.0;

        outputFile << "Number of iterations: " << numberOfIterations << "\n";
        outputFile << "(upX, upY) - (downX, downY): (" << std::fixed << std::setprecision(50) << upperLeftX << "," << std::fixed << std::setprecision(50) << upperLeftY << ") - (" << std::fixed << std::setprecision(50) << downRightX << "," << std::fixed << std::setprecision(50) << downRightY << ")\n";

        outputFile << "CPU serial: " << std::fixed << std::setprecision(6) << elapsedCPUSerial << "[ms]\n";
        outputFile << "CPU parallel: " <<  std::fixed << std::setprecision(6) << elapsedCPUParallel << "[ms]\n";
        outputFile << "GPU: " << std::fixed << std::setprecision(6) << elapsedGPU << "[ms]\n";
        outputFile << "AVX serial: " << std::fixed << std::setprecision(6) << elapsedAVXSerial << "[ms]\n";
        outputFile << "AVX parallel: " << std::fixed << std::setprecision(6) << elapsedAVXParallel << "[ms]\n";
        outputFile << "=============================================================\n\n";
    }

    qInfo() << "benchmark done";*/

    if(benchmarkCounter >= upperLeftXBenchmark.size())
        return;
    upperLeftX = upperLeftXBenchmark[benchmarkCounter];
    upperLeftY = upperLeftYBenchmark[benchmarkCounter];
    downRightX = downRightXBenchmark[benchmarkCounter];
    downRightY = downRightYBenchmark[benchmarkCounter];

    incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    incrementY = (upperLeftY - downRightY) / (double)canvasHeight;


    qInfo() << upperLeftX << "," << upperLeftY << "," << downRightX << "," << downRightY;

    benchmarkCounter++;

    emit(resetLabels());
    calculateAVXParallel();
}


Canvas::~Canvas()
{
    free(screen);
    //_aligned_free(screen);

    //serialize the visited coordinates
    serializeCoordinates();
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

    //canvasWidth = 1024;
    //canvasHeight = 768;

    screen = (unsigned int*)calloc(canvasHeight*canvasWidth, sizeof(unsigned int));
    //screen = (unsigned int*)_aligned_malloc(canvasWidth*canvasHeight*sizeof(unsigned int), 32); //which alignment boundary?32 or 64?


    mandelbrotterSerialCPU = new Mandelbrot_CPU_Serial(screen, canvasWidth, canvasHeight);
    mandelbrotterParallelCPU = new Mandelbrot_CPU_Parallel(*mandelbrotterSerialCPU);
    mandelbrotterAvxSerial = new Mandelbrot_AVX_Serial(*mandelbrotterSerialCPU);
    mandelbrotterAvxParallel = new Mandelbrot_AVX_Parallel(*mandelbrotterSerialCPU);
    mandelbrotterGPU = new Mandelbrot_GPU(*mandelbrotterSerialCPU);

    //mandelbrotter = new MandelbrotCalculatorOLD(screen, canvasWidth, canvasHeight);

    leftClickHold = false;

    this->setMinimumSize(canvasWidth, canvasHeight);
    this->setMaximumSize(canvasWidth, canvasHeight);


    selectionRectangleX = selectionRectangleY = -1;
    numberOfIterations = 500;
    upperLeftX = -2;
    upperLeftY = 1;
    downRightX = 1;
    downRightY = -1;

    upperLeftXDatabase.push_back(upperLeftX);
    upperLeftYDatabase.push_back(upperLeftY);
    downRightXDatabase.push_back(downRightX);
    downRightYDatabase.push_back(downRightY);


    //aspectRatio = abs ((upperLeftX - downRightX) / (upperLeftY - downRightY));
    incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    incrementY = (upperLeftY - downRightY) / (double)canvasHeight;
    //aspectRatio = canvasWidth/canvasHeight;


    screenColors = new QColor*[canvasHeight];
    for(int i = 0; i < canvasHeight; i++)
        screenColors[i] = new QColor[canvasWidth];
    QImage tempImage(canvasWidth, canvasHeight, QImage::Format_RGB32); //might be better to allocate on heap so as to not waste the time on copy ctor in the next line
    image = tempImage;

    deserializeCoordinates();
}

void Canvas::deserializeCoordinates()
{
    std::ifstream inputFile(serializedFileName, std::ios::in | std::ios::binary);
    //upperLeftXBenchmark;
    //upperLeftYBenchmark;
    //downRightXBenchmark;
    //downRightYBenchmark;
    if(!inputFile)
        return;
    qInfo() << "input file is ok";
    size_t size;
    inputFile.read((char*)(&size), sizeof(size_t));

    for(size_t i = 0; i < size; i++)
    {
        double temp;
        inputFile.read((char*)(&temp), sizeof(double));
        upperLeftXBenchmark.push_back(temp);
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp;
        inputFile.read((char*)(&temp), sizeof(double));
        upperLeftYBenchmark.push_back(temp);
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp;
        inputFile.read((char*)(&temp), sizeof(double));
        downRightXBenchmark.push_back(temp);
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp;
        inputFile.read((char*)(&temp), sizeof(double));
        downRightYBenchmark.push_back(temp);
    }
}

void Canvas::serializeCoordinates()
{
    std::ofstream outputFile(serializedFileName, std::ios::out | std::ios::binary);
    if(!outputFile)
        return;
    size_t size = downRightXDatabase.size();
    outputFile.write((const char*)(&size), sizeof(size_t));

    /*
    Vectors are written in the following order: upperLeftX - upperLeftY - downRightX - downRightY
    */
    for(size_t i = 0; i < size; i++)
    {
        double temp = upperLeftXDatabase[i];
        outputFile.write((const char*)(&temp), sizeof(double));
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp = upperLeftYDatabase[i];
        outputFile.write((const char*)(&temp), sizeof(double));
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp = downRightXDatabase[i];
        outputFile.write((const char*)(&temp), sizeof(double));
    }
    for(size_t i = 0; i < size; i++)
    {
        double temp = downRightYDatabase[i];
        outputFile.write((const char*)(&temp), sizeof(double));
    }
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
    emit(resetLabels());
    calculateAVXParallel();
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

    //qInfo() << "screenRatio: " << screenRatio;
    //qInfo() << "before:";
    //qInfo() << "planeWidth: " << planeWidth;
    //qInfo() << "planeHeight: " << planeHeight;
    //qInfo() << "planeRatio: " << planeWidth / planeHeight;

    if(planeWidth < planeHeight) //image is too narrow
        expectedXdifference = ((screenRatio * abs(clickedNumY - releasedNumY)) - planeWidth) / 2.0;
    else                        //image is too wide
        expectedYdifference = (((1.0/screenRatio) * abs(clickedNumX - releasedNumX)) - planeHeight) / 2.0;
    //a half of expected differences will be added and subtracted from corresponding coordinates (thus dividing by 2), existing widths and heights have to be accounted
        //so planewidth and planeheight are subtracted

    //qInfo() << "expectedXdifference: " << expectedXdifference;
    //qInfo() << "expectedYdifference" << expectedYdifference;

    //qInfo() << qSetRealNumberPrecision( 40 )  << "clickedNumX, clickedNumY,releasedNumX, releasedNumY" <<  clickedNumX << "," << clickedNumY<< "," << releasedNumX<< "," <<  releasedNumY;

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


    //qInfo() << "after:";
    //qInfo() << "planeWidth: " << abs(upperLeftX - downRightX);
    //qInfo() << "planeHeight: " << abs(upperLeftY - downRightY);
    //qInfo() << "planeRatio: " << abs((upperLeftX - downRightX) / (upperLeftY - downRightY));

}


void Canvas::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        leftClickHold = true;
        clickedX = event->x();
        clickedY = event->y();
    }

}

void Canvas::calculateCPUSerial()
{
    timer.start();
    mandelbrotterSerialCPU->calculate(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY);
    qint64 elapsed = timer.nsecsElapsed();
    emit(setCPUSerialLabel(elapsed));
    draw();
}

void Canvas::calculateCPUParallel()
{
    timer.start();
    mandelbrotterParallelCPU->calculate(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY);
    qint64 elapsed = timer.nsecsElapsed();
    emit(setCPUParallelLabel(elapsed));
    draw();
}

void Canvas::calculateGPU()
{
    timer.start();
    mandelbrotterGPU->calculate(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY);
    qint64 elapsed = timer.nsecsElapsed();
    emit(setGPULabel(elapsed));
    draw();
}

void Canvas::calculateAVXSerial()
{
    timer.start();
    mandelbrotterAvxSerial->calculate(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY);
    qint64 elapsed = timer.nsecsElapsed();
    emit(setAVXSerialLabel(elapsed));
    draw();
}

void Canvas::calculateAVXParallel()
{
    timer.start();
    mandelbrotterAvxParallel->calculate(numberOfIterations, upperLeftX, upperLeftY, downRightX, downRightY);
    qint64 elapsed = timer.nsecsElapsed();
    emit(setAVXParallelLabel(elapsed));
    draw();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
       leftClickHold = false;
       releasedMovedX = event->x();
       releasedMovedY = event->y();
       selectionRectangleX = -1;
       //qInfo() << "before:";
       //qInfo() << "clickedX, clickedY, releasedMovedX, releasedMovedY" << clickedX << "," << clickedY<<"," <<  releasedMovedX<<"," <<  releasedMovedY;
       //qInfo() << "upperLeftX, upperLeftY, downRightX, downRightY" << upperLeftX << "," << upperLeftY<< "," << downRightX<< "," << downRightY;
       calculateCoords();
       //qInfo() << "after";
       //qInfo() << "upperLeftX, upperLeftY, downRightX, downRightY" << upperLeftX << "," << upperLeftY<< "," << downRightX<< "," << downRightY;
       //draw();
    }
    upperLeftXDatabase.push_back(upperLeftX);
    upperLeftYDatabase.push_back(upperLeftY);
    downRightXDatabase.push_back(downRightX);
    downRightYDatabase.push_back(downRightY);


    emit(resetLabels());
    calculateAVXParallel();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    //is activated only when a button is pressed
    if(leftClickHold)
    {
        int releasedMovedX = event->x();
        int releasedMovedY = event->y();
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
            {
                double t = (double)screen[y*canvasWidth + x] / numberOfIterations;

                unsigned char r = 9*(1-t)*t*t*t*255;
                unsigned char g = 15*(1-t)*(1-t)*t*t*255;
                unsigned char b = 8.5*(1-t)*(1-t)*(1-t)*t*255;
                tempColor = QColor(r,g,b);
                tempColor.setAlpha(255);
                //tempColor = mapping[screen[y*canvasWidth + x] % numberOfColors];
            }

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

