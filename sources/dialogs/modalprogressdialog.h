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

#ifndef MODALPROGRESSDIALOG_H
#define MODALPROGRESSDIALOG_H

#include <QProgressDialog>
#include <QKeyEvent>

class ModalProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    ModalProgressDialog(const QString &labelText, const QString &cancelButtonText,
                        int minimum, int maximum, QWidget *parent = 0, Qt::WindowFlags flags = 0) :
        QProgressDialog(labelText, cancelButtonText, minimum, maximum, parent, flags)
    {
        this->setWindowModality(Qt::WindowModal);
        this->setCancelButton(nullptr);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
        this->reset();
    }

    void keyPressEvent(QKeyEvent *evnt)
    {
        if (evnt->key() == Qt::Key_Escape)
            evnt->ignore();
        else
            QDialog::keyPressEvent(evnt);
    }
};

#endif // MODALPROGRESSDIALOG_H
