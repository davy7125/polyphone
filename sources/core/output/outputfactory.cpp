#include "outputfactory.h"
#include <QFileInfo>
#include "soundfontmanager.h"
#include "abstractoutput.h"
#include "sf2/outputsf2.h"
#include "sfz/outputsfz.h"
#include "not_supported/outputnotsupported.h"
#include "empty/outputdummy.h"

AbstractOutput * OutputFactory::getOutput(QString fileName)
{
    AbstractOutput * output = nullptr;

    if (fileName.isEmpty())
    {
        // Do nothing
        output = new OutputDummy();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "sf2" || extension == "sf3")
        {
            // Format sf2 or sf3
            output = new OutputSf2();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            output = new OutputSfz();
        }
        else
        {
            // Input "not supported format"
            output = new OutputNotSupported();
        }
    }
    output->initialize(fileName, SoundfontManager::getInstance());

    return output;
}
