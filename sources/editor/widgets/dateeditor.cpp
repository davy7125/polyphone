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

#include "dateeditor.h"
#include <QCalendarWidget>
#include <QMenu>
#include <QWidgetAction>
#include <QTextCharFormat>
#include <QEvent>
#include <QKeyEvent>

DateEditor::DateEditor(QWidget * parent) : QLabel(parent)
{
    this->setCursor(Qt::PointingHandCursor);
    this->setFocusPolicy(Qt::StrongFocus);

    _menu = new QMenu(this);
    _calendar = new QCalendarWidget(_menu);
    QWidgetAction * action = new QWidgetAction(_menu);
    action->setDefaultWidget(_calendar);
    _menu->addAction(action);

    _calendar->setWeekdayTextFormat(Qt::Saturday, _calendar->weekdayTextFormat(Qt::Monday));
    _calendar->setWeekdayTextFormat(Qt::Sunday, _calendar->weekdayTextFormat(Qt::Monday));
    connect(_calendar, SIGNAL(clicked(QDate)), this, SLOT(onDateClicked(QDate)));
    _menu->installEventFilter(this);
}

DateEditor::~DateEditor()
{
    _menu->removeEventFilter(this);
}

void DateEditor::setInitialText(QString text)
{
    _initialText = text;

    _currentDate = QDate::fromString(text, "dddd d MMMM yyyy, hh:mm:ss");
    if (!_currentDate.isValid())
        _currentDate = QDate::fromString(text, "yyyyy-MM-ddThh:mm:ss");
    if (!_currentDate.isValid())
        _currentDate = QDate::fromString(text, "yyyyy-MM-dd");

    this->setText(_currentDate.isValid() ? _currentDate.toString() : text);
}

void DateEditor::focusInEvent(QFocusEvent *e)
{
    QLabel::focusInEvent(e);

    // Show the popup
    _calendar->setSelectedDate(_currentDate.isValid() ? _currentDate : QDate::currentDate());
    _menu->popup(this->mapToGlobal(this->rect().bottomLeft()));
    this->clearFocus();
    _calendar->setFocus();
}

void DateEditor::onDateClicked(QDate date)
{
    setInitialText(date.toString("yyyy-MM-dd"));
    _menu->hide();
    emit(editingFinished());
}

bool DateEditor::eventFilter(QObject * watched, QEvent * event)
{
    if ((watched == _calendar || watched == _menu) && event->type() == QEvent::KeyPress)
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            qDebug() << "next";
            QWidget * next = this->parentWidget()->nextInFocusChain();
            if (next)
                next->setFocus();
        }
        else if (keyEvent->key() == Qt::Key_Backtab ||
                   (keyEvent->key() == Qt::Key_Tab && keyEvent->modifiers() & Qt::ShiftModifier))
        {
            qDebug() << "previous";
            QWidget * previous = this->parentWidget()->previousInFocusChain();
            if (previous)
                previous->setFocus();
        }
    }

    return false;
}
