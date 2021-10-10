#ifndef ITERATIONTOCOLORINTERFACE_H
#define ITERATIONTOCOLORINTERFACE_H

#include <QColor>

class IterationToColorInterface
{
public:
    IterationToColorInterface(unsigned int width, unsigned int height);

    virtual QColor* escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations) = 0;

protected:
    unsigned int width, height;
};

#endif // ITERATIONTOCOLORINTERFACE_H
