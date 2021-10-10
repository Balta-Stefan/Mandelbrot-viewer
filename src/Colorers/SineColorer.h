#ifndef SINECOLORER_H
#define SINECOLORER_H

#include <QColor>
#include "Colorers/iterationtocolorinterface.h"

class SineColorer : public IterationToColorInterface
{
public:
    SineColorer(unsigned int width, unsigned int height, QColor& color1, QColor& color2, QColor& color3);
    QColor* escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations);

private:
    QColor color1, color2, color3;

    // each color component (RGB) will have to be interpolated separately
    // since there are 3 points, 2 lines are needed
    // each line requires the slope coefficient and 2 parameters: x and y
    // there are 6 lines in total

    // colors are laid out such that f(0)=color1, f(0.5)=color2 and f(1)=color3

    // red
    double r_slope1, r_slope2;
    double r_y1, r_y2;
    unsigned int r_x1=0, r_x2;

    // green
    double g_slope1, g_slope2;
    double g_y1, g_y2;
    unsigned int g_x1=0, g_x2;

    // blue
    double b_slope1, b_slope2;
    double b_y1, b_y2;
    unsigned int b_x1=0, b_x2;
};

#endif // SINECOLORER_H
