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

#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QObject>
class MainStackedWidget;
class ConfigPanel;
class SoundfontFilter;
class SoundfontBrowser;
class Tab;
class UserArea;
class SoundfontViewer;
class PlayerOptions;
class DialogKeyboard;

class TabManager : public QObject
{
    Q_OBJECT

public:
    static TabManager * prepareInstance(DialogKeyboard * dialogKeyboard, MainStackedWidget * stackedWidget);
    static TabManager * getInstance();
    static void kill();

    void showHome();
    void setCurrentWidget(QWidget * widget);

public slots:
    /// Open the configuration
    void openConfiguration();

    /// Open the soundfont browser
    void openSoundfont(QString fileName, PlayerOptions * playerOptions);
    void openNewSoundfont();

    /// Open the repository, initialized with a filter
    /// Note: this function delete the filter, no need to do it elsewhere
    /// The filter can be NULL
    void openRepository(SoundfontFilter * filter);

    /// Open a soundfont available on the repository
    void openRepositorySoundfont(int id);

    /// Open the user profile
    void openUser();

    /// The function is called when pages have to be updated
    void editingDone(QString source, QList<int> sf2Indexes);

    /// Get the current sf2 if an editor or player is displayed, or -1
    int getCurrentSf2();

    /// Close the current tab
    void closeCurrentTab();

signals:
    /// Emitted when the visibility of the keyboard or recorder changes
    void keyboardDisplayChanged(bool isDisplayed, bool propagate = true);
    void recorderDisplayChanged(bool isDisplayed, bool propagate = true);

    /// Emitted when the current tab changed
    void tabOpen(bool isOpen);

private slots:
    /// Called when the name or the filepath of a soundfont changed
    void onTabTitleChanged(QString title);
    void onFilePathChanged(QString filePath);

    /// Called when the user wants to close a tab
    void onTabCloseRequested(QWidget * widget);

    /// Called every time the tab changes
    void onTabIndexChanged(int tabIndex);

private:
    explicit TabManager(DialogKeyboard * dialogKeyboard, MainStackedWidget * stackedWidget);
    ~TabManager();
    void saveWindowState();

    DialogKeyboard * _dialogKeyboard;
    MainStackedWidget * _stackedWidget;
    ConfigPanel * _configTab;
    SoundfontBrowser * _browserTab;
    UserArea * _userTab;
    QList<Tab*> _tabs;
    QList<SoundfontViewer*> _viewers;

    static TabManager * s_instance;
};

#endif // TABMANAGER_H
