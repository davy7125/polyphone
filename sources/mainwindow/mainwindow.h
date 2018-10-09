/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sf2_types.h"

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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void dragAndDrop(QString path, EltID idDest = EltID(elementUnknown, -1, -1, -1, -1), int *arg = NULL);

public slots:
    void slotAddTab();
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
    void on_pushButtonSoundfonts_clicked();
    void on_pushButtonOpen_clicked();
    void on_pushButtonNew_clicked();
    void onKeyboardDisplayChange(bool isDisplayed);
    void onRecorderDisplayChange(bool isDisplayed);

private:
    Ui::MainWindow * ui;
    WindowManager * _windowManager;
    DialogKeyboard * _keyboard;
    DialogRecorder * _recorder;
};

#endif // WINDOW_H
