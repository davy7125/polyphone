#include "inputsfark.h"
#include "soundfontmanager.h"
#include "abstractextractor.h"
#include "sfarkextractor1.h"
#include "sfarkextractor2.h"
#include <QDataStream>

InputSfArk::InputSfArk() : AbstractInput() {}

void InputSfArk::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index)
{
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
        //success = true;
    }
    else
        error = sfArkExtractor->getError();
}
