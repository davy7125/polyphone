#include "outputdummy.h"
#include "soundfontmanager.h"

OutputDummy::OutputDummy() : AbstractOutput() {}

void OutputDummy::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)
    Q_UNUSED(options)

    // Nothing special
    success = true;
    error = "";
}
