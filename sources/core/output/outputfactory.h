#ifndef OUTPUTFACTORY_H
#define OUTPUTFACTORY_H

#include <QString>
class AbstractOutput;

class OutputFactory
{
public:
    /// Get an output related to a destination file
    static AbstractOutput * getOutput(QString fileName);

    /// Save a sf2
    static bool save(int indexSf2, bool saveAs);
};

#endif // OUTPUTFACTORY_H
