#include "inputsf3.h"
#include "soundfontmanager.h"
#include "sfont.h"
#include "inputfactory.h"

InputSf3::InputSf3() : AbstractInput() {}

void InputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(sm)
    success = false;

    // Name of the temporary file
    tempFilePath = QDir::tempPath() + "/" + QFileInfo(fileName).completeBaseName() + "_tmp";
    if (QFile(tempFilePath + ".sf2").exists())
    {
        int index = 1;
        while (QFile(tempFilePath + "-" + QString::number(index) + ".sf2").exists())
            index++;
        tempFilePath = tempFilePath + "-" + QString::number(index);
    }
    tempFilePath += ".sf2";

    // First convert to sf2
    SfTools::SoundFont sf(fileName);
    if (sf.read())
    {
        QFile fo(tempFilePath);
        if (fo.open(QIODevice::WriteOnly))
        {
            if (sf.uncompress(&fo))
            {
                error = "";
                success = true;

                // Then load the sf2
                AbstractInput * sf2Input = InputFactory::getInput(tempFilePath);
                sf2Input->process(false);
                if (sf2Input->isSuccess())
                {
                    success = true;
                    sf2Index = sf2Input->getSf2Index();
                }
                else
                    error = sf2Input->getError();
                delete sf2Input;
            }
            else
                error = trUtf8("Erreur lors de la conversion sf3 => sf2");

            fo.close();
        }
        else
            error = trUtf8("Impossible de créer le fichier '%0'").arg(tempFilePath);
    }
    else
        error = trUtf8("Impossible de lire le fichier '%0'").arg(fileName);
}
