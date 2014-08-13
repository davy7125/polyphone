/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#include "sfarkextractor.h"
#include "sfarkextractor_v1.h"
#include <QFileInfo>
#include <QDataStream>
#include "sfArkLib.h"
#include "pile_sf2.h"

SfArkExtractor::SfArkExtractor(Pile_sf2 *sf2) :
    _sf2(sf2)
{}

// Extraction des données et chargement sf2
bool SfArkExtractor::extract(QString fileName)
{
    // Ouverture du fichier
    QFile fileSfArk(fileName);
    if (!fileSfArk.open(QIODevice::ReadOnly))
        return false;
    QDataStream streamIn(&fileSfArk);

    // Stockage des données converties
    QByteArray convertedData;
    QDataStream streamOut(&convertedData, QIODevice::WriteOnly);

    // Décodage
    int ret = sfkl_Decode(streamIn, streamOut);

    // Fermeture fichier sfArk
    fileSfArk.close();

    // Création sf2
    if (ret == 0 || ret == -8) // erreur checksum acceptée
    {
        QDataStream streamSf2(&convertedData, QIODevice::ReadOnly);
        int indexSf2 = -1;
        ret = _sf2->ouvrir("", &streamSf2, indexSf2, true);
    }
    else
    {
        // Tentative d'ouverture avec 2ème librairie
        SfArkExtractorV1 extractorV1(_sf2);
        ret = !extractorV1.extract(fileName);
    }

    return (ret == 0);
}
