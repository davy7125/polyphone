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

#include "dialog_about.h"
#include "ui_dialog_about.h"
#include "contextmanager.h"
#include <QDate>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    ui->labelNomVersion->setText(trUtf8("Polyphone") + " " +
                                 (qAbs(VERSION) == VERSION ? QString::number(VERSION, 'f', 1) : QString::number(VERSION)) +
                                 (FINAL ? "" : " unreleased"));

    // Icon
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    ui->labelIcon->setPixmap(ContextManager::theme()->getColoredSvg(":/misc/logo.svg", QSize(128, 128), replacement));

    // Description
    QString annee = "2013 -" + QString::number(QDate::currentDate().year());
    QString url = trUtf8("https://www.polyphone-soundfonts.com/en");
    QString urlDonation = "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=ESBLSGPJ7P938&lc=US&item_name=Polyphone&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHosted";
    ui->labelDescription->setText("<html><head/><body><p align='center'>" + trUtf8("Copyright") + " © " + annee +
                                  " " + trUtf8("Davy Triponney") + "<br/><br/><a href='" + url +
                                  "'><span style=' text-decoration: underline; color:" +
                                  this->palette().color(QPalette::Link).name() + ";'>" +
                                  trUtf8("Polyphone website") + "</span></a>" +
                                  "<br/><br/><a href='" + urlDonation +
                                  "'><span style=' text-decoration: underline; color:" +
                                  this->palette().color(QPalette::Link).name() + ";'>" +
                                  trUtf8("Donate") + "</span></a>" +
                                  "</p></body></html>");

    // Credits
    _credit.addCreator(trUtf8("Davy Triponney"), "contact@polyphone-soundfonts.com");

    _credit.addContributor(trUtf8("Andrea Celani"), "acelani74@gmail.com");
    _credit.addContributor(trUtf8("Kinwie"), "kinwie@yahoo.com");
    _credit.addContributor(trUtf8("Michael Schyllberg"), "michael.schyllberg@bredband.net");
    _credit.addContributor(trUtf8("Paul Stratman"), "pcstratman@gmail.com");
    _credit.addContributor(trUtf8("Steve Clarke"), "stevertyu@yahoo.com");

    _credit.addTranslator(trUtf8("Andrea Celani"), "acelani74@gmail.com"); // Italian
    _credit.addTranslator(trUtf8("Davy Triponney"), "contact@polyphone-soundfonts.com"); // French
    _credit.addTranslator(trUtf8("F.J. Martínez Murcia"), "fjesusmartinez@ugr.es"); // Spanish
    _credit.addTranslator(trUtf8("Georg Gergull"), "geoger@hotmail.de"); // German
    _credit.addTranslator(trUtf8("Jay Alexander Fleming"), "tito.nehru.naser@gmail.com"); // Serbian
    _credit.addTranslator(trUtf8("Joel Gomes"), "joelgomes1994@hotmail.com"); // Portuguese
    _credit.addTranslator(trUtf8("Magson"), "magsom@qq.com"); // Chinese
    _credit.addTranslator(trUtf8("Pavel Fric"), "pavelfric@seznam.cz"); // Czech
    _credit.addTranslator(trUtf8("Steve Clarke"), "stevertyu@yahoo.com"); // English

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

    text += getGroup(QObject::trUtf8("Created by") + " ", _listCreatorName, _listCreatorMail) + "<tr></tr>";
    text += getGroup(QObject::trUtf8("Contributors") + " ", _listContributorName, _listContributorMail) + "<tr></tr>";
    text += getGroup(QObject::trUtf8("Translated by") + " ", _listTranslatorName, _listTranslatorMail) + "<tr></tr>";
    text += getAwesomeCredit();

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
        text += "<a href='mailto:" + email + "'><span style='font-size:9pt; text-decoration: underline; color:" +
                QApplication::palette().color(QPalette::Link).name() + ";'>" +
                QString::fromUtf8(name.toStdString().c_str()) + "</span></a>";
    else
        text += "<span style='font-size:9pt;'>" + QString::fromUtf8(name.toStdString().c_str()) + "</span>";
    return text + "</p>";
}

QString Credit::getFormattedLink(QString text, QString link)
{
    return "<a href='" + link + "'><span style='font-size:9pt; text-decoration: underline; color:" +
            QApplication::palette().color(QPalette::Link).name() + ";'>" +
            QString::fromUtf8(text.toStdString().c_str()) + "</span></a>";
}

QString Credit::getAwesomeCredit()
{
    return "<tr><td width='50%'><p align='right'><span style='font-size:9pt;'>" +
            QObject::trUtf8("Icons") +
            " </span></p></td>" +
            "<td width='50%'>Most of the icons are provided by " +
            getFormattedLink("Awesome", "https://fontawesome.com/") +
            " under the license " +
            getFormattedLink("CC BY 4.0", "https://creativecommons.org/licenses/by/4.0/") +
            " and have been colored to fit the themes.</td></tr>";
}
