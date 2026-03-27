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

#ifndef DATEEDITOR_H
#define DATEEDITOR_H

#include <QLabel>
#include <QDate>
class QMenu;
class QCalendarWidget;

class DateEditor : public QLabel
{
    Q_OBJECT

public:
    DateEditor(QWidget *parent = nullptr);
    ~DateEditor();

    void setInitialText(QString text);
    QString getInitialText() { return _initialText; }

signals:
    void editingFinished();

protected:
    void focusInEvent(QFocusEvent * e) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void onDateClicked(QDate date);

private:
    QMenu * _menu;
    QCalendarWidget * _calendar;
    QString _initialText;
    QDate _currentDate;
};

#endif // DATEEDITOR_H
