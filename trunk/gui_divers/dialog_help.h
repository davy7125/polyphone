/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOG_HELP_H
#define DIALOG_HELP_H

#include <QDialog>
#include <QUrl>

namespace Ui
{
    class DialogHelp;
}

class DialogHelp : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogHelp(QWidget * parent);
    ~DialogHelp();
    
private slots:
    void on_pushPrevious_clicked();
    void on_pushNext_clicked();
    void on_pushHome_clicked();
    void on_textBrowser_sourceChanged(const QUrl &arg1);

private:
    Ui::DialogHelp *ui;
    QString _previous, _next;
};

#endif // DIALOG_HELP_H
