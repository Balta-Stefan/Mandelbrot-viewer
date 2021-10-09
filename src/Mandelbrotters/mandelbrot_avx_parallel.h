#ifndef MANDELBROT_AVX_PARALLEL_H
#define MANDELBROT_AVX_PARALLEL_H
#include <Mandelbrotters/mandelbrotcalculator.h>

class Mandelbrot_AVX_Parallel : public MandelbrotCalculator
{
public:
    Mandelbrot_AVX_Parallel(int width, int height);
    Mandelbrot_AVX_Parallel(const MandelbrotCalculator &obj);
    ~Mandelbrot_AVX_Parallel();

    unsigned int* calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);

private:
    double** temporaryResultsParallelAVX;
};

#endif // MANDELBROT_AVX_PARALLEL_H
