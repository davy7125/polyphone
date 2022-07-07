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

#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include "basetypes.h"

class PageSelector;
class AbstractInputParser;

namespace Ui {
class Editor;
}

class Editor : public QWidget
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    /// Initialize the editor with a parser that can extract data and build a soundfont
    void initialize(AbstractInputParser * input);

    /// Index of the soundfont created
    int getSf2Index() { return _sf2Index; }

    /// Notify that a change has been made somewhere
    void update(QString editingSource);

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void tabTitleChanged(QString title);
    void filePathChanged(QString filePath);
    void recorderDisplayChanged(bool isDisplayed);
    void keyboardDisplayChanged(bool isDisplayed);

private slots:
    void inputProcessed();
    void onSelectionChanged(IdList ids);
    void displayOptionChanged(int displayOption);
    void onKeyPlayed(int key, int vel);
    void customizeKeyboard();
    void onEditingDone(QString editingSource, QList<int> sf2Indexes);

private:
    void updateTitleAndPath();

    Ui::Editor *ui;
    int _sf2Index;
    PageSelector * _pageSelector;
    ElementType _currentElementType;
    IdList _currentIds;
};

#endif // EDITOR_H
