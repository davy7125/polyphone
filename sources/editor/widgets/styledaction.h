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

#ifndef STYLEDACTION_H
#define STYLEDACTION_H

#include <QToolButton>

class StyledAction : public QToolButton
{
    Q_OBJECT

public:
    StyledAction(QString label, QString iconName, QWidget *parent);
    void disable(bool isDisabled);
    void setData(int data);
    int getData();

signals:
    void clicked();

protected:
    void leaveEvent(QEvent * event) override;
    void enterEvent(QEvent * event) override;
    QPixmap _icon;
    QPixmap _iconHover;
    QPixmap _iconDisabled;

private slots:
    void onToggled(bool isChecked);
    void onClicked(bool isClicked);

private:
    bool _isDisabled;
    static const QSize ICON_SIZE;
    QColor _checkedBackgroundColor;
    int _data;
};

#endif // STYLEDACTION_H
