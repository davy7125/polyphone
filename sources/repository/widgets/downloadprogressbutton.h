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

#ifndef DOWNLOADPROGRESSBUTTON_H
#define DOWNLOADPROGRESSBUTTON_H

#include <QToolButton>
#include <QMap>
#include <QMutex>
class DownloadProgressCell;
class QMenu;

class DownloadProgressButton : public QToolButton
{
    Q_OBJECT

public:
    DownloadProgressButton(QWidget *parent = nullptr);
    ~DownloadProgressButton();

    void progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName);
    void downloadCanceled(int soundfontId);

signals:
    void cleared();

private slots:
    void clearCompletedDownloads();
    void onCloseMenu();

private:
    void updatePercent();
    void removeCell(DownloadProgressCell * cell);

    QMap<QString, QString> _svgReplacements;
    QMenu * _menu;
    QMap<int, DownloadProgressCell *> _cells;
    QMutex _mutex;
};

#endif // DOWNLOADPROGRESSBUTTON_H
