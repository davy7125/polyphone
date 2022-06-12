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

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
class ColoredTabWidget;
class ConfigPanel;
class SoundfontFilter;
class SoundfontBrowser;
class Editor;
class UserArea;
class SoundfontViewer;

class WindowManager : public QObject
{
    Q_OBJECT

public:
    static WindowManager * getInstance(ColoredTabWidget * tabWidget = nullptr);
    static void kill();

public slots:
    /// Open the configuration
    void openConfiguration();

    /// Open the soundfont browser
    void openSoundfont(QString fileName);
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

    /// Get the current sf2 if an editor is displayed, or -1
    int getCurrentSf2();

    /// Close the current tab
    void closeCurrentTab();

signals:
    /// Emitted when the visibility of the recorder changes
    void recorderDisplayChanged(bool isDisplayed);

    /// Emitted when the visibility of the keyboard changes
    void keyboardDisplayChanged(bool isDisplayed);

    /// Emitted when the current tab changed
    void editorOpen(bool isOpen);

private slots:
    /// Called when the name or the filepath of a soundfont changed
    void onTabTitleChanged(QString title);
    void onFilePathChanged(QString filePath);

    /// Called when the user wants to close a tab
    void onTabCloseRequested(int tabIndex);

    /// Called every time the tab changes
    void onTabIndexChanged(int tabIndex);

private:
    explicit WindowManager(ColoredTabWidget * tabWidget);
    ~WindowManager();

    ColoredTabWidget * _tabWidget;
    ConfigPanel * _configTab;
    SoundfontBrowser * _browserTab;
    UserArea * _userTab;
    QList<Editor*> _editors;
    QList<SoundfontViewer*> _viewers;

    static WindowManager * s_instance;
};

#endif // WINDOWMANAGER_H
