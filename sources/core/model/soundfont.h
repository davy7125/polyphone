#ifndef SOUNDFONT_H
#define SOUNDFONT_H

#include "basetypes.h"
#include <QList>
#include <QMap>
class Smpl;
class InstPrst;
class Soundfonts;
class QAbstractItemModel;
class TreeItem;
class TreeItemRoot;

class Soundfont
{
public:
    Soundfont(EltID id);
    ~Soundfont();

    // Add, get or delete a sample
    int addSample();
    Smpl * getSample(int index);
    const QMap<int, Smpl *> getSamples() { return _smpl; }
    bool deleteSample(int index);
    int indexOfSample(Smpl * smpl);

    // Add, get or delete an instrument
    int addInstrument();
    InstPrst *getInstrument(int index);
    const QMap<int, InstPrst *> getInstruments() { return _inst; }
    bool deleteInstrument(int index);
    int indexOfInstrument(InstPrst * inst);

    // Add, get or delete a preset
    int addPreset();
    InstPrst * getPreset(int index);
    const QMap<int, InstPrst *> getPresets() { return _prst; }
    bool deletePreset(int index);
    int indexOfPreset(InstPrst * prst);

    // Tree model associated to the soundfont
    QAbstractItemModel * getModel() { return _model; }

    // Doc: "max 255 characters except for "comments", ended by 1 or 2 \0 for an even number of bits"
    SfVersionTag _IFIL; // version of the Sound Font RIFF file     e.g. 2.01                                MANDATORY
    QString _ISNG; // Target Sound Engine                          e.g. “EMU8000”                           MANDATORY
    QString _INAM; // Sound Font Bank Name                         e.g. “General MIDI”                      MANDATORY
    QString _IROM; // Sound ROM Name                               e.g. “1MGM”
    SfVersionTag _IVER; // Sound ROM Version                       e.g. 2.08
    QString _ICRD; // Date of Creation of the Bank                 e.g. “July 15, 1997”
    QString _IENG; // Sound Designers and Engineers for the Bank   e.g. “John Q. Sounddesigner”
    QString _IPRD; // Product for which the Bank was intended      e.g. “SBAWE64 Gold”
    QString _ICOP; // Copyright message                            e.g. “Copyright (c) 1997 E-mu Systems, Inc.”
    QString _ICMT; // Comments on the Bank                         e.g. “This is a comment”                           /!\  65,535 bits maxi
    QString _ISFT; // Soundfont tools used                         e.g. “:Preditor 2.00a:Vienna SF Studio 2.0:”

    QString _fileNameInitial; // File that is initially opened, updated after each save
    QString _fileNameForData; // sf2 file (_fileNameInitial or extraction or the initial file). The sounds are read from this file

    // Other
    double _numEdition;  // numéro de l'édition sauvegardée
    quint16 _wBpsInit;   // résolution sample à l'ouverture du fichier (16, 24 ou 0 si nouveau)
    quint16 _wBpsSave;   // résolution souhaitée lors d'une sauvegarde (16 ou 24)

private:
    int _smplCounter, _instCounter, _prstCounter;
    EltID _id;
    QMap<int, Smpl *> _smpl;
    QMap<int, InstPrst *> _inst;
    QMap<int, InstPrst *> _prst;

    QAbstractItemModel * _model;
    TreeItemRoot * _rootItem;
    TreeItem * _generalTreeItem;
    TreeItem * _sampleTreeItem;
    TreeItem * _instrumentTreeItem;
    TreeItem * _presetTreeItem;

    QMutex _mutexInst;
};

#endif // SOUNDFONT_H
