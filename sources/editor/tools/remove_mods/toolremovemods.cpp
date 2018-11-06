#include "toolremovemods.h"
#include "soundfontmanager.h"

ToolRemoveMods::ToolRemoveMods() :
    AbstractToolIterating(
        QList<ElementType>() << elementInst << elementPrst << elementSf2, nullptr, nullptr)
{

}

void ToolRemoveMods::beforeProcess(IdList ids)
{
    _deletionType = DeletionGlobal;
    if (!ids.empty())
    {
        switch (ids[0].typeElement)
        {
        case elementInst: case elementInstSmpl:
            _deletionType = DeletionForInstrument;
            break;
        case elementPrst: case elementPrstInst:
            _deletionType = DeletionForPreset;
            break;
        default:
            break;
        }
    }
    _count = 0;
}

void ToolRemoveMods::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    switch (_deletionType)
    {
    case DeletionGlobal:
        id.typeElement = elementInst;
        foreach (int i, sm->getSiblings(EltID(elementInst, id.indexSf2)))
            clearModInst(sm, EltID(elementInst, id.indexSf2, i));
        foreach (int i, sm->getSiblings(EltID(elementPrst, id.indexSf2)))
            clearModPrst(sm, EltID(elementPrst, id.indexSf2, i));
        break;
    case DeletionForInstrument:
        id.typeElement = elementInst;
        clearModInst(sm, id);
        break;
    case DeletionForPreset:
        id.typeElement = elementPrst;
        clearModPrst(sm, id);
        break;
    }
}

void ToolRemoveMods::clearModInst(SoundfontManager *sm, EltID idInst)
{
    // Mods in the global division
    clearMod(sm, EltID(elementInstMod, idInst.indexSf2, idInst.indexElt));

    // Mods in each division linked to an element
    foreach (int i, sm->getSiblings(EltID(elementInstSmpl, idInst.indexSf2, idInst.indexElt)))
        clearMod(sm, EltID(elementInstSmplMod, idInst.indexSf2, idInst.indexElt, i));
}

void ToolRemoveMods::clearModPrst(SoundfontManager *sm, EltID idPrst)
{
    // Mods in the global division
    clearMod(sm, EltID(elementPrstMod, idPrst.indexSf2, idPrst.indexElt));

    // Mods in each division linked to an element
    foreach (int i, sm->getSiblings(EltID(elementPrstInst, idPrst.indexSf2, idPrst.indexElt)))
        clearMod(sm, EltID(elementPrstInstMod, idPrst.indexSf2, idPrst.indexElt, i));
}

void ToolRemoveMods::clearMod(SoundfontManager * sm, EltID idMod)
{
    foreach (int i, sm->getSiblings(idMod))
    {
        idMod.indexMod = i;
        sm->remove(idMod);
        _count++;
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
