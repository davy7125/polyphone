/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#ifndef DIALOG_ABOUT_H
#define DIALOG_ABOUT_H

#include <QDialog>

namespace Ui {
class DialogAbout;
}


class Credit
{
public:
    Credit() {}
    void addCreator(QString name, QString mail);
    void addContributor(QString name, QString mail);
    void addTranslator(QString name, QString mail);
    QString getText();

private:
    QString getGroup(QString nameGroup, QStringList listName, QStringList listMail);
    QString getFormattedName(QString name, QString email);
    QStringList _listCreatorName, _listCreatorMail,
        _listContributorName, _listContributorMail,
        _listTranslatorName, _listTranslatorMail;
};


class DialogAbout : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();
    
private slots:
    void on_pushCredit_clicked(bool checked);
    void on_pushClose_clicked();

private:
    Ui::DialogAbout *ui;
    Credit _credit;
};

#endif // DIALOG_ABOUT_H
