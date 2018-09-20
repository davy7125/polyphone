#include "inputsfz.h"
#include "import_sfz.h"
#include "soundfontmanager.h"

InputSfz::InputSfz() : AbstractInput() {}

void InputSfz::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index)
{
    // Check that the file is not already open
    foreach (int i, sm->getSiblings(EltID(elementSf2)))
    {
        if (sm->getQstr(EltID(elementSf2, i), champ_filenameInitial) == fileName)
        {
            success = false;
            error = trUtf8("Le fichier est déjà ouvert");
            return;
        }
    }

    // Import data
    ImportSfz import(sm);
    import.import(fileName, &sf2Index, success, error);

    if (success)
    {
        EltID id(elementSf2, sf2Index);
        sm->set(id, champ_filenameInitial, fileName);
        sm->set(id, champ_filenameForData, fileName);
    }
    sm->clearNewEditing();
}
