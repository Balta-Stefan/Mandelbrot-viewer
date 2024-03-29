#ifndef MANDELBROT_CPU_SERIAL_H
#define MANDELBROT_CPU_SERIAL_H
#include <Mandelbrotters/mandelbrotcalculator.h>

class Mandelbrot_CPU_Serial : public MandelbrotCalculator
{
public:
    Mandelbrot_CPU_Serial(const MandelbrotCalculator &obj);
    Mandelbrot_CPU_Serial(int width, int height);

    unsigned int* calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
};

#endif // MANDELBROT_CPU_SERIAL_H
