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

#include "dialog_about.h"
#include "ui_dialog_about.h"
#include <QDate>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->labelNomVersion->setText("Polyphone " + QString(VERSION));

    // Description
    QString annee = "2013";
    QString url = "http://www.polyphone.fr/";
    QString locale = QLocale::system().name().section('_', 0, 0);
    if (locale.compare("fr") == 0)
        url += "index.php?lang=fr&amp;page=home";
    if (QDate::currentDate().year() > 2013)
        annee += "-" + QString::number(QDate::currentDate().year());
    ui->labelDescription->setText(QString::fromUtf8("<html><head/><body><p align='center'>Copyright © ") + annee +
                                  " Davy Triponney<br/><br/><a href='" + url +
                                  "'><span style=' text-decoration: underline; color:#0000ff;'>" +
                                  trUtf8("Site internet de Polyphone") + "</span></a></p></body></html>");

    // Crédit
    _credit.addCreator("Davy Triponney", "info@polyphone.fr");
    _credit.addContributor("Kinwie", "kinwie@yahoo.com");
    _credit.addContributor("Steve Clarke", "stevertyu@yahoo.com");
    _credit.addTranslator("Davy Triponney", "info@polyphone.fr");
    _credit.addTranslator("F.J. Martínez Murcia", "fjesusmartinez@ugr.es");
    ui->textBrowser->setHtml(_credit.getText());
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushCredit_clicked(bool checked)
{
    ui->stackedWidget->setCurrentIndex(!checked);
}

void DialogAbout::on_pushClose_clicked()
{
    QDialog::close();
}


void Credit::addCreator(QString name, QString mail)
{
    _listCreatorName << name;
    _listCreatorMail << mail;
}

void Credit::addContributor(QString name, QString mail)
{
    _listContributorName << name;
    _listContributorMail << mail;
}

void Credit::addTranslator(QString name, QString mail)
{
    _listTranslatorName << name;
    _listTranslatorMail << mail;
}

QString Credit::getText()
{
    QString text = "<html><head/><body><table border='0' style='margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;' "
            "cellspacing='2' cellpadding='0'>";

    text += getGroup(QObject::trUtf8("Créé par "), _listCreatorName, _listCreatorMail) + "<tr></tr>";
    text += getGroup(QObject::trUtf8("Contributeurs "), _listContributorName, _listContributorMail) + "<tr></tr>";
    text += getGroup(QObject::trUtf8("Traduit par "), _listTranslatorName, _listTranslatorMail);

    return text + "</table></body></html>";
}

QString Credit::getGroup(QString nameGroup, QStringList listName, QStringList listMail)
{
    if (listName.size() == 0 || listName.size() != listMail.size())
        return "";

    QString text = "<tr><td width='50%'><p align='right'><span style='font-size:9pt;'>" + nameGroup + "</span></p></td>" +
            "<td width='50%'>" + getFormattedName(listName.at(0), listMail.at(0)) + "</td></tr>";

    for (int i = 1; i < listName.size(); i++)
        text += "<tr><td></td><td>" + getFormattedName(listName.at(i), listMail.at(i)) + "</td></tr>";

    return text;
}

QString Credit::getFormattedName(QString name, QString email)
{
    QString text = "<p>";
    if (email.size())
        text += "<a href='mailto:" + email + "'><span style='font-size:9pt; text-decoration: underline; color:#0000ff;'>" +
                QString::fromUtf8(name.toStdString().c_str()) + "</span></a>";
    else
        text += "<span style='font-size:9pt;'>" + QString::fromUtf8(name.toStdString().c_str()) + "</span>";
    return text + "</p>";
}
