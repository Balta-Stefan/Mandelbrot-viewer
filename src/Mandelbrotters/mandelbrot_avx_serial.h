#ifndef MANDELBROT_AVX_SERIAL_H
#define MANDELBROT_AVX_SERIAL_H
#include <Mandelbrotters/mandelbrotcalculator.h>

class Mandelbrot_AVX_Serial : public MandelbrotCalculator
{
public:
    Mandelbrot_AVX_Serial(unsigned int* escapeCounts, int width, int height);
    Mandelbrot_AVX_Serial(const MandelbrotCalculator &obj);
    ~Mandelbrot_AVX_Serial();

    void calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);

private:
    double* temporaryResultSerialAVX;
};

#endif // MANDELBROT_AVX_SERIAL_H
