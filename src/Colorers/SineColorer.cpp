#include "SineColorer.h"
#include <climits>
#include <cmath>

SineColorer::SineColorer(unsigned int width, int unsigned height, QColor& color1, QColor& color2, QColor& color3) : IterationToColorInterface(width, height)
{
    this->color1 = color1;
    this->color2 = color2;
    this->color3 = color3;

    r_slope1 = (color2.red() - color1.red())/0.5;
    r_slope2 = (color3.red() - color2.red())/0.5;
    r_y1 = color1.red();
    r_y2 = color2.red();

    g_slope1 = (color2.green() - color1.green())/0.5;
    g_slope2 = (color3.green() - color2.green())/0.5;
    g_y1 = color1.green();
    g_y2 = color2.green();

    b_slope1 = (color2.blue() - color1.blue())/0.5;
    b_slope2 = (color3.blue() - color2.blue())/0.5;
    b_y1 = color1.blue();
    b_y2 = color2.blue();
}

QColor* SineColorer::escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations)
{
    const double PI = 3.14159265;

    unsigned int minimumIteration = INT_MAX, maxIteration = 0;

    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int x = 0; x < width; x++)
        {
            unsigned int iteration = escapeTimes[y*width + x];

            if(iteration < minimumIteration)
                minimumIteration = iteration;
            if(iteration > maxIteration)
                maxIteration = iteration;
        }
    }

    double maxMinDifference = maxIteration - minimumIteration;

    QColor* colorsArr = new QColor[height*width];

    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int x = 0; x < width; x++)
        {
            unsigned int iteration = escapeTimes[y*width + x];
            QColor tempColor;
            if(iteration == 0)
            {
                tempColor = Qt::black;
                colorsArr[y*width + x] = tempColor;
                continue;
            }

            double normalizedIterationCount = (iteration-minimumIteration) / maxMinDifference;

            double compressedIteration = sin(normalizedIterationCount*PI/2);


            if(compressedIteration < 0.5)
            {
                tempColor.setRed(r_slope1*compressedIteration + r_y1);
                tempColor.setGreen(g_slope1*compressedIteration + g_y1);
                tempColor.setBlue(b_slope1*compressedIteration + b_y1);
            }
            else
            {
                tempColor.setRed(r_slope2*(compressedIteration - 0.5) + r_y2);
                tempColor.setGreen(g_slope2*(compressedIteration - 0.5) + g_y2);
                tempColor.setBlue(b_slope2*(compressedIteration - 0.5) + b_y2);
            }

            colorsArr[y*width + x] = tempColor;
        }
    }

    return colorsArr;
}
