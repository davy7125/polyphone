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

#ifndef SOUNDFONTBROWSER_H
#define SOUNDFONTBROWSER_H

#include <QWidget>
#include "usermanager.h"
class SoundfontFilter;
class SoundfontInformation;

namespace Ui {
class SoundfontBrowser;
}

class SoundfontBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontBrowser(QWidget *parent = nullptr);
    ~SoundfontBrowser();

public slots:
    void applyFilter(SoundfontFilter * filter);
    void initialize();
    void soundfontListAvailable(QString error);

protected:
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

private slots:
    void on_pushRetry_clicked();
    void on_pushResetFilters_clicked();
    void updateFilter();
    void on_listWidget_itemSelectionChanged();
    void onComboSortCurrentIndexChanged(int index);
    void on_pushBecomePremium_clicked();
    void userStatusChanged(UserManager::ConnectionState state);
    void on_pushGoPrevious_clicked();
    void on_pushGoNext_clicked();

private:
    void fillFilter();
    SoundfontFilter * getFilter();
    void updateSort();
    void updatePage();
    void updateCellHeight();

    Ui::SoundfontBrowser *ui;
    bool _loadingFilter;
    QList<SoundfontInformation *> _currentSoundfontInfos;
    int _currentPage;
    static const int ITEMS_PER_PAGE;
};

#endif // SOUNDFONTBROWSER_H
