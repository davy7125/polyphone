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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PILE_SF2_H
#define PILE_SF2_H

#include "pile_actions.h"
#include "sound.h"
#include "tree.h"
#include "treewidgetitem.h"
#include <QList>

class Pile_sf2 : public QObject
{
    Q_OBJECT

public:
    // METHODES PUBLIQUES DE LA CLASSE PILE_SF2
    Pile_sf2();
    ~Pile_sf2();

    // Ajout / suppression des données
    int add(EltID id, bool storeAction = true);
    void remove(EltID id, int *message = NULL);

    // Accès / modification des propriétés
    bool isSet(EltID id, Champ champ);
    Valeur get(EltID id, Champ champ);
    QString getQstr(EltID id, Champ champ);
    Sound getSon(EltID id);
    QByteArray getData(EltID id, Champ champ);
    int set(EltID id, Champ champ, Valeur value, bool storeAction = true, bool sort = true);
    int set(EltID id, Champ champ, QString qStr, bool storeAction = true, bool sort = true);
    int set(EltID id, Champ champ, QByteArray data, bool storeAction = true);
    int reset(EltID id, Champ champ, bool storeAction = true);
    void simplify(EltID id, Champ champ);

    // Nombre de freres de id (id compris)
    int count(EltID id, bool withHidden = true);

    // Gestionnaire d'actions
    void prepareNewActions(bool removeOldActions = true);
    void cleanActions();
    bool isUndoable();
    bool isRedoable();
    void undo();
    void redo();

    // Chargement / sauvegarde / nouveau
    void nouveau(QString name, int &indexSf2);
    int open(QString fileName, int &indexSf2);
    int open(QString fileName, QDataStream *stream, int &indexSf2, bool copySamples = false);
    int save(int indexSf2, QString fileName, int quality = 1);
    bool isEdited(int indexSf2);

    // Récupération d'une liste de champs et de valeurs contenues dans les bags de l'élément id
    void getListeBags(EltID id, QList<Champ> &listeChamps, QList<genAmountType> &listeValeurs);

    // Détermination de la validité d'un ID (en acceptant ou non les ID masqués, par défaut non)
    bool isValid(EltID id, bool acceptHidden = false);

    // Disponibilité de bank / preset
    void firstAvailablePresetBank(EltID id, int &nBank, int &nPreset);
    int closestAvailablePreset(EltID id, quint16 wBank, quint16 wPreset);
    bool isAvailable(EltID id, quint16 wBank, quint16 wPreset);

signals:
    void updateTable(int type, int _sf2, int elt, int elt2);
    void newElement(EltID id);
    void hideElement(EltID id, bool isHidden);
    void removeElement(EltID id);
    void changeElementName(EltID id, QString name);
    void changeElementOrder(EltID id, QString order, bool sort);

private:
    // Type de fichier
    typedef enum
    {
        fileUnknown = 0,
        fileSf2 = 1,
        fileSf3 = 2
    } FileType;

    // DEFINITION DE LA SOUS-CLASSE SF2
    class SF2
    {
    public:
        class BAG
        {
        public:
            class MOD
            {
            public:
                MOD() : _hidden(false) {}
                SFModulator _sfModSrcOper;
                Champ _sfModDestOper;
                short _modAmount;
                SFModulator _sfModAmtSrcOper;
                SFTransform _sfModTransOper;
                int _index;
                bool _hidden;
            };

            struct GEN
            {
                Champ sfGenOper;
                genAmountType genAmount;
            };

            BAG();
            QList<MOD> _mods;
            QList<GEN> _gens;
            bool _hidden;

            bool isSet(Champ champ);
            void enleverMod(int index);
            void setGen(Champ champ, Valeur value);
            void resetGen(Champ champ);
            void supprGenAndStore(EltID id, int storeAction, Pile_sf2 *root);
            Valeur getGen(Champ champ);
        };

        class SMPL
        {
        public:
            SMPL();
            QString _name;
            Sound _sound;
            quint16 _wSampleLink;
            SFSampleLink _sfSampleType;
            bool _hidden;
        };

        class INST
        {
        public:
            QString _name;
            QList<BAG *> _bags;
            BAG _bagGlobal;
            bool _hidden;

            INST();
            ~INST()
            {
                while (!_bags.isEmpty())
                    delete _bags.takeFirst();
            }
            void decrementerSMPL(int indexSmpl);
        };

        class PRST
        {
        public:
            QString _name;
            quint16 _wPreset;
            quint16 _wBank;
            quint32 _dwLibrary;
            quint32 _dwGenre;
            quint32 _dwMorphology;
            QList<BAG *> _bags;
            BAG _bagGlobal;
            bool _hidden;

            PRST();
            ~PRST()
            {
                while (!_bags.isEmpty())
                    delete _bags.takeFirst();
            }
            void decrementerINST(int indexInst);
        };

        // ATTRIBUTS DE LA CLASSE SF2
        // INFO
        // max 255 caractères sauf pour comments, terminés par 1 ou 2 terminateurs pour un nombre pair de bits
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
        QString _ISFT; // SoundFont tools used                         e.g. “:Preditor 2.00a:Vienna SF Studio 2.0:”
        QString _fileName;   // nom du fichier

        // SMPL INST ET PRST
        QList<SMPL *> _smpl;
        QList<INST *> _inst;
        QList<PRST *> _prst;

        // Autres
        double _numEdition;  // numéro de l'édition sauvegardée
        bool _hidden;        // fichier supprimé ou non (avant suppression définitive)
        quint16 _wBpsInit;   // résolution sample à l'ouverture du fichier (16, 24 ou 0 si nouveau)
        quint16 _wBpsSave;   // résolution souhaitée lors d'une sauvegarde (16 ou 24)

        // METHODES DE LA CLASSE SF2
        SF2();
        ~SF2()
        {
            while (!_smpl.isEmpty())
                delete _smpl.takeFirst();
            while (!_inst.isEmpty())
                delete _inst.takeFirst();
            while (!_prst.isEmpty())
                delete _prst.takeFirst();
        }
        void decrementerLinkSMPL(int indexSmpl);
    };

    // Classes utilitaires
    class ConvertMod
    {
    public:
        ConvertMod(Pile_sf2 *_sf2, EltID id);
        ~ConvertMod();
        int calculDestIndex(int destIndex);

    private:
        int _nbElt;
        int * _listHidden;
    };

    class ConvertSmpl
    {
    public:
        ConvertSmpl(Pile_sf2 *_sf2, int indexSf2);
        ~ConvertSmpl();
        int calculIndex(int index);

    private:
        int _nbElt;
        int * _listHidden;
    };

    class ConvertInst
    {
    public:
        ConvertInst(Pile_sf2 *_sf2, int indexSf2);
        ~ConvertInst();
        int calculIndex(int index);

    private:
        int _nbElt;
        int * _listHidden;
    };

    // ELEMENTS PRIVES DE LA CLASSE PILE_SF2
    QList<SF2 *> _sf2;
    ActionManager *_undoRedo;

    // METHODES PRIVEES DE LA CLASSE PILE_SF2
    /// Affiche l'élément id
    int display(EltID id);

    /// Delete or hide the element id. Error if the element is used by another
    int remove(EltID id, bool permanently, bool storeAction, int *message = NULL);

    /// Remove very old actions and previous actions that have been erased
    void releaseActions(bool withVeryOldActions);

    /// Type of file
    static FileType getFileType(QString fileName);

    /// Save
    int sauvegarderSf2(int indexSf2, QString fileName);

    /// Store the edition
    void storeEdition(int indexSf2);
};

#endif // PILE_SF2_H
