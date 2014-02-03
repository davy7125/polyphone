#include "sfarkextractor.h"
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

    return (ret == 0);
}
