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

#ifndef INFOBUTTON_H
#define INFOBUTTON_H

#include <QToolButton>
class QLabel;
class QTimer;

// Information icon showing its tooltip text (possibly a table) when the mouse stays over it, or immediately with a
// click. The text is shown in a window of its own below the icon, which stays open while the mouse is over the icon
// or over the window, so that it can be read or reached (a QToolTip closes as soon as the mouse leaves the icon).
class InfoButton : public QToolButton
{
    Q_OBJECT

public:
    explicit InfoButton(QWidget * parent = nullptr);

    // Rows of a table with two columns (names, then values, possibly aligned on the right), headers spanning both columns
    static QString tableRow(QString name, QString value, bool alignRight = false);
    static QString tableHeader(QString text, bool first = false);

protected:
    bool event(QEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void onClicked();
    void onHideTimeout();

private:
    void showPopup();
    void hidePopup();
    void updatePopup();

    QLabel * _popup;
    QTimer * _hideTimer; // Delay for going from the icon to the window, or back
};

#endif // INFOBUTTON_H
