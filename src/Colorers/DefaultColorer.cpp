#include "DefaultColorer.h"

DefaultColorer::DefaultColorer(unsigned int width, int unsigned height) : IterationToColorInterface(width, height)
{

}

QColor* DefaultColorer::escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations)
{
    QColor* colorsArr = new QColor[height*width];

    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int x = 0; x < width; x++)
        {
            QColor tempColor;

            if(escapeTimes[y*width + x] == 0)
                tempColor = Qt::black;
            else
            {
                double t = (double)escapeTimes[y*width + x] / numberOfIterations;

                unsigned char r = 9*(1-t)*t*t*t*255;
                unsigned char g = 15*(1-t)*(1-t)*t*t*255;
                unsigned char b = 8.5*(1-t)*(1-t)*(1-t)*t*255;
                tempColor = QColor(r,g,b);
                tempColor.setAlpha(255);
            }

            colorsArr[y*width+x] = tempColor;
        }
    }

    return colorsArr;
}
