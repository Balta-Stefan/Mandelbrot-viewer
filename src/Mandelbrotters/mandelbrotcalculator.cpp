#include "mandelbrotcalculator.h"

MandelbrotCalculator::MandelbrotCalculator(unsigned int* escapeCounts, int width, int height)
{
    this->width = width;
    this->height = height;
    this->escapeCounts = escapeCounts;
}

MandelbrotCalculator::MandelbrotCalculator(const MandelbrotCalculator &obj)
{
    width = obj.width;
    height = obj.height;
    escapeCounts = obj.escapeCounts;
}

unsigned int MandelbrotCalculator::isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations)
{
    //coordinates are to be used in multithreaded implementation for determining which pixel should be coloured, thread safe container will receive a tuple (convergenceSpeed, coordinateX, coordinateY)
    double secondaryReal = 0;
    double secondaryImaginary = 0;

    for (unsigned int i = 0; i < numberOfIterations; i++)
    {

        //Z^2=(a+ib)^2 = a^2 - b^2 + i2ab
        double a2 = secondaryReal * secondaryReal; //a^2
        double b2 = secondaryImaginary*secondaryImaginary; //b^2

        //check divergence
        if((a2 + b2) > 4)
            return i;

        secondaryImaginary = 2*secondaryReal*secondaryImaginary + imaginary;
        secondaryReal = a2 - b2 + real;
    }
    return 0;
}
