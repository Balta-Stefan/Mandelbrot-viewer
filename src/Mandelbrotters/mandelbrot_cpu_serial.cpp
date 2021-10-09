#include "Mandelbrotters/mandelbrot_cpu_serial.h"

Mandelbrot_CPU_Serial::Mandelbrot_CPU_Serial(int width, int height) : MandelbrotCalculator(width, height)
{

}

Mandelbrot_CPU_Serial::Mandelbrot_CPU_Serial(const MandelbrotCalculator &obj) : MandelbrotCalculator(obj)
{

}


unsigned int* Mandelbrot_CPU_Serial::calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY)
{
    double incrementX = (downRightX - upperLeftX) / (double)width;
    double incrementY = (upperLeftY - downRightY) / (double)height;

    //imaginaryValue and realValue are put before loops to avoid multiplication (that would lead to: imaginaryValue = upperLeftY + y*incrementY in every iteration, same for realValue)

    double imaginaryValue = upperLeftY;
    for(unsigned int y = 0; y < height; y++)
    {
        double realValue = upperLeftX;
        for(unsigned int x = 0; x < width; x++)
        {
            escapeCounts[y*width + x] = escapeTime(realValue, imaginaryValue, numberOfIterations);
            realValue += incrementX;
        }
        imaginaryValue -= incrementY;
    }

    return escapeCounts;
}
