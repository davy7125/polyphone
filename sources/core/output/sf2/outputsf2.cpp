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

#include "outputsf2.h"
#include "sf2indexconverter.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include <QFile>
#include <QFileInfo>

OutputSf2::OutputSf2() : AbstractOutput() {}

void OutputSf2::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(options)

    // Check that we don't save over another soundfont already open
    EltID idSf2(elementSf2);
    foreach (int i, sm->getSiblings(idSf2))
    {
        if (i != sf2Index && QString::compare(sm->getQstr(EltID(elementSf2, i), champ_filenameInitial), fileName, Qt::CaseSensitive) == 0)
        {
            success = false;
            error = trUtf8("Please close file before overriding it.");
            return;
        }
    }

    // Do we override the current file?
    EltID id(elementSf2, sf2Index);
    if (sm->getQstr(id, champ_filenameForData) == fileName)
    {
        // Use a temporary file
        QString filenameTmp = fileName.left(fileName.length() - 4) + "_tmp";
        if (QFile(filenameTmp + ".sf2").exists())
        {
            int index = 1;
            while (QFile(filenameTmp + "-" + QString::number(index) + ".sf2").exists())
                index++;
            filenameTmp = filenameTmp + "-" + QString::number(index);
        }
        filenameTmp += ".sf2";

        // Save the file
        this->save(filenameTmp, sm, success, error, sf2Index);
        if (!success)
            return;

        // Delete the initial file and
        if (!QFile(fileName).remove())
        {
            error = trUtf8("Couldn't delete file \"%1\".").arg(fileName);
            success = false;
            sm->clearNewEditing();
            sm->markAsSaved(sf2Index);
            return;
        }

        // Rename the tmp file
        if (!QFile(filenameTmp).rename(fileName))
        {
            error = trUtf8("Couldn't rename file \"%1\".").arg(filenameTmp);
            success = false;
            sm->clearNewEditing();
            sm->markAsSaved(sf2Index);
            return;
        }

        // Update the source for data
        sm->set(id, champ_filenameInitial, fileName);
        sm->set(id, champ_filenameForData, fileName);
        id.typeElement = elementSmpl;
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt = i;
            sm->set(id, champ_filenameForData, fileName);
        }
    }
    else
    {
        // Just save the file
        this->save(fileName, sm, success, error, sf2Index);
    }

    sm->clearNewEditing();
    sm->markAsSaved(sf2Index);
}

void OutputSf2::save(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index)
{
    EltID id(elementSf2, sf2Index, 0, 0, 0);

    // Préparation de la sauvegarde
    SfVersionTag sfVersionTmp;
    quint32 dwTmp, dwTmp2;
    quint16 wTmp;
    quint8 byTmp;
    char charTmp;
    char tcharTmp[20];
    AttributeValue valTmp;

    // Modification du logiciel d'édition
    sm->set(id, champ_ISFT, QString("Polyphone"));

    //////////////////// TAILLES DES BLOCS ////////////////////

    quint32 taille_fichier, taille_info, taille_smpl, taille_pdta,
            taille_phdr, taille_pbag, taille_pmod, taille_pgen,
            taille_inst, taille_ibag, taille_imod, taille_igen,
            taille_shdr, taille_sm24;
    taille_info = 16; // INFO + champ ifil
    dwTmp = sm->getQstr(id, champ_ISNG).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
        taille_info += dwTmp2 + 8;
    else
        taille_info += 8 + 8;
    dwTmp = sm->getQstr(id, champ_name).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
        taille_info += dwTmp2 + 8;
    else
        taille_info += 10 + 8;
    dwTmp = sm->getQstr(id, champ_IROM).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    sfVersionTmp = sm->get(id, champ_IVER).sfVerValue;
    if (sfVersionTmp.wMinor != 0 || sfVersionTmp.wMajor != 0)
        taille_info += 12;
    dwTmp = sm->getQstr(id, champ_ICRD).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = sm->getQstr(id, champ_IENG).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = sm->getQstr(id, champ_IPRD).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = sm->getQstr(id, champ_ICOP).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = sm->getQstr(id, champ_ICMT).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 65536) dwTmp = 65536;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = sm->getQstr(id, champ_ISFT).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }

    taille_smpl = 12;
    EltID id2(elementSmpl, id.indexSf2);
    foreach (int i, sm->getSiblings(id2))
    {
        id2.indexElt = i;
        taille_smpl += 2 * (sm->get(id2, champ_dwLength).dwValue + 46); // 46 zeros supplémentaires
    }
    if (sm->get(id, champ_wBpsSave).wValue == 24)
    {
        // Sauvegarde 24 bits
        char T[20];
        sprintf(T, "%u", taille_smpl);
        taille_sm24 = (taille_smpl - 12) / 2 + 8;
        taille_sm24 += taille_sm24 % 2; // chiffre pair
    }
    else
    {
        // Sauvegarde 16 bits
        taille_sm24 = 0;
    }
    EltID id3(elementPrst, id.indexSf2);
    id.typeElement = elementPrst;
    taille_phdr = 38 + sm->getSiblings(id).count() * 38;

    id2.typeElement = elementPrstInst;
    taille_pbag = 4;
    taille_pmod = 10;
    taille_pgen = 4;

    // pour chaque preset
    foreach (int i, sm->getSiblings(id))
    {
        id2.indexElt = i;
        id3.typeElement = elementPrst;
        id3.indexElt = i;

        taille_pbag += 4; // bag global
        id3.typeElement = elementPrstMod;
        taille_pmod += 10 * sm->getSiblings(id3).count(); // mod globaux

        id3.typeElement = elementPrstGen;
        taille_pgen += 4 * sm->getSiblings(id3).count(); // gen globaux

        // pour chaque instrument lié
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            taille_pbag += 4; // 1 bag par instrument lié
            id3.indexElt2 = j;
            id3.typeElement = elementPrstInstMod;
            taille_pmod += 10 * sm->getSiblings(id3).count(); // mod par instrument

            id3.typeElement = elementPrstInstGen;
            taille_pgen += 4 * sm->getSiblings(id3).count(); // gen par instrument
        }
    }
    id.typeElement = elementInst;
    taille_inst = 22 + sm->getSiblings(id).count() * 22;

    id2.typeElement = elementInstSmpl;
    taille_ibag = 4;
    taille_imod = 10;
    taille_igen = 4;

    // pour chaque instrument
    foreach (int i, sm->getSiblings(id))
    {
        id2.indexElt = i;
        id3.typeElement = elementInst;
        id3.indexElt = i;
        taille_ibag += 4; // bag global
        id3.typeElement = elementInstMod;
        taille_imod += 10 * sm->getSiblings(id3).count(); // mod globaux

        id3.typeElement = elementInstGen;
        taille_igen += 4 * sm->getSiblings(id3).count(); // gen globaux
        // pour chaque sample lié
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            taille_ibag += 4; // 1 bag par sample lié
            id3.indexElt2 = j;
            id3.typeElement = elementInstSmplMod;
            taille_imod += 10 * sm->getSiblings(id3).count(); // mod par instrument

            id3.typeElement = elementInstSmplGen;
            taille_igen += 4 * sm->getSiblings(id3).count(); // gen par instrument
        }
    }
    id.typeElement = elementSmpl;
    taille_shdr = 46 + sm->getSiblings(id).count() * 46;

    taille_pdta = taille_phdr + taille_pbag + taille_pmod + taille_pgen +
            taille_inst + taille_ibag + taille_imod + taille_igen +
            taille_shdr + 19*4;

    taille_fichier = taille_info + taille_smpl + taille_sm24 + taille_pdta + 7*4;

    // Préparation de la sauvegarde
    id.typeElement = elementSmpl;
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    valTmp.sfVerValue = sfVersionTmp;

    // Mise à jour de la version
    id.typeElement = elementSf2;
    sm->set(id, champ_IFIL, valTmp);

    // Sauvegarde sous le nom fileName
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly))
    {
        success = false;
        error = trUtf8("Cannot create file \"%1\"").arg(fileName);
        return;
    }

    // entête
    fi.write("RIFF", 4);

    // taille du fichier -8 octets
    fi.write((char *)&taille_fichier, 4);
    fi.write("sfbk", 4);

    /////////////////////////////////////// BLOC INFO ///////////////////////////////////////
    fi.write("LIST", 4);
    fi.write((char *)&taille_info, 4);
    fi.write("INFO", 4);

    fi.write("ifil", 4); // version, champ obligatoire
    dwTmp = 4; fi.write((char *)&dwTmp, 4);
    id.typeElement = elementSf2;

    if (sm->get(id, champ_wBpsSave).wValue == 24)
    {
        sfVersionTmp.wMajor = 2;
        sfVersionTmp.wMinor = 4;
    }
    else
    {
        sfVersionTmp.wMajor = 2;
        sfVersionTmp.wMinor = 1;
    }
    fi.write((char *)&sfVersionTmp, 4);

    fi.write("isng", 4); // wavetable sound engine, champ obligatoire
    dwTmp = sm->getQstr(id, champ_ISNG).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
    {
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_ISNG).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }
    else
    {
        dwTmp2 = 8;
        fi.write((char *)&dwTmp2, 4);
        fi.write("Generic", 7);
        charTmp = '\0';
        fi.write(&charTmp, 1);
    }
    fi.write("INAM", 4); // nom du sf2, champ obligatoire
    dwTmp = sm->getQstr(id, champ_name).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
    {
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_name).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }
    else
    {
        dwTmp2 = 10;
        fi.write((char *)&dwTmp2, 4);
        fi.write("no title", 8);
        charTmp = '\0';
        fi.write(&charTmp, 1);
        fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_IROM).length(); // identification d'une table d'onde, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("irom", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_IROM).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    sfVersionTmp = sm->get(id, champ_IVER).sfVerValue; // révision de la table d'onde, champ optionnel
    if (sfVersionTmp.wMinor != 0 || sfVersionTmp.wMajor != 0)
    {
        fi.write("iver", 4);
        dwTmp = 4;
        fi.write((char *)&dwTmp, 4);
        fi.write((char *)&sfVersionTmp, 4);
    }

    dwTmp = sm->getQstr(id, champ_ICRD).length(); // date de création du sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICRD", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_ICRD).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_IENG).length(); // responsable de la création du sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("IENG", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_IENG).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_IPRD).length(); // produit de destination, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("IPRD", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_IPRD).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_ICOP).length(); // copyright, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICOP", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_ICOP).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_ICMT).length(); // commentaires, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 65536) dwTmp = 65536;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICMT", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_ICMT).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = sm->getQstr(id, champ_ISFT).length(); // outil d'édition sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ISFT", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(sm->getQstr(id, champ_ISFT).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    /////////////////////////////////////// BLOC SDTA ///////////////////////////////////////

    fi.write("LIST", 4);
    dwTmp = taille_smpl + taille_sm24;
    fi.write((char *)&dwTmp, 4);
    fi.write("sdta", 4);
    fi.write("smpl", 4);
    taille_smpl -= 12;
    fi.write((char *)&taille_smpl, 4);
    id2.typeElement = elementSmpl;
    dwTmp2 = 10 * 4 + taille_info;
    QByteArray baData;
    foreach (int i, sm->getSiblings(id2))
    {
        // Copy each sample
        id2.indexElt = i;
        dwTmp = 2 * sm->get(id2, champ_dwLength).dwValue;
        baData = sm->getData(id2, champ_sampleData16);
        fi.write(baData.data(), dwTmp);

        // Add 46 null sample points
        charTmp = '\0';
        for (int i = 0; i < 46 * 2; i++)
            fi.write(&charTmp, 1);
        dwTmp += 92;

        // Mise à jour des champs fileName, dwStart
        if (sm->get(id2, champ_dwStart16).dwValue != dwTmp2)
        {
            valTmp.dwValue = dwTmp2;
            sm->set(id2, champ_dwStart16, valTmp);
        }
        sm->set(id2, champ_filenameForData, fileName);
        dwTmp2 += dwTmp;
    }

    // 24 bits
    id.typeElement = elementSf2;
    if (sm->get(id, champ_wBpsSave).wValue == 24)
    {
        // Ajout données 24 bits
        fi.write("sm24", 4);
        taille_sm24 -= 8;
        fi.write((char *)&taille_sm24, 4);
        dwTmp2 = 12*4 + taille_info + taille_smpl;
        foreach (int i, sm->getSiblings(id2))
        {
            // copie de chaque sample
            id2.indexElt = i;
            dwTmp = sm->get(id2, champ_dwLength).dwValue;
            baData = sm->getData(id2, champ_sampleData24);
            fi.write(baData.data(), dwTmp);
            // ajout de 46 zeros
            charTmp = '\0';
            for (quint32 i = 0; i < 46; i++)
                fi.write(&charTmp, 1);
            dwTmp += 46;
            // Mise à jour du champ dwStart24
            if (sm->get(id2, champ_dwStart24).dwValue != dwTmp2)
            {
                valTmp.dwValue = dwTmp2;
                sm->set(id2, champ_dwStart24, valTmp);
            }
            dwTmp2 += dwTmp;
        }

        // 0 de fin
        if (dwTmp2 % 2)
        {
            charTmp = '\0';
            fi.write(&charTmp, sizeof(char));
        }
    }

    // Mise à jour wBpsFile
    if (sm->get(id, champ_wBpsSave).wValue == 24)
    {
        foreach (int i, sm->getSiblings(id2))
        {
            id2.indexElt = i;
            if (sm->get(id2, champ_bpsFile).wValue != 24)
            {
                valTmp.wValue = 24;
                sm->set(id2, champ_bpsFile, valTmp);
            }
        }
    }
    else
    {
        foreach (int i, sm->getSiblings(id2))
        {
            id2.indexElt = i;
            if (sm->get(id2, champ_bpsFile).wValue != 16)
            {
                valTmp.wValue = 16;
                sm->set(id2, champ_bpsFile, valTmp);
            }
        }
    }

    /////////////////////////////////////// BLOC PDTA ///////////////////////////////////////

    int nBag, nMod, nGen;
    fi.write("LIST", 4);
    fi.write((char *)&taille_pdta, 4);
    fi.write("pdta", 4);
    fi.write("phdr", 4);
    fi.write((char *)&taille_phdr, 4);

    // un bloc phdr par preset
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    nBag = 0;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        // Name
        dwTmp = sm->getQstr(id, champ_name).length();
        if (dwTmp > 20) dwTmp = 20;
        if (dwTmp != 0)
        {
            fi.write(sm->getQstr(id, champ_name).toLatin1());
            charTmp = '\0';
            for (quint32 i = 0; i < 20-dwTmp; i++)
                fi.write(&charTmp, 1);
        }
        else
        {
            dwTmp = sprintf(tcharTmp, "preset %d", i+1);
            fi.write(tcharTmp, dwTmp);
            charTmp = '\0';
            for (quint32 i = 0; i < 20-dwTmp; i++)
                fi.write(&charTmp, 1);
        }
        // wPreset
        wTmp = sm->get(id, champ_wPreset).wValue;
        fi.write((char *)&wTmp, 2);
        // wBank
        wTmp = sm->get(id, champ_wBank).wValue;
        fi.write((char *)&wTmp, 2);
        // wPresetBagNdx
        wTmp = nBag;
        fi.write((char *)&wTmp, 2);
        nBag++; // bag global
        nBag += sm->getSiblings(id2).count();

        // dwLibrary
        dwTmp = sm->get(id, champ_dwLibrary).dwValue;
        fi.write((char *)&wTmp, 4);
        // dwGenre
        dwTmp = sm->get(id, champ_dwGenre).dwValue;
        fi.write((char *)&wTmp, 4);
        // dwMorphology
        dwTmp = sm->get(id, champ_dwMorphology).dwValue;
        fi.write((char *)&wTmp, 4);
    }
    // phdr de fin (38 byte)
    fi.write("EOP", 3);
    charTmp = '\0';
    for (quint32 i = 0; i < 21; i++)
        fi.write(&charTmp, 1);
    // index bag de fin
    wTmp = nBag;
    fi.write((char *)&wTmp, 2);
    for (quint32 i = 0; i < 12; i++)
        fi.write(&charTmp, 1);

    fi.write("pbag", 4);
    fi.write((char*)&taille_pbag, 4);
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    nGen = 0;
    nMod = 0;

    // pour chaque preset
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;

        // bag global
        wTmp = nGen;
        fi.write((char *)&wTmp, 2);
        id2.typeElement = elementPrstGen;
        nGen += sm->getSiblings(id2).count();
        wTmp = nMod;
        fi.write((char *)&wTmp, 2);
        id2.typeElement = elementPrstMod;
        nMod += sm->getSiblings(id2).count();

        // un bag par instrument lié
        id2.typeElement = elementPrstInst;
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            wTmp = nGen;
            fi.write((char *)&wTmp, 2);
            id3.typeElement = elementPrstInstGen;
            id3.indexElt2 = j;
            nGen += sm->getSiblings(id3).count();
            wTmp = nMod;
            fi.write((char *)&wTmp, 2);
            id3.typeElement = elementPrstInstMod;
            nMod += sm->getSiblings(id3).count();
        }
    }

    // bag de fin
    wTmp = nGen;
    fi.write((char *)&wTmp, 2);
    wTmp = nMod;
    fi.write((char *)&wTmp, 2);

    fi.write("pmod", 4);
    fi.write((char *)&taille_pmod, 4);
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    SFModulator sfTmp;

    // pour chaque preset
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;

        // mods du bag global
        id3.typeElement = elementPrstMod;
        Sf2IndexConverter converterMod(id3);
        foreach (int k, sm->getSiblings(id3))
        {
            id3.indexMod = k;

            // écriture
            sfTmp = sm->get(id3, champ_sfModSrcOper).sfModValue;
            byTmp = sfTmp.Index + sfTmp.CC * 128;
            fi.write((char *)&byTmp, 1);
            byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
            fi.write((char *)&byTmp, 1);
            wTmp = converterMod.getIndexOf(sm->get(id3, champ_sfModDestOper).wValue, true);
            fi.write((char *)&wTmp, 2);
            wTmp = sm->get(id3, champ_modAmount).wValue;
            fi.write((char *)&wTmp, 2);
            sfTmp = sm->get(id3, champ_sfModAmtSrcOper).sfModValue;
            byTmp = sfTmp.Index + sfTmp.CC * 128;
            fi.write((char *)&byTmp, 1);
            byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
            fi.write((char *)&byTmp, 1);
            wTmp = sm->get(id3, champ_sfModTransOper).wValue;
            fi.write((char *)&wTmp, 2);
        }

        // pour chaque instrument associé
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            id3.indexElt2 = j;
            id3.typeElement = elementPrstInstMod;
            Sf2IndexConverter converterMod2(id3);

            // mods associés aux instruments
            foreach (int k, sm->getSiblings(id3))
            {
                id3.indexMod = k;

                // écriture
                sfTmp = sm->get(id3, champ_sfModSrcOper).sfModValue;
                byTmp = sfTmp.Index + sfTmp.CC * 128;
                fi.write((char *)&byTmp, 1);
                byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
                fi.write((char *)&byTmp, 1);
                wTmp = converterMod2.getIndexOf(sm->get(id3, champ_sfModDestOper).wValue, true);
                fi.write((char *)&wTmp, 2);
                wTmp = sm->get(id3, champ_modAmount).wValue;
                fi.write((char *)&wTmp, 2);
                sfTmp = sm->get(id3, champ_sfModAmtSrcOper).sfModValue;
                byTmp = sfTmp.Index + sfTmp.CC * 128;
                fi.write((char *)&byTmp, 1);
                byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
                fi.write((char *)&byTmp, 1);
                wTmp = sm->get(id3, champ_sfModTransOper).wValue;
                fi.write((char *)&wTmp, 2);
            }
        }
    }

    // mod de fin
    charTmp = '\0';
    for (quint32 i = 0; i < 10; i++)
        fi.write(&charTmp, 1);

    AttributeValue genTmp;
    fi.write("pgen", 4);
    fi.write((char *)&taille_pgen, 4);
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    Sf2IndexConverter converterInst(EltID(elementInst, id.indexSf2));

    // pour chaque preset
    foreach (int i , sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;
        id3.typeElement = elementPrstGen;

        // gens du bag global
        // - 1er gen : keyrange si présent
        // - 2ème gen : velocity si présent
        if (sm->isSet(id, champ_keyRange))
        {
            wTmp = champ_keyRange;
            fi.write((char *)&wTmp, 2);
            genTmp = sm->get(id, champ_keyRange);
            if (genTmp.rValue.byLo > 127)
                genTmp.rValue.byLo = 127;
            if (genTmp.rValue.byHi > 127)
                genTmp.rValue.byHi = 127;
            fi.write((char *)&genTmp, 2);
        }
        if (sm->isSet(id, champ_velRange))
        {
            wTmp = champ_velRange;
            fi.write((char *)&wTmp, 2);
            genTmp = sm->get(id, champ_velRange);
            if (genTmp.rValue.byLo > 127)
                genTmp.rValue.byLo = 127;
            if (genTmp.rValue.byHi > 127)
                genTmp.rValue.byHi = 127;
            fi.write((char *)&genTmp, 2);
        }
        foreach (int k, sm->getSiblings(id3))
        {
            if (k != champ_keyRange && k != champ_velRange && k != champ_instrument)
            {
                wTmp = k;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id, (AttributeType)k);
                fi.write((char *)&genTmp, 2);
            }
        }
        id3.typeElement = elementPrstInstGen;

        // pour chaque instrument associé
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            id3.indexElt2 = j;

            // gens associés aux instruments
            // - 1er gen : keyrange si présent
            // - 2ème gen : velocity si présent
            // - dernier gen : sample_ID
            if (sm->isSet(id2, champ_keyRange))
            {
                wTmp = champ_keyRange;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id2, champ_keyRange);
                if (genTmp.rValue.byLo > 127)
                    genTmp.rValue.byLo = 127;
                if (genTmp.rValue.byHi > 127)
                    genTmp.rValue.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            if (sm->isSet(id2, champ_velRange))
            {
                wTmp = champ_velRange;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id2, champ_velRange);
                if (genTmp.rValue.byLo > 127)
                    genTmp.rValue.byLo = 127;
                if (genTmp.rValue.byHi > 127)
                    genTmp.rValue.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            foreach (int k, sm->getSiblings(id3))
            {
                if (k != champ_keyRange && k != champ_velRange && k != champ_instrument)
                {
                    wTmp = k;
                    fi.write((char *)&wTmp, 2);
                    genTmp = sm->get(id2, (AttributeType)k);
                    fi.write((char *)&genTmp, 2);
                }
            }
            wTmp = champ_instrument;
            fi.write((char *)&wTmp, 2);
            genTmp.wValue = converterInst.getIndexOf(sm->get(id2, champ_instrument).wValue, false);
            fi.write((char *)&genTmp, 2);
        }
    }

    // gen de fin
    charTmp = '\0';
    for (quint32 i = 0; i < 4; i++)
        fi.write(&charTmp, 1);

    fi.write("inst", 4);
    fi.write((char *)&taille_inst, 4);

    // un bloc inst par instrument
    id.typeElement = elementInst;
    id2.typeElement = elementInstSmpl;
    nBag = 0;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;

        // Name
        dwTmp = sm->getQstr(id, champ_name).length();
        if (dwTmp > 20) dwTmp = 20;
        if (dwTmp != 0)
        {
            fi.write(sm->getQstr(id, champ_name).toLatin1());
            charTmp = '\0';
            for (quint32 iteration = 0; iteration < 20-dwTmp; iteration++)
                fi.write(&charTmp, 1);
        }
        else
        {
            dwTmp = sprintf(tcharTmp, "instrument %d", i+1);
            fi.write(tcharTmp, dwTmp);
            charTmp = '\0';
            for (quint32 iteration = 0; iteration < 20-dwTmp; iteration++)
                fi.write(&charTmp, 1);
        }

        // wInstBagNdx
        wTmp = nBag;
        fi.write((char *)&wTmp, 2);
        nBag++; // bag global
        nBag += sm->getSiblings(id2).count(); // un bag par sample lié
    }

    // inst de fin
    fi.write("EOI", 3);
    charTmp = '\0';
    for (quint32 iteration = 0; iteration < 17; iteration++)
        fi.write(&charTmp, 1);

    // index bag de fin
    wTmp = nBag;
    fi.write((char *)&wTmp, 2);

    fi.write("ibag", 4);
    fi.write((char *)&taille_ibag, 4);
    id.typeElement = elementInst;
    id2.typeElement = elementInstSmpl;
    nGen = 0;
    nMod = 0;

    // pour chaque instrument
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;

        // bag global
        wTmp = nGen;
        fi.write((char *)&wTmp, 2);
        id2.typeElement = elementInstGen;
        nGen += sm->getSiblings(id2).count();
        wTmp = nMod;
        fi.write((char *)&wTmp, 2);
        id2.typeElement = elementInstMod;
        nMod += sm->getSiblings(id2).count();

        // un bag par instrument lié
        id2.typeElement = elementInstSmpl;
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            wTmp = nGen;
            fi.write((char *)&wTmp, 2);
            id3.typeElement = elementInstSmplGen;
            id3.indexElt2 = j;
            nGen += sm->getSiblings(id3).count();
            wTmp = nMod;
            fi.write((char *)&wTmp, 2);
            id3.typeElement = elementInstSmplMod;
            nMod += sm->getSiblings(id3).count();
        }
    }

    // bag de fin
    wTmp = nGen;
    fi.write((char *)&wTmp, 2);
    wTmp = nMod;
    fi.write((char *)&wTmp, 2);

    fi.write("imod", 4);
    fi.write((char *)&taille_imod, 4);
    id.typeElement = elementInst;
    id2.typeElement = elementInstSmpl;

    // pour chaque instrument
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;

        // mods du bag global
        id3.typeElement = elementInstMod;
        Sf2IndexConverter converterMod(id3);
        foreach (int k, sm->getSiblings(id3))
        {
            id3.indexMod = k;

            // écriture
            sfTmp = sm->get(id3, champ_sfModSrcOper).sfModValue;
            byTmp = sfTmp.Index + sfTmp.CC * 128;
            fi.write((char *)&byTmp, 1);
            byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
            fi.write((char *)&byTmp, 1);
            wTmp = converterMod.getIndexOf(sm->get(id3, champ_sfModDestOper).wValue, true);
            fi.write((char *)&wTmp, 2);
            wTmp = sm->get(id3, champ_modAmount).wValue;
            fi.write((char *)&wTmp, 2);
            sfTmp = sm->get(id3, champ_sfModAmtSrcOper).sfModValue;
            byTmp = sfTmp.Index + sfTmp.CC * 128;
            fi.write((char *)&byTmp, 1);
            byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
            fi.write((char *)&byTmp, 1);
            wTmp = sm->get(id3, champ_sfModTransOper).wValue;
            fi.write((char *)&wTmp, 2);
        }

        // pour chaque sample associé
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            id3.indexElt2 = j;
            id3.typeElement = elementInstSmplMod;
            Sf2IndexConverter converterMod2(id3);

            // mods associés aux samples
            foreach (int k, sm->getSiblings(id3))
            {
                id3.indexMod = k;

                // écriture
                sfTmp = sm->get(id3, champ_sfModSrcOper).sfModValue;
                byTmp = sfTmp.Index + sfTmp.CC * 128;
                fi.write((char *)&byTmp, 1);
                byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
                fi.write((char *)&byTmp, 1);
                wTmp = converterMod2.getIndexOf(sm->get(id3, champ_sfModDestOper).wValue, true);
                fi.write((char *)&wTmp, 2);
                wTmp = sm->get(id3, champ_modAmount).wValue;
                fi.write((char *)&wTmp, 2);
                sfTmp = sm->get(id3, champ_sfModAmtSrcOper).sfModValue;
                byTmp = sfTmp.Index + sfTmp.CC * 128;
                fi.write((char *)&byTmp, 1);
                byTmp = sfTmp.isDescending + 2 * sfTmp.isBipolar + 4 * sfTmp.Type;
                fi.write((char *)&byTmp, 1);
                wTmp = sm->get(id3, champ_sfModTransOper).wValue;
                fi.write((char *)&wTmp, 2);
            }
        }
    }

    // mod de fin
    charTmp = '\0';
    for (quint32 iteration = 0; iteration < 10; iteration++)
        fi.write(&charTmp, 1);

    fi.write("igen", 4);
    fi.write((char *)&taille_igen, 4);
    id.typeElement = elementInst;
    id2.typeElement = elementInstSmpl;
    Sf2IndexConverter converterSmpl(EltID(elementSmpl, id.indexSf2));

    // pour chaque instrument
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id2.indexElt = i;
        id3.indexElt = i;
        id3.typeElement = elementInstGen;

        // gens du bag global
        // - 1er gen : keyrange si présent
        // - 2ème gen : velocity si présent
        if (sm->isSet(id, champ_keyRange))
        {
            wTmp = champ_keyRange;
            fi.write((char *)&wTmp, 2);
            genTmp = sm->get(id, champ_keyRange);
            if (genTmp.rValue.byLo > 127)
                genTmp.rValue.byLo = 127;
            if (genTmp.rValue.byHi > 127)
                genTmp.rValue.byHi = 127;
            fi.write((char *)&genTmp, 2);
        }
        if (sm->isSet(id, champ_velRange))
        {
            wTmp = champ_velRange;
            fi.write((char *)&wTmp, 2);
            genTmp = sm->get(id, champ_velRange);
            if (genTmp.rValue.byLo > 127)
                genTmp.rValue.byLo = 127;
            if (genTmp.rValue.byHi > 127)
                genTmp.rValue.byHi = 127;
            fi.write((char *)&genTmp, 2);
        }
        foreach (int k, sm->getSiblings(id3))
        {
            if (k != champ_keyRange && k != champ_velRange && k != champ_sampleID)
            {
                wTmp = k;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id, (AttributeType)k);
                fi.write((char *)&genTmp, 2);
            }
        }
        id3.typeElement = elementInstSmplGen;

        // pour chaque sample associé
        foreach (int j, sm->getSiblings(id2))
        {
            id2.indexElt2 = j;
            id3.indexElt2 = j;

            // gens associés aux samples
            // - 1er gen : keyrange si présent
            // - 2ème gen : velocity si présent
            // - dernier gen : sample_ID
            if (sm->isSet(id2, champ_keyRange))
            {
                wTmp = champ_keyRange;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id2, champ_keyRange);
                if (genTmp.rValue.byLo > 127)
                    genTmp.rValue.byLo = 127;
                if (genTmp.rValue.byHi > 127)
                    genTmp.rValue.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            if (sm->isSet(id2, champ_velRange))
            {
                wTmp = champ_velRange;
                fi.write((char *)&wTmp, 2);
                genTmp = sm->get(id2, champ_velRange);
                if (genTmp.rValue.byLo > 127)
                    genTmp.rValue.byLo = 127;
                if (genTmp.rValue.byHi > 127)
                    genTmp.rValue.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            foreach (int k, sm->getSiblings(id3))
            {
                if (k != champ_keyRange && k != champ_velRange && k != champ_sampleID)
                {
                    wTmp = k;
                    fi.write((char *)&wTmp, 2);
                    genTmp = sm->get(id2, (AttributeType)k);
                    fi.write((char *)&genTmp, 2);
                }
            }
            wTmp = champ_sampleID;
            fi.write((char *)&wTmp, 2);
            genTmp.wValue = converterSmpl.getIndexOf(sm->get(id2, champ_sampleID).wValue, false);
            fi.write((char *)&genTmp, 2);
        }
    }

    // gen de fin
    charTmp = '\0';
    for (quint32 iteration = 0; iteration < 4; iteration++)
        fi.write(&charTmp, 1);

    fi.write("shdr", 4);
    fi.write((char *)&taille_shdr, 4);

    // un bloc shdr par sample
    id.typeElement = elementSmpl;
    nBag = 0;
    dwTmp2 = 0;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;

        // Name
        dwTmp = sm->getQstr(id, champ_name).length();
        if (dwTmp > 20) dwTmp = 20;
        if (dwTmp != 0)
        {
            fi.write(sm->getQstr(id, champ_name).toLatin1());
            charTmp = '\0';
            for (quint32 iteration = 0; iteration < 20 - dwTmp; iteration++)
                fi.write(&charTmp, 1);
        }
        else
        {
            dwTmp = sprintf(tcharTmp, "sample %d", i+1);
            fi.write(tcharTmp, dwTmp);
            charTmp = '\0';
            for (quint32 iteration = 0; iteration < 20 - dwTmp; iteration++)
                fi.write(&charTmp, 1);
        }
        // dwStart
        fi.write((char *)&dwTmp2, 4);
        // dwEnd
        dwTmp = dwTmp2 + sm->get(id, champ_dwLength).dwValue;
        fi.write((char *)&dwTmp, 4);
        // dwStartLoop
        dwTmp = dwTmp2 + sm->get(id, champ_dwStartLoop).dwValue;
        fi.write((char *)&dwTmp, 4);
        // dwEndLoop
        dwTmp = dwTmp2 + sm->get(id, champ_dwEndLoop).dwValue;
        fi.write((char *)&dwTmp, 4);
        // on avance
        dwTmp2 += sm->get(id, champ_dwLength).dwValue + 46; // 46 zeros
        // dwSampleRate
        dwTmp = sm->get(id, champ_dwSampleRate).dwValue;
        fi.write((char *)&dwTmp, 4);
        // byOriginalPitch
        byTmp = sm->get(id, champ_byOriginalPitch).bValue;
        fi.write((char *)&byTmp, 1);
        // chPitchCorrection
        charTmp = sm->get(id, champ_chPitchCorrection).cValue;
        fi.write((char *)&charTmp, sizeof(char));
        // wSampleLink
        wTmp = converterSmpl.getIndexOf(sm->get(id, champ_wSampleLink).wValue, false);
        fi.write((char *)&wTmp, 2);
        // sfSampleType
        wTmp = sm->get(id, champ_sfSampleType).sfLinkValue;
        fi.write((char *)&wTmp, 2);
    }

    // shdr de fin
    fi.write("EOS", 3);
    charTmp = '\0';
    for (quint32 iteration = 0; iteration < 43; iteration++)
        fi.write(&charTmp, 1);

    // Fermeture du fichier
    fi.close();

    // Sauvegarde de fileName, wBpsInit
    id.typeElement = elementSf2;
    sm->set(id, champ_filenameInitial, fileName);
    sm->set(id, champ_filenameForData, fileName);
    sm->set(id, champ_wBpsInit, sm->get(id, champ_wBpsSave));

    success = true;
    error = "";
}
