/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SOUNDFONTMANAGER_H
#define SOUNDFONTMANAGER_H

#include "sound.h"
#include "basetypes.h"
#include <QMap>
#include <QObject>
class Action;
class ActionManager;
class Soundfonts;
class QAbstractItemModel;
class SoloManager;

class SoundfontManager : public QObject
{
    Q_OBJECT

public:
    static SoundfontManager * getInstance();
    static void kill();
    ~SoundfontManager() override;
    QAbstractItemModel * getModel(int indexSf2);

    // Add / delete data
    int add(EltID id);
    void remove(EltID id, int *message = nullptr);

    // Get / set properties
    bool isSet(EltID id, AttributeType champ);
    AttributeValue get(EltID id, AttributeType champ);
    QString getQstr(EltID id, AttributeType champ);
    Sound *getSound(EltID id);
    QVector<float> getData(EltID idSmpl);
    int set(EltID id, AttributeType champ, AttributeValue value);
    int set(EltID id, AttributeType champ, QString qStr);
    int set(EltID idSmpl, QVector<float> data);
    void reset(EltID id, AttributeType champ);
    void simplify(EltID id, AttributeType champ);

    // Brother index list of ID (id included)
    QList<int> getSiblings(EltID &id);

    // Action management
    void endEditing(QString editingSource);
    void clearNewEditing(); // Keep the changes but don't make an undo
    void revertNewEditing(); // Doesn't keep the changes
    bool isUndoable(int indexSf2);
    bool isRedoable(int indexSf2);
    void undo(int indexSf2);
    void redo(int indexSf2);

    // Version management
    void markAsSaved(int indexSf2);
    bool isEdited(int indexSf2);

    // Find if an ID is valid (allowing or not browing in hidden ID, not allowed by default)
    bool isValid(EltID &id, bool acceptHidden = false, bool justCheckParentLevel = false);

    // Availability of banks / presets
    void firstAvailablePresetBank(EltID id, int &nBank, int &nPreset);
    int closestAvailablePreset(EltID id, quint16 wBank, quint16 wPreset);
    bool isAvailable(EltID id, quint16 wBank, quint16 wPreset);

    // Access to the solo manager
    SoloManager * solo() { return _solo; }

    // Create a notification about a new soundfont that has been loaded
    void emitNewSoundfontLoaded(int sf2Index) { emit this->soundfontLoaded(sf2Index); }

    // Get the division order
    // Sort type: 0: key, 1: velocity, 2: name
    // Result:
    // * -1 if id1 should be before id2,
    // * 0 if equals,
    // * 1 if id1 should be after id2
    int sortDivisions(EltID id1, EltID id2, int sortType);

    // Get all data + mutex (for the synth)
    Soundfonts * getSoundfonts() { return _soundfonts; }
    QRecursiveMutex * getMutex() { return &_mutex; }

    // Load all samples in RAM
    void loadAllSamples(int sf2Index);

signals:
    // Emitted when a group of actions is finished
    // "editingSource" can be:
    //   * command:{command name} (for instance "command:undo", "command:redo", "command:display", "command:selection")
    //   * tool:{tool kind}:{tool name}
    //   * page:{page name}
    void editingDone(QString editingSource, QList<int> sf2Indexes);

    // Emitted when a new soundfont is loaded
    void soundfontLoaded(int indexSf2);

    // Emitted when a soundfont is closed
    void soundfontClosed(int indexSf2);

    // Emitted when a modulator changed, useful for defining the CC default values
    void inputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending);

    // Emitted when a key range or a rootkey changed, useful for calling "customizeKeyboard" in editor.h
    void parameterForCustomizingKeyboardChanged();

    // Emitted when an error occurred
    void errorEncountered(QString text);

private slots:
    void onDropId(EltID id);

private:
    SoundfontManager();

    /// Display the element ID
    int display(EltID id);

    /// Delete or hide the element id. Error if the element is used by another
    int remove(EltID id, bool permanently, bool storeAction, int *message = nullptr);

    /// Clear parameters
    void supprGenAndStore(EltID id, int storeAction);

    QList<int> undo(QList<Action *> actions);

    // Division order
    int compareKey(EltID idDiv1, EltID idDiv2);
    int compareVelocity(EltID idDiv1, EltID idDiv2);
    int compareName(EltID idDiv1, EltID idDiv2);

    static SoundfontManager * s_instance;
    Soundfonts * _soundfonts;
    ActionManager * _undoRedo;
    QRecursiveMutex _mutex;
    SoloManager * _solo;

    bool _parameterForCustomizingKeyboardChanged;
};

#endif // SOUNDFONTMANAGER_H
