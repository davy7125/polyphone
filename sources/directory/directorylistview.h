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

#ifndef DIRECTORYLISTVIEW_H
#define DIRECTORYLISTVIEW_H

#include <QListView>
#include "basetypes.h"
class DirectoryListModel;
class DirectoryListDelegate;
class DirectoryFileData;
class DirectorySortProxyModel;

class DirectoryListView : public QListView
{
    Q_OBJECT

public:
    explicit DirectoryListView(QWidget *parent = nullptr);

    void addFile(DirectoryFileData * fileData);
    void removeFile(QString filePath);
    void updateFile(DirectoryFileData * fileData);

signals:
    void contentChanged();
    void itemDoubleClicked(QString filePath, EltID elt);

public slots:
    void setSortType(int sortType);
    void setFilter(QString filter);

private:
    DirectorySortProxyModel * _proxy;
    DirectoryListModel * _model;
    DirectoryListDelegate * _delegate;
};

#endif // DIRECTORYLISTVIEW_H
