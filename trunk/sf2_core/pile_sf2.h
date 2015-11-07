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

#ifndef PILE_SF2_H
#define PILE_SF2_H

#include "pile_actions.h"
#include "sound.h"
#include "tree.h"
#include "treewidgetitem.h"

class Pile_sf2 : public QObject
{
    Q_OBJECT

public:
    // METHODES PUBLIQUES DE LA CLASSE PILE_SF2
    Pile_sf2(Tree *tree, bool ram);
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
    void nouveau(QString name);
    int open(QString fileName);
    int open(QString fileName, QDataStream *stream, int &indexSf2, bool copySamples = false);
    int save(int indexSf2, QString fileName, int quality = 1);
    bool isEdited(int indexSf2);

    // Récupération d'une liste de champs et de valeurs contenues dans les bags de l'élément id
    void getListeBags(EltID id, QList<Champ> &listeChamps, QList<genAmountType> &listeValeurs);

    // Détermination de la validité d'un ID (en acceptant ou non les ID masqués, par défaut non)
    bool isValide(EltID id, bool acceptHidden = 0);

    // Disponibilité de bank / preset
    void firstAvailablePresetBank(EltID id, int &nBank, int &nPreset);
    int closestAvailablePreset(EltID id, quint16 wBank, quint16 wPreset);
    bool isAvailable(EltID id, quint16 wBank, quint16 wPreset);

signals:
    void updateTable(int type, int sf2, int elt, int elt2);

private:
    // Type de fichier
    typedef enum
    {
        fileUnknown = 0,
        fileSf2 = 1,
        fileSf3 = 2
    }FileType;

    // DEFINITION DE LA SOUS-CLASSE SF2
    class SF2
    {
    public:
        class BAG
        {
        public:
            // DEFINITION DES SOUS-CLASSES DE BAG
            class MOD
            {
            public:
                // ATTRIBUTS DE MOD
                SFModulator sfModSrcOper;
                Champ sfModDestOper;
                short modAmount;
                SFModulator sfModAmtSrcOper;
                SFTransform sfModTransOper;
                int index;
                bool hidden;
                MOD *suivant;

                // METHODES DE MOD
                MOD();
                ~MOD()
                {
                    delete this->suivant;
                }
                MOD *getElt(int pos);
                int nombreElt();
                void enleverMod(int index);
            };

            class GEN
            {
            public:
                // ATTRIBUTS DE GEN
                Champ sfGenOper;
                genAmountType genAmount;
                GEN *suivant;

                // METHODES DE GEN
                GEN();
                ~GEN()
                {
                    delete this->suivant;
                }
                GEN *getElt(int pos);
                bool isSet(Champ champ);
                GEN *setGen(Champ champ, Valeur value);
                GEN *resetGen(Champ champ);
                GEN *supprGenAndStore(EltID id, int storeAction, Pile_sf2 *root);
                Valeur getGen(Champ champ);
                int nombreElt();
            };

            // ATTRIBUTS DE BAG
            BAG();
            ~BAG()
            {
                delete suivant;
                delete mod;
                delete gen;
            }
            MOD *mod;
            GEN *gen;
            QTreeWidgetItem *eltTree;
            bool hidden;
            BAG *suivant;

            // METHODES DE BAG
            BAG *getElt(int pos);
            int nombreElt();
            void decrementerSF2();
            void decrementerSMPL(int indexSmpl);
            void decrementerINST(int indexInst);
            void decrementerINST_PRST();
            void decrementerBAG();
        };

        class SMPL
        {
        public:
            // ATTRIBUTS DE SMPL
            SMPL();
            ~SMPL()
            {
                delete suivant;
            }
            QString Name;
            Sound son;
            quint16 wSampleLink;
            SFSampleLink sfSampleType;
            QTreeWidgetItem *eltTree;
            bool hidden;
            SMPL *suivant;

            // METHODES DE SMPL
            SMPL *getElt(int pos);
            int nombreElt();
            void decrementerSF2();
            void decrementerSMPL();
            void decrementerLinkSMPL(int indexSmpl);
        };

        class INST
        {
        public:
            // ATTRIBUTS DE INST
            QString Name;
            BAG *bag;
            BAG bagGlobal;
            QTreeWidgetItem *eltTree;
            bool hidden;
            INST *suivant;
            // METHODES DE INST
            INST();
            ~INST()
            {
                delete bag;
                delete suivant;
            }
            INST *getElt(int pos);
            int nombreElt();
            void decrementerSF2();
            void decrementerINST();
        };

        class PRST
        {
        public:
            // ATTRIBUTS DE PRST
            QString Name;
            quint16 wPreset;
            quint16 wBank;
            quint32 dwLibrary;
            quint32 dwGenre;
            quint32 dwMorphology;
            BAG *bag;
            BAG bagGlobal;
            QTreeWidgetItem *eltTree;
            bool hidden;
            PRST *suivant;

            // METHODES DE PRST
            PRST();
            ~PRST()
            {
                delete bag;
                delete suivant;
            }
            PRST *getElt(int pos);
            int nombreElt();
            void decrementerSF2();
            void decrementerPRST();
        };

        // ATTRIBUTS DE LA CLASSE SF2
        // INFO
        // max 255 caractères sauf pour comments, terminés par 1 ou 2 terminateurs pour un nombre pair de bits
        SfVersionTag IFIL; // version of the Sound Font RIFF file     e.g. 2.01                                MANDATORY
        QString ISNG; // Target Sound Engine                          e.g. “EMU8000”                           MANDATORY
        QString INAM; // Sound Font Bank Name                         e.g. “General MIDI”                      MANDATORY
        QString IROM; // Sound ROM Name                               e.g. “1MGM”
        SfVersionTag IVER; // Sound ROM Version                       e.g. 2.08
        QString ICRD; // Date of Creation of the Bank                 e.g. “July 15, 1997”
        QString IENG; // Sound Designers and Engineers for the Bank   e.g. “John Q. Sounddesigner”
        QString IPRD; // Product for which the Bank was intended      e.g. “SBAWE64 Gold”
        QString ICOP; // Copyright message                            e.g. “Copyright (c) 1997 E-mu Systems, Inc.”
        QString ICMT; // Comments on the Bank                         e.g. “This is a comment”                           /!\  65,535 bits maxi
        QString ISFT; // SoundFont tools used                         e.g. “:Preditor 2.00a:Vienna SF Studio 2.0:”
        QString fileName;   // nom du fichier

        // SMPL INST ET PRST
        SMPL *smpl;
        INST *inst;
        PRST *prst;

        // Elements graphiques
        QTreeWidgetItem *eltTree;
        QTreeWidgetItem *eltTreeSmpl;
        QTreeWidgetItem *eltTreeInst;
        QTreeWidgetItem *eltTreePrst;

        // Autres
        double numEdition;  // numéro de l'édition sauvegardée
        bool hidden;        // fichier supprimé ou non (avant suppression définitive)
        SF2 *suivant;       // fichier sf2 suivant
        quint16 wBpsInit;      // résolution sample à l'ouverture du fichier (16, 24 ou 0 si nouveau)
        quint16 wBpsSave;      // résolution souhaitée lors d'une sauvegarde (16 ou 24)

        // METHODES DE LA CLASSE SF2
        SF2(); // constructeur
        ~SF2()
        {
            delete suivant;
            delete smpl;
            delete inst;
            delete prst;
        }

        SF2 *getElt(int pos);
        int nombreElt();
        void decrementerSF2();
    };

    // Classes utilitaires
    class ConvertMod
    {
    public:
        ConvertMod(Pile_sf2 *sf2, EltID id);
        ~ConvertMod();
        int calculDestIndex(int destIndex);
    private:
        int nbElt;
        int *listHidden;
    };

    class ConvertSmpl
    {
    public:
        ConvertSmpl(Pile_sf2 *sf2, int indexSf2);
        ~ConvertSmpl();
        int calculIndex(int index);
    private:
        int nbElt;
        int *listHidden;
    };

    class ConvertInst
    {
    public:
        ConvertInst(Pile_sf2 *sf2, int indexSf2);
        ~ConvertInst();
        int calculIndex(int index);
    private:
        int nbElt;
        int *listHidden;
    };

    // ELEMENTS PRIVES DE LA CLASSE PILE_SF2
    SF2 *sf2;
    Tree *tree;
    Pile_actions *pileActions;
    bool static CONFIG_RAM;

    // METHODES PRIVEES DE LA CLASSE PILE_SF2
    // Affiche l'élément id
    int display(EltID id);

    // Supprime ou masque l'élément id. Si l'élément est utilisé par un autre : erreur
    int remove(EltID id, bool permanently, bool storeAction, int *message = NULL);

    // Remove very old actions and previous actions that have been erased
    void releaseActions(bool withVeryOldActions);

    // Type de fichier
    static FileType getFileType(QString fileName);

    // Gestion de la sauvegarde
    int sauvegarderSf2(int indexSf2, QString fileName);
    void storeEdition(int indexSf2);
};

#endif // PILE_SF2_H
