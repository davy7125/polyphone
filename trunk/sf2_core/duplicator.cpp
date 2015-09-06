/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "duplicator.h"
#include "mainwindow.h"
#include "config.h"

Duplicator::Duplicator(Pile_sf2 *source, Pile_sf2 *destination, QWidget * parent) :
    _parent(parent),
    _source(source),
    _destination(destination),
    _copieSmpl(DUPLIQUER),
    _copieInst(DUPLIQUER),
    _copiePrst(DUPLIQUER),
    _presetFull(false),
    _displayWarningGlobal(Config::getInstance()->getActivationWarning_GlobalNotOverwritten())
{
    // Initial counts
    EltID id = EltID(elementSf2, -1, -1, -1, -1);
    int nbSf2 = _destination->count(id);
    for (int indexSf2 = 0; indexSf2 < nbSf2; indexSf2++)
    {
        id.indexSf2 = indexSf2;
        id.typeElement = elementSmpl;
        _initialSmplCount << destination->count(id);
        id.typeElement = elementInst;
        _initialInstCount << destination->count(id);
        id.typeElement = elementPrst;
        _initialPrstCount << destination->count(id);
    }
}

void Duplicator::copy(EltID idSource, EltID idDest)
{
    if (idDest.typeElement != elementSf2 && idDest.typeElement != elementInst &&
            idDest.typeElement != elementInstSmpl && idDest.typeElement != elementRootInst &&
            idDest.typeElement != elementPrst && idDest.typeElement != elementPrstInst &&
            idDest.typeElement != elementRootPrst && idDest.typeElement != elementRootSmpl &&
            idDest.typeElement != elementSmpl) return;

    if (_source == _destination && idSource.indexSf2 == idDest.indexSf2)
    {
        // Lien dans le même Sf2
        switch (idSource.typeElement)
        {
        case elementSmpl:
            if (idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl)
                linkSmpl(idSource, idDest);
            break;
        case elementInst:
            if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst)
                linkInst(idSource, idDest);
            else if ((idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl) &&
                     idSource.indexElt != idDest.indexElt)
            {
                // Copie de la division globale
                copyGlobal(idSource, idDest);

                // Copie de tous les samples d'un instrument dans un autre
                idSource.typeElement = elementInstSmpl;
                int nbInstSmpl = _source->count(idSource);
                for (int i = 0; i < nbInstSmpl; i++)
                {
                    idSource.indexElt2 = i;
                    if (!_source->get(idSource, champ_hidden).bValue)
                        copy(idSource, idDest);
                }
            }
            break;
        case elementInstSmpl:
            if ((idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl) &&
                    idDest.indexElt != idSource.indexElt)
                copyLink(idSource, idDest);
            break;
        case elementPrst:
            if ((idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst) &&
                    idSource.indexElt != idDest.indexElt)
            {
                // Copie de la division globale
                copyGlobal(idSource, idDest);

                // Copie de tous les instruments d'un preset dans un autre
                idSource.typeElement = elementPrstInst;
                int nbPrstInst = _source->count(idSource);
                for (int i = 0; i < nbPrstInst; i++)
                {
                    idSource.indexElt2 = i;
                    if (!_source->get(idSource, champ_hidden).bValue)
                        copy(idSource, idDest);
                }
            }
            break;
        case elementPrstInst:
            if ((idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst) &&
                    idDest.indexElt != idSource.indexElt)
                copyLink(idSource, idDest);
            break;
        default:
            return;
        }
    }
    else
    {
        // Copie entre 2 sf2
        switch (idSource.typeElement)
        {
        case elementSmpl:
            copySmpl(idSource, idDest);
            break;
        case elementInst:
            copyInst(idSource, idDest);
            break;
        case elementPrst:
            copyPrst(idSource, idDest);
            break;
        default:
            return;
        }
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
    idDest.indexElt2 = _destination->add(idDest);
    Valeur val;
    val.wValue = idSource.indexElt;
    _destination->set(idDest, champ_sampleID, val);

    // Balance
    if (_source->get(idSource, champ_sfSampleType).sfLinkValue == rightSample ||
            _source->get(idSource, champ_sfSampleType).sfLinkValue == RomRightSample)
    {
        val.shValue = 500;
        _destination->set(idDest, champ_pan, val);
    }
    else if (_source->get(idSource, champ_sfSampleType).sfLinkValue == leftSample ||
             _source->get(idSource, champ_sfSampleType).sfLinkValue == RomLeftSample)
    {
        val.shValue = -500;
        _destination->set(idDest, champ_pan, val);
    }
    else
    {
        val.shValue = 0;
        _destination->set(idDest, champ_pan, val);
    }
}

// Lien d'un instrument dans un preset
// idSource : instrument
// idDest   : prst ou prstInst
void Duplicator::linkInst(EltID idSource, EltID idDest)
{
    idDest.typeElement = elementPrstInst;
    idDest.indexElt2 = _destination->add(idDest);
    Valeur val;
    val.wValue = idSource.indexElt;
    _destination->set(idDest, champ_instrument, val);

    // Keyrange
    int keyMin = 127;
    int keyMax = 0;
    EltID idLinked = idSource;
    idLinked.typeElement = elementInstSmpl;
    for (int i = 0; i < _source->count(idLinked); i++)
    {
        idLinked.indexElt2 = i;
        if (!_source->get(idLinked, champ_hidden).bValue)
        {
            if (_source->isSet(idLinked, champ_keyRange))
            {
                keyMin = qMin(keyMin, (int)_source->get(idLinked, champ_keyRange).rValue.byLo);
                keyMax = qMax(keyMax, (int)_source->get(idLinked, champ_keyRange).rValue.byHi);
            }
        }
    }
    Valeur value;
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
    _destination->set(idDest, champ_keyRange, value);
}

// Copie d'un instrument lié dans un autre instrument
// idSource : instSmpl ou prstInst
// idDest   : inst/instSmpl ou prst/prstInst (autre élément)
void Duplicator::copyLink(EltID idSource, EltID idDest)
{
    idDest.typeElement = idSource.typeElement;
    idDest.indexElt2 = _destination->add(idDest);

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
        msgBox.setText(QObject::trUtf8("<b>Des paramètres globaux sont déjà renseignés.</b>"));
        msgBox.setInformativeText(QObject::trUtf8("La division globale ne sera pas recopiée."));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::YesAll);
        msgBox.button(QMessageBox::Yes)->setText(QObject::trUtf8("&Ok"));
        msgBox.button(QMessageBox::YesAll)->setText(QObject::trUtf8("Ok, &désactiver ce message"));
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::YesAll)
            Config::getInstance()->setActivationWarning_GlobalNotOverwritten(false);
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
    QString nom = _source->getQstr(idSource, champ_name);
    int index = -1;
    if (_copieSmpl != DUPLIQUER_TOUT)
    {
        for (int j = 0; j < _initialSmplCount[idDest.indexSf2]; j++)
        {
            idDest.indexElt = j;
            if (!_destination->get(idDest, champ_hidden).bValue)
            {
                if (_destination->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                    index = j;
            }
        }

        // Remplacement ?
        if (_copieSmpl != REMPLACER_TOUT && _copieSmpl != IGNORER_TOUT && index != -1)
            _copieSmpl = openDialog(QObject::trUtf8("L'échantillon « ") + nom.left(20).toUtf8() +
                                    QObject::trUtf8(" » existe déjà.<br />Que faire ?"));
    }
    if (index != -1 && _copieSmpl != DUPLIQUER_TOUT && _copieSmpl != DUPLIQUER)
    {
        // utilisation de l'index d'un sample existant
        idDest.indexElt = index;
    }
    else
    {
        // création d'un nouveau smpl
        idDest.indexElt = _destination->add(idDest);
    }

    // Adaptation éventuelle du nom
    if (_copieSmpl == DUPLIQUER || _copieSmpl == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    if (index == -1 || (_copieSmpl != IGNORER && _copieSmpl != IGNORER_TOUT))
    {
        // Configuration du sample
        _destination->set(idDest, champ_sampleData16, _source->getData(idSource, champ_sampleData16));
        EltID id3 = idDest;
        id3.typeElement = elementSf2;
        if (_source->get(id3, champ_wBpsSave).wValue == 24)
            _destination->set(idDest, champ_sampleData24,   _source->getData(idSource, champ_sampleData24));
        _destination->set(idDest, champ_dwLength,           _source->get(idSource, champ_dwLength));
        _destination->set(idDest, champ_dwSampleRate,       _source->get(idSource, champ_dwSampleRate));
        _destination->set(idDest, champ_dwStartLoop,        _source->get(idSource, champ_dwStartLoop));
        _destination->set(idDest, champ_dwEndLoop,          _source->get(idSource, champ_dwEndLoop));
        _destination->set(idDest, champ_sfSampleType,       _source->get(idSource, champ_sfSampleType));
        _destination->set(idDest, champ_byOriginalPitch,    _source->get(idSource, champ_byOriginalPitch));
        _destination->set(idDest, champ_chPitchCorrection,  _source->get(idSource, champ_chPitchCorrection));
        _destination->set(idDest, champ_name,               nom);

        // Lien
        if (_source->get(idSource, champ_sfSampleType).wValue != RomMonoSample &&
                _source->get(idSource, champ_sfSampleType).wValue != monoSample)
        {
            // Possible ?
            EltID idSourceLink = idSource;
            idSourceLink.indexElt = _source->get(idSource, champ_wSampleLink).wValue;
            Valeur val;
            if (_listCopy.contains(idSourceLink))
            {
                // Association
                EltID idDestLink = _listPaste.at(_listCopy.indexOf(idSourceLink));
                val.wValue = idDestLink.indexElt;
                _destination->set(idDest, champ_wSampleLink, val);
                val.wValue = idDest.indexElt;
                _destination->set(idDestLink, champ_wSampleLink, val);

                switch (_source->get(idSource, champ_sfSampleType).wValue)
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
                _destination->set(idDestLink, champ_sfSampleType, val);
            }
            else
            {
                if (_source->get(idSource, champ_sfSampleType).wValue == linkedSample ||
                        _source->get(idSource, champ_sfSampleType).wValue == rightSample ||
                        _source->get(idSource, champ_sfSampleType).wValue == leftSample)
                    val.sfLinkValue = monoSample;
                else
                    val.sfLinkValue = RomMonoSample;
                _destination->set(idDest, champ_sfSampleType, val);
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
void Duplicator::copyInst(EltID idSource, EltID idDest)
{
    // Copie des samples associés
    EltID idSmpl = idSource;
    idSource.typeElement = elementInstSmpl;
    idSmpl.typeElement = elementSmpl;
    for (int i = 0; i < _source->count(idSource); i++)
    {
        idSource.indexElt2 = i;
        if (!_source->get(idSource, champ_hidden).bValue)
        {
            idSmpl.indexElt = _source->get(idSource, champ_sampleID).wValue;
            if (!_listCopy.contains(idSmpl)) // Ne pas copier un smpl déjà copié
                copy(idSmpl, idDest);

            // Sample stéréo ?
            if (_source->get(idSource, champ_sfSampleType).wValue != RomMonoSample &&
                    _source->get(idSource, champ_sfSampleType).wValue != monoSample)
            {
                EltID idSourceLink = idSource;
                idSourceLink.indexElt = _source->get(idSource, champ_wSampleLink).wValue;
                if (!_listCopy.contains(idSourceLink))
                    copy(idSourceLink, idDest);
            }
        }
    }

    // Recherche si un instrument du même nom existe déjà dans les éléments initiaux
    idSource.typeElement = elementInst;
    idDest.typeElement = elementInst;
    QString nom = _source->getQstr(idSource, champ_name);
    int index = -1;
    if (_copieInst != DUPLIQUER_TOUT)
    {
        for (int j = 0; j < _initialInstCount[idDest.indexSf2]; j++)
        {
            idDest.indexElt = j;
            if (!_destination->get(idDest, champ_hidden).bValue)
            {
                if (_destination->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                    index = j;
            }
        }

        // Remplacement ?
        if (_copieInst != REMPLACER_TOUT && _copieInst != IGNORER_TOUT && (index != -1))
            _copieInst = openDialog(QObject::trUtf8("L'instrument « ") + nom.left(20).toUtf8() +
                                    QObject::trUtf8(" » existe déjà.<br />Souhaitez-vous le remplacer ?"));
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
        idDest.indexElt = _destination->add(idDest);
    }

    // Adaptation éventuelle du nom
    if (_copieInst == DUPLIQUER || _copieInst == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    // Modification de l'instrument
    if (index == -1 || (_copieInst != IGNORER && _copieInst != IGNORER_TOUT))
    {
        // Info
        _destination->set(idDest, champ_name, nom.left(20));

        // Gen globaux
        copyGen(idSource, idDest);

        // Mod globaux
        copyMod(idSource, idDest);

        // Copie des InstSmpl
        idSource.typeElement = elementInstSmpl;
        idDest.typeElement = elementInstSmpl;
        for (int i = 0; i < _source->count(idSource); i++)
        {
            idSource.indexElt2 = i;
            if (!_source->get(idSource, champ_hidden).bValue)
            {
                // Création d'un lien avec sample
                idDest.indexElt2 = _destination->add(idDest);

                // Copie des gens
                copyGen(idSource, idDest);

                // Copie des mods
                copyMod(idSource, idDest);
            }
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
void Duplicator::copyPrst(EltID idSource, EltID idDest)
{
    // Copie des instruments associés
    EltID idInst = idSource;
    idSource.typeElement = elementPrstInst;
    idInst.typeElement = elementInst;
    for (int i = 0; i < _source->count(idSource); i++)
    {
        idSource.indexElt2 = i;
        if (!_source->get(idSource, champ_hidden).bValue)
        {
            idInst.indexElt = _source->get(idSource, champ_instrument).wValue;
            if (!_listCopy.contains(idInst)) // Ne pas copier un inst déjà copié
                copy(idInst, idDest);
        }
    }

    // Recherche si un preset du même nom existe déjà dans les éléments initiaux
    idSource.typeElement = elementPrst;
    idDest.typeElement = elementPrst;
    QString nom = _source->getQstr(idSource, champ_name);
    int index = -1;
    if (_copiePrst != DUPLIQUER_TOUT)
    {
        for (int j = 0; j < _initialPrstCount[idDest.indexSf2]; j++)
        {
            idDest.indexElt = j;
            if (!_destination->get(idDest, champ_hidden).bValue)
            {
                if (_destination->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                    index = j;
            }
        }

        // Remplacement ?
        if (_copiePrst != REMPLACER_TOUT && _copiePrst != IGNORER_TOUT && (index != -1))
            _copiePrst = openDialog(QObject::trUtf8("Le preset « ") + nom.left(20).toUtf8() +
                                    QObject::trUtf8(" » existe déjà.<br />Souhaitez-vous le remplacer ?"));
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
        int numPreset = _source->get(idSource, champ_wPreset).wValue;
        int numBank = _source->get(idSource, champ_wBank).wValue;
        _destination->firstAvailablePresetBank(idDest, numBank, numPreset);
        if (numPreset == -1)
        {
            // Aucun preset disponible
            if (!_presetFull)
            {
                QMessageBox::warning(_parent, QObject::trUtf8("Attention"),
                                     QObject::trUtf8("Aucun preset n'est disponible."));
                _presetFull = true;
            }
            idDest.indexElt = -1;
        }
        else
        {
            // Création d'un nouveau preset
            idDest.indexElt = _destination->add(idDest);
            Valeur val;
            val.wValue = numBank;
            _destination->set(idDest, champ_wBank, val);
            val.wValue = numPreset;
            _destination->set(idDest, champ_wPreset, val);
        }
    }

    // Adaptation éventuelle du nom
    if (_copiePrst == DUPLIQUER || _copiePrst == DUPLIQUER_TOUT)
        nom = adaptName(nom, idDest);

    // Modification du preset
    if (idDest.indexElt != -1 && (index == -1 || (_copiePrst != IGNORER && _copiePrst != IGNORER_TOUT)))
    {
        // Info
        _destination->set(idDest, champ_name, nom.left(20));

        // Gen globaux
        copyGen(idSource, idDest);

        // Mod globaux
        copyMod(idSource, idDest);

        // Copie des PrstInst
        idSource.typeElement = elementPrstInst;
        idDest.typeElement = elementPrstInst;
        for (int i = 0; i < _source->count(idSource); i++)
        {
            idSource.indexElt2 = i;
            if (!_source->get(idSource, champ_hidden).bValue)
            {
                // Création d'un lien avec instrument
                idDest.indexElt2 = _destination->add(idDest);

                // Copie des gens
                copyGen(idSource, idDest);

                // Copie des mods
                copyMod(idSource, idDest);
            }
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
    switch (idSource.typeElement)
    {
    case elementInstSmpl: case elementInstSmplGen:
        if (idDest.typeElement != elementInstSmpl)
            return;
        idSource.typeElement = elementInstSmplGen;
        break;
    case elementInst: case elementInstGen:
        if (idDest.typeElement != elementInst)
            return;
        idSource.typeElement = elementInstGen;
        break;
    case elementPrstInst: case elementPrstInstGen:
        if (idDest.typeElement != elementPrstInst)
            return;
        idSource.typeElement = elementPrstInstGen;
        break;
    case elementPrst: case elementPrstGen:
        if (idDest.typeElement != elementPrst)
            return;
        idSource.typeElement = elementPrstGen;
        break;
    default:
        return;
    }

    Valeur val;
    for (int i = 0; i < _source->count(idSource); i++)
    {
        idSource.indexMod = i;

        // Copie, sauf bank et preset
        if (_source->get(idSource, champ_sfGenOper).sfGenValue != champ_wBank &&
                _source->get(idSource, champ_sfGenOper).sfGenValue != champ_wPreset)
        {
            // Ajustement des liens
            if ((_source->get(idSource, champ_sfGenOper).sfGenValue == champ_sampleID ||
                    _source->get(idSource, champ_sfGenOper).sfGenValue == champ_instrument))
            {
                EltID idLinkSource = idSource;
                if (idSource.typeElement == elementInstSmplGen)
                    idLinkSource.typeElement = elementSmpl;
                else
                    idLinkSource.typeElement = elementInst;
                idLinkSource.indexElt = _source->get(idSource, champ_sfGenAmount).wValue;
                if (_listCopy.contains(idLinkSource))
                    val.wValue = _listPaste.at(_listCopy.indexOf(idLinkSource)).indexElt;
                else
                    val.wValue = idLinkSource.indexElt;
            }
            else
                val.genValue = _source->get(idSource, champ_sfGenAmount).genValue;
            _destination->set(idDest, _source->get(idSource, champ_sfGenOper).sfGenValue, val);
        }
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

    // Correspondance des index et initialisation
    int *indMod = new int[_source->count(idSource)];
    for (int i = 0; i < _source->count(idSource); i++)
        indMod[i] = -1;

    for (int i = 0; i < _source->count(idSource); i++)
    {
        idSource.indexMod = i;
        if (!_source->get(idSource, champ_hidden).bValue)
        {
            idDest.indexMod = _destination->add(idDest);
            indMod[i] = idDest.indexMod;
            _destination->set(idDest, champ_modAmount,       _source->get(idSource, champ_modAmount));
            _destination->set(idDest, champ_sfModSrcOper,    _source->get(idSource, champ_sfModSrcOper));
            _destination->set(idDest, champ_sfModAmtSrcOper, _source->get(idSource, champ_sfModAmtSrcOper));
            _destination->set(idDest, champ_sfModDestOper,   _source->get(idSource, champ_sfModDestOper));
            if (_source->get(idSource, champ_sfModDestOper).wValue < 32768) // pas de lien
                _destination->set(idDest, champ_sfModTransOper,  _source->get(idSource, champ_sfModTransOper));
        }
    }

    // Mise en place des liens (les éléments cachés ayant disparus)
    Valeur val;
    for (int i = 0; i < _source->count(idSource); i++)
    {
        idSource.indexMod = i;
        if (!_source->get(idSource, champ_hidden).bValue)
        {
            if (_source->get(idSource, champ_sfModDestOper).wValue >= 32768)
            {
                idDest.indexMod = indMod[i];
                val.wValue = 32768 + indMod[_source->get(idSource, champ_sfModDestOper).wValue - 32768];
                _destination->set(idDest, champ_sfModDestOper, val);
            }
        }
    }

    delete [] indMod;
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
    msgBox.button(QMessageBox::Yes)->setText(QObject::trUtf8("&Remplacer"));
    msgBox.button(QMessageBox::YesAll)->setText(QObject::trUtf8("R&emplacer tout"));
    msgBox.button(QMessageBox::Save)->setText(QObject::trUtf8("&Dupliquer"));
    msgBox.button(QMessageBox::SaveAll)->setText(QObject::trUtf8("D&upliquer tout"));
    msgBox.button(QMessageBox::No)->setText(QObject::trUtf8("&Ignorer"));
    msgBox.button(QMessageBox::NoAll)->setText(QObject::trUtf8("I&gnorer tout"));
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
    int number = _destination->count(id);
    for (int i = number - 1; i >= 0; i--)
    {
        id.indexMod = i;
        if (_destination->get(id, champ_sfGenOper).sfGenValue != champ_wBank &&
                _destination->get(id, champ_sfGenOper).sfGenValue != champ_wPreset) // On garde bank et preset
            _destination->reset(id, _destination->get(id, champ_sfGenOper).sfGenValue);
    }

    // Suppression des modulateurs
    if (idDest.typeElement == elementInst)
        id.typeElement = elementInstMod;
    else
        id.typeElement = elementPrstMod;
    for (int i = 0; i < _destination->count(id); i++)
    {
        id.indexMod = i;
        if (!_destination->get(id, champ_hidden).bValue)
            _destination->remove(id);
    }

    // Suppression des éléments liés
    if (idDest.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    for (int i = 0; i < _destination->count(id); i++)
    {
        id.indexElt2 = i;
        if (!_destination->get(id, champ_hidden).bValue)
            _destination->remove(id);
    }
}

bool Duplicator::isGlobalEmpty(EltID id)
{
    // Nombre de paramètres présents
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
        id.typeElement = elementInstGen;
    else
        id.typeElement = elementPrstGen;
    bool isEmpty = _destination->count(id, false) == 0;

    // Nombre de modulateurs
    if (id.typeElement == elementInstGen)
        id.typeElement = elementInstMod;
    else
        id.typeElement = elementPrstMod;
    isEmpty = isEmpty && _destination->count(id, false) == 0;

    return isEmpty;
}

QString Duplicator::adaptName(QString nom, EltID idDest)
{
    QStringList listName;
    int nbElt = 0;
    if (idDest.typeElement == elementSmpl)
        nbElt = _initialSmplCount[idDest.indexSf2];
    else if (idDest.typeElement == elementInst)
        nbElt = _initialInstCount[idDest.indexSf2];
    else if (idDest.typeElement == elementPrst)
        nbElt = _initialPrstCount[idDest.indexSf2];
    for (int j = 0; j < nbElt; j++)
    {
        idDest.indexElt = j;
        if (!_destination->get(idDest, champ_hidden).bValue)
            listName << _destination->getQstr(idDest, champ_name);
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
