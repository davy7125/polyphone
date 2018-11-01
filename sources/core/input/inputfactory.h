#ifndef INPUTFACTORY_H
#define INPUTFACTORY_H

#include <QString>
class AbstractInput;

class InputFactory
{
public:
    /// Get an input related to a file
    static AbstractInput * getInput(QString fileName);
};

#endif // INPUTFACTORY_H
