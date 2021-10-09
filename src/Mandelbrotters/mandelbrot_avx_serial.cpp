#include "Mandelbrotters/mandelbrot_avx_serial.h"

Mandelbrot_AVX_Serial::Mandelbrot_AVX_Serial(int width, int height) : MandelbrotCalculator(width, height)
{
    temporaryResultSerialAVX = (double*)_aligned_malloc(4*sizeof(double), 32); //which boundary?32 or 64?
}

Mandelbrot_AVX_Serial::Mandelbrot_AVX_Serial(const MandelbrotCalculator &obj) : MandelbrotCalculator(obj)
{
    temporaryResultSerialAVX = (double*)_aligned_malloc(4*sizeof(double), 32); //which boundary?32 or 64?
}

Mandelbrot_AVX_Serial::~Mandelbrot_AVX_Serial()
{
     _aligned_free(temporaryResultSerialAVX);
}

unsigned int* Mandelbrot_AVX_Serial::calculate(unsigned int numberOfIterations, double upperLeftX, double upperLeftY, double downRightX, double downRightY)
{
    double incrementX = (downRightX - upperLeftX) / (double)width;
    double incrementY = (upperLeftY - downRightY) / (double)height;


    __m256d divergenceIterations, groupOfFour, imaginary, _upperLeftX, _four, _two, _incrementX, _secondaryReal, _secondaryImaginary;

    //double real, double imaginary
    //double secondaryReal = real
    //double secondaryImaginary = imaginary
    //(a+ib)^2 = a^2 - b^2 + i2ab

    _upperLeftX = _mm256_set1_pd(upperLeftX);
    _four = _mm256_set1_pd(4.0);
    _two = _mm256_set1_pd(2.0);
    _incrementX = _mm256_set1_pd(incrementX);


    int wholeParts = width / 4; //4 pixels are read at a time (AVX has 256-bit registers, it can fit 4 doubles).It is possible that canvas dimension won't be a multiple of 4

    for(unsigned int y = 0; y < height; y++)
    {
        __m256d _incrementor = _mm256_set_pd(3, 2, 1, 0); //will be used for calculating the real values with _incrementX.Should it be 3, 2, 1, 0 or 0, 1, 2, 3?

        //load imaginary value into the register (repeating it 4 times)
        double imaginaryComponent = upperLeftY - y*incrementY;
        imaginary = _mm256_set1_pd(imaginaryComponent);

        for(int x = 0; x < wholeParts*4; x += 4)
        {
            divergenceIterations = _mm256_setzero_pd();
            __m256d diverged = _mm256_castsi256_pd(_mm256_set1_epi64x(-1)); //when some part diverges, 0x0 will be written into that part.Initially, this register is set to all ones.This is accomplished by broadcasting -1 (0xFFF...FF) and casting it from integer variable to double variable
            //load real part (4 different numbers in total) into groupOfFour
            groupOfFour = _mm256_fmadd_pd(_incrementor, _incrementX, _upperLeftX);
            _secondaryImaginary = _mm256_setzero_pd();
            _secondaryReal = _mm256_setzero_pd();

            //evaluate divergence
            for(unsigned int i = 0; i < numberOfIterations; i++)
            {
                __m256d currentIteration = _mm256_castsi256_pd(_mm256_set1_epi64x((long long)i));
                //Z^2=(a+ib)^2 = a^2 - b^2 + i2ab
                __m256d a2 = _mm256_mul_pd(_secondaryReal, _secondaryReal); //a^2
                __m256d b2 = _mm256_mul_pd(_secondaryImaginary, _secondaryImaginary); //b^2


                //check for divergence ((a2 + b2) > 4)
                __m256d moduloSquare = _mm256_add_pd(a2, b2); //a2 + b2
                __m256d comparisonMask = _mm256_cmp_pd(moduloSquare, _four, _CMP_LE_OQ);
                groupOfFour = _mm256_and_pd(groupOfFour, comparisonMask);
                //those that haven't diverged will be 0xFFF...FF in comparisonMask

                //there must be a register that holds the current iteration (repeats 4 times)
                //there must be a "bool" register that tells which parts have not diverged yet (1 = not diverged, 0 = diverged)
                //divergenceIterations &= diverged AND NOT(comparisonMask) - modify only when a flag in diverged is 1, and flag in comparisonMask is 0.
                //write 0 into the "bool" register
                //if all parts are zeroed out, stop calculating because all of them have diverged

                //divergenceIterations needs to get currentIteration at the part that diverged only if corresponding flag in the "bool" register is 1 and if corresponding flag in comparisonMask is 0
                divergenceIterations =_mm256_or_pd(divergenceIterations, _mm256_and_pd(currentIteration, _mm256_andnot_pd(comparisonMask, diverged))); //divergenceIterations = divergenceIterations AND (diverged AND NOT(comparisonMask))
                //set the corresponding "bool" flags: diverged = diverged AND comparisonMask.There might be problems if overflow happens.
                diverged = _mm256_and_pd(diverged, comparisonMask);

                //break if all 4 numbers have diverged
                if(_mm256_movemask_pd(diverged) == 0)
                    break;

                //end of divergence test

                __m256d tempReal = _mm256_add_pd(_mm256_sub_pd(a2, b2), groupOfFour); // a2 - b2 + r0
                _secondaryImaginary = _mm256_fmadd_pd(_mm256_mul_pd(_secondaryReal, _secondaryImaginary), _two, imaginary); //2*_secondaryReal*_secondaryImaginary + imaginary
                _secondaryReal = tempReal;
            }

            //store the results in escapeCounts
            _mm256_store_pd(temporaryResultSerialAVX, divergenceIterations);


            unsigned int first = *((unsigned int*)(temporaryResultSerialAVX));
            unsigned int second = *((unsigned int*)(temporaryResultSerialAVX + 1));
            unsigned int third = *((unsigned int*)(temporaryResultSerialAVX + 2));
            unsigned int fourth = *((unsigned int*)(temporaryResultSerialAVX + 3));

            //in which order should they be stored (little endian)?
            escapeCounts[y*width + x] = first;
            escapeCounts[y*width + x+1] = second;
            escapeCounts[y*width + x+2] = third;
            escapeCounts[y*width + x+3] = fourth;

            //prepare registers for loading new group of 4 real numbers
            //add _four to _incrementor
            _incrementor = _mm256_add_pd(_incrementor, _four);


        }

        if((wholeParts*4) != width)
        {
            //evaluate the rest one by one
            double realValue = upperLeftX + incrementX*(wholeParts*4);
            int counter = 0;
            for(int x = wholeParts*4; x < width; x++)
                escapeCounts[y*width + x] = escapeTime(realValue + incrementX*(counter++), imaginaryComponent, numberOfIterations);
        }
    }

    return escapeCounts;
}
