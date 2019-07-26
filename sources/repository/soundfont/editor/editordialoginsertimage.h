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

#ifndef EDITORDIALOGINSERTIMAGE_H
#define EDITORDIALOGINSERTIMAGE_H

#include <QDialog>

namespace Ui {
class EditorDialogInsertImage;
}

class EditorDialogInsertImage : public QDialog
{
    Q_OBJECT

public:
    explicit EditorDialogInsertImage(QWidget *parent = nullptr);
    ~EditorDialogInsertImage();

signals:
    void accepted(QString url);

private slots:
    virtual void accept();

private:
    Ui::EditorDialogInsertImage *ui;
};

#endif // EDITORDIALOGINSERTIMAGE_H
