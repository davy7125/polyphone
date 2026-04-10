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

#ifndef DIRECTORYBROWSER_H
#define DIRECTORYBROWSER_H

#include <QWidget>
#include "basetypes.h"
class QFileSystemWatcher;

namespace Ui {
class DirectoryBrowser;
}

class DirectoryBrowser : public QWidget
{
    Q_OBJECT

public:
    DirectoryBrowser(QWidget *parent = nullptr);
    ~DirectoryBrowser();

    void initialize(QString dirPath);
    QString getDirectoryPath() { return _dirPath; }

signals:
    void itemDoubleClicked(QString filePath, EltID id);

private slots:
    void on_pushRetry_clicked();
    void onDirectoryChanged(const QString &path);
    void onContentChanged();
    void onSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void on_listView_doubleClicked(const QModelIndex &index);
    void onRenameRequested(QString path);
    void onDeleteRequested(QString path);

private:
    Ui::DirectoryBrowser *ui;
    QString _dirPath;
    QFileSystemWatcher * _watcher;
    QMap<QString, QDateTime> _currentFiles;
};

#endif // DIRECTORYBROWSER_H
