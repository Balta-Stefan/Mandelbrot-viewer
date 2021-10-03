#ifndef MANDELBROTCALCULATOROLD_H
#define MANDELBROTCALCULATOROLD_H
#include <omp.h>
#include <CL/cl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <immintrin.h>

class MandelbrotCalculatorOLD
{
public:
    MandelbrotCalculatorOLD(unsigned int* escapeCounts, int width, int height);
    ~MandelbrotCalculatorOLD();


    void calculateCPUSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
    void calculateCPUParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
    void calculateGPU(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
    void calculateAVXSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);
    void calculateAVXParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);


private:
    const std::string mandelbrotKernelFilename = "Mandelbrot.cl";
    const char* kernelName = "mandelbrot";

    unsigned int isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations);
    unsigned int width, height, sizeOfTheWorld;

    void GPUInit();

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
    std::string readKernelSource(const std::string filename);

    double* temporaryResultSerialAVX;
    double** temporaryResultsParallelAVX;

};

#endif // MANDELBROTCALCULATOROLD_H
