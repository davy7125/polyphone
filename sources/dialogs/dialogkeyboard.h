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

#ifndef DIALOGKEYBOARD_H
#define DIALOGKEYBOARD_H

#include <QDialog>

namespace Ui {
class DialogKeyboard;
}

class DialogKeyboard : public QDialog
{
    Q_OBJECT

public:
    explicit DialogKeyboard(QWidget *parent = nullptr);
    ~DialogKeyboard() override;

    // Focus on the keyboard and animate with a glow effect
    void glow();

protected:
    void closeEvent(QCloseEvent * event) override;

private slots:
    void on_comboType_currentIndexChanged(int index);
    void onMouseHover(int key, int vel);
    void keyPlayed(int key, int vel);
    void polyPressureChanged(int key, int pressure);
    void on_pushExpand_clicked();

private:
    void displayKeyInfo();
    void updateControlAreaVisibility();
    void resizeWindow();

    Ui::DialogKeyboard *ui;
    QList<QPair<int, QPair<int, int> > > _triggeredKeys; // velocity and aftertouch by key
};

#endif // DIALOGKEYBOARD_H
