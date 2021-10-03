#ifndef MANDELBROT_CPU_PARALLEL_H
#define MANDELBROT_CPU_PARALLEL_H
#include <Mandelbrotters/mandelbrotcalculator.h>

class Mandelbrot_CPU_Parallel : public MandelbrotCalculator
{
public:
    Mandelbrot_CPU_Parallel(unsigned int* escapeCounts, int width, int height);
    Mandelbrot_CPU_Parallel(const MandelbrotCalculator &obj);

    void calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
};

#endif // MANDELBROT_CPU_PARALLEL_H
