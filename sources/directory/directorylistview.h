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
class QTimer;

class DirectoryListView : public QListView
{
    Q_OBJECT

public:
    enum Column
    {
        COLUMN_NAME,
        COLUMN_SIZE,
        COLUMN_DATE,
        COLUMN_SAMPLES,
        COLUMN_INSTRUMENTS,
        COLUMN_PRESETS
    };
    static const int COLUMN_NUMBER = 6;

    explicit DirectoryListView(QWidget *parent = nullptr);

    void addFile(DirectoryFileData * fileData);
    void removeFile(QString filePath);
    void updateFile(DirectoryFileData * fileData);

    // Display options: bit 0 for the preset summary, bit i for the column i (the name is always displayed)
    bool isPresetSummaryDisplayed() const;
    bool isColumnDisplayed(int column) const;

    // Horizontal limits of a column in a row having the specified width
    // Columns are aligned on the right, the name takes the remaining space
    void getColumnLimits(int column, int totalWidth, int &left, int &right) const;
    void setMinimumColumnWidth(int column, int width);

    // Sort
    int getSortColumn() const;
    bool isSortDescending() const;
    void toggleSort(int column); // Sort by the column, or reverse the order if already sorted by it

signals:
    void contentChanged();
    void columnsChanged();
    void itemDoubleClicked(QString filePath, EltID elt);
    void renameRequested(QString filePath);
    void deleteRequested(QString filePath);

public slots:
    void setDisplayOptions(int displayOptions);
    void setFilter(QString filter);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onModelChanged();
    void updateContentWidths();

private:
    void setSort(int column, bool descending);

    DirectorySortProxyModel * _proxy;
    DirectoryListModel * _model;
    DirectoryListDelegate * _delegate;
    QTimer * _timerContentWidths;
    int _displayOptions;
    int _minimumWidths[COLUMN_NUMBER];
    int _contentWidths[COLUMN_NUMBER];

    static const int COLUMN_SPACING;
};

#endif // DIRECTORYLISTVIEW_H
