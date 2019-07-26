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

#ifndef SHOWSOUNDFONTS_H
#define SHOWSOUNDFONTS_H

#include <QWidget>
#include <QMutex>
class UrlReaderJson;
class QListWidgetItem;
class SoundfontFilter;

namespace Ui {
class ShowSoundfonts;
}

class ShowSoundfonts : public QWidget
{
    Q_OBJECT

public:
    explicit ShowSoundfonts(QWidget *parent = nullptr);
    ~ShowSoundfonts();

public slots:
    void initialize();
    void soundfontListAvailable(QString error);

signals:
    void itemClicked(SoundfontFilter * filter);

protected:
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

private slots:
    void dailyListAvailable(QString error);
    void on_listWidget_itemSelectionChanged();
    void on_pushRetry_clicked();

private:
    void populate();
    QString loadDailyIds();
    void updateCellHeight();

    Ui::ShowSoundfonts *ui;
    UrlReaderJson * _urlReaderJson;
    QList<int> _dailyIds;
    bool _soundfontListOk, _dailyListOk;
    QMutex _mutex; // Because two URL queries are concurrent
    QString _error1, _error2;
    int _lastWidth;
};

#endif // SHOWSOUNDFONTS_H
