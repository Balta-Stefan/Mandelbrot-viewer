#ifndef MANDELBROTCALCULATOR_H
#define MANDELBROTCALCULATOR_H
#include <omp.h>
#include <CL/cl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <immintrin.h>

class MandelbrotCalculator
{
public:
    MandelbrotCalculator(unsigned int* escapeCounts, int width, int height);
    MandelbrotCalculator(const MandelbrotCalculator &obj);

    virtual void calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY) = 0;

protected:
    unsigned int isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations);

    unsigned int width, height;
    unsigned int* escapeCounts;

};

#endif // MANDELBROTCALCULATOR_H
