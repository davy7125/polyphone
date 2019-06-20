/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "duplicator.h"
#include <QApplication>
#include "soundfontmanager.h"
#include "contextmanager.h"
#include <QMessageBox>
#include <QAbstractButton>

Duplicator::Duplicator() :
    _parent(QApplication::activeWindow()),
    _sm(SoundfontManager::getInstance()),
    _copieSmpl(DUPLIQUER),
    _copieInst(DUPLIQUER),
    _copiePrst(DUPLIQUER),
    _presetFull(false),
    _displayWarningGlobal(ContextManager::configuration()->getValue(ConfManager::SECTION_WARNINGS, "global_not_overwritten", true).toBool())
{
    // Initial counts
    EltID id = EltID(elementSf2, -1, -1, -1, -1);
    foreach (int indexSf2, _sm->getSiblings(id))
    {
        id.indexSf2 = indexSf2;
        id.typeElement = elementSmpl;
        _initialSmplIndexes[indexSf2] = _sm->getSiblings(id);
        id.typeElement = elementInst;
        _initialInstIndexes[indexSf2] = _sm->getSiblings(id);
        id.typeElement = elementPrst;
        _initialPrstIndexes[indexSf2] = _sm->getSiblings(id);
    }
}

void Duplicator::copy(EltID idSource, EltID idDest)
{
    if (idDest.typeElement != elementSf2 && idDest.typeElement != elementInst &&
            idDest.typeElement != elementInstSmpl && idDest.typeElement != elementRootInst &&
            idDest.typeElement != elementPrst && idDest.typeElement != elementPrstInst &&
            idDest.typeElement != elementRootPrst && idDest.typeElement != elementRootSmpl &&
            idDest.typeElement != elementSmpl) return;

    if (idSource.indexSf2 == idDest.indexSf2)
    {
        // Link in the same file
        switch (idSource.typeElement)
        {
        case elementSmpl:
            if (idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl)
                linkSmpl(idSource, idDest);
            break;
        case elementInst:
            if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst)
                linkInst(idSource, idDest);
            else if (idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl)
            {
                // Copy the global division
                copyGlobal(idSource, idDest);

                // Link all samples of an instrument to the other instrument (can be the same instrument)
                idSource.typeElement = elementInstSmpl;
                foreach (int i, _sm->getSiblings(idSource))
                {
                    idSource.indexElt2 = i;
                    copy(idSource, idDest);
                }
            }
            break;
        case elementInstSmpl:
            if (idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl)
                copyLink(idSource, idDest);
            break;
        case elementPrst:
            if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst)
            {
                // Copy the global division
                copyGlobal(idSource, idDest);

                // Link all instruments of a preset to the other preset (can be the same preset)
                idSource.typeElement = elementPrstInst;
                foreach (int i, _sm->getSiblings(idSource))
                {
                    idSource.indexElt2 = i;
                    copy(idSource, idDest);
                }
            }
            break;
        case elementPrstInst:
            if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst)
                copyLink(idSource, idDest);
            break;
        default:
            return;
        }
    }
    else
    {
        // Copy between two different files
        switch (idSource.typeElement)
        {
        case elementSmpl:
            copySmpl(idSource, idDest);
            break;
        case elementInst:
            copyInst(idSource, idDest, true);
            break;
        case elementPrst:
            copyPrst(idSource, idDest, true);
            break;
        default:
            return;
        }
    }
}

void Duplicator::duplicate(EltID idSource)
{
    // Copy within the same soundfont
    switch (idSource.typeElement)
    {
    case elementSmpl:
        _copieSmpl = DUPLIQUER_TOUT;
        copySmpl(idSource, EltID(elementSf2, idSource.indexSf2));
        break;
    case elementInst:
        _copieInst = DUPLIQUER_TOUT;
        copyInst(idSource, EltID(elementSf2, idSource.indexSf2), false);
        break;
    case elementInstSmpl:
        copyLink(idSource, EltID(elementInst, idSource.indexSf2, idSource.indexElt));
        break;
    case elementPrst:
        _copiePrst = DUPLIQUER_TOUT;
        copyPrst(idSource, EltID(elementSf2, idSource.indexSf2), false);
        break;
    case elementPrstInst:
        copyLink(idSource, EltID(elementPrst, idSource.indexSf2, idSource.indexElt));
        break;
    default:
        return;
    }
}

/////////////////////////////////
// OPERATIONS DANS UN MEME SF2 //
/////////////////////////////////

// Lien d'un sample dans un instrument
// idSource : sample
// idDest   : inst ou instSmpl
void Duplicator::linkSmpl(EltID idSource, EltID idDest)
{
    idDest.typeElement = elementInstSmpl;
    idDest.indexElt2 = _sm->add(idDest);
    AttributeValue val;
    val.wValue = idSource.indexElt;
    _sm->set(idDest, champ_sampleID, val);

    // Balance
    if (_sm->get(idSource, champ_sfSampleType).sfLinkValue == rightSample ||
            _sm->get(idSource, champ_sfSampleType).sfLinkValue == RomRightSample)
    {
        val.shValue = 500;
        _sm->set(idDest, champ_pan, val);
    }
    else if (_sm->get(idSource, champ_sfSampleType).sfLinkValue == leftSample ||
             _sm->get(idSource, champ_sfSampleType).sfLinkValue == RomLeftSample)
    {
        val.shValue = -500;
        _sm->set(idDest, champ_pan, val);
    }
    else
    {
        val.shValue = 0;
        _sm->set(idDest, champ_pan, val);
    }
}

// Lien d'un instrument dans un preset
// idSource : instrument
// idDest   : prst ou prstInst
void Duplicator::linkInst(EltID idSource, EltID idDest)
{
    idDest.typeElement = elementPrstInst;
    idDest.indexElt2 = _sm->add(idDest);
    AttributeValue val;
    val.wValue = idSource.indexElt;
    _sm->set(idDest, champ_instrument, val);

    // Keyrange
    int keyMin = 127;
    int keyMax = 0;
    EltID idLinked = idSource;
    idLinked.typeElement = elementInstSmpl;
    foreach (int i, _sm->getSiblings(idLinked))
    {
        idLinked.indexElt2 = i;
        if (_sm->isSet(idLinked, champ_keyRange))
        {
            keyMin = qMin(keyMin, (int)_sm->get(idLinked, champ_keyRange).rValue.byLo);
            keyMax = qMax(keyMax, (int)_sm->get(idLinked, champ_keyRange).rValue.byHi);
        }
    }
    AttributeValue value;
    if (keyMin <= keyMax)
    {
        value.rValue.byLo = keyMin;
        value.rValue.byHi = keyMax;
    }
    else
    {
        value.rValue.byLo = 0;
        value.rValue.byHi = 127;
    }
    _sm->set(idDest, champ_keyRange, value);
}

// Copie d'un instrument lié dans un autre instrument
// idSource : instSmpl ou prstInst
// idDest   : inst/instSmpl ou prst/prstInst (autre élément)
void Duplicator::copyLink(EltID idSource, EltID idDest)
{
    idDest.typeElement = idSource.typeElement;
    idDest.indexElt2 = _sm->add(idDest);

    // Copie des gens
    copyGen(idSource, idDest);

    // Copie des mods
    copyMod(idSource, idDest);
}

// Copie d'une division global d'un instrument ou d'un preset à un autre
// idSource : inst ou prst
// idDest   : inst/instSmpl ou prst/prstInst (autre élément)
void Duplicator::copyGlobal(EltID idSource, EltID idDest)
{
    if (isGlobalEmpty(idSource))
        return;

    if (idSource.typeElement == elementInst)
        idDest.typeElement = elementInst;
    else
        idDest.typeElement = elementPrst;

    if (isGlobalEmpty(idDest))
    {
        // Copie des paramètres et modulateurs globaux
        copyGen(idSource, idDest);
        copyMod(idSource, idDest);
    }
    else if (_displayWarningGlobal)
    {
        // Affichage warning
        QMessageBox msgBox(_parent);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("<b>" + trUtf8("Global parameters are already filled.") + "</b>");
        msgBox.setInformativeText(trUtf8("The global division will not be copied."));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::YesAll);
        msgBox.button(QMessageBox::Yes)->setText(trUtf8("&Ok"));
        msgBox.button(QMessageBox::YesAll)->setText(trUtf8("Ok, &disable this message"));
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::YesAll)
            ContextManager::configuration()->setValue(ConfManager::SECTION_WARNINGS, "global_not_overwritten", false);
    }
    _displayWarningGlobal = false;
}

//////////////////////////////////
// OPERATIONS DANS UN AUTRE SF2 //
//////////////////////////////////

// Copie d'un sample dans un autre sf2
// idSource : sample
// idDest   : indexSf2 différent ou provenant d'un autre fichier
void Duplicator::copySmpl(EltID idSource, EltID idDest)
{
    // Recherche si un sample du même nom existe déjà dans les éléments initiaux
    idDest.typeElement = elementSmpl;
    QString nom = _sm->getQstr(idSource, champ_name);
    int index = -1;
    if (_copieSmpl != DUPLIQUER_TOUT)
    {
        foreach (int j, _initialSmplIndexes[idDest.indexSf2])
        {
            idDest.indexElt = j;
            if (_sm->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                index = j;
        }

        // Remplacement ?
        if (_copieSmpl != REMPLACER_TOUT && _copieSmpl != IGNORER_TOUT && index != -1)
            _copieSmpl = openDialog(trUtf8("The sample \"%1\" already exists.<br />Replace?").arg(nom.left(20)));
    }
    if (index != -1 && _copieSmpl != DUPLIQUER_TOUT && _copieSmpl != DUPLIQUER)
    {
        // utilisation de l'index d'un sample existant
        idDest.indexElt = index;
    }
    else
    {
        // création d'un nouveau smpl
        idDest.indexElt = _sm->add(idDest);
    }

    // Adaptation éventuelle du nom
    if (_copieSmpl == DUPLIQUER || _copieSmpl == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    if (index == -1 || (_copieSmpl != IGNORER && _copieSmpl != IGNORER_TOUT))
    {
        // Configuration du sample
        _sm->set(idDest, champ_sampleData16, _sm->getData(idSource, champ_sampleData16));
        EltID id3 = idDest;
        id3.typeElement = elementSf2;
        if (_sm->get(id3, champ_wBpsSave).wValue == 24)
            _sm->set(idDest, champ_sampleData24,   _sm->getData(idSource, champ_sampleData24));
        _sm->set(idDest, champ_dwLength,           _sm->get(idSource, champ_dwLength));
        _sm->set(idDest, champ_dwSampleRate,       _sm->get(idSource, champ_dwSampleRate));
        _sm->set(idDest, champ_dwStartLoop,        _sm->get(idSource, champ_dwStartLoop));
        _sm->set(idDest, champ_dwEndLoop,          _sm->get(idSource, champ_dwEndLoop));
        _sm->set(idDest, champ_sfSampleType,       _sm->get(idSource, champ_sfSampleType));
        _sm->set(idDest, champ_byOriginalPitch,    _sm->get(idSource, champ_byOriginalPitch));
        _sm->set(idDest, champ_chPitchCorrection,  _sm->get(idSource, champ_chPitchCorrection));
        _sm->set(idDest, champ_name,               nom);

        // Lien
        if (_sm->get(idSource, champ_sfSampleType).wValue != RomMonoSample &&
                _sm->get(idSource, champ_sfSampleType).wValue != monoSample)
        {
            // Possible ?
            EltID idSourceLink = idSource;
            idSourceLink.indexElt = _sm->get(idSource, champ_wSampleLink).wValue;
            AttributeValue val;
            if (_listCopy.contains(idSourceLink))
            {
                // Association
                EltID idDestLink = _listPaste.at(_listCopy.indexOf(idSourceLink));
                val.wValue = idDestLink.indexElt;
                _sm->set(idDest, champ_wSampleLink, val);
                val.wValue = idDest.indexElt;
                _sm->set(idDestLink, champ_wSampleLink, val);

                switch (_sm->get(idSource, champ_sfSampleType).wValue)
                {
                case linkedSample:
                    val.sfLinkValue = linkedSample;
                    break;
                case rightSample:
                    val.sfLinkValue = leftSample;
                    break;
                case leftSample:
                    val.sfLinkValue = rightSample;
                    break;
                case RomLinkedSample:
                    val.sfLinkValue = RomLinkedSample;
                    break;
                case RomRightSample:
                    val.sfLinkValue = RomLeftSample;
                    break;
                case RomLeftSample:
                    val.sfLinkValue = RomRightSample;
                    break;
                }
                _sm->set(idDestLink, champ_sfSampleType, val);
            }
            else
            {
                if (_sm->get(idSource, champ_sfSampleType).wValue == linkedSample ||
                        _sm->get(idSource, champ_sfSampleType).wValue == rightSample ||
                        _sm->get(idSource, champ_sfSampleType).wValue == leftSample)
                    val.sfLinkValue = monoSample;
                else
                    val.sfLinkValue = RomMonoSample;
                _sm->set(idDest, champ_sfSampleType, val);
            }
        }
    }

    // Enregistrement de l'élément copié, avec sa correspondance
    _listCopy << idSource;
    _listPaste << idDest;
}

// Copie d'un instrument dans un autre sf2
// idSource : instrument
// idDest   : indexSf2 différent ou provenant d'un autre fichier
void Duplicator::copyInst(EltID idSource, EltID idDest, bool withSmpl)
{
    if (withSmpl)
    {
        // Copie des samples associés
        EltID idSmpl = idSource;
        idSource.typeElement = elementInstSmpl;
        idSmpl.typeElement = elementSmpl;
        foreach (int i, _sm->getSiblings(idSource))
        {
            idSource.indexElt2 = i;
            idSmpl.indexElt = _sm->get(idSource, champ_sampleID).wValue;
            if (!_listCopy.contains(idSmpl)) // Ne pas copier un smpl déjà copié
                copy(idSmpl, idDest);

            // Sample stéréo ?
            if (_sm->get(idSource, champ_sfSampleType).wValue != RomMonoSample &&
                    _sm->get(idSource, champ_sfSampleType).wValue != monoSample)
            {
                EltID idSourceLink = idSource;
                idSourceLink.indexElt = _sm->get(idSource, champ_wSampleLink).wValue;
                if (!_listCopy.contains(idSourceLink))
                    copy(idSourceLink, idDest);
            }
        }
    }

    // Recherche si un instrument du même nom existe déjà dans les éléments initiaux
    idSource.typeElement = elementInst;
    idDest.typeElement = elementInst;
    QString nom = _sm->getQstr(idSource, champ_name);
    int index = -1;
    if (_copieInst != DUPLIQUER_TOUT)
    {
        foreach (int j, _initialInstIndexes[idDest.indexSf2])
        {
            idDest.indexElt = j;
            if (_sm->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                index = j;
        }

        // Remplacement ?
        if (_copieInst != REMPLACER_TOUT && _copieInst != IGNORER_TOUT && (index != -1))
            _copieInst = openDialog(trUtf8("The instrument \"%1\" already exists.<br />Replace?").arg(nom.left(20)));
    }
    if (index != -1 && _copieInst != DUPLIQUER_TOUT && _copieInst != DUPLIQUER)
    {
        // Utilisation de l'index d'un instrument existant
        idDest.indexElt = index;

        // Effacement des mods globaux, gens globaux et des InstSmpl
        if (_copieInst == REMPLACER || _copieInst == REMPLACER_TOUT)
            reset(idDest);
    }
    else
    {
        // Création d'un nouvel instrument
        idDest.indexElt = _sm->add(idDest);
    }

    // Adaptation éventuelle du nom
    if (_copieInst == DUPLIQUER || _copieInst == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    // Modification de l'instrument
    if (index == -1 || (_copieInst != IGNORER && _copieInst != IGNORER_TOUT))
    {
        // Info
        _sm->set(idDest, champ_name, nom.left(20));

        // Gen globaux
        copyGen(idSource, idDest);

        // Mod globaux
        copyMod(idSource, idDest);

        // Copie des InstSmpl
        idSource.typeElement = elementInstSmpl;
        idDest.typeElement = elementInstSmpl;
        foreach (int i, _sm->getSiblings(idSource))
        {
            idSource.indexElt2 = i;
            // Création d'un lien avec sample
            idDest.indexElt2 = _sm->add(idDest);

            // Copie des gens
            copyGen(idSource, idDest);

            // Copie des mods
            copyMod(idSource, idDest);
        }
        idSource.typeElement = elementInst;
        idDest.typeElement = elementInst;
    }

    // Enregistrement de l'élément copié, avec sa correspondance
    _listCopy << idSource;
    _listPaste << idDest;
}

// Copie d'un preset dans un autre sf2
// idSource : preset
// idDest   : indexSf2 différent ou provenant d'un autre fichier
void Duplicator::copyPrst(EltID idSource, EltID idDest, bool withInst)
{
    if (withInst)
    {
        // Copie des instruments associés
        EltID idInst = idSource;
        idSource.typeElement = elementPrstInst;
        idInst.typeElement = elementInst;
        foreach (int i,_sm->getSiblings(idSource))
        {
            idSource.indexElt2 = i;
            idInst.indexElt = _sm->get(idSource, champ_instrument).wValue;
            if (!_listCopy.contains(idInst)) // Ne pas copier un inst déjà copié
                copy(idInst, idDest);
        }
    }

    // Recherche si un preset du même nom existe déjà dans les éléments initiaux
    idSource.typeElement = elementPrst;
    idDest.typeElement = elementPrst;
    QString nom = _sm->getQstr(idSource, champ_name);
    int index = -1;
    if (_copiePrst != DUPLIQUER_TOUT)
    {
        foreach (int j, _initialPrstIndexes[idDest.indexSf2])
        {
            idDest.indexElt = j;
            if (_sm->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                index = j;
        }

        // Remplacement ?
        if (_copiePrst != REMPLACER_TOUT && _copiePrst != IGNORER_TOUT && (index != -1))
            _copiePrst = openDialog(trUtf8("The preset \"%1\" already exists.<br/>Replace?").arg(nom.left(20)));
    }
    if (index != -1 && _copiePrst != DUPLIQUER_TOUT && _copiePrst != DUPLIQUER)
    {
        // Utilisation de l'index d'un preset existant
        idDest.indexElt = index;

        // Effacement des mods globaux, gens globaux et des PrstInst
        if (_copiePrst == REMPLACER || _copiePrst == REMPLACER_TOUT)
            reset(idDest);
    }
    else
    {
        // Vérification qu'un emplacement dans la banque est disponible
        int numPreset = _sm->get(idSource, champ_wPreset).wValue;
        int numBank = _sm->get(idSource, champ_wBank).wValue;
        _sm->firstAvailablePresetBank(idDest, numBank, numPreset);
        if (numPreset == -1)
        {
            // Aucun preset disponible
            if (!_presetFull)
            {
                QMessageBox::warning(_parent, trUtf8("Warning"),
                                     trUtf8("No preset available."));
                _presetFull = true;
            }
            idDest.indexElt = -1;
        }
        else
        {
            // Création d'un nouveau preset
            idDest.indexElt = _sm->add(idDest);
            AttributeValue val;
            val.wValue = numBank;
            _sm->set(idDest, champ_wBank, val);
            val.wValue = numPreset;
            _sm->set(idDest, champ_wPreset, val);
        }
    }

    // Adaptation éventuelle du nom
    if (_copiePrst == DUPLIQUER || _copiePrst == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    // Modification du preset
    if (idDest.indexElt != -1 && (index == -1 || (_copiePrst != IGNORER && _copiePrst != IGNORER_TOUT)))
    {
        // Info
        _sm->set(idDest, champ_name, nom.left(20));

        // Gen globaux
        copyGen(idSource, idDest);

        // Mod globaux
        copyMod(idSource, idDest);

        // Copie des PrstInst
        idSource.typeElement = elementPrstInst;
        idDest.typeElement = elementPrstInst;
        foreach (int i, _sm->getSiblings(idSource))
        {
            idSource.indexElt2 = i;

            // Création d'un lien avec instrument
            idDest.indexElt2 = _sm->add(idDest);

            // Copie des gens
            copyGen(idSource, idDest);

            // Copie des mods
            copyMod(idSource, idDest);
        }
        idSource.typeElement = elementPrst;
        idDest.typeElement = elementPrst;
    }

    // Enregistrement de l'élément copié, avec sa correspondance
    _listCopy << idSource;
    _listPaste << idDest;
}


/////////////////
// UTILITAIRES //
/////////////////

// Copie de tous les gen d'un élément dans un autre
void Duplicator::copyGen(EltID idSource, EltID idDest)
{
    EltID idGen = idSource;
    switch (idSource.typeElement)
    {
    case elementInstSmpl: case elementInstSmplGen:
        if (idDest.typeElement != elementInstSmpl)
            return;
        idGen.typeElement = elementInstSmplGen;
        break;
    case elementInst: case elementInstGen:
        if (idDest.typeElement != elementInst)
            return;
        idGen.typeElement = elementInstGen;
        break;
    case elementPrstInst: case elementPrstInstGen:
        if (idDest.typeElement != elementPrstInst)
            return;
        idGen.typeElement = elementPrstInstGen;
        break;
    case elementPrst: case elementPrstGen:
        if (idDest.typeElement != elementPrst)
            return;
        idGen.typeElement = elementPrstGen;
        break;
    default:
        return;
    }

    AttributeValue val;
    foreach (int i, _sm->getSiblings(idGen))
    {
        // Copie, sauf bank et preset
        if (i == champ_wBank || i == champ_wPreset)
            continue;

        if (i == champ_sampleID || i == champ_instrument)
        {
            // Ajustement des liens
            EltID idLinkSource = idSource;
            if (i == champ_sampleID)
                idLinkSource.typeElement = elementSmpl;
            else
                idLinkSource.typeElement = elementInst;
            idLinkSource.indexElt = _sm->get(idSource, (AttributeType)i).wValue;
            if (_listCopy.contains(idLinkSource))
                val.wValue = _listPaste.at(_listCopy.indexOf(idLinkSource)).indexElt;
            else
                val.wValue = idLinkSource.indexElt;
        }
        else
            val = _sm->get(idSource, (AttributeType)i);
        _sm->set(idDest, (AttributeType)i, val);
    }
}

// Copie de tous les mod d'un élément dans un autre
void Duplicator::copyMod(EltID idSource, EltID idDest)
{
    switch (idSource.typeElement)
    {
    case elementInstSmpl: case elementInstSmplMod:
        if (idDest.typeElement == elementInstSmpl || idDest.typeElement == elementInstSmplMod)
            idDest.typeElement = elementInstSmplMod;
        else
            return;
        idSource.typeElement = elementInstSmplMod;
        break;
    case elementInst: case elementInstMod:
        if (idDest.typeElement == elementInst || idDest.typeElement == elementInstMod)
            idDest.typeElement = elementInstMod;
        else
            return;
        idSource.typeElement = elementInstMod;
        break;
    case elementPrstInst: case elementPrstInstMod:
        if (idDest.typeElement == elementPrstInst || idDest.typeElement == elementPrstInstMod)
            idDest.typeElement = elementPrstInstMod;
        else
            return;
        idSource.typeElement = elementPrstInstMod;
        break;
    case elementPrst: case elementPrstMod:
        if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstMod)
            idDest.typeElement = elementPrstMod;
        else
            return;
        idSource.typeElement = elementPrstMod;
        break;
    default:
        return;
    }

    // Correspondance des index
    QMap<int, int> indMod;

    foreach (int i, _sm->getSiblings(idSource))
    {
        idSource.indexMod = i;
        idDest.indexMod = _sm->add(idDest);
        indMod[i] = idDest.indexMod;
        _sm->set(idDest, champ_modAmount,       _sm->get(idSource, champ_modAmount));
        _sm->set(idDest, champ_sfModSrcOper,    _sm->get(idSource, champ_sfModSrcOper));
        _sm->set(idDest, champ_sfModAmtSrcOper, _sm->get(idSource, champ_sfModAmtSrcOper));
        _sm->set(idDest, champ_sfModDestOper,   _sm->get(idSource, champ_sfModDestOper));
        if (_sm->get(idSource, champ_sfModDestOper).wValue < 32768) // pas de lien
            _sm->set(idDest, champ_sfModTransOper,  _sm->get(idSource, champ_sfModTransOper));
    }

    // Mise en place des liens (les éléments cachés ayant disparus)
    AttributeValue val;
    foreach (int i, _sm->getSiblings(idSource))
    {
        idSource.indexMod = i;
        if (_sm->get(idSource, champ_sfModDestOper).wValue >= 32768)
        {
            idDest.indexMod = indMod[i];
            val.wValue = 32768 + indMod[_sm->get(idSource, champ_sfModDestOper).wValue - 32768];
            _sm->set(idDest, champ_sfModDestOper, val);
        }
    }
}

// Ouverture d'un dialogue concernant le remplacement
Duplicator::EtatMessage Duplicator::openDialog(QString question)
{
    QMessageBox msgBox(_parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(question);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save |
                              QMessageBox::NoAll | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(trUtf8("&Replace"));
    msgBox.button(QMessageBox::YesAll)->setText(trUtf8("R&eplace all"));
    msgBox.button(QMessageBox::Save)->setText(trUtf8("&Duplicate"));
    msgBox.button(QMessageBox::SaveAll)->setText(trUtf8("D&uplicate all"));
    msgBox.button(QMessageBox::No)->setText(trUtf8("&Ignore"));
    msgBox.button(QMessageBox::NoAll)->setText(trUtf8("I&gnore all"));
    msgBox.setDefaultButton(QMessageBox::YesAll);
    EtatMessage reponse = IGNORER_TOUT;
    switch (msgBox.exec())
    {
    case QMessageBox::NoAll:    reponse = IGNORER_TOUT;   break;
    case QMessageBox::No:       reponse = IGNORER;        break;
    case QMessageBox::YesAll:   reponse = REMPLACER_TOUT; break;
    case QMessageBox::Yes:      reponse = REMPLACER;      break;
    case QMessageBox::Save:     reponse = DUPLIQUER;      break;
    case QMessageBox::SaveAll:  reponse = DUPLIQUER_TOUT; break;
    }
    return reponse;
}

// Réinitialisation d'un instrument ou preset
void Duplicator::reset(EltID idDest)
{
    // Suppression des paramètres
    EltID id = idDest;
    if (idDest.typeElement == elementInst)
        id.typeElement = elementInstGen;
    else
        id.typeElement = elementPrstGen;
    QList<int> paramTypes = _sm->getSiblings(id);
    foreach (int paramType, paramTypes)
    {
        id.indexMod = paramType;
        if (paramType != champ_wBank && paramType != champ_wPreset) // On garde bank et preset
            _sm->reset(id, (AttributeType)paramType);
    }

    // Suppression des modulateurs
    if (idDest.typeElement == elementInst)
        id.typeElement = elementInstMod;
    else
        id.typeElement = elementPrstMod;
    foreach (int i, _sm->getSiblings(id))
    {
        id.indexMod = i;
        _sm->remove(id);
    }

    // Suppression des éléments liés
    if (idDest.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    foreach (int i, _sm->getSiblings(id))
    {
        id.indexElt2 = i;
        _sm->remove(id);
    }
}

bool Duplicator::isGlobalEmpty(EltID id)
{
    // Nombre de paramètres présents
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
        id.typeElement = elementInstGen;
    else
        id.typeElement = elementPrstGen;
    bool isEmpty = _sm->getSiblings(id).empty();

    // Nombre de modulateurs
    if (id.typeElement == elementInstGen)
        id.typeElement = elementInstMod;
    else
        id.typeElement = elementPrstMod;
    isEmpty = isEmpty && _sm->getSiblings(id).empty();

    return isEmpty;
}

QString Duplicator::adaptName(QString nom, EltID idDest)
{
    QStringList listName;
    QList<int> nbElt;
    if (idDest.typeElement == elementSmpl)
        nbElt = _initialSmplIndexes[idDest.indexSf2];
    else if (idDest.typeElement == elementInst)
        nbElt = _initialInstIndexes[idDest.indexSf2];
    else if (idDest.typeElement == elementPrst)
        nbElt = _initialPrstIndexes[idDest.indexSf2];
    foreach (int j, nbElt)
    {
        idDest.indexElt = j;
        listName << _sm->getQstr(idDest, champ_name);
    }

    int suffixNumber = 0;
    while (listName.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
    {
        suffixNumber++;
    }
    nom = getName(nom, 20, suffixNumber);

    return nom;
}

QString Duplicator::getName(QString name, int maxCharacters, int suffixNumber)
{
    if (suffixNumber == 0)
        return name.left(maxCharacters);
    QString suffix = QString::number(suffixNumber);
    int suffixSize = suffix.length();
    if (suffixSize > 3 || maxCharacters < suffixSize + 1)
        return name.left(maxCharacters);
    return name.left(maxCharacters - suffixSize - 1) + "-" + suffix;
}
