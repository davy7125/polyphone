/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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
public:
    /// Constructor
    ElidedLabel(QWidget * parent = NULL) : QLabel(parent) {}

    /// Set the text to elide. The displayed text will be set automatically
    void setTextToElide(const QString text)
    {
        _text = text;
        _linkContent = "";
        this->resizeEvent(nullptr);
    }

    /// Set the text to elide, being wrap in <a> for creating a link. The displayed text will be set automatically
    void setTextToElide(const QString text, const QString linkContent)
    {
        _text = text;
        _linkContent = linkContent;
        this->resizeEvent(nullptr);
    }

protected:
    void resizeEvent(QResizeEvent * event)
    {
        QFontMetrics metrics(this->font());
        int width = (event == nullptr ? this->width() : event->size().width());
        QString elidedText = metrics.elidedText(_text, Qt::ElideMiddle, width);
        if (_linkContent.isEmpty())
            this->setText(elidedText);
        else
            this->setText("<a " + _linkContent + ">" + elidedText + "</a>");
    }

private:
    QString _text;
    QString _linkContent;
};

#endif // ELIDEDLABEL_H
