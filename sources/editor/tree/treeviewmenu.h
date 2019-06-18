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

#ifndef TREEVIEWMENU_H
#define TREEVIEWMENU_H

#include <QMenu>
#include "basetypes.h"
class DialogList;

class TreeViewMenu : public QMenu
{
    Q_OBJECT

public:
    TreeViewMenu(QWidget * parent);
    ~TreeViewMenu();

    /// Initialize the menu with the selected IDs
    void initialize(IdList ids);

public slots:
    void associate();
    void replace();
    void itemSelectedFromList(EltID id, bool isAssociation);
    void copy();
    void paste();
    void duplicate();
    void remove();
    void rename();

private slots:
    void bulkRename(int renameType, QString text1, QString text2, int val1, int val2);

private:
    void associate(IdList ids, EltID idDest);
    void replace(EltID idSrc, EltID idDest);

    IdList _currentIds;
    QAction * _replaceAction;
    QAction * _associateAction;
    QAction * _copyAction;
    QAction * _pasteAction;
    QAction * _duplicateAction;
    QAction * _removeAction;
    QAction * _renameAction;
    DialogList * _dialogList;

    // Same copy for all instances
    static IdList s_copy;
};

#endif // TREEVIEWMENU_H
