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

#include "styledlineedit.h"
#include "contextmanager.h"
#include <QPainter>
#include <QKeyEvent>

StyledLineEdit::StyledLineEdit(QWidget * parent) : QLineEdit(parent),
    _resize(true),
    _defaultWidth(0)
{
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited(QString)));

    // Default color
    _textColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
    this->setStyle(false);
}

void StyledLineEdit::setTextToElide(const QString text)
{
    this->setText(text);

    // Initial width
    this->setMinimumWidth(sizeHint().width());
    this->setMinimumWidth(0);
}

void StyledLineEdit::focusInEvent(QFocusEvent *e)
{
    // Restore the full text
    this->blockSignals(true);
    this->setText(_fullText);
    this->blockSignals(false);

    this->setStyle(true);
    QLineEdit::focusInEvent(e);
    emit focussed(true);
}

void StyledLineEdit::focusOutEvent(QFocusEvent *e)
{
    this->setStyle(false);
    QLineEdit::focusOutEvent(e);
    emit focussed(false);

    // Adapt the text
    resizeEvent(nullptr);
}

void StyledLineEdit::keyPressEvent(QKeyEvent * e)
{
    if (e->key() == Qt::Key_Enter)
        this->clearFocus();

    QLineEdit::keyPressEvent(e);
}

void StyledLineEdit::setColor(QColor textColor)
{
    _textColor = textColor;this->sizeHint();
    this->setStyle(this->hasFocus());
}

void StyledLineEdit::setStyle(bool hasFocus)
{
    if (hasFocus)
    {
        this->setCursor(Qt::IBeamCursor);
        this->setStyleSheet("");
    }
    else
    {
        this->setCursor(Qt::PointingHandCursor);
        this->setStyleSheet("QLineEdit{color:" + _textColor.name() + ";background-color:transparent; border: 0}");
    }
}

void StyledLineEdit::onTextEdited(QString text)
{
    if (!_resize)
        return;

    _fullText = text;

    // Adapt the width
    QFontMetrics fm(this->font());
    _defaultWidth = fm.horizontalAdvance(text.isEmpty() ? this->placeholderText() : text) + 20;
}

QSize StyledLineEdit::sizeHint() const
{
    int width = _defaultWidth;
    if (width == 0 && this->text().isEmpty())
    {
        QFontMetrics fm(this->font());
        width = fm.horizontalAdvance(this->placeholderText()) + 20;
    }
    return QSize(width, QLineEdit::sizeHint().height());
}

void StyledLineEdit::resizeEvent(QResizeEvent * e)
{
    // Compute the text to display
    QFontMetrics metrics(this->font());
    int width = (e == nullptr ? this->width() : e->size().width());
    QString elidedText = metrics.elidedText(_fullText, Qt::ElideRight, width - 10);

    // Display the elided text
    this->blockSignals(true);
    this->setText(elidedText);
    this->blockSignals(false);

    if (e != nullptr)
        QLineEdit::resizeEvent(e);
}
