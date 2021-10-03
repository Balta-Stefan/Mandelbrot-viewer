#ifndef MANDELBROT_GPU_H
#define MANDELBROT_GPU_H
#include <Mandelbrotters/mandelbrotcalculator.h>

class Mandelbrot_GPU : public MandelbrotCalculator
{
public:
    Mandelbrot_GPU(unsigned int* escapeCounts, int width, int height);
    Mandelbrot_GPU(const MandelbrotCalculator &obj);

    void calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY);

private:
    unsigned int sizeOfTheWorld;

    const std::string mandelbrotKernelFilename = "Mandelbrot.cl";
    const char* kernelName = "mandelbrot";

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
    void GPUInit();
};

#endif // MANDELBROT_GPU_H
