#include "toolautodistribution.h"
#include "soundfontmanager.h"

void ToolAutoDistribution::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);

    // Liste of all sample linked with the corresponding root key
    QList<EltID> listID;
    QList<int> listNote;
    int notePrecedente, note, noteSuivante;
    id.typeElement = elementInstSmpl;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt2 = i;

        if (sm->isSet(id, champ_overridingRootKey))
            note = sm->get(id, champ_overridingRootKey).wValue;
        else
        {
            EltID id2 = id;
            id2.typeElement = elementSmpl;
            id2.indexElt = sm->get(id, champ_sampleID).wValue;
            note = sm->get(id2, champ_byOriginalPitch).bValue;
        }

        // Insert so that the key is increasing
        int pos = 0;
        for (int j = 0; j < listID.size(); j++)
            if (note > listNote.at(j)) pos++;
        listID.insert(pos, id);
        listNote.insert(pos, note);
    }

    // Compute the key range of each sample linked
    AttributeValue val;
    noteSuivante = -1;
    notePrecedente = -1;
    for (int i = 0; i < listID.size(); i++)
    {
        // Corresponding key
        note = listNote.at(i);
        if (note != notePrecedente)
        {
            // Next key
            noteSuivante = -1;
            for (int j = i+1; j < listID.size(); j++)
            {
                if (noteSuivante == -1 && listNote.at(i) != listNote.at(j))
                    noteSuivante = listNote.at(j);
            }

            // Key min
            if (notePrecedente == -1)
                val.rValue.byLo = 0;
            else
                val.rValue.byLo = val.rValue.byHi + 1;
            if (val.rValue.byLo > 127)
                val.rValue.byLo = 127;

            // Key max
            if (noteSuivante == -1)
                val.rValue.byHi = 127;
            else
            {
                val.rValue.byHi = qMin(note + (127 - note) / 20,
                                       (int)floor((double)(2 * note + noteSuivante) / 3));
                if (val.rValue.byLo > val.rValue.byHi)
                    val.rValue.byHi = val.rValue.byLo;
            }
            notePrecedente = note;
        }

        // Update the range
        if (sm->get(listID.at(i), champ_keyRange).rValue.byLo != val.rValue.byLo ||
                sm->get(listID.at(i), champ_keyRange).rValue.byHi != val.rValue.byHi)
            sm->set(listID.at(i), champ_keyRange, val);
    }
}
