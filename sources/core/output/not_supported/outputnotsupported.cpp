#include "outputnotsupported.h"
#include "soundfontmanager.h"

OutputNotSupported::OutputNotSupported() : AbstractOutput() {}

void OutputNotSupported::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)
    Q_UNUSED(options)

    // File not supported
    success = false;
    error = trUtf8("Le format du fichier n'est pas supporté.");
}
