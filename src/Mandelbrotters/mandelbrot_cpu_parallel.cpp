#include "mandelbrot_cpu_parallel.h"

Mandelbrot_CPU_Parallel::Mandelbrot_CPU_Parallel(unsigned int* escapeCounts, int width, int height) : MandelbrotCalculator(escapeCounts, width, height)
{

}

Mandelbrot_CPU_Parallel::Mandelbrot_CPU_Parallel(const MandelbrotCalculator &obj) : MandelbrotCalculator(obj)
{

}

void Mandelbrot_CPU_Parallel::calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY)
{
    double incrementX = (downRightX - upperLeftX) / (double)width;
    double incrementY = (upperLeftY - downRightY) / (double)height;

    #pragma omp parallel for
    for(int y = 0; y < height; y++)
    {
        double imaginaryValue = upperLeftY - y*incrementY;
        double realValue = upperLeftX;
        for(unsigned int x = 0; x < width; x++)
        {
            escapeCounts[y*width + x] = isMandelbrotNumber(realValue, imaginaryValue, numberOfIterations);
            realValue += incrementX;
        }
    }
}
