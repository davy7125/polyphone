#include "sfarkextractor.h"
#include <QFileInfo>
#include <QDataStream>
#include "sfArkLib.h"
#include "pile_sf2.h"

SfArkExtractor::SfArkExtractor(Pile_sf2 *sf2) :
    _sf2(sf2)
{}

// Callbacks sfArk

void sfkl_DisplayNotes(const char *NotesFilePath, const char* OutFileName)                                // Display notes text file
{
    Q_UNUSED(OutFileName)
    Q_UNUSED(NotesFilePath)
//    qDebug() << "Notes text file extracted to: " << NotesFilePath;
}

bool sfkl_GetLicenseAgreement(const char *LicenseText, const char *OutFileName)
{
    Q_UNUSED(LicenseText)
    Q_UNUSED(OutFileName)
    return true;
}

void sfkl_msg(const char *MessageText, int Flags)
{
    Q_UNUSED(MessageText)
    Q_UNUSED(Flags)
    QString msg;
    if (Flags & SFARKLIB_MSG_PopUp)
        msg = "*** ";
    qDebug() << msg << MessageText;
}

void sfkl_UpdateProgress(int ProgressPercent)
{
    Q_UNUSED(ProgressPercent)
//    qDebug() << "avancement extraction sfark" << ProgressPercent;
}

// Extraction des données et chargement sf2
void SfArkExtractor::extract(QString fileName)
{
    // Stockage des données converties
    QByteArray convertedData;
    QDataStream streamOut(&convertedData, QIODevice::WriteOnly);

    // Décodage
    sfkl_Decode(fileName.toStdString().c_str(), streamOut);

    // Création sf2
    QDataStream streamIn(&convertedData, QIODevice::ReadOnly);
    int indexSf2 = -1;
    _sf2->ouvrir("", &streamIn, indexSf2, true);
}
