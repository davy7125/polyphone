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

    // Extraction sfArk
    bool isVersion1;
    SfArkExtractor1 * extractorTmp = new SfArkExtractor1(fileName.toStdString().c_str(), isVersion1);
    delete extractorTmp;

    AbstractExtractor * sfArkExtractor;
    if (isVersion1)
        sfArkExtractor = new SfArkExtractor1(fileName.toStdString().c_str());
    else
        sfArkExtractor = new SfArkExtractor2(fileName.toStdString().c_str());

    sfArkExtractor->extract();
    int size = 0;
    char * rawData = NULL;
    if (sfArkExtractor->getData(rawData, size))
    {
        QByteArray data(rawData, size);
        QDataStream streamSf2(&data, QIODevice::ReadOnly);
        //sm->open(fileName, &streamSf2, sf2Index, true);

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
}
