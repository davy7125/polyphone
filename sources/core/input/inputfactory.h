#ifndef INPUTFACTORY_H
#define INPUTFACTORY_H

#include <QString>
class AbstractInput;
class SoundfontManager;

class InputFactory
{
public:
    InputFactory();

    /// Get an input related to a file
    AbstractInput * getInput(QString fileName);

private:
    SoundfontManager * _sm;
};

#endif // INPUTFACTORY_H
