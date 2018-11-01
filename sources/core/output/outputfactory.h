#ifndef OUTPUTFACTORY_H
#define OUTPUTFACTORY_H

#include <QString>
class AbstractOutput;

class OutputFactory
{
public:
    /// Get an output related to a destination file
    static AbstractOutput * getOutput(QString fileName);
};

#endif // OUTPUTFACTORY_H
