#include "toolremovemods.h"
#include "soundfontmanager.h"

ToolRemoveMods::ToolRemoveMods() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst, NULL, NULL)
{

}

void ToolRemoveMods::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    _count = 0;
}

void ToolRemoveMods::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    // Mods in the global division
    EltID idMod = id;
    idMod.typeElement = _isInst ? elementInstMod : elementPrstMod;
    foreach (int i, sm->getSiblings(idMod))
    {
        idMod.indexMod = i;
        sm->remove(idMod);
        _count++;
    }

    // Mods in each division linked to an element
    EltID idSub = id;
    idSub.typeElement = _isInst ? elementInstSmpl : elementPrstInst;
    foreach (int i, sm->getSiblings(idSub))
    {
        idSub.indexElt2 = i;
        idMod = idSub;
        idMod.typeElement = _isInst ? elementInstSmplMod : elementPrstInstMod;
        foreach (int j, sm->getSiblings(idMod))
        {
            idMod.indexMod = j;
            sm->remove(idMod);
            _count++;
        }
    }
}

QString ToolRemoveMods::getConfirmation()
{
    if (_count == 1)
        return "1 modulateur a été supprimé.";
    else if (_count > 1)
        return trUtf8("%0 modulateurs ont été supprimés.").arg(QString::number(_count));
    return "";
}

QString ToolRemoveMods::getWarning()
{
    if (_count == 0)
        return trUtf8("La sélection ne contient aucun modulateur.");
    return "";
}
