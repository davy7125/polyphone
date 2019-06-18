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

#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>
#include <QResizeEvent>

class ElidedLabel : public QLabel
{
    Q_OBJECT

public:
    /// Constructor
    ElidedLabel(QWidget * parent = nullptr) : QLabel(parent) {}

    /// Set the text to elide. The displayed text will be set automatically
    void setTextToElide(const QString text);

    /// Set the text to elide and specify a link when we click on it
    void setTextToElide(const QString text, const QString link);

protected:
    void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;

private:
    QString _text;
    QString _linkContent;
};

#endif // ELIDEDLABEL_H
