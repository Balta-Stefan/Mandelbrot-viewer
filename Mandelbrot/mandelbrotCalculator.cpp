#include "mandelbrotCalculator.h"

MandelbrotCalculator::MandelbrotCalculator(unsigned int* escapeCounts, int width, int height)
{
    this->width = width;
    this->height = height;
    this->escapeCounts = escapeCounts;
    sizeOfTheWorld = width*height*sizeof(unsigned int);

    //initialize OpenCL
    GPUInit();
}

std::string MandelbrotCalculator::readKernelSource(const char* filename)
{

    std::ifstream inputFile(filename);

    if(!inputFile)
        throw "Can't open file";

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();

    std::string sourceCode = buffer.str();

    inputFile.close();
    return sourceCode;
}

void MandelbrotCalculator::GPUInit()
{
    cl_int err;

    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    std::string kernelSource = readKernelSource(mandelbrotKernelFilename);
    const char* tempSource = kernelSource.c_str();

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char**)&tempSource, NULL, &err);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    //if(err != CL_SUCCESS)
        //throw TranslateOpenCLError(err);


    if (err != CL_SUCCESS)
    {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        // Allocate memory for the log
        char* log = new char[log_size];

        // Get the log
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        qInfo() << "build not successful: " << log;

        std::string temp(log);
        delete[] log;
        return; //notify the GUI using signal/slot mechanism.The passed value should be a string.
    }

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, kernelName, &err);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    // Number of work items in each local work group
    clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(localSize), &localSize, NULL);

    // Number of total work items - localSize must be devisor

    globalSize = std::ceil((double)(width*height) / localSize) * localSize;

    // Create the worlds in device memory for our calculation
    escapeCountsGPU = clCreateBuffer(context, CL_MEM_READ_WRITE, width*height*sizeof(unsigned int), NULL, &err);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    //set kernel arguments
    //mandelbrot(__global escapeCounts, int width, int height, double upperLeftX, double upperLeftY, double incrementX, double incrementY, int iterationCount)
    //only the first 3 args are set once

    err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &escapeCountsGPU);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &width);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &height);
    if(err != CL_SUCCESS)
        qInfo() << TranslateOpenCLError(err);
    clFinish(queue);
}

void MandelbrotCalculator::calculateCPUSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight)
{
    double incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    double incrementY = (upperLeftY - downRightY) / (double)canvasHeight;

    //imaginaryValue and realValue are put before loops to avoid multiplication (that would lead to: imaginaryValue = upperLeftY + y*incrementY in every iteration, same for realValue)

    double imaginaryValue = upperLeftY;
    for(int y = 0; y < canvasHeight; y++)
    {
        double realValue = upperLeftX;
        for(int x = 0; x < canvasWidth; x++)
        {
            escapeCounts[y*width + x] = isMandelbrotNumber(realValue, imaginaryValue, numberOfIterations);
            realValue += incrementX;
        }
        imaginaryValue -= incrementY;
    }

}

void MandelbrotCalculator::calculateGPU(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double incrementX, double incrementY)
{
    //set kernel arguments
    //only the first 3 args are set once

    cl_int err = 0;

    err |= clSetKernelArg(kernel, 3, sizeof(double), &upperLeftX);
    err |= clSetKernelArg(kernel, 4, sizeof(double), &upperLeftY);
    err |= clSetKernelArg(kernel, 5, sizeof(double), &incrementX);
    err |= clSetKernelArg(kernel, 6, sizeof(double), &incrementY);
    err |= clSetKernelArg(kernel, 7, sizeof(int), &numberOfIterations);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);
    clFinish(queue);

    //execute the kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);
    clFinish(queue);


    err = clEnqueueReadBuffer(queue, escapeCountsGPU, CL_TRUE, 0, sizeOfTheWorld, escapeCounts, 0, NULL, NULL);
    if(err != CL_SUCCESS)
        throw TranslateOpenCLError(err);

    clFinish(queue);
}



void MandelbrotCalculator::calculateCPUParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight)
{
    double incrementX = (downRightX - upperLeftX) / (double)canvasWidth;
    double incrementY = (upperLeftY - downRightY) / (double)canvasHeight;

    //imaginaryValue and realValue are put before loops to avoid multiplication (that would lead to: imaginaryValue = upperLeftY + y*incrementY in every iteration, same for realValue)


    #pragma omp parallel for
    for(int y = 0; y < canvasHeight; y++)
    {
        double imaginaryValue = upperLeftY - y*incrementY;
        for(int x = 0; x < canvasWidth; x++)
        {
            double realValue = upperLeftX + x*incrementX;
            escapeCounts[y*width + x] = isMandelbrotNumber(realValue, imaginaryValue, numberOfIterations);
        }
    }
}

void MandelbrotCalculator::calculateAVXSerial(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight)
{

}

void MandelbrotCalculator::calculateAVXParallel(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY, int canvasWidth, int canvasHeight)
{

}

unsigned int MandelbrotCalculator::isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations)
{
    //coordinates are to be used in multithreaded implementation for determining which pixel should be coloured, thread safe container will receive a tuple (convergenceSpeed, coordinateX, coordinateY)
    double secondaryReal = real;
    double secondaryImaginary = imaginary;

    //qInfo() << "(r, i) = " << real << ", " << imaginary;
    //qInfo() << "real, imaginary:" << real << imaginary;
    for (unsigned int i = 0; i < numberOfIterations; i++)
    {

        //(a+ib)^2 = a^2 - b^2 + i2ab
        double tempReal = secondaryReal * secondaryReal - secondaryImaginary * secondaryImaginary + real;
        double tempImaginary = 2 * secondaryReal * secondaryImaginary + imaginary;

        if ((tempReal * tempReal + tempImaginary * tempImaginary) > 4)
        {
            //screen[coordinateY][coordinateX] = 1;
            //image.setPixelColor(coordinateX, coordinateY, Qt::white);
            return i;
        }



        secondaryReal = tempReal;
        secondaryImaginary = tempImaginary;
    }

    //screen[coordinateY][coordinateX] = 0;
    //image.setPixelColor(coordinateX, coordinateY, Qt::black);
    return 0;
}


const char* MandelbrotCalculator::TranslateOpenCLError(cl_int errorCode)
{
    switch (errorCode)
    {
    case CL_SUCCESS:                            return "CL_SUCCESS";
    case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
    case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
    case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
    case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
    case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
    case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
    case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
    case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
    case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
    case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
    case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
    case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
    case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
    case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
    case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
    case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
    case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
    case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
    case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
    case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
    case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
    case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
    case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
    case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
    case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
    case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
    case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
    case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
    case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
    case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
    case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
    case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
    case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
    case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
    case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
    case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
    case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
    case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
    case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
    case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
    case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
    case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
    case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
    case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
    case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
    case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
    case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
    case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
    case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
                                                                                                                //    case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
                                                                                                                //    case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70

    default:
        return "UNKNOWN ERROR CODE";
    }
}