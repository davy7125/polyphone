#include "outputsfz.h"
#include "soundfontmanager.h"
#include "conversion_sfz.h"

OutputSfz::OutputSfz() : AbstractOutput() {}

void OutputSfz::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(sm)

    // Directory (possibly remove the extension .sfz)
    if (fileName.endsWith(".sfz"))
        fileName = QFileInfo(fileName).absolutePath() + "/" + QFileInfo(fileName).completeBaseName();

    // Options
    bool presetPrefix = options["prefix"].toBool();
    bool bankDir = options["bankdir"].toBool();
    bool gmSort = options["gmsort"].toBool();

    // Convert
    error = ConversionSfz().convert(fileName, EltID(elementSf2, sf2Index), presetPrefix, bankDir, gmSort);
    success = error.isEmpty();
}
