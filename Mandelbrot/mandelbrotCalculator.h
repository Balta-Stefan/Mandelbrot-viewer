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
    ~MandelbrotCalculator();


    void calculateCPUSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight);
    void calculateCPUParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight);
    void calculateGPU(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double incrementX, double incrementY);
    void calculateAVXSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight);
    void calculateAVXParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight);


private:
    unsigned int isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations);
    unsigned int width, height, sizeOfTheWorld;

    void GPUInit();
    const char* mandelbrotKernelFilename = "Mandelbrot.cl";
    const char* kernelName = "mandelbrot";
    unsigned int* escapeCounts;

    // Device input buffers
    cl_mem escapeCountsGPU;

    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel
    size_t globalSize, localSize;

    const char* TranslateOpenCLError(cl_int errorCode);
    std::string readKernelSource(const char* filename);

    double* temporaryResultAVX;

};

#endif // MANDELBROTCALCULATOR_H
