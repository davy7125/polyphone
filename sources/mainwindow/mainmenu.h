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

#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenu>
class AbstractTool;

class MainMenu : public QMenu
{
    Q_OBJECT

public:
    MainMenu(QWidget *parent = nullptr);
    ~MainMenu();
    void setFullScreen(bool isOn);

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

private slots:
    void onExport();

private:
    QAction * _newAction;
    QAction * _openAction;
    QAction * _saveAction;
    QAction * _saveAsAction;
    QAction * _exportAction;
    QAction * _fullScreenAction;
    QAction * _settingsAction;
    QAction * _helpAction;
    QAction * _aboutAction;
    QAction * _closeFileAction;
    QAction * _closeAction;

    AbstractTool * _toolExport;
};

#endif // MAINMENU_H
