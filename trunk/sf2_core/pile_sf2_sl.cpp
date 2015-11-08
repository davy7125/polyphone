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

#include "pile_sf2.h"
#include <QFile>
#include <QFileInfo>
#include "oggconverter.h"
#include "sfont.h"

//////////// SF2 : SAVE / LOAD ////////////

void Pile_sf2::nouveau(QString name)
{
    EltID id(elementSf2, -1, 0, 0, 0);
    id.indexSf2 = this->add(id);

    // Initialisation de l'édition
    this->sf2->getElt(id.indexSf2)->numEdition = this->pileActions->getEdition(id.indexSf2) - 1;

    // Modification du nom
    this->set(id, champ_name, name, false);

    // Sélection dans l'arborescence
    if (tree)
    {
        tree->clearSelection();
        tree->select(id, true);
    }
}

int Pile_sf2::open(QString fileName)
{
    // ouverture d'un fichier soundfont
    // Valeur retour :
    // -1 : QMessageBox::warning(NULL, "Attention", "Fichier corrompu : utilisation des échantillons en qualité 16 bits.");
    // 0: ok
    // 1: format inconnu
    // 2: QMessageBox::warning(NULL, "Attention", "Le fichier est déjà chargé."));
    // 3: QMessageBox::warning(NULL, "Attention", "Impossible d'ouvrir le fichier.");
    // 4: QMessageBox::warning(NULL, "Attention", "Lecture impossible.");
    // 5: QMessageBox::warning(NULL, "Attention", "Le fichier est corrompu.");
    // 6: QMessageBox::warning(NULL, "Attention", "La somme de la taille des blocs ne donne pas la taille totale du fichier.");

    // Vérification que le fichier n'a pas déjà été chargé
    bool ok = 1;
    EltID id(elementSf2, -1, 0, 0, 0);
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexSf2 = i;
        if (!this->get(id, champ_hidden).bValue)
            if (QString::compare(this->getQstr(id, champ_filename), fileName, Qt::CaseSensitive) == 0)
                ok = 0;
    }
    if (!ok) return 2;

    FileType type = getFileType(fileName);
    switch (type)
    {
    case fileSf2: case fileSf3:
    {
        QFile fi(fileName);
        if (!fi.open(QIODevice::ReadOnly))
            return 3;
        QDataStream stream(&fi);
        int indexSf2 = -1;
        int valRet = this->open(fileName, &stream, indexSf2);
        fi.close();
        if (valRet <= 0 && type == fileSf2)
            this->storeEdition(indexSf2);
        return valRet;
    }break;
    case fileUnknown:
        return 1;
    }
    return 1;
}

/// Save as sf2 or sf3
/// If sf3, quality is 0 = low, 1 = middle, 2 = high (default is 1)
int Pile_sf2::save(int indexSf2, QString fileName, int quality)
{
    // Sauvegarde d'un fichier soundfont
    // Valeur retour :
    // 0: ok
    // 1: extension non connue
    // 2: fichier déjà ouvert, impossible de sauvegarder
    // 3: impossible d'enregistrer le fichier

    // Vérification qu'une autre soundfont ne s'appelle pas fileName
    int ret = 0;
    EltID id(elementSf2, -1, 0, 0, 0);
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexSf2 = i;
        if (!this->get(id, champ_hidden).bValue && i != indexSf2)
            if (QString::compare(this->getQstr(id, champ_filename), fileName, Qt::CaseSensitive) == 0)
                ret = 2;
    }

    if (ret == 0)
    {
        switch (getFileType(fileName))
        {
        case fileSf2:
            ret = this->sauvegarderSf2(indexSf2, fileName);
            break;
        case fileSf3: {
            // Temporary file
            QString fileNameSf2 = fileName.left(fileName.length() - 4) + "_tmp.sf2";
            if (QFile(fileNameSf2).exists()) {
                int index = 2;
                QString left = fileNameSf2.left(fileNameSf2.length() - 8);
                while (QFile(left + "-" + QString::number(index) + "_tmp.sf2").exists())
                    index++;
                fileNameSf2 = left + "-" + QString::number(index) + "_tmp.sf2";
            }

            // Save sf2 first
            if (this->sauvegarderSf2(indexSf2, fileNameSf2) == 0)
            {
                // Then create sf3
                SfConvert::SoundFont sf(fileNameSf2);
                if (sf.read())
                {
                    QFile fo(fileName);
                    if (fo.open(QIODevice::WriteOnly))
                    {
                        sf.write(&fo, quality);
                        fo.close();
                    }
                    else
                        ret = 3;
                }
                else
                    ret = 3;
            }
            else
                ret = 3;

            // Delete the sf2 temporary file
            QFile::remove(fileNameSf2);
        } break;
        case fileUnknown: default:
            ret = 1;
            break;
        }
    }

    return ret;
}

//////////// METHODES PRIVEES ////////////

Pile_sf2::FileType Pile_sf2::getFileType(QString fileName)
{
    QFileInfo fileInfo = fileName;
    QString ext = fileInfo.suffix().toLower();
    if (ext.compare("sf2") == 0)
        return fileSf2;
    else if (ext.compare("sf3") == 0)
        return fileSf3;
    else
        return fileUnknown;
}

int Pile_sf2::open(QString fileName, QDataStream * stream, int &indexSf2, bool copySamples)
{
    int openResult = 0;

    bool isSf3 = fileName.endsWith('3');

    //////////////////////////// CHARGEMENT ////////////////////////////////////////
    char buffer[65536];
    char bloc[5];
    qint64 taille, taille_info, taille_sdta, taille_pdta;

    /////////////////////////   ENTETE   //////////////////////
    // RIFF
    if (stream->readRawData(bloc, 4) != 4)
        return 4;

    bloc[4]='\0';
    if (strcmp("RIFF",bloc))
        return 5;

    // Taille totale du fichier - 8 octets
    taille = freadSize(stream);
    if (taille == 0)
        return 5;

    // Bloc 'sfbk'
    if (stream->readRawData(bloc, 4) != 4)
        return 4;

    bloc[4]='\0';
    if (strcmp("sfbk",bloc))
        return 5;

    /////////////////////////   INFO   //////////////////////
    // Bloc 'LIST'
    if (stream->readRawData(bloc, 4) != 4)
        return 5;

    bloc[4]='\0';
    if (strcmp("LIST",bloc))
        return 5;

    // Taille de la partie INFO
    taille_info = freadSize(stream);

    // limite en taille
    if (taille_info == 0)
        return 5;

    if (taille_info > 10000000)
        return 5;

    // EXTRACTION DES DONNEES DU BLOC INFO
    char *bloc_info = (char *)malloc((taille_info+1));
    if (stream->readRawData(bloc_info, taille_info) != taille_info)
    {
        free(bloc_info);
        return 4;
    }

    /////////////////////////   SDTA   //////////////////////
    // Bloc 'LIST'
    if (stream->readRawData(bloc, 4) != 4)
    {
        free(bloc_info);
        return 4;
    }
    bloc[4]='\0';
    if (strcmp("LIST",bloc))
    {
        free(bloc_info);
        return 5;
    }

    // Taille de la partie sdta
    taille_sdta = freadSize(stream);

    // limite en taille
    if (taille_sdta == 0)
    {
        free(bloc_info);
        return 5;
    }

    // mot sdta
    if (stream->readRawData(bloc, 4) != 4)
    {
        free(bloc_info);
        return 4;
    }
    bloc[4]='\0';
    if (strcmp("sdta",bloc))
    {
        free(bloc_info);
        return 5;
    }
    quint32 taille_smpl, taille_sm24;
    quint32 wSmpl, wSm24;

    // Blocs SMPL et SM24
    if (stream->readRawData(bloc, 4) != 4)
    {
        free(bloc_info);
        return 4;
    }
    if (strcmp("smpl", bloc))
    {
        // pas de sous-bloc, donc 16 bits
        taille_smpl = taille_sdta-4;
        taille_sm24 = 0;
        wSmpl = 20 + taille_info + 12;
        wSm24 = 0;
        stream->skipRawData(taille_smpl - 4); // en avant de taille_smpl - 4
    }
    else
    {
        // présence d'un sous-bloc smpl
        // taille du bloc smpl
        taille_smpl = freadSize(stream);
        wSmpl = 20 + taille_info + 20;
        stream->skipRawData(taille_smpl); // en avant de taille_smpl

        // bloc sm24 ?
        if (stream->readRawData(bloc, 4) != 4)
        {
            free(bloc_info);
            return 4;
        }
        if (strcmp("sm24",bloc))
        {
            // Pas de bloc sm24, en arrière de 4
            QIODevice * fi = stream->device();
            fi->seek(fi->pos() - 4);
            taille_sm24 = 0;
            wSm24 = 0;
        }
        else
        {
            // présence d'un bloc sm24
            taille_sm24 = freadSize(stream);
            if (taille_sm24 == taille_smpl/2 + ((taille_smpl/2) % 2))
            {
                wSm24 = 20 + taille_info + 20 + taille_sm24 + 8;
                stream->skipRawData(taille_sm24); // en avant de taille_sm24
            }
            else
            {
                // on ignore le bloc sm24
                openResult = -1; // For a warning
                taille_sm24 = 0;
                wSm24 = 0;
                stream->skipRawData(taille_sm24); // en avant de taille_sm24
            }
        }
    }

    /////////////////////////   PDTA   //////////////////////
    // Bloc 'LIST'
    if (stream->readRawData(bloc, 4) != 4)
    {
        free(bloc_info);
        return 4;
    }
    bloc[4]='\0';
    if (strcmp("LIST", bloc))
    {
        free(bloc_info);
        return 5;
    }

    // Taille de la partie pdta
    taille_pdta = freadSize(stream);

    // limite en taille
    if (taille_pdta == 0)
    {
        free(bloc_info);
        return 5;
    }
    if (taille_pdta > 10000000)
    {
        free(bloc_info);
        return 5;
    }

    // EXTRACTION DES DONNEES DU BLOC PDTA
    char *bloc_pdta = (char *)malloc((taille_pdta+1));
    if (stream->readRawData(bloc_pdta, taille_pdta) != taille_pdta)
    {
        free(bloc_info);
        free(bloc_pdta);
        return 4;
    }

    /////////////////////////   VERIFICATION GLOBALE   //////////////////////
    // Vérification de l'entete des 3 blocs
    if (bloc_info[0]!='I' || bloc_info[1]!='N' || bloc_info[2]!='F' || bloc_info[3]!='O' || \
            bloc_pdta[0]!='p' || bloc_pdta[1]!='d' || bloc_pdta[2]!='t' || bloc_pdta[3]!='a' )
    {
        free(bloc_info);
        free(bloc_pdta);
        return 5;
    }

    // Vérification des tailles
    if ((taille_info + taille_sdta + taille_pdta + 28) != taille)
    {
        free(bloc_info);
        free(bloc_pdta);
        return 6;
    }

    /////////////////////////   SEPARATION DES CHAMPS DANS LE BLOC PDTA   //////////////////////
    unsigned int pos = 4;

    // Séparation des champs PHDR, PBAG, PMOD, PGEN
    /////// PHDR
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("phdr",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        return 5;
    }
    quint32 taille_p = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_p % 38 != 0) // Doit être un multiple de 38
    {
        free(bloc_info);
        free(bloc_pdta);
        return 5;
    }
    char *bloc_pdta_phdr = (char *)malloc((taille_p+1));
    bloc_pdta_phdr = readdata(bloc_pdta_phdr, bloc_pdta, pos, taille_p);
    pos = pos + taille_p;

    /////// PBAG
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("pbag",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        return 5;
    }
    quint32 taille_b = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_b % 4 != 0) // Doit être un multiple de 4
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        return 5;
    }
    char *bloc_pdta_pbag = (char *)malloc((taille_b+1));
    bloc_pdta_pbag = readdata(bloc_pdta_pbag, bloc_pdta, pos, taille_b);
    pos = pos + taille_b;

    /////// PMOD
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("pmod",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        return 5;
    }
    quint32 taille_m = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_m % 10 != 0) // Doit être un multiple de 10
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        return 5;
    }
    char *bloc_pdta_pmod = (char *)malloc((taille_m+1));
    bloc_pdta_pmod = readdata(bloc_pdta_pmod, bloc_pdta, pos, taille_m);
    pos = pos + taille_m;

    /////// PGEN
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("pgen",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        return 5;
    }
    quint32 taille_g = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_g % 4 != 0) // Doit être un multiple de 4
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        return 5;
    }
    char *bloc_pdta_pgen = (char *)malloc((taille_g+1));
    bloc_pdta_pgen = readdata(bloc_pdta_pgen, bloc_pdta, pos, taille_g);
    pos = pos + taille_g;

    // Séparation des champs INST, IBAG, IMOD, IGEN
    /////// INST
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("inst",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        return 5;
    }
    quint32 taille_p2 = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_p2 % 22 != 0) // Doit être un multiple de 22
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        return 5;
    }
    char *bloc_pdta_inst = (char *)malloc((taille_p2+1));
    bloc_pdta_inst = readdata(bloc_pdta_inst, bloc_pdta, pos, taille_p2);
    pos = pos + taille_p2;

    /////// IBAG
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("ibag",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);;
        return 5;
    }

    quint32 taille_b2 = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_b2 % 4 != 0) // Doit être un multiple de 4
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        return 5;
    }
    char *bloc_pdta_ibag = (char *)malloc((taille_b2+1));
    bloc_pdta_ibag = readdata(bloc_pdta_ibag, bloc_pdta, pos, taille_b2);
    pos = pos + taille_b2;

    /////// IMOD
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("imod",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        return 5;
    }
    quint32 taille_m2 = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_m2 % 10 != 0) // Doit être un multiple de 10
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        return 5;
    }
    char *bloc_pdta_imod = (char *)malloc((taille_m2+1));
    bloc_pdta_imod = readdata(bloc_pdta_imod, bloc_pdta, pos, taille_m2);
    pos = pos + taille_m2;

    /////// IGEN
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("igen",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        free(bloc_pdta_imod);
        return 5;
    }
    quint32 taille_g2 = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille_g2 % 4 != 0) // Doit être un multiple de 4
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        free(bloc_pdta_imod);
        return 5;
    }
    char *bloc_pdta_igen = (char *)malloc((taille_g2+1));
    bloc_pdta_igen = readdata(bloc_pdta_igen, bloc_pdta, pos, taille_g2);
    pos = pos + taille_g2;

    /////// SHDR
    readbloc(bloc, bloc_pdta, pos);
    pos = pos + 4;
    if (strcmp("shdr",bloc))
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        free(bloc_pdta_imod);
        free(bloc_pdta_igen);
        return 5;
    }
    taille = readDWORD(bloc_pdta, pos);
    pos = pos + 4;
    if (taille % 46 != 0) // Doit être un multiple de 46
    {
        free(bloc_info);
        free(bloc_pdta);
        free(bloc_pdta_phdr);
        free(bloc_pdta_pbag);
        free(bloc_pdta_pmod);
        free(bloc_pdta_pgen);
        free(bloc_pdta_inst);
        free(bloc_pdta_ibag);
        free(bloc_pdta_imod);
        free(bloc_pdta_igen);
        return 5;
    }
    char *bloc_pdta_shdr = (char *)malloc((taille+1));
    bloc_pdta_shdr = readdata(bloc_pdta_shdr, bloc_pdta, pos, taille);

    // Fin de la séparation des blocs de pdta
    free(bloc_pdta);

    // Création d'un nouvel SF2
    EltID id(elementSf2, -1, 0, 0, 0);
    indexSf2 = this->add(id, false); // Nouvelle action
    EltID idSf2(elementSf2, indexSf2, 0, 0, 0);
    Valeur value;

    if (fileName.endsWith('2'))
        this->set(idSf2, champ_filename, fileName, false);

    ////////////////////   EXTRACTION DES CHAMPS DANS LE BLOC SHDR  ////////////////////

    id.typeElement = elementSmpl;
    id.indexSf2 = indexSf2;
    id.indexElt = -1;
    quint32 temp1;
    pos = 0;

    for (unsigned int i = 0; i < taille / 46 - 1; i++)
    {
        id.indexElt = this->add(id, false);
        this->set(id, champ_filename, fileName, false);
        this->set(id, champ_name, QString::fromLatin1(readdata(buffer, bloc_pdta_shdr, pos, 20)).trimmed(), false, i == taille / 46 - 2);

        // Configuration d'un sample
        value.bValue = readQUINT8(bloc_pdta_shdr, pos+40);
        this->set(id, champ_byOriginalPitch, value, false);
        value.cValue = bloc_pdta_shdr[pos+41];
        this->set(id, champ_chPitchCorrection, value, false);
        value.wValue = readWORD(bloc_pdta_shdr, pos+42);
        this->set(id, champ_wSampleLink, value, false);
        value.sfLinkValue = readSFSL(bloc_pdta_shdr, pos+44);
        this->set(id, champ_sfSampleType, value, false);
        temp1 = readDWORD(bloc_pdta_shdr, pos+20); // Start of the sample in the data bloc

        // Nombre de canaux, échantillonnage, début / fin de boucle
        value.wValue = 1;
        this->set(id, champ_wChannel, value, false);
        value.dwValue = readDWORD(bloc_pdta_shdr, pos+36);
        this->set(id, champ_dwSampleRate, value, false);
        if (readDWORD(bloc_pdta_shdr, pos+24) * (isSf3 ? 1: 2) > taille_smpl ||
                (wSm24 && (readWORD(bloc_pdta_shdr, pos+24) > taille_sm24)))
        {
            // Sample non valide
            value.dwValue = 0;
            this->set(id, champ_dwLength, value, false);
            this->set(id, champ_dwStart16, value, false);
            if (wSm24)
                this->set(id, champ_dwStart24, value, false);
            this->set(id, champ_dwStartLoop, value, false);
            this->set(id, champ_dwEndLoop, value, false);
        }
        else
        {
            value.dwValue = readDWORD(bloc_pdta_shdr, pos+24) - temp1; // End of the sample in the data bloc
            this->set(id, champ_dwLength, value, false);
            value.dwValue = temp1 * (isSf3 ? 1: 2) + wSmpl;
            this->set(id, champ_dwStart16, value, false);
            if (wSm24)
            {
                value.dwValue = temp1 * 2 + wSm24;
                this->set(id, champ_dwStart24, value, false);
                value.wValue = 24;
                this->set(id, champ_bpsFile, value, false);
            }
            else
            {
                value.dwValue = 0;
                this->set(id, champ_dwStart24, value, false);
                value.wValue = 16;
                this->set(id, champ_bpsFile, value, false);
            }

            value.dwValue = readDWORD(bloc_pdta_shdr, pos + 28) - (isSf3 ? 0 : temp1); // Start of the loop in the data bloc
            this->set(id, champ_dwStartLoop, value, false);
            value.dwValue = readDWORD(bloc_pdta_shdr, pos + 32) - (isSf3 ? 0 : temp1); // End of the loop in the data bloc
            this->set(id, champ_dwEndLoop, value, false);
        }

        // Récupération des données
        if (copySamples || isSf3)
        {
            // Remplissage des champ smpl et smpl24 à partir des données
            quint32 length = get(id, champ_dwLength).dwValue;
            QIODevice * fi = stream->device();
            fi->seek(get(id, champ_dwStart16).dwValue);
            QByteArray baData;
            if (isSf3)
            {
                QByteArray arrayTmp;
                arrayTmp.resize(length);
                stream->readRawData(arrayTmp.data(), length);

                OggConverter oggConverter(arrayTmp);
                baData = oggConverter.GetDecodedData();

                value.dwValue = baData.size() / 2;
                this->set(id, champ_dwLength, value, false);
            }
            else
            {
                baData.resize(2 * length);
                stream->readRawData(baData.data(), 2 * length);
            }
            this->set(id, champ_sampleData16, baData, false);
            if (wSm24)
            {
                fi->seek(get(id, champ_dwStart24).dwValue);
                baData.resize(length);
                stream->readRawData(baData.data(), length);
                this->set(id, champ_sampleData24, baData, false);
            }
        }
        else if (CONFIG_RAM)
        {
            // Chargement dans la ram
            value.wValue = 1;
            this->set(id, champ_ram, value, false);
        }

        pos = pos + 46;
    }

    // Mode 16 ou 24 bits au chargement
    id.typeElement = elementSf2;
    if (wSm24)
    {
        value.wValue = 24;
        this->set(id, champ_wBpsInit, value, false);
        this->set(id, champ_wBpsSave, value, false);
    }
    else
    {
        value.wValue = 16;
        this->set(id, champ_wBpsInit, value, false);
        this->set(id, champ_wBpsSave, value, false);
    }
    free(bloc_pdta_shdr);

    /////////////////////////   EXTRACTION DES CHAMPS DANS LE BLOC INFO   //////////////////////
    Valeur val;
    pos = 4;
    while (pos < taille_info)
    {
        // Nom du bloc
        readbloc(bloc, bloc_info, pos);
        pos = pos + 4;
        // Taille du bloc
        taille = readDWORD(bloc_info, pos);
        pos = pos + 4;
        // Remplissage des champs
        if (!strcmp(bloc, "ifil"))
        {
            val.sfVerValue = readSFVersionTag(bloc_info,pos);
            this->set(idSf2, champ_IFIL, val, false);
        }
        else if (!strcmp(bloc, "isng"))
            this->set(idSf2, champ_ISNG, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "INAM"))
            this->set(idSf2, champ_name, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "irom"))
            this->set(idSf2, champ_IROM, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "iver"))
        {
            val.sfVerValue = readSFVersionTag(bloc_info, pos);
            this->set(idSf2, champ_IVER, val, false);
        }
        else if (!strcmp(bloc, "ICRD"))
            this->set(idSf2, champ_ICRD, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "IENG"))
            this->set(idSf2, champ_IENG, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "IPRD"))
            this->set(idSf2, champ_IPRD, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "ICOP"))
            this->set(idSf2, champ_ICOP, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "ICMT"))
            this->set(idSf2, champ_ICMT, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);
        else if (!strcmp(bloc, "ISFT"))
            this->set(idSf2, champ_ISFT, QString::fromLatin1(readdata(buffer, bloc_info, pos, taille)).trimmed(), false);

        pos = pos + taille;
    }
    if (this->getQstr(idSf2, champ_name).isEmpty())
        this->set(idSf2, champ_name, trUtf8("sans titre"), false);
    free(bloc_info);

    /////////////////////////   EXTRACTION DES CHAMPS DANS LES BLOCS INST, IBAG, IMOD, IGEN   //////////////////////

    EltID id2;
    id.typeElement = elementInst;
    id.indexSf2 = indexSf2;
    id.indexElt = -1;
    id2.indexSf2 = indexSf2;
    quint16 bagmin, bagmax, modmin, modmax, genmin, genmax;
    int l;
    int global;

    // Remplissage de la sous-classe INST
    for (unsigned int i = 0; i < taille_p2 / 22 - 1; i++)
    {
        l = 0;
        id.indexElt = this->add(id, false);
        id2.indexElt = id.indexElt;
        this->set(id, champ_name, QString::fromLatin1(readdata(buffer, bloc_pdta_inst, 22*i, 20)).trimmed(), false,
                  i == taille_p2 / 22 - 2);

        // Indices des IBAG
        bagmin = readWORD(bloc_pdta_inst, 22 * i + 20);
        if (i < taille_p2/22 - 2)
            bagmax = readWORD(bloc_pdta_inst, 22 * (i+1) + 20);
        else
            bagmax = taille_b2/4 - 1;

        // Constitution de la liste des IBAG
        for (unsigned int j = bagmin; j < bagmax; j++)
        {
            // Indices des IMOD et IGEN
            modmin = readWORD(bloc_pdta_ibag, 4 * j + 2);
            genmin = readWORD(bloc_pdta_ibag, 4 * j);
            if (j < taille_b2/4 - 2)
            {
                modmax = readWORD(bloc_pdta_ibag, 4 * (j+1) + 2);
                genmax = readWORD(bloc_pdta_ibag, 4 * (j+1));
            }
            else
            {
                modmax = taille_m2 / 10 - 1;
                genmax = taille_g2 / 4 - 1;
            }

            // Bag global ?
            global = 1;
            for (unsigned int k = genmin; k < genmax; k++)
                if (readSFGenerator(bloc_pdta_igen, 4*k) == champ_sampleID)
                    global = 0;
            id2.indexElt2 = -1;

            // Constitution de la liste des IGEN
            if (global)
                id2.typeElement = elementInst;
            else
            {
                // ajout d'un sample à l'instrument
                id2.typeElement = elementInstSmpl;
                id2.indexElt2 = this->add(id2, false);
            }

            // Modification des propriétés
            for (unsigned int k = genmin; k < genmax; k++)
            {
                value.genValue = readGenAmountType(bloc_pdta_igen, 4*k+2);
                this->set(id2, readSFGenerator(bloc_pdta_igen, 4*k), value, false, (signed)j == bagmax - 1);
            }

            // Constitution de la liste des IMOD
            for (unsigned int k = modmin; k < modmax; k++)
            {
                id2.indexMod = -1;
                if (global)
                {
                    // ajout d'un mod à l'instrument
                    id2.typeElement = elementInstMod;
                    id2.indexMod = this->add(id2, false);
                }
                else
                {
                    // ajout d'un mod à un sample lié à l'instrument
                    id2.typeElement = elementInstSmplMod;
                    id2.indexMod = this->add(id2, false);
                }
                // Modification des propriétés
                value.sfModValue = readSFModulator(bloc_pdta_imod, 10*k);
                this->set(id2, champ_sfModSrcOper, value, false);
                value.sfGenValue = readSFGenerator(bloc_pdta_imod, 10*k+2);
                this->set(id2, champ_sfModDestOper, value, false);
                value.shValue = readSHORT(bloc_pdta_imod, 10*k+4);
                this->set(id2, champ_modAmount, value, false);
                value.sfModValue = readSFModulator(bloc_pdta_imod, 10*k+6);
                this->set(id2, champ_sfModAmtSrcOper, value, false);
                value.sfTransValue = readSFTransform(bloc_pdta_imod, 10*k+8);
                this->set(id2, champ_sfModTransOper, value, false);
                value.wValue = l;
                this->set(id2, champ_indexMod, value, false);
                l++;
            }
        }
    }
    free(bloc_pdta_inst);
    free(bloc_pdta_ibag);
    free(bloc_pdta_imod);
    free(bloc_pdta_igen);

    /////////////////////////   EXTRACTION DES CHAMPS DANS LES BLOCS PHDR, PBAG, PMOD, PGEN   //////////////////////
    id.typeElement = elementPrst;
    id.indexSf2 = indexSf2;
    id.indexElt = -1;
    for (unsigned int i = 0; i < taille_p / 38 - 1; i++)
    {
        l = 0;
        id.indexElt = this->add(id, false);
        id2.indexElt = id.indexElt;
        this->set(id, champ_name, QString::fromLatin1(readdata(buffer, bloc_pdta_phdr, 38*i, 20)).trimmed(), false,
                  i == taille_p / 38 - 2);
        value.wValue = readWORD(bloc_pdta_phdr, 38*i+20);
        this->set(id, champ_wPreset, value, false);
        value.wValue = readWORD(bloc_pdta_phdr, 38*i+22);
        this->set(id, champ_wBank, value, false);
        value.dwValue = readDWORD(bloc_pdta_phdr, 38*i+26);
        this->set(id, champ_dwLibrary, value, false);
        value.dwValue = readDWORD(bloc_pdta_phdr, 38*i+30);
        this->set(id, champ_dwGenre, value, false);
        value.dwValue = readDWORD(bloc_pdta_phdr, 38*i+34);
        this->set(id, champ_dwMorphology, value, false);
        // Indices des PBAG
        bagmin = readWORD(bloc_pdta_phdr, 38 * i + 24);
        if (i < taille_p/38 - 2)
            bagmax = readWORD(bloc_pdta_phdr, 38 * (i+1) + 24);
        else
            bagmax = taille_b/4 - 1;
        // Constitution de la liste des PBAG
        for (unsigned int j = bagmin; j < bagmax; j++)
        {
            // Indices des PMOD et PGEN
            modmin = readWORD(bloc_pdta_pbag, 4 * j + 2);
            genmin = readWORD(bloc_pdta_pbag, 4 * j);
            if (j < taille_b/4 - 2)
            {
                modmax = readWORD(bloc_pdta_pbag, 4 * (j+1) + 2);
                genmax = readWORD(bloc_pdta_pbag, 4 * (j+1));
            }
            else
            {
                modmax = taille_m/10-1;
                genmax = taille_g/4-1;
            }
            // Bag global ?
            global = 1;
            for (unsigned int k = genmin; k < genmax; k++)
                if (readSFGenerator(bloc_pdta_pgen, 4*k) == champ_instrument)
                    global = 0;
            // Constitution de la liste des PGEN
            id2.indexElt2 = -1;
            if (global) id2.typeElement = elementPrst;
            else
            {
                // ajout d'un sample à l'instrument
                id2.typeElement = elementPrstInst;
                id2.indexElt2 = this->add(id2, false);
            }
            // Modification des propriétés
            for (unsigned int k = genmin; k < genmax; k++)
            {
                value.genValue = readGenAmountType(bloc_pdta_pgen, 4*k+2);
                this->set(id2, readSFGenerator(bloc_pdta_pgen, 4*k), value, false, (signed)j == bagmax - 1);
            }
            // Constitution de la liste des PMOD
            for (unsigned int k = modmin; k < modmax; k++)
            {
                id2.indexMod = -1;
                if (global)
                {
                    // ajout d'un mod au preset
                    id2.typeElement = elementPrstMod;
                    id2.indexMod = this->add(id2, false);
                }
                else
                {
                    // ajout d'un mod à un instrument lié au preset
                    id2.typeElement = elementPrstInstMod;
                    id2.indexMod = this->add(id2, false);
                }
                // Modification des propriétés
                value.sfModValue = readSFModulator(bloc_pdta_pmod, 10*k);
                this->set(id2, champ_sfModSrcOper, value, false);
                value.sfGenValue = readSFGenerator(bloc_pdta_pmod, 10*k+2);
                this->set(id2, champ_sfModDestOper, value, false);
                value.shValue = readSHORT(bloc_pdta_pmod, 10*k+4);
                this->set(id2, champ_modAmount, value, false);
                value.sfModValue = readSFModulator(bloc_pdta_pmod, 10*k+6);
                this->set(id2, champ_sfModAmtSrcOper, value, false);
                value.sfTransValue = readSFTransform(bloc_pdta_pmod, 10*k+8);
                this->set(id2, champ_sfModTransOper, value, false);
                value.wValue = l;
                this->set(id2, champ_indexMod, value, false);
                l++;
            }
        }
    }
    free(bloc_pdta_phdr);
    free(bloc_pdta_pbag);
    free(bloc_pdta_pmod);
    free(bloc_pdta_pgen);

    id.typeElement = elementSf2;
    if (tree)
    {
        tree->clearSelection();
        tree->select(id, true);
    }

    return openResult;
}

int Pile_sf2::sauvegarderSf2(int indexSf2, QString fileName)
{
    EltID id(elementSf2, indexSf2, 0, 0, 0);

    // Préparation de la sauvegarde
    sfVersionTag sfVersionTmp;
    quint32 dwTmp, dwTmp2;
    quint16 wTmp;
    quint8 byTmp;
    char charTmp;
    char tcharTmp[20];
    Valeur valTmp;

    // Modification du logiciel d'édition
    this->set(id, champ_ISFT, QString("Polyphone"), 0);

    //////////////////// TAILLES DES BLOCS ////////////////////

    quint32 taille_fichier, taille_info, taille_smpl, taille_pdta,
            taille_phdr, taille_pbag, taille_pmod, taille_pgen,
            taille_inst, taille_ibag, taille_imod, taille_igen,
            taille_shdr, taille_sm24;
    taille_info = 16; // INFO + champ ifil
    dwTmp = this->getQstr(id, champ_ISNG).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
        taille_info += dwTmp2 + 8;
    else
        taille_info += 8 + 8;
    dwTmp = this->getQstr(id, champ_name).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
        taille_info += dwTmp2 + 8;
    else
        taille_info += 10 + 8;
    dwTmp = this->getQstr(id, champ_IROM).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    sfVersionTmp = this->get(id, champ_IVER).sfVerValue;
    if (sfVersionTmp.wMinor != 0 || sfVersionTmp.wMajor != 0)
        taille_info += 12;
    dwTmp = this->getQstr(id, champ_ICRD).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = this->getQstr(id, champ_IENG).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = this->getQstr(id, champ_IPRD).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = this->getQstr(id, champ_ICOP).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = this->getQstr(id, champ_ICMT).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 65536) dwTmp = 65536;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }
    dwTmp = this->getQstr(id, champ_ISFT).length();
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        taille_info += dwTmp2 + 8;
    }

    taille_smpl = 12;
    EltID id2(elementSmpl, id.indexSf2, 0, 0, 0);
    for (int i = 0; i < this->count(id2); i++)
    {
        id2.indexElt = i;
        if (!this->get(id2, champ_hidden).bValue)
            taille_smpl += 2 * (this->get(id2, champ_dwLength).dwValue + 46); // 46 zeros supplémentaires
    }
    if (this->get(id, champ_wBpsSave).wValue == 24)
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
    EltID id3(elementPrst, id.indexSf2, 0, 0, 0);
    id.typeElement = elementPrst;
    taille_phdr = 38;
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
            taille_phdr += 38;
    }
    id2.typeElement = elementPrstInst;
    taille_pbag = 4;
    taille_pmod = 10;
    taille_pgen = 4;

    // pour chaque preset
    for (int i = 0; i < this->count(id); i++)
    {
        id2.indexElt = i;
        id3.typeElement = elementPrst;
        id3.indexElt = i;
        if (!this->get(id3, champ_hidden).bValue)
        {
            taille_pbag += 4; // bag global
            id3.typeElement = elementPrstMod;
            for (int j = 0; j < this->count(id3); j++)
            {
                id3.indexMod = j;
                if (!this->get(id3, champ_hidden).bValue)
                    taille_pmod += 10; // mod globaux
            }
            id3.typeElement = elementPrstGen;
            taille_pgen += 4*this->count(id3); // gen globaux
            // pour chaque instrument lié
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    taille_pbag += 4; // 1 bag par instrument lié
                    id3.indexElt2 = j;
                    id3.typeElement = elementPrstInstMod;
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                            taille_pmod += 10; // mod par instrument
                    }
                    id3.typeElement = elementPrstInstGen;
                    taille_pgen += 4*this->count(id3); // gen par instrument
                }
            }
        }
    }
    id.typeElement = elementInst;
    taille_inst = 22;
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
            taille_inst += 22;
    }
    id2.typeElement = elementInstSmpl;
    taille_ibag = 4;
    taille_imod = 10;
    taille_igen = 4;

    // pour chaque instrument
    for (int i = 0; i < this->count(id); i++)
    {
        id2.indexElt = i;
        id3.typeElement = elementInst;
        id3.indexElt = i;
        if (!this->get(id3, champ_hidden).bValue)
        {
            taille_ibag += 4; // bag global
            id3.typeElement = elementInstMod;
            for (int j = 0; j < this->count(id3); j++)
            {
                id3.indexMod = j;
                if (!this->get(id3, champ_hidden).bValue)
                    taille_imod += 10; // mod globaux
            }
            id3.typeElement = elementInstGen;
            taille_igen += 4*this->count(id3); // gen globaux
            // pour chaque sample lié
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    taille_ibag += 4; // 1 bag par sample lié
                    id3.indexElt2 = j;
                    id3.typeElement = elementInstSmplMod;
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                            taille_imod += 10; // mod par instrument
                    }
                    id3.typeElement = elementInstSmplGen;
                    taille_igen += 4*this->count(id3); // gen par instrument
                }
            }
        }
    }
    id.typeElement = elementSmpl;
    taille_shdr = 46;
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
            taille_shdr += 46;
    }

    taille_pdta = taille_phdr + taille_pbag + taille_pmod + taille_pgen +
            taille_inst + taille_ibag + taille_imod + taille_igen +
            taille_shdr + 19*4;

    taille_fichier = taille_info + taille_smpl + taille_sm24 + taille_pdta + 7*4;

    // Préparation de la sauvegarde : chargement dans la ram de TOUS les samples
    id.typeElement = elementSmpl;
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        valTmp.wValue = 1;
        this->set(id, champ_ram, valTmp, false);
    }
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    valTmp.sfVerValue = sfVersionTmp;

    // Mise à jour de la version
    id.typeElement = elementSf2;
    this->set(id, champ_IFIL, valTmp, false);

    // Sauvegarde sous le nom fileName
    QFile fi(fileName);
    if (!fi.open(QIODevice::WriteOnly))
        return 3; // ouvre et écrase

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

    // sfVersionTmp = this->get(id, champ_IFIL).sfVerValue; // Message d'erreur dans Viena
    if (this->get(id, champ_wBpsSave).wValue == 24)
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
    dwTmp = this->getQstr(id, champ_ISNG).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
    {
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_ISNG).toLatin1());
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
    dwTmp = this->getQstr(id, champ_name).length();
    if (dwTmp > 255) dwTmp = 255;
    dwTmp2 = dwTmp + 2 - (dwTmp)%2;
    if (dwTmp != 0)
    {
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_name).toLatin1());
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

    dwTmp = this->getQstr(id, champ_IROM).length(); // identification d'une table d'onde, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("irom", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_IROM).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    sfVersionTmp = this->get(id, champ_IVER).sfVerValue; // révision de la table d'onde, champ optionnel
    if (sfVersionTmp.wMinor != 0 || sfVersionTmp.wMajor != 0)
    {
        fi.write("iver", 4);
        dwTmp = 4;
        fi.write((char *)&dwTmp, 4);
        fi.write((char *)&sfVersionTmp, 4);
    }

    dwTmp = this->getQstr(id, champ_ICRD).length(); // date de création du sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICRD", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_ICRD).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = this->getQstr(id, champ_IENG).length(); // responsable de la création du sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("IENG", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_IENG).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = this->getQstr(id, champ_IPRD).length(); // produit de destination, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("IPRD", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_IPRD).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = this->getQstr(id, champ_ICOP).length(); // copyright, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICOP", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_ICOP).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = this->getQstr(id, champ_ICMT).length(); // commentaires, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 65536) dwTmp = 65536;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ICMT", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_ICMT).toLatin1());
        charTmp = '\0';
        for (quint32 i = 0; i < dwTmp2-dwTmp; i++)
            fi.write(&charTmp, 1);
    }

    dwTmp = this->getQstr(id, champ_ISFT).length(); // outil d'édition sf2, champ optionnel
    if (dwTmp > 0)
    {
        if (dwTmp > 255) dwTmp = 255;
        dwTmp2 = dwTmp + 2 - (dwTmp)%2;
        fi.write("ISFT", 4);
        fi.write((char *)&dwTmp2, 4);
        fi.write(this->getQstr(id, champ_ISFT).toLatin1());
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
    dwTmp2 = 10*4 + taille_info;
    QByteArray baData;
    for (int i = 0; i < this->count(id2); i++)
    {
        // copie de chaque sample
        id2.indexElt = i;
        if (!this->get(id2, champ_hidden).bValue)
        {
            dwTmp = 2 * this->get(id2, champ_dwLength).dwValue;
            baData = this->getData(id2, champ_sampleData16);
            fi.write(baData.data(), dwTmp);

            // ajout de 46 zeros (sample de 2 valeurs)
            charTmp = '\0';
            for (int i = 0; i < 46 * 2; i++)
                fi.write(&charTmp, 1);
            dwTmp += 92;

            // Mise à jour des champs fileName, dwStart
            if (this->get(id2, champ_dwStart16).dwValue != dwTmp2)
            {
                valTmp.dwValue = dwTmp2;
                this->set(id2, champ_dwStart16, valTmp, false);
            }
            if (this->getQstr(id2, champ_filename).compare(fileName) != 0)
                this->set(id2, champ_filename, fileName, false);
            dwTmp2 += dwTmp;
        }
    }

    // 24 bits
    dwTmp2 = 11*4 + taille_info + taille_smpl;
    id.typeElement = elementSf2;
    if (this->get(id, champ_wBpsSave).wValue == 24)
    {
        // Ajout données 24 bits
        fi.write("sm24", 4);
        taille_sm24 -= 8;
        fi.write((char *)&taille_sm24, 4);
        for (int i = 0; i < this->count(id2); i++)
        {
            // copie de chaque sample
            id2.indexElt = i;
            if (!this->get(id2, champ_hidden).bValue)
            {
                dwTmp = this->get(id2, champ_dwLength).dwValue;
                baData = this->getData(id2, champ_sampleData24);
                fi.write(baData.data(), dwTmp);
                // ajout de 46 zeros
                charTmp = '\0';
                for (quint32 i = 0; i < 46; i++)
                    fi.write(&charTmp, 1);
                dwTmp += 46;
                // Mise à jour du champ dwStart24
                if (this->get(id2, champ_dwStart24).dwValue != dwTmp2)
                {
                    valTmp.dwValue = dwTmp2;
                    this->set(id2, champ_dwStart24, valTmp, false);
                }
                dwTmp2 += dwTmp;
            }
        }

        // 0 de fin
        if (dwTmp2 % 2)
        {
            charTmp = '\0';
            fi.write(&charTmp, sizeof(char));
        }
    }

    // Mise à jour wBpsFile
    if (this->get(id, champ_wBpsSave).wValue == 24)
    {
        for (int i = 0; i < this->count(id2); i++)
        {
            id2.indexElt = i;
            if (!this->get(id2, champ_hidden).bValue)
            {
                if (this->get(id2, champ_bpsFile).wValue != 24)
                {
                    valTmp.wValue = 24;
                    this->set(id2, champ_bpsFile, valTmp, false);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < this->count(id2); i++)
        {
            id2.indexElt = i;
            if (!this->get(id2, champ_hidden).bValue)
            {
                if (this->get(id2, champ_bpsFile).wValue != 16)
                {
                    valTmp.wValue = 16;
                    this->set(id2, champ_bpsFile, valTmp, false);
                }
            }
        }
    }

    // Libération de la RAM
    if (!CONFIG_RAM)
    {
        id.typeElement = elementSmpl;
        for (int i = 0; i < this->count(id); i++)
        {
            id.indexElt = i;
            if (!this->get(id, champ_hidden).bValue)
            {
                valTmp.wValue = 0;
                this->set(id, champ_ram, valTmp, false);
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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            // Name
            dwTmp = this->getQstr(id, champ_name).length();
            if (dwTmp > 20) dwTmp = 20;
            if (dwTmp != 0)
            {
                fi.write(this->getQstr(id, champ_name).toLatin1());
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
            wTmp = this->get(id, champ_wPreset).wValue;
            fi.write((char *)&wTmp, 2);
            // wBank
            wTmp = this->get(id, champ_wBank).wValue;
            fi.write((char *)&wTmp, 2);
            // wPresetBagNdx
            wTmp = nBag;
            fi.write((char *)&wTmp, 2);
            nBag++; // bag global
            for (int j = 0; j < count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                    nBag++; // un bag par instrument lié
            }
            // dwLibrary
            dwTmp = this->get(id, champ_dwLibrary).dwValue;
            fi.write((char *)&wTmp, 4);
            // dwGenre
            dwTmp = this->get(id, champ_dwGenre).dwValue;
            fi.write((char *)&wTmp, 4);
            // dwMorphology
            dwTmp = this->get(id, champ_dwMorphology).dwValue;
            fi.write((char *)&wTmp, 4);
        }
    }
    // phdr de fin
    fi.write("EOP", 3);
    charTmp = '\0';
    fi.write(&charTmp, 21);
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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // bag global
            wTmp = nGen;
            fi.write((char *)&wTmp, 2);
            id2.typeElement = elementPrstGen;
            nGen += this->count(id2);
            wTmp = nMod;
            fi.write((char *)&wTmp, 2);
            id2.typeElement = elementPrstMod;
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexMod = j;
                if (!this->get(id2, champ_hidden).bValue)
                    nMod++;
            }

            // un bag par instrument lié
            id2.typeElement = elementPrstInst;
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    wTmp = nGen;
                    fi.write((char *)&wTmp, 2);
                    id3.typeElement = elementPrstInstGen;
                    id3.indexElt2 = j;
                    nGen += this->count(id3);
                    wTmp = nMod;
                    fi.write((char *)&wTmp, 2);
                    id3.typeElement = elementPrstInstMod;
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                            nMod++;
                    }
                }
            }
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
    ConvertMod *converterMod = NULL;

    // pour chaque preset
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // mods du bag global
            id3.typeElement = elementPrstMod;
            converterMod = new ConvertMod(this, id3);
            for (int k = 0; k < this->count(id3); k++)
            {
                id3.indexMod = k;
                if (!this->get(id3, champ_hidden).bValue)
                {
                    // écriture
                    sfTmp = this->get(id3, champ_sfModSrcOper).sfModValue;
                    byTmp = sfTmp.Index + sfTmp.CC * 128;
                    fi.write((char *)&byTmp, 1);
                    byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                    fi.write((char *)&byTmp, 1);
                    wTmp = converterMod->calculDestIndex(this->get(id3, champ_sfModDestOper).wValue);
                    fi.write((char *)&wTmp, 2);
                    wTmp = this->get(id3, champ_modAmount).wValue;
                    fi.write((char *)&wTmp, 2);
                    sfTmp = this->get(id3, champ_sfModAmtSrcOper).sfModValue;
                    byTmp = sfTmp.Index + sfTmp.CC * 128;
                    fi.write((char *)&byTmp, 1);
                    byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                    fi.write((char *)&byTmp, 1);
                    wTmp = this->get(id3, champ_sfModTransOper).wValue;
                    fi.write((char *)&wTmp, 2);
                }
            }
            delete converterMod;

            // pour chaque instrument associé
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id3.indexElt2 = j;
                    id3.typeElement = elementPrstInstMod;
                    converterMod = new ConvertMod(this, id3);

                    // mods associés aux instruments
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                        {
                            // écriture
                            sfTmp = this->get(id3, champ_sfModSrcOper).sfModValue;
                            byTmp = sfTmp.Index + sfTmp.CC * 128;
                            fi.write((char *)&byTmp, 1);
                            byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                            fi.write((char *)&byTmp, 1);
                            wTmp = converterMod->calculDestIndex(this->get(id3, champ_sfModDestOper).wValue);
                            fi.write((char *)&wTmp, 2);
                            wTmp = this->get(id3, champ_modAmount).wValue;
                            fi.write((char *)&wTmp, 2);
                            sfTmp = this->get(id3, champ_sfModAmtSrcOper).sfModValue;
                            byTmp = sfTmp.Index + sfTmp.CC * 128;
                            fi.write((char *)&byTmp, 1);
                            byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                            fi.write((char *)&byTmp, 1);
                            wTmp = this->get(id3, champ_sfModTransOper).wValue;
                            fi.write((char *)&wTmp, 2);
                        }
                    }
                    delete converterMod;
                }
            }
        }
    }

    // mod de fin
    charTmp = '\0';
    for (quint32 i = 0; i < 10; i++)
        fi.write(&charTmp, 1);

    genAmountType genTmp;
    fi.write("pgen", 4);
    fi.write((char *)&taille_pgen, 4);
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    ConvertInst *converterInst = new ConvertInst(this, id.indexSf2);

    // pour chaque preset
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;
            id3.typeElement = elementPrstGen;

            // gens du bag global
            // - 1er gen : keyrange si présent
            // - 2ème gen : velocity si présent
            if (this->isSet(id, champ_keyRange))
            {
                wTmp = champ_keyRange;
                fi.write((char *)&wTmp, 2);
                genTmp = this->get(id, champ_keyRange).genValue;
                if (genTmp.ranges.byLo > 127)
                    genTmp.ranges.byLo = 127;
                if (genTmp.ranges.byHi > 127)
                    genTmp.ranges.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            if (this->isSet(id, champ_velRange))
            {
                wTmp = champ_velRange;
                fi.write((char *)&wTmp, 2);
                genTmp = this->get(id, champ_velRange).genValue;
                if (genTmp.ranges.byLo > 127)
                    genTmp.ranges.byLo = 127;
                if (genTmp.ranges.byHi > 127)
                    genTmp.ranges.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            for (int k = 0; k < this->count(id3); k++)
            {
                id3.indexMod = k;
                wTmp = this->get(id3, champ_sfGenOper).wValue;
                if (wTmp != champ_keyRange && wTmp != champ_velRange && wTmp != champ_instrument)
                {
                    fi.write((char *)&wTmp, 2);
                    genTmp = this->get(id3, champ_sfGenAmount).genValue;
                    fi.write((char *)&genTmp, 2);
                }
            }
            id3.typeElement = elementPrstInstGen;

            // pour chaque instrument associé
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id3.indexElt2 = j;

                    // gens associés aux instruments
                    // - 1er gen : keyrange si présent
                    // - 2ème gen : velocity si présent
                    // - dernier gen : sample_ID
                    if (this->isSet(id2, champ_keyRange))
                    {
                        wTmp = champ_keyRange;
                        fi.write((char *)&wTmp, 2);
                        genTmp = this->get(id2, champ_keyRange).genValue;
                        if (genTmp.ranges.byLo > 127)
                            genTmp.ranges.byLo = 127;
                        if (genTmp.ranges.byHi > 127)
                            genTmp.ranges.byHi = 127;
                        fi.write((char *)&genTmp, 2);
                    }
                    if (this->isSet(id2, champ_velRange))
                    {
                        wTmp = champ_velRange;
                        fi.write((char *)&wTmp, 2);
                        genTmp = this->get(id2, champ_velRange).genValue;
                        if (genTmp.ranges.byLo > 127)
                            genTmp.ranges.byLo = 127;
                        if (genTmp.ranges.byHi > 127)
                            genTmp.ranges.byHi = 127;
                        fi.write((char *)&genTmp, 2);
                    }
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        wTmp = this->get(id3, champ_sfGenOper).wValue;
                        if (wTmp != champ_keyRange && wTmp != champ_velRange && wTmp != champ_instrument)
                        {
                            fi.write((char *)&wTmp, 2);
                            genTmp = this->get(id3, champ_sfGenAmount).genValue;
                            fi.write((char *)&genTmp, 2);
                        }
                    }
                    wTmp = champ_instrument;
                    fi.write((char *)&wTmp, 2);
                    genTmp.wAmount = converterInst->calculIndex(this->get(id2, champ_instrument).wValue);
                    fi.write((char *)&genTmp, 2);
                }
            }
        }
    }
    delete converterInst;

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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;

            // Name
            dwTmp = this->getQstr(id, champ_name).length();
            if (dwTmp > 20) dwTmp = 20;
            if (dwTmp != 0)
            {
                fi.write(this->getQstr(id, champ_name).toLatin1());
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
            for (int j = 0; j < count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                    nBag++; // un bag par sample lié
            }
        }
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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // bag global
            wTmp = nGen;
            fi.write((char *)&wTmp, 2);
            id2.typeElement = elementInstGen;
            nGen += this->count(id2);
            wTmp = nMod;
            fi.write((char *)&wTmp, 2);
            id2.typeElement = elementInstMod;
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexMod = j;
                if (!this->get(id2, champ_hidden).bValue)
                    nMod++;
            }

            // un bag par instrument lié
            id2.typeElement = elementInstSmpl;
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    wTmp = nGen;
                    fi.write((char *)&wTmp, 2);
                    id3.typeElement = elementInstSmplGen;
                    id3.indexElt2 = j;
                    nGen += this->count(id3);
                    wTmp = nMod;
                    fi.write((char *)&wTmp, 2);
                    id3.typeElement = elementInstSmplMod;
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                            nMod++;
                    }
                }
            }
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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // mods du bag global
            id3.typeElement = elementInstMod;
            converterMod = new ConvertMod(this, id3);
            for (int k = 0; k < this->count(id3); k++)
            {
                id3.indexMod = k;
                if (!this->get(id3, champ_hidden).bValue)
                {
                    // écriture
                    sfTmp = this->get(id3, champ_sfModSrcOper).sfModValue;
                    byTmp = sfTmp.Index + sfTmp.CC * 128;
                    fi.write((char *)&byTmp, 1);
                    byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                    fi.write((char *)&byTmp, 1);
                    wTmp = converterMod->calculDestIndex(this->get(id3, champ_sfModDestOper).wValue);
                    fi.write((char *)&wTmp, 2);
                    wTmp = this->get(id3, champ_modAmount).wValue;
                    fi.write((char *)&wTmp, 2);
                    sfTmp = this->get(id3, champ_sfModAmtSrcOper).sfModValue;
                    byTmp = sfTmp.Index + sfTmp.CC * 128;
                    fi.write((char *)&byTmp, 1);
                    byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                    fi.write((char *)&byTmp, 1);
                    wTmp = this->get(id3, champ_sfModTransOper).wValue;
                    fi.write((char *)&wTmp, 2);
                }
            }
            delete converterMod;

            // pour chaque sample associé
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id3.indexElt2 = j;
                    id3.typeElement = elementInstSmplMod;
                    converterMod = new ConvertMod(this, id3);

                    // mods associés aux samples
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        if (!this->get(id3, champ_hidden).bValue)
                        {
                            // écriture
                            sfTmp = this->get(id3, champ_sfModSrcOper).sfModValue;
                            byTmp = sfTmp.Index + sfTmp.CC * 128;
                            fi.write((char *)&byTmp, 1);
                            byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                            fi.write((char *)&byTmp, 1);
                            wTmp = converterMod->calculDestIndex(this->get(id3, champ_sfModDestOper).wValue);
                            fi.write((char *)&wTmp, 2);
                            wTmp = this->get(id3, champ_modAmount).wValue;
                            fi.write((char *)&wTmp, 2);
                            sfTmp = this->get(id3, champ_sfModAmtSrcOper).sfModValue;
                            byTmp = sfTmp.Index + sfTmp.CC * 128;
                            fi.write((char *)&byTmp, 1);
                            byTmp = sfTmp.D + 2 * sfTmp.P + 4 * sfTmp.Type;
                            fi.write((char *)&byTmp, 1);
                            wTmp = this->get(id3, champ_sfModTransOper).wValue;
                            fi.write((char *)&wTmp, 2);
                        }
                    }
                    delete converterMod;
                }
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
    ConvertSmpl *converterSmpl = new ConvertSmpl(this, id.indexSf2);

    // pour chaque instrument
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            id3.indexElt = i;
            id3.typeElement = elementInstGen;

            // gens du bag global
            // - 1er gen : keyrange si présent
            // - 2ème gen : velocity si présent
            if (this->isSet(id, champ_keyRange))
            {
                wTmp = champ_keyRange;
                fi.write((char *)&wTmp, 2);
                genTmp = this->get(id, champ_keyRange).genValue;
                if (genTmp.ranges.byLo > 127)
                    genTmp.ranges.byLo = 127;
                if (genTmp.ranges.byHi > 127)
                    genTmp.ranges.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            if (this->isSet(id, champ_velRange))
            {
                wTmp = champ_velRange;
                fi.write((char *)&wTmp, 2);
                genTmp = this->get(id, champ_velRange).genValue;
                if (genTmp.ranges.byLo > 127)
                    genTmp.ranges.byLo = 127;
                if (genTmp.ranges.byHi > 127)
                    genTmp.ranges.byHi = 127;
                fi.write((char *)&genTmp, 2);
            }
            for (int k = 0; k < this->count(id3); k++)
            {
                id3.indexMod = k;
                wTmp = this->get(id3, champ_sfGenOper).wValue;
                if (wTmp != champ_keyRange && wTmp != champ_velRange && wTmp != champ_sampleID)
                {
                    fi.write((char *)&wTmp, 2);
                    genTmp = this->get(id3, champ_sfGenAmount).genValue;
                    fi.write((char *)&genTmp, 2);
                }
            }
            id3.typeElement = elementInstSmplGen;

            // pour chaque sample associé
            for (int j = 0; j < this->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->get(id2, champ_hidden).bValue)
                {
                    id3.indexElt2 = j;

                    // gens associés aux samples
                    // - 1er gen : keyrange si présent
                    // - 2ème gen : velocity si présent
                    // - dernier gen : sample_ID
                    if (this->isSet(id2, champ_keyRange))
                    {
                        wTmp = champ_keyRange;
                        fi.write((char *)&wTmp, 2);
                        genTmp = this->get(id2, champ_keyRange).genValue;
                        if (genTmp.ranges.byLo > 127)
                            genTmp.ranges.byLo = 127;
                        if (genTmp.ranges.byHi > 127)
                            genTmp.ranges.byHi = 127;
                        fi.write((char *)&genTmp, 2);
                    }
                    if (this->isSet(id2, champ_velRange))
                    {
                        wTmp = champ_velRange;
                        fi.write((char *)&wTmp, 2);
                        genTmp = this->get(id2, champ_velRange).genValue;
                        if (genTmp.ranges.byLo > 127)
                            genTmp.ranges.byLo = 127;
                        if (genTmp.ranges.byHi > 127)
                            genTmp.ranges.byHi = 127;
                        fi.write((char *)&genTmp, 2);
                    }
                    for (int k = 0; k < this->count(id3); k++)
                    {
                        id3.indexMod = k;
                        wTmp = this->get(id3, champ_sfGenOper).wValue;
                        if (wTmp != champ_keyRange && wTmp != champ_velRange && wTmp != champ_sampleID)
                        {
                            fi.write((char *)&wTmp, 2);
                            genTmp = this->get(id3, champ_sfGenAmount).genValue;
                            fi.write((char *)&genTmp, 2);
                        }
                    }
                    wTmp = champ_sampleID;
                    fi.write((char *)&wTmp, 2);
                    genTmp.wAmount = converterSmpl->calculIndex(this->get(id2, champ_sampleID).wValue);
                    fi.write((char *)&genTmp, 2);
                }
            }
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
    for (int i = 0; i < this->count(id); i++)
    {
        id.indexElt = i;
        if (!this->get(id, champ_hidden).bValue)
        {
            // Name
            dwTmp = this->getQstr(id, champ_name).length();
            if (dwTmp > 20) dwTmp = 20;
            if (dwTmp != 0)
            {
                fi.write(this->getQstr(id, champ_name).toLatin1());
                charTmp = '\0';
                for (quint32 iteration = 0; iteration < 20-dwTmp; iteration++)
                    fi.write(&charTmp, 1);
            }
            else
            {
                dwTmp = sprintf(tcharTmp, "sample %d", i+1);
                fi.write(tcharTmp, dwTmp);
                charTmp = '\0';
                for (quint32 iteration = 0; iteration < 20-dwTmp; iteration++)
                    fi.write(&charTmp, 1);
            }
            // dwStart
            fi.write((char *)&dwTmp2, 4);
            // dwEnd
            dwTmp = dwTmp2 + this->get(id, champ_dwLength).dwValue;
            fi.write((char *)&dwTmp, 4);
            // dwStartLoop
            dwTmp = dwTmp2 + this->get(id, champ_dwStartLoop).dwValue;
            fi.write((char *)&dwTmp, 4);
            // dwEndLoop
            dwTmp = dwTmp2 + this->get(id, champ_dwEndLoop).dwValue;
            fi.write((char *)&dwTmp, 4);
            // on avance
            dwTmp2 += this->get(id, champ_dwLength).dwValue + 46; // 46 zeros
            // dwSampleRate
            dwTmp = this->get(id, champ_dwSampleRate).dwValue;
            fi.write((char *)&dwTmp, 4);
            // byOriginalPitch
            byTmp = this->get(id, champ_byOriginalPitch).bValue;
            fi.write((char *)&byTmp, 1);
            // chPitchCorrection
            charTmp = this->get(id, champ_chPitchCorrection).cValue;
            fi.write((char *)&charTmp, sizeof(char));
            // wSampleLink
            wTmp = converterSmpl->calculIndex(this->get(id, champ_wSampleLink).wValue);
            fi.write((char *)&wTmp, 2);
            // sfSampleType
            wTmp = this->get(id, champ_sfSampleType).sfLinkValue;
            fi.write((char *)&wTmp, 2);
        }
    }
    delete converterSmpl;

    // shdr de fin
    fi.write("EOS", 3);
    charTmp = '\0';
    for (quint32 iteration = 0; iteration < 43; iteration++)
        fi.write(&charTmp, 1);

    // Fermeture du fichier
    fi.close();

    // Sauvegarde de fileName, wBpsInit
    id.typeElement = elementSf2;
    this->set(id, champ_filename, fileName, false);
    this->set(id, champ_wBpsInit, this->get(id, champ_wBpsSave), false);
    this->storeEdition(indexSf2);
    return 0;
}

// Classe utilitaire pour conversion des index d'une série de mods
Pile_sf2::ConvertMod::ConvertMod(Pile_sf2 *sf2, EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementPrstMod && \
            id.typeElement != elementInstSmplMod && id.typeElement != elementPrstInstMod)
    {
        this->nbElt = 0;
        this->listHidden = NULL;
    }
    else
    {
        this->nbElt = sf2->count(id);
        this->listHidden = new int[this->nbElt];
        int pos = 0;
        for (int i = 0; i < this->nbElt; i++)
        {
            id.indexMod = i;
            if (sf2->get(id, champ_hidden).bValue)
                this->listHidden[pos++] = i;
        }
        for (int i = pos; i < this->nbElt; i++)
            this->listHidden[i] = -1;
    }
}
Pile_sf2::ConvertMod::~ConvertMod()
{
    delete this->listHidden;
}
int Pile_sf2::ConvertMod::calculDestIndex(int destIndex)
{
    if (destIndex < 32768)
        return destIndex;
    bool hidden = false;
    int pos = destIndex - 32768;
    int nbHidden = 0;
    for (int i = 0; i < this->nbElt; i++)
    {
        if (this->listHidden[i] != -1)
        {
            if (this->listHidden[i] < pos)
                nbHidden++;
            else if (this->listHidden[i] == pos)
                hidden = true;
        }
    }
    if (hidden)
        return 0;
    else
        return 32768 + pos - nbHidden;
}

// Conversion du numéro de sample lié
Pile_sf2::ConvertSmpl::ConvertSmpl(Pile_sf2 *sf2, int indexSf2)
{
    EltID id(elementSmpl, indexSf2, 0, 0, 0);
    this->nbElt = sf2->count(id);
    this->listHidden = new int[this->nbElt];
    int pos = 0;
    for (int i = 0; i < this->nbElt; i++)
    {
        id.indexElt = i;
        if (sf2->get(id, champ_hidden).bValue)
            this->listHidden[pos++] = i;
    }
    for (int i = pos; i < this->nbElt; i++)
        this->listHidden[i] = -1;
}

Pile_sf2::ConvertSmpl::~ConvertSmpl()
{
    delete this->listHidden;
}

int Pile_sf2::ConvertSmpl::calculIndex(int index)
{
    bool hidden = false;
    int pos = index;
    int nbHidden = 0;
    for (int i = 0; i < this->nbElt; i++)
    {
        if (this->listHidden[i] != -1)
        {
            if (this->listHidden[i] < pos)
                nbHidden++;
            else if (this->listHidden[i] == pos)
                hidden = true;
        }
    }
    if (hidden)
        return 0;
    else
        return pos - nbHidden;
}

// Conversion du numéro d'instrument lié
Pile_sf2::ConvertInst::ConvertInst(Pile_sf2 *sf2, int indexSf2)
{
    EltID id(elementInst, indexSf2, 0, 0, 0);
    this->nbElt = sf2->count(id);
    this->listHidden = new int[this->nbElt];
    int pos = 0;
    for (int i = 0; i < this->nbElt; i++)
    {
        id.indexElt = i;
        if (sf2->get(id, champ_hidden).bValue)
            this->listHidden[pos++] = i;
    }
    for (int i = pos; i < this->nbElt; i++)
        this->listHidden[i] = -1;
}

Pile_sf2::ConvertInst::~ConvertInst()
{
    delete this->listHidden;
}

int Pile_sf2::ConvertInst::calculIndex(int index)
{
    bool hidden = false;
    int pos = index;
    int nbHidden = 0;
    for (int i = 0; i < this->nbElt; i++)
    {
        if (this->listHidden[i] != -1)
        {
            if (this->listHidden[i] < pos)
                nbHidden++;
            else if (this->listHidden[i] == pos)
                hidden = true;
        }
    }
    if (hidden)
        return 0;
    else
        return pos - nbHidden;
}
