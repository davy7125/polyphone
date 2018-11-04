#include "inputempty.h"
#include "soundfontmanager.h"

InputEmpty::InputEmpty() : AbstractInput() {}

void InputEmpty::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(tempFilePath)
    Q_UNUSED(fileName)

    // Simply add a new empty soundfont
    sf2Index = sm->add(EltID(elementSf2));

    // No error possible
    error = "";
    success = true;
}
