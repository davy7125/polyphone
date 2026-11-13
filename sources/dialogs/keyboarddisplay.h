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

#ifndef KEYBOARDDISPLAY_H
#define KEYBOARDDISPLAY_H

#include <QWidget>
#include <QKeySequence>
#include "keyboarddisplaycontent.h"
class QToolButton;
class KeyboardDisplayScreen;

// Display of the virtual keyboard below its banner: name of the element played in a dot-matrix style, with the name of
// its file above, and buttons for playing the previous / next element and the previous / next file (file of a soundfont
// explorer or editor in the order of the tabs), disabled when not available
class KeyboardDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardDisplay(QWidget * parent = nullptr);
    void setContent(const KeyboardDisplayContent &content);

    // Click on a button with the keyboard (Page Up / Page Down, with Shift for the files)
    // Return false if the button is not available
    bool triggerButton(bool next, bool file);

signals:
    void previousElementRequested();
    void nextElementRequested();
    void previousFileRequested();
    void nextFileRequested();

protected:
    void paintEvent(QPaintEvent * event) override;

private:
    QToolButton * createButton(QString svgPath, QSize iconSize, int width, QString toolTip, QKeySequence shortcut);

    QColor _backgroundColor;
    QColor _textColor;
    QToolButton * _buttonPreviousFile;
    QToolButton * _buttonPreviousElement;
    QToolButton * _buttonNextElement;
    QToolButton * _buttonNextFile;
    KeyboardDisplayScreen * _screen;
    KeyboardDisplayContent _content;
};

#endif // KEYBOARDDISPLAY_H
