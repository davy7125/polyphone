/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#ifndef DIALOGQUESTION_H
#define DIALOGQUESTION_H

#include <QDialog>

namespace Ui {
class DialogQuestion;
}

class DialogQuestion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogQuestion(QWidget *parent = nullptr);
    ~DialogQuestion();

    void initialize(QString title, QString placeHolder, QString defaultValue);
    void setTextLimit(int textLimit);

signals:
    void onOk(QString txt);

private slots:
    void on_pushCancel_clicked();
    void on_pushOk_clicked();

private:
    Ui::DialogQuestion *ui;
};

#endif // DIALOGQUESTION_H
