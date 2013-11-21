/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#include "import_sfz.h"
#include "pile_sf2.h"

ImportSfz::ImportSfz(Pile_sf2 * sf2) :
    _sf2(sf2),
    _currentBloc(BLOC_UNKNOWN)
{
}

void ImportSfz::import(QString fileName, int &numSf2)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            // Lecture ligne par ligne
            QString line = in.readLine();

            // Découpage
            QStringList list = line.split(" ", QString::SkipEmptyParts);
            int length = list.size();
            for (int i = length - 1; i >= 1; i--)
            {
                if (!list.at(i).contains("[<>=]"))
                {
                    list[i-1] += " " + list[i];
                    list.removeAt(i);
                }
            }

            // Stockage
            for (int i = 0; i < list.size(); i++)
            {
                QString str = list.at(i);

                // Valide ?
                if (str.size() > 2)
                {
                    if (str.left(1) == "<" && str.right(1) == ">")
                        changeBloc(str.right(str.size()-1).left(str.size()-2));
                    else if (str.contains("="))
                    {
                        int index = str.indexOf("=");
                        QString opcode = str.left(index).toLower();
                        QString value = str.right(str.length() - index - 1).toLower();
                        if (opcode.size() && value.size())
                            addOpcode(opcode, value);
                    }
                }
            }
        }
        inputFile.close();

        // Création d'un sf2
        QFileInfo fileInfo(fileName);
        QString nom = fileInfo.completeBaseName().left(20);
        EltID idSf2(elementSf2, -1, -1, -1, -1);
        if (numSf2 == -1)
        {
            idSf2.indexSf2 = _sf2->add(idSf2);
            numSf2 = idSf2.indexSf2;
            _sf2->set(idSf2, champ_name, nom);
        }
        else
        {
            idSf2.indexSf2 = numSf2;
            _sf2->set(idSf2, champ_name, QObject::trUtf8("Import sfz"));
        }

        // Création d'un preset
        EltID idPrst = idSf2;
        idPrst.typeElement = elementPrst;
        idPrst.indexElt = _sf2->add(idPrst);
        _sf2->set(idPrst, champ_name, nom);

        // Création des instruments
        EltID idInst = idSf2;
        Valeur val;
        idInst.typeElement = elementInst;
        for (int i = 0; i < _listeEnsembles.size(); i++)
        {
            idInst.indexElt = _sf2->add(idInst);
            QString nomInst = nom;
            if (_listeEnsembles.size() > 10)
                nomInst = nom.left(17) + "-" + QString::number(i);
            else if (_listeEnsembles.size() > 1)
                nomInst = nom.left(18) + "-" + QString::number(i);
            else
                nomInst = nom;
            _sf2->set(idInst, champ_name, nomInst);

            EltID idInstSmpl = idInst;
            idInstSmpl.typeElement = elementInstSmpl;

            // Lien dans le preset
            EltID idPrstInst = idPrst;
            idPrstInst.typeElement = elementPrstInst;
            idPrstInst.indexElt2 = _sf2->add(idPrstInst);
            val.dwValue = idInst.indexElt;
            _sf2->set(idPrstInst, champ_instrument, val);

            // Remplissage de l'instrument et création des samples
            _listeEnsembles.at(i).decode(_sf2, idInst);

            // Détermination keyRange
            int nbInstSmpl = _sf2->count(idInstSmpl);
            int keyMin = 127;
            int keyMax = 0;
            for (int j = 0; j < nbInstSmpl; j++)
            {
                idInstSmpl.indexElt2 = j;
                rangesType range = _sf2->get(idInstSmpl, champ_keyRange).rValue;
                keyMin = qMin(keyMin, (int)range.byLo);
                keyMax = qMax(keyMax, (int)range.byHi);
            }
            if (keyMin > keyMax)
            {
                keyMin = 0;
                keyMax = 127;
            }
            val.rValue.byLo = keyMin;
            val.rValue.byHi = keyMax;
            _sf2->set(idPrstInst, champ_keyRange, val);
        }
    }
}

void ImportSfz::changeBloc(QString bloc)
{
    if (bloc == "group")
    {
        _currentBloc = BLOC_GROUP;
        _listeEnsembles << EnsembleGroupes();
    }
    else if (bloc == "region")
    {
        _currentBloc = BLOC_REGION;
        _listeEnsembles.last().newGroup();
    }
    else
        _currentBloc = BLOC_UNKNOWN;
}

void ImportSfz::addOpcode(QString opcode, QString value)
{
    if (_currentBloc == BLOC_GROUP || _currentBloc == BLOC_REGION)
        _listeEnsembles.last().addParam(opcode, value);
}

void EnsembleGroupes::decode(Pile_sf2 * sf2, EltID idInst) const
{
    // Remplissage des paramètres globaux
    _paramGlobaux.decode(sf2, idInst);

    // Lien avec samples
    EltID idInstSmpl = idInst;
    idInstSmpl.typeElement = elementInstSmpl;
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        if (_listeDivisions.at(i).containsSample())
        {
            // Création InstSmpl
            idInstSmpl.indexElt2 = sf2->add(idInstSmpl);

            // Remplissage paramètres de la division, avec ajout sample
            _listeDivisions.at(i).decode(sf2, idInstSmpl);
        }
    }
}

Parametre::Parametre(QString opcode, QString valeur) :
    _opcode(op_unknown),
    _intValue(0),
    _dblValue(0.)
{
    if (opcode == "sample")
    {
        _opcode = op_sample;
        _strValue = valeur;
    }
    else if (opcode == "key")
    {
        _opcode = op_key;
        _intValue = getNumNote(valeur);
    }
    else if (opcode == "lokey")
    {
        _opcode = op_keyMin;
        _intValue = getNumNote(valeur);
    }
    else if (opcode == "hikey")
    {
        _opcode = op_keyMax;
        _intValue = getNumNote(valeur);
    }
    else if (opcode == "lovel")
    {
        _opcode = op_velMin;
        _intValue = valeur.toInt();
    }
    else if (opcode == "hivel")
    {
        _opcode = op_velMax;
        _intValue = valeur.toInt();
    }
    else if (opcode == "pitch_keycenter")
    {
        _opcode = op_rootKey;
        _intValue = getNumNote(valeur);
    }
    else if (opcode == "group")
    {
        _opcode = op_exclusiveClass;
        _intValue = valeur.toInt();
    }
    else if (opcode == "tune")
    {
        _opcode = op_tuningFine;
        _intValue = valeur.toInt();
    }
    else if (opcode == "transpose")
    {
        _opcode = op_tuningCoarse;
        _intValue = valeur.toInt();
    }
    else
        qDebug() << "non pris en charge: " + opcode + " (" + valeur + ")";
}

int Parametre::getNumNote(QString noteStr)
{
    int note = noteStr.toInt();
    if (note == 0 && noteStr != "0" && noteStr.size() >= 2)
    {
        switch (noteStr.at(0).toAscii())
        {
        case 'c': note = 60; break;
        case 'd': note = 62; break;
        case 'e': note = 64; break;
        case 'f': note = 65; break;
        case 'g': note = 67; break;
        case 'a': note = 69; break;
        case 'b': note = 71; break;
        default : return -1; break;
        }
        noteStr = noteStr.right(noteStr.size() - 1);
        if (noteStr.at(0).toAscii() == '#')
        {
            note ++;
            noteStr = noteStr.right(noteStr.size() - 1);
        }
        else if (noteStr.at(0).toAscii() == 'b')
        {
            note --;
            noteStr = noteStr.right(noteStr.size() - 1);
        }

        int octave = noteStr.toInt();
        if ((octave == 0 && noteStr != "0") || noteStr.isEmpty())
            return -1;
        else
            note += (octave - 4) * 12;
    }
    return note;
}

void GroupeParametres::decode(Pile_sf2 * sf2, EltID idElt) const
{
    Valeur val;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        switch (_listeParam.at(i).getOpCode())
        {
        case Parametre::op_sample:
            break;
        case Parametre::op_key:
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            val.rValue.byHi = val.rValue.byLo;
            sf2->set(idElt, champ_keyRange, val);
            break;
        case Parametre::op_keyMin:
            if (sf2->isSet(idElt, champ_keyRange))
            {
                val = sf2->get(idElt, champ_keyRange);
                val.rValue.byLo = _listeParam.at(i).getIntValue();
            }
            else
            {
                val.rValue.byLo = _listeParam.at(i).getIntValue();
                val.rValue.byHi = 127;
            }
            sf2->set(idElt, champ_keyRange, val);
            break;
        case Parametre::op_keyMax:
            if (sf2->isSet(idElt, champ_keyRange))
            {
                val = sf2->get(idElt, champ_keyRange);
                val.rValue.byHi = _listeParam.at(i).getIntValue();
            }
            else
            {
                val.rValue.byLo = 0;
                val.rValue.byHi = _listeParam.at(i).getIntValue();
            }
            sf2->set(idElt, champ_keyRange, val);
            break;
        case Parametre::op_velMin:
            if (sf2->isSet(idElt, champ_velRange))
            {
                val = sf2->get(idElt, champ_velRange);
                val.rValue.byLo = _listeParam.at(i).getIntValue();
            }
            else
            {
                val.rValue.byLo = _listeParam.at(i).getIntValue();
                val.rValue.byHi = 127;
            }
            sf2->set(idElt, champ_velRange, val);
            break;
        case Parametre::op_velMax:
            if (sf2->isSet(idElt, champ_velRange))
            {
                val = sf2->get(idElt, champ_velRange);
                val.rValue.byHi = _listeParam.at(i).getIntValue();
            }
            else
            {
                val.rValue.byLo = 0;
                val.rValue.byHi = _listeParam.at(i).getIntValue();
            }
            sf2->set(idElt, champ_velRange, val);
            break;
        case Parametre::op_rootKey:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_overridingRootKey, val);
            break;
        case Parametre::op_exclusiveClass:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_exclusiveClass, val);
            break;
        case Parametre::op_tuningFine:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_fineTune, val);
            break;
        case Parametre::op_tuningCoarse:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_coarseTune, val);
            break;
        default:
            break;
        }
    }
}
