/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include <QToolBar>
#include "page.h"
class ToolMenu;
class StyledAction;

class EditorToolBar : public QToolBar
{
    Q_OBJECT

public:
    EditorToolBar(QWidget *parent = nullptr);
    ~EditorToolBar();
    void setSf2Index(int sf2index);
    void updateActions();
    void disable();
    void setDisplayOptions(QList<Page::DisplayOption> displayOptions);

    static void updateRecorderButtonsState(bool isChecked);
    static void updateKeyboardButtonsState(bool isChecked);

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
    void onNewSmplClicked();
    void onNewInstClicked();
    void onNewPrstClicked();
    void onSaveClicked();
    void onDisplayActionClicked();
    void onRecorderActionClicked();
    void onKeyboardActionClicked();
    void onUndo();
    void onRedo();

private:
    StyledAction * _actionAddSample;
    StyledAction * _actionAddInstrument;
    StyledAction * _actionAddPreset;
    StyledAction * _actionToolBox;
    StyledAction * _actionUndo;
    StyledAction * _actionRedo;
    StyledAction * _actionSave;
    StyledAction * _actionShowRecorder;
    StyledAction * _actionShowKeyboard;

    int _sf2Index;
    QAction * _displayActionSeparator;
    QList<StyledAction *> _displayActions;
    bool _updatingDisplayOptions;
    ToolMenu * _toolMenu;
    IdList _currentSelection;

    static QList<EditorToolBar *> s_instances;
    static bool s_recorderOpen;
    static bool s_keyboardOpen;
};

#endif // EDITORTOOLBAR_H
