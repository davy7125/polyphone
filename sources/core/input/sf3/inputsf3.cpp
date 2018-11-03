#include "inputsf3.h"
#include "soundfontmanager.h"
#include "sfont.h"

InputSf3::InputSf3() : AbstractInput() {}

void InputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)


}
