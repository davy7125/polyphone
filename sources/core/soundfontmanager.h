/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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

#include "sound.h"
#include "sf2_types.h"
#include <QMap>
#include <QObject>
class Action;
class ActionManager;
class Soundfonts;
class QAbstractItemModel;

class SoundfontManager : public QObject
{
    Q_OBJECT

public:
    static SoundfontManager * getInstance();
    static SoundfontManager * getOtherInstance() { return new SoundfontManager(); }
    static void kill();
    ~SoundfontManager();
    QAbstractItemModel * getModel(int indexSf2);

    // Ajout / suppression des données
    int add(EltID id);
    void remove(EltID id, int *message = NULL);

    // Accès / modification des propriétés
    bool isSet(EltID id, Champ champ);
    Valeur get(EltID id, Champ champ);
    QString getQstr(EltID id, Champ champ);
    Sound getSon(EltID id);
    QByteArray getData(EltID id, Champ champ);
    int set(EltID id, Champ champ, Valeur value);
    int set(EltID id, Champ champ, QString qStr);
    int set(EltID id, Champ champ, QByteArray data);
    int reset(EltID id, Champ champ);
    void simplify(EltID id, Champ champ);

    // Nombre de freres de id (id compris)
    QList<int> getSiblings(EltID id);

    // Gestionnaire d'actions
    void endEditing(QString editingSource);
    void clearNewEditing(); // Keep the changes but don't make an undo
    void revertNewEditing(); // Doesn't keep the changes
    bool isUndoable(int indexSf2);
    bool isRedoable(int indexSf2);
    void undo(int indexSf2);
    void redo(int indexSf2);

    // Chargement / sauvegarde / nouveau
    int save(int indexSf2, QString fileName, int quality = 1);
    void markAsSaved(int indexSf2);
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
    // Emitted when a group of actions is finished
    // "editingSource" can be:
    //   * command:{command name} (for instance "command:undo" or "command:redo")
    //   * tool:{tool name}
    //   * page:{page name}
    void editingDone(QString editingSource, QList<int> sf2Indexes);

private slots:
    void onDropId(EltID id);

private:
    SoundfontManager();

    // Type de fichier
    typedef enum
    {
        fileUnknown = 0,
        fileSf2 = 1,
        fileSf3 = 2
    } FileType;

    // Classes utilitaires
    class ConvertMod
    {
    public:
        ConvertMod(SoundfontManager *_soundfonts, EltID id);
        int calculDestIndex(int destIndex);

    private:
        QList<int> _listIndex;
    };

    class ConvertSmpl
    {
    public:
        ConvertSmpl(SoundfontManager *_soundfonts, int indexSf2);
        int calculIndex(int index);

    private:
        QList<int> _listIndex;
    };

    class ConvertInst
    {
    public:
        ConvertInst(SoundfontManager *_soundfonts, int indexSf2);
        int calculIndex(int index);

    private:
        QList<int> _listIndex;
    };

    /// Affiche l'élément id
    int display(EltID id);

    /// Delete or hide the element id. Error if the element is used by another
    int remove(EltID id, bool permanently, bool storeAction, int *message = NULL);

    /// Clear parameters
    void supprGenAndStore(EltID id, int storeAction);

    /// Save
    int sauvegarderSf2(int indexSf2, QString fileName);

    QList<int> undo(QList<Action *> actions);

    static SoundfontManager * s_instance;
    Soundfonts * _soundfonts;
    ActionManager * _undoRedo;
};

#endif // PILE_SF2_H
