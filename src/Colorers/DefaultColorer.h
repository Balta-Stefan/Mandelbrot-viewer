#ifndef DEFAULTCOLORER_H
#define DEFAULTCOLORER_H

#include <QColor>
#include "Colorers/iterationtocolorinterface.h"

class DefaultColorer : public IterationToColorInterface
{
public:
    DefaultColorer(unsigned int width, unsigned int height);
    QColor* escapeTimeToColor(unsigned int* escapeTimes, unsigned int numberOfIterations);

};

#endif // DEFAULTCOLORER_H
