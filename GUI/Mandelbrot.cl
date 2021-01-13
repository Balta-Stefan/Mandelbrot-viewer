

__kernel void mandelbrot(__global int* escapeCounts, int width, int height, double upperLeftX, double upperLeftY, double incrementX, double incrementY, int iterationCount)
// MandelbrotCalculator::isMandelbrotNumber(double real, double imaginary, unsigned int numberOfIterations)
{
    int ID = get_global_id(0);
    int x = ID % width;
    int y = ID / width;

    if(ID >= (width*height))
        return;


    double real0 = upperLeftX + incrementX*x;
    double imaginary0 = upperLeftY - incrementY*y;
    //coordinates are to be used in multithreaded implementation for determining which pixel should be coloured, thread safe container will receive a tuple (convergenceSpeed, coordinateX, coordinateY)
    double secondaryReal = 0;
    double secondaryImaginary = 0;


    //qInfo() << "(r, i) = " << real << ", " << imaginary;
    //qInfo() << "real, imaginary:" << real << imaginary;
    for (unsigned int i = 0; i < iterationCount; i++)
    {
        //Z^2=(a+ib)^2 = a^2 - b^2 + i2ab
        double a2 = secondaryReal * secondaryReal; //a^2
        double b2 = secondaryImaginary*secondaryImaginary; //b^2



        //check divergence
        if((a2 + b2) > 4)
        {
            escapeCounts[y*width + x] = i;
            return;
        }

        secondaryImaginary = 2*secondaryReal*secondaryImaginary + imaginary0;
        secondaryReal = a2 - b2 + real0;

        /*//(a+ib)^2 = a^2 - b^2 + i2ab
        double tempReal = secondaryReal * secondaryReal - secondaryImaginary * secondaryImaginary + real0;
        double tempImaginary = 2 * secondaryReal * secondaryImaginary + imaginary0;

        if ((tempReal * tempReal + tempImaginary * tempImaginary) > 4)
        {
            //screen[coordinateY][coordinateX] = 1;
            //image.setPixelColor(coordinateX, coordinateY, Qt::white);
            escapeCounts[y*width + x] = i;
            return;
        }



        secondaryReal = tempReal;
        secondaryImaginary = tempImaginary;*/
    }

    //screen[coordinateY][coordinateX] = 0;
    //image.setPixelColor(coordinateX, coordinateY, Qt::black);
    escapeCounts[y*width + x] = 0;
}
