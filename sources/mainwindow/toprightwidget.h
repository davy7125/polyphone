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

#ifndef TOPRIGHTWIDGET_H
#define TOPRIGHTWIDGET_H

#include <QWidget>
#include <QMap>
#include "usermanager.h"
class MainMenu;

namespace Ui {
class TopRightWidget;
}

class TopRightWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TopRightWidget(QWidget *parent = nullptr);
    ~TopRightWidget();

public slots:
    void onEditorOpen(bool isOpen);

signals:
    void openClicked();
    void newClicked();
    void openSettingsClicked();
    void onlineHelpClicked();
    void aboutClicked();
    void closeFileClicked();
    void closeClicked();
    void fullScreenTriggered();
    void save();
    void saveAs();
    void userClicked();

private slots:
    void userStateChanged(UserManager::ConnectionState state);
    void on_pushUser_clicked();
    void downloadCleared();
    void progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName);
    void downloadCanceled(int soundfontId);

private:
    Ui::TopRightWidget *ui;
    MainMenu * _menu;
    QMap<QString, QString> _colorReplacement;
};

#endif // TOPRIGHTWIDGET_H
