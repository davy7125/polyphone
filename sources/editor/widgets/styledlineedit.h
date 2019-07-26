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

#ifndef STYLEDLINEEDIT_H
#define STYLEDLINEEDIT_H

#include <QLineEdit>

class StyledLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    StyledLineEdit(QWidget *parent = nullptr);

    // Set it to false if you want the widget keeps the same size
    // By default: true
    void resizeAfterChange(bool resizeOn) { _resize = resizeOn; }

    // Change the color of the text
    void setColor(QColor textColor);

signals:
    void focussed(bool hasFocus);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void keyPressEvent(QKeyEvent * e);

private slots:
    void onTextEdited(QString text);

private:
    void setStyle(bool hasFocus);

    bool _resize;
    QColor _textColor;
};

#endif // STYLEDLINEEDIT_H
