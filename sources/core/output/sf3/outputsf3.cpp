#include "outputsf3.h"
#include "sfont.h"
#include "outputfactory.h"
#include <QFile>
#include <QVariant>

OutputSf3::OutputSf3() : AbstractOutput() {}

void OutputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(sm)

    // First save in the sf2 format
    QString fileNameSf2 = fileName.left(fileName.length() - 4) + "_tmp";
    if (QFile(fileNameSf2 + ".sf2").exists())
    {
        int index = 1;
        while (QFile(fileNameSf2 + "-" + QString::number(index) + ".sf2").exists())
            index++;
        fileNameSf2 = fileNameSf2 + "-" + QString::number(index);
    }
    fileNameSf2 += ".sf2";

    AbstractOutput * outputSf2 = OutputFactory::getOutput(fileNameSf2);
    outputSf2->process(sf2Index, false);
    error = outputSf2->getError();
    success = outputSf2->isSuccess();
    delete outputSf2;
    if (!success)
        return;

    // Then create an sf3 file
    int quality = options.contains("quality") ? options["quality"].toInt() : 1;
    double qualityValue = 1.0;
    switch (quality)
    {
    case 0: qualityValue = 0.2; break;
    case 1: qualityValue = 0.6; break;
    case 2: qualityValue = 1.0; break;
    }

    SfTools::SoundFont sf(fileNameSf2);
    if (sf.read())
    {
        QFile fo(fileName);
        if (fo.open(QIODevice::WriteOnly))
        {
            if (sf.compress(&fo, qualityValue, -1.0))
            {
                error = "";
                success = true;
            }
            else
            {
                error = trUtf8("Error during the sf3 conversion");
                success = false;
            }
            fo.close();
        }
        else
        {
            error = trUtf8("Cannot create file \"%1\"").arg(fileName);
            success = false;
        }
    }
    else
    {
        error = trUtf8("Cannot read file \"%1\"").arg(fileNameSf2);
        success = false;
    }

    // Delete the sf2 temporary file and the sf2
    QFile::remove(fileNameSf2);
}
