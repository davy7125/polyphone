#include "inputfactory.h"
#include <QFileInfo>
#include "soundfontmanager.h"
#include "abstractinput.h"
#include "sf2/inputsf2.h"
#include "sfz/inputsfz.h"
#include "sfark/inputsfark.h"
#include "not_supported/inputnotsupported.h"
#include "empty/inputempty.h"

AbstractInput * InputFactory::getInput(QString fileName)
{
    AbstractInput * input = NULL;

    if (fileName.isEmpty())
    {
        // New soundfont from scratch
        input = new InputEmpty();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "sf2" || extension == "sf3")
        {
            // Format sf2 or sf3
            input = new InputSf2();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            input = new InputSfz();
        }
        else if (extension == "sfark")
        {
            // Format sfArk
            input = new InputSfArk();
        }
        else
        {
            // Input "not supported format"
            input = new InputNotSupported();
        }
    }
    input->initialize(fileName, SoundfontManager::getInstance());

    return input;
}
