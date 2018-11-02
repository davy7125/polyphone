#include "outputsf2.h"
#include "soundfontmanager.h"

OutputSf2::OutputSf2() : AbstractOutput() {}

void OutputSf2::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)
    Q_UNUSED(options)

    // Nothing special
    success = true;
    error = "";

    //QString result = sm->save(idDest.indexSf2, name, params->getQuality());
}
