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

#ifndef EDITOR_H
#define EDITOR_H

#include "soundfonttab.h"
#include "basetypes.h"

class PageSelector;
class AbstractInputParser;
class DialogKeyboard;
class PianoKeybdCustom;
class QVariantAnimation;

namespace Ui {
class Editor;
}

class Editor : public SoundfontTab
{
    Q_OBJECT

public:
    Editor(DialogKeyboard * dialogKeyboard, EltID initialSelection = EltID());
    ~Editor() override;

    // MIDI signals
    bool processKey(int channel, int key, int vel) override;

    void selectElement(EltID id);
    IdList getSelection() { return _currentIds; }
    void onActionRequired(TabAction action) override;
    KeyboardDisplayContent getKeyboardDisplayContent() override;

signals:
    void processKeyMainThread(int channel, int key, int vel);

protected:
    void showEvent(QShowEvent* event) override;
    void tabInitializing(QString filename) override;
    void tabInError(QString errorMessage) override;
    void tabInitialized(int indexSf2) override;
    void tabUpdate(QString editingSource) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    DialogKeyboard * _dialogKeyboard;

private slots:
    void onProcessKeyMainThread(int channel, int key, int vel);
    void onSelectionChanged(IdList ids);
    void displayOptionChanged(int displayOption);
    void customizeKeyboard();
    void onEditingDone(QString editingSource, QList<int> sf2Indexes);
    void onErrorEncountered(QString text);
    void onRootKeyChanged(int rootKey);
    void onPageHidden();
    void onSearchAnimationValueChanged(const QVariant &value);

public:
    // Display on a keyboard the ranges and root keys of elements (also used by the soundfont explorer)
    static void customizeKeyboard(PianoKeybdCustom * keyboard, IdList currentIds);

    // Emphasize on a keyboard the ranges of the instrument divisions played by a key, and return these divisions
    static IdList showPlayedRanges(PianoKeybdCustom * keyboard, EltID idInst, int key, int vel);

private:
    static QVector<bool> getEnabledKeysForInstrument(EltID idInst);
    void updateSearchStyle(QColor focusBackgroundColor);

    // Sample, instrument or preset played by the keyboard (unique in the selection), or elementUnknown
    EltID getPlayedElement();

    // Element to play before (direction < 0) or after (direction > 0) the element played, or the first element of the
    // category selected if nothing is played (elementUnknown if none)
    EltID getNavigationTarget(int direction);

    // Display of the virtual keyboard, updated once when several changes occur
    void scheduleKeyboardDisplayUpdate();
    void updateKeyboardDisplay();

    Ui::Editor *ui;
    PageSelector * _pageSelector;
    ElementType _currentElementType;
    IdList _currentIds;
    EltID _initialSelection;
    bool _firstShow;
    QVariantAnimation * _searchAnimation;
    bool _keyboardDisplayUpdateScheduled;
};

#endif // EDITOR_H
