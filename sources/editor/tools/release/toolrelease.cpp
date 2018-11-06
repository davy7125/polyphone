#include "toolrelease.h"
#include "toolrelease_gui.h"
#include "toolrelease_parameters.h"
#include "soundfontmanager.h"

ToolRelease::ToolRelease() : AbstractToolIterating(elementInst, new ToolRelease_parameters(), new ToolRelease_gui())
{

}

void ToolRelease::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolRelease_parameters * params = (ToolRelease_parameters *)parameters;
    double duree36 = params->getBaseDuration();
    double division = params->getEvolution();
    double deTune = params->getDetune();

    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt2 = i;

        // Note moyenne
        double noteMoy = (double)(sm->get(id, champ_keyRange).rValue.byHi +
                                  sm->get(id, champ_keyRange).rValue.byLo) / 2;
        // Calcul durée release
        double release = pow(division, ((36. - noteMoy) / 12.)) * duree36;
        if (release < 0.001) release = 0.001;
        else if (release > 101.594) release = 101.594;
        // Valeur correspondante
        short val = 1200 * qLn(release) / 0.69314718056;
        // Modification instSmpl
        AttributeValue valeur;
        if (sm->get(id, champ_releaseVolEnv).shValue != val)
        {
            valeur.shValue = val;
            sm->set(id, champ_releaseVolEnv, valeur);
        }
        if (deTune != 0)
        {
            // Release de l'enveloppe de modulation
            if (sm->get(id, champ_releaseModEnv).shValue != val)
            {
                valeur.shValue = val;
                sm->set(id, champ_releaseModEnv, valeur);
            }
            if (sm->get(id, champ_modEnvToPitch).shValue != -(int)100 * deTune)
            {
                valeur.shValue = -(int)100 * deTune;
                sm->set(id, champ_modEnvToPitch, valeur);
            }
            // Hauteur de l'effet
            int tuningCoarse = floor(deTune);
            int tuningFine = 100 * (deTune - tuningCoarse);
            valeur.shValue = sm->get(id, champ_coarseTune).shValue + tuningCoarse;
            if (valeur.shValue != 0)
                sm->set(id, champ_coarseTune, valeur);
            else
                sm->reset(id, champ_coarseTune);
            valeur.shValue = sm->get(id, champ_fineTune).shValue + tuningFine;
            if (valeur.shValue != 0)
                sm->set(id, champ_fineTune, valeur);
            else
                sm->reset(id, champ_fineTune);
        }
    }

    // Simplification
    sm->simplify(id, champ_fineTune);
    sm->simplify(id, champ_coarseTune);
    sm->simplify(id, champ_modEnvToPitch);
    sm->simplify(id, champ_releaseModEnv);
    sm->simplify(id, champ_releaseVolEnv);
}

