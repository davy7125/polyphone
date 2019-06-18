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

#ifndef TOOLMENU_H
#define TOOLMENU_H

#include <QMenu>
#include "basetypes.h"
class ToolFactory;
class AbstractTool;

class ToolMenu : public QMenu
{
    Q_OBJECT

public:
    ToolMenu(QWidget *parent = NULL);
    ~ToolMenu();

    /// Notify that the selection changed
    void selectionChanged(IdList ids);

private slots:
    /// When a QAction is clicked
    void onTriggered(QAction * action);

private:
    void addCategory(QString categoryName);
    static bool lessThan(const AbstractTool * tool1, const AbstractTool * tool2);
    QString _separatorBackgroundColor;
    QString _separatorTextColor;

    ToolFactory * _toolFactory;
    QMap<QAction *, AbstractTool *> _currentActions;
};

#endif // TOOLMENU_H
