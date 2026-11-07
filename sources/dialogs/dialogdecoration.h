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

#ifndef DIALOGDECORATION_H
#define DIALOGDECORATION_H

#include <QObject>
#include <QColor>
#include <QPainterPath>
class QDialog;
class QLabel;

// Like the main window, the windows having a banner (virtual keyboard, recorder, MIDI player) lose their borders if
// the option "Window borders" is unchecked. Their banner then shows the title of the window on its left side and a
// close button on its right side, both having the background of the top of the main window. The banner moves the
// window, except where a widget reacts to the mouse. A line surrounds the window, rounded at the top as the main
// window, and its edges resize it.
class DialogDecoration : public QObject
{
    Q_OBJECT

public:
    // Remove the borders of a dialog if required by the configuration. "banner" is at the top of the main layout of the
    // dialog
    static void apply(QDialog * dialog, QWidget * banner);

protected:
    bool eventFilter(QObject * watched, QEvent * event) override;

private:
    DialogDecoration(QDialog * dialog, QWidget * banner);
    QPainterPath getOutline(bool closed);
    void paintBackground();
    bool isControl(QWidget * widget);

    static const int CLOSE_MARGIN;

    QDialog * _dialog;
    QWidget * _titleBar;
    QLabel * _labelTitle;
    QColor _borderColor;
    QColor _titleColor;
    QColor _bannerColor;
    bool _aboutToMove;
};

#endif // DIALOGDECORATION_H
