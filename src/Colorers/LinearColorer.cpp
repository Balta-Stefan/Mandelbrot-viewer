#include "LinearColorer.h"
#include <climits>
#include <QDebug>

LinearColorer::LinearColorer(unsigned int width, int unsigned height, QColor& color1, QColor& color2, QColor& color3) : IterationToColorInterface(width, height)
{
    // color1 = (255, 16, 0)
    // color2 = (255, 249, 0)
    // color3 = (255, 96, 158)

    this->color1 = color1;
    this->color2 = color2;
    this->color3 = color3;

    // line through 2 points: y = m(x-x1) + y1
    // m - slope ; m = (y2-y1)/(x2-x1)


    /*
    unsigned int r_slope1, r_slope2;
    unsigned int r_y1, r_y2;
    unsigned int r_x1, r_x2;
    */

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

QColor* LinearColorer::escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations)
{
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

            if(normalizedIterationCount < 0.5)
            {
                tempColor.setRed(r_slope1*normalizedIterationCount + r_y1);
                tempColor.setGreen(g_slope1*normalizedIterationCount + g_y1);
                tempColor.setBlue(b_slope1*normalizedIterationCount + b_y1);
            }
            else
            {
                tempColor.setRed(r_slope2*(normalizedIterationCount - 0.5) + r_y2);
                tempColor.setGreen(g_slope2*(normalizedIterationCount - 0.5) + g_y2);
                tempColor.setBlue(b_slope2*(normalizedIterationCount - 0.5) + b_y2);
            }

            colorsArr[y*width + x] = tempColor;
        }
    }

    return colorsArr;
}
