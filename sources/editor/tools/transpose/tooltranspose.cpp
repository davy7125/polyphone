#include "tooltranspose.h"
#include "tooltranspose_gui.h"
#include "tooltranspose_parameters.h"
#include "soundfontmanager.h"

ToolTranspose::ToolTranspose() : AbstractToolIterating(elementInst, new ToolTranspose_parameters(), new ToolTranspose_gui())
{

}

void ToolTranspose::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _elementsInError.clear();
}

void ToolTranspose::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolTranspose_parameters * params = (ToolTranspose_parameters *)parameters;
    
    // Elements to transpose
    EltID divId = EltID(elementInstSmpl, id.indexSf2, id.indexElt);
    if (sm->getSiblings(divId).empty())
    {
        // No divisions => error
        _mutex.lock();
        _elementsInError << sm->getQstr(id, champ_name);
        _mutex.unlock();
    }
    else
    {
        double shift = params->getSemiTones();

        // Semi tones number
        int semiTones = qRound(shift);

        // Correction
        int correction = qRound(100. * (shift - semiTones));

        // Process all linked samples
        foreach (int i, sm->getSiblings(divId))
        {
            divId.indexElt2 = i;
            AttributeValue valeur;

            // Update keyrange?
            if (params->getAdaptKeyRanges())
            {
                int noteInf = sm->get(divId, champ_keyRange).rValue.byLo;
                int noteSup = sm->get(divId, champ_keyRange).rValue.byHi;

                // Déplacement de l'étendue
                noteInf -= semiTones;
                noteSup -= semiTones;

                // Ajustement
                if (noteInf < 0)
                    noteInf = 0;
                else if (noteInf > 127)
                    noteInf = 127;
                if (noteSup < 0)
                    noteSup = 0;
                else if (noteSup > 127)
                    noteSup = 127;

                // Enregistrement de la nouvelle étendue
                valeur.rValue.byLo = noteInf;
                valeur.rValue.byHi = noteSup;
                sm->set(divId, champ_keyRange, valeur);
            }

            // Note de base
            EltID idSmpl = divId;
            idSmpl.typeElement = elementSmpl;
            idSmpl.indexElt = sm->get(divId, champ_sampleID).wValue;
            int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
            if (sm->isSet(id, champ_overridingRootKey))
                rootKey = sm->get(id, champ_overridingRootKey).wValue;
            if (sm->isSet(divId, champ_overridingRootKey))
                rootKey = sm->get(divId, champ_overridingRootKey).wValue;

            // Modification rootkey et enregistrement
            rootKey -= semiTones;
            if (rootKey < 0)
                rootKey = 0;
            else if (rootKey > 127)
                rootKey = 127;
            valeur.wValue = rootKey;
            sm->set(divId, champ_overridingRootKey, valeur);

            // Correction
            int fineTune = 0;
            if (sm->isSet(id, champ_fineTune))
                fineTune = sm->get(id, champ_fineTune).wValue;
            if (sm->isSet(divId, champ_fineTune))
                fineTune = sm->get(divId, champ_fineTune).wValue;
            int coarseTune = 0;
            if (sm->isSet(id, champ_coarseTune))
                coarseTune = sm->get(id, champ_coarseTune).wValue;
            if (sm->isSet(divId, champ_coarseTune))
                coarseTune = sm->get(divId, champ_coarseTune).wValue;

            // Modification de la correction
            fineTune += correction;
            if (fineTune >= 100)
            {
                fineTune -= 100;
                coarseTune += 1;
            }
            else if (fineTune <= -100)
            {
                fineTune += 100;
                coarseTune -= 1;
            }

            // Enregistrement de la nouvelle correction
            valeur.shValue = fineTune;
            sm->set(divId, champ_fineTune, valeur);
            valeur.shValue = coarseTune;
            sm->set(divId, champ_coarseTune, valeur);
        }

        // Simplification
        sm->simplify(divId, champ_fineTune);
        sm->simplify(divId, champ_coarseTune);
    }
}

QString ToolTranspose::getWarning()
{
    QString txt = "";

    if (!_elementsInError.empty())
    {
        txt = trUtf8("Un instrument ne contenant aucun sample n'est pas compatible avec cet outil :");
        txt += "<ul>";
        foreach (QString element, _elementsInError)
            txt += "<li>" + element + "</li>";
        txt += "</ul>";
    }

    return txt;
}
