#include "inputsfark.h"
#include "abstractextractor.h"
#include "sfarkextractor1.h"
#include "sfarkextractor2.h"
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include "inputfactory.h"

InputSfArk::InputSfArk() : AbstractInput() {}

void InputSfArk::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
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

    // Take the right version of the extractor
    AbstractExtractor * sfArkExtractor;
    SfArkExtractor1 * extractorV1 = new SfArkExtractor1(fileName.toStdString().c_str());
    if (extractorV1->isVersion1())
        sfArkExtractor = extractorV1;
    else
    {
        delete extractorV1;
        sfArkExtractor = new SfArkExtractor2(fileName.toStdString().c_str());
    }

    // Convert data
    if (sfArkExtractor->extract(tempFilePath.toStdString().c_str()))
    {
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
        error = sfArkExtractor->getError();

    delete sfArkExtractor;
}
