#include "outputsfz.h"
#include "soundfontmanager.h"

OutputSfz::OutputSfz() : AbstractOutput() {}

void OutputSfz::processInternal(QString &fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(fileName)
    Q_UNUSED(sm)
    Q_UNUSED(sf2Index)
    Q_UNUSED(options)

    // Nothing special
    success = true;
    error = "";


//    foreach (int sf2Index, presets.keys())
//    {
//        QList<EltID> ids;
//        foreach (int presetIndex, presets[sf2Index])
//            ids << EltID(elementPrst, sf2Index, presetIndex);
//        ConversionSfz().convert(directory, ids, presetPrefix, bankDir, gmSort);
//    }
}
