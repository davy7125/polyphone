/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <QWidget>
#include "page.h"
class ToolMenu;
class StyledAction;

namespace Ui {
class EditorToolBar;
}

class EditorToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit EditorToolBar(QWidget *parent = nullptr);
    ~EditorToolBar();
    void setSf2Index(int sf2index);
    void updateActions();
    void disable();
    void setDisplayOptions(QList<Page::DisplayOption> displayOptions);

    static void updateRecorderButtonsState(bool isChecked);
    static void updateKeyboardButtonsState(bool isChecked);
    static void updateMidiExtensionButtonsState(int midiExtensionIndex, bool isChecked);

public slots:
    void onSelectionChanged(IdList ids);
    void disableDisplayOption(QList<int> disabledOptions);
    void selectDisplayOption(int displayOption);

signals:
    void displayOptionChanged(int displayOption);
    void recorderDisplayChanged(bool isDisplayed);
    void keyboardDisplayChanged(bool isDisplayed);
    void selectionChanged(IdList id);

private slots:
    void onDisplayActionClicked();

    void on_pushAddSample_clicked();
    void on_pushAddInstrument_clicked();
    void onNewInstClicked(QString name, bool linkElements);
    void on_pushAddPreset_clicked();
    void onNewPrstClicked(QString name, bool linkElements);
    void on_pushUndo_clicked();
    void on_pushRedo_clicked();
    void on_pushSave_clicked();
    void on_pushShowRecorder_clicked();
    void on_pushShowKeyboard_clicked();

    void onMidiExtensionActionClicked();
    void onExtensionDialogClosed(int result);

private:
    Ui::EditorToolBar *ui;

    int _sf2Index;
    QList<StyledAction *> _displayActions, _midiControllerActions;
    bool _updatingDisplayOptions;
    ToolMenu * _toolMenu;
    IdList _currentSelection;

    static QList<EditorToolBar *> s_instances;
    static bool s_recorderOpen;
    static bool s_keyboardOpen;
    static QMap<int, bool> s_midiExtensionOpen;
};


#endif // EDITORTOOLBAR_H
