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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "basetypes.h"
#include "dialog_about.h"

namespace Ui {
class MainWindow;
}
class WindowManager;
class DialogKeyboard;
class DialogRecorder;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotCloseTab(int index);
    void recentSf2Changed();
    void openFile(QString fileName);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_pushButtonDocumentation_clicked();
    void on_pushButtonForum_clicked();
    void on_pushButtonSettings_clicked();
    void on_pushButtonSearch_clicked();
    void on_lineSearch_returnPressed();
    void on_pushButtonSoundfonts_clicked();
    void on_pushButtonOpen_clicked();
    void on_pushButtonNew_clicked();
    void onAboutClicked();
    void onKeyboardDisplayChange(bool isDisplayed);
    void onRecorderDisplayChange(bool isDisplayed);
    void fullScreenTriggered();
    void onCloseFile();
    void onSave();
    void onSaveAs();
    void onUserClicked();

private:
    Ui::MainWindow * ui;
    WindowManager * _windowManager;
    DialogKeyboard * _keyboard;
    DialogRecorder * _recorder;
    DialogAbout _dialogAbout;
};

#endif // WINDOW_H
