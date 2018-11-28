#include "inputnotsupported.h"

InputNotSupported::InputNotSupported() : AbstractInput() {}

void InputNotSupported::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)
    Q_UNUSED(tempFilePath)

    // File not supported
    success = false;
    error = trUtf8("This file format is not supported.");
}
