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

#include "elidedlabel.h"
#include <QDesktopServices>

void ElidedLabel::setTextToElide(const QString text)
{
    _text = text;
    _linkContent = "";
    this->resizeEvent(nullptr);
}

void ElidedLabel::setTextToElide(const QString text, const QString link)
{
    _text = text;
    _linkContent = link;
    this->resizeEvent(nullptr);
}

void ElidedLabel::resizeEvent(QResizeEvent * event)
{
    QFontMetrics metrics(this->font());
    int width = (event == nullptr ? this->width() : event->size().width());
    QString elidedText = metrics.elidedText(_text, Qt::ElideMiddle, width);
    this->setText(elidedText);

    if (event != nullptr)
        QLabel::resizeEvent(event);
}

void ElidedLabel::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton && !_linkContent.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(_linkContent));
    }
    QLabel::mousePressEvent(event);
}
