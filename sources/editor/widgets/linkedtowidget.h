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

#ifndef LINKEDTOWIDGET_H
#define LINKEDTOWIDGET_H

#include <QWidget>
#include <QMap>
#include "basetypes.h"
class FlowLayout;

class LinkedToWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LinkedToWidget(QWidget *parent = nullptr);
    ~LinkedToWidget();

    /// Clear all links
    void clear();

    /// Initialization
    void initialize(EltID id);

    /// Get the number of elements linked
    int getLinkNumber();

signals:
    /// Emitted when an item is clicked
    void itemClicked(EltID id);

private slots:
    void onClick(bool isClicked);

private:
    void addLink(EltID id, QString text);

    FlowLayout * _layout;
    QMap<QWidget *, EltID> _buttonIds;
};

#endif // LINKEDTOWIDGET_H
