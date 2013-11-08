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

#include "dialog_help.h"
#include "ui_dialog_help.h"

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint |
            Qt::WindowSystemMenuHint | Qt::Window | Qt::WindowStaysOnTopHint),
    ui(new Ui::DialogHelp)
{
    ui->setupUi(this);
    on_pushHome_clicked();
}

DialogHelp::~DialogHelp()
{
    delete ui;
}

void DialogHelp::on_pushPrevious_clicked()
{
    ui->textBrowser->setSource(QUrl(_previous));
}

void DialogHelp::on_pushNext_clicked()
{
    ui->textBrowser->setSource(QUrl(_next));
}

void DialogHelp::on_pushHome_clicked()
{
    ui->textBrowser->setSource(QUrl(tr("qrc:/aide/aide.html")));
}

void DialogHelp::on_textBrowser_sourceChanged(const QUrl &arg1)
{
    QString currentPage = arg1.toString();

    if (currentPage.compare("qrc:/aide/help.html") == 0)
    {
        _previous = "qrc:/aide/help.html";
        ui->pushPrevious->setEnabled(false);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page01en.html";
        ui->labelTitre->setText("POLYPHONE");
        ui->labelPage->setText("1/10");
    }
    else if (currentPage.compare("qrc:/aide/page01en.html") == 0)
    {
        _previous = "qrc:/aide/help.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page02en.html";
        ui->labelTitre->setText("MENU BAR");
        ui->labelPage->setText("2/10");
    }
    else if (currentPage.compare("qrc:/aide/page02en.html") == 0)
    {
        _previous = "qrc:/aide/page01en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page03en.html";
        ui->labelTitre->setText("TOOLBAR");
        ui->labelPage->setText("3/10");
    }
    else if (currentPage.compare("qrc:/aide/page03en.html") == 0)
    {
        _previous = "qrc:/aide/page02en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page04en.html";
        ui->labelTitre->setText("TREE");
        ui->labelPage->setText("4/10");
    }
    else if (currentPage.compare("qrc:/aide/page04en.html") == 0)
    {
        _previous = "qrc:/aide/page03en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page05en.html";
        ui->labelTitre->setText("GLOBAL INFORMATION");
        ui->labelPage->setText("5/10");
    }
    else if (currentPage.compare("qrc:/aide/page05en.html") == 0)
    {
        _previous = "qrc:/aide/page04en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page06en.html";
        ui->labelTitre->setText("SAMPLES");
        ui->labelPage->setText("6/10");
    }
    else if (currentPage.compare("qrc:/aide/page06en.html") == 0)
    {
        _previous = "qrc:/aide/page05en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page07en.html";
        ui->labelTitre->setText("INSTRUMENTS");
        ui->labelPage->setText("7/10");
    }
    else if (currentPage.compare("qrc:/aide/page07en.html") == 0)
    {
        _previous = "qrc:/aide/page06en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page08en.html";
        ui->labelTitre->setText("PRESETS");
        ui->labelPage->setText("8/10");
    }
    else if (currentPage.compare("qrc:/aide/page08en.html") == 0)
    {
        _previous = "qrc:/aide/page07en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page09en.html";
        ui->labelTitre->setText("TOOLS");
        ui->labelPage->setText("9/10");
    }
    else if (currentPage.compare("qrc:/aide/page09en.html") == 0)
    {
        _previous = "qrc:/aide/page08en.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(false);
        _next = "qrc:/aide/page09en.html";
        ui->labelTitre->setText("PREFERENCES");
        ui->labelPage->setText("10/10");
    }
    else if (currentPage.compare("qrc:/aide/aide.html") == 0)
    {
        _previous = "qrc:/aide/aide.html";
        ui->pushPrevious->setEnabled(false);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page01fr.html";
        ui->labelTitre->setText("POLYPHONE");
        ui->labelPage->setText("1/10");
    }
    else if (currentPage.compare("qrc:/aide/page01fr.html") == 0)
    {
        _previous = "qrc:/aide/aide.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page02fr.html";
        ui->labelTitre->setText("BARRE DE MENU");
        ui->labelPage->setText("2/10");
    }
    else if (currentPage.compare("qrc:/aide/page02fr.html") == 0)
    {
        _previous = "qrc:/aide/page01fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page03fr.html";
        ui->labelTitre->setText("BARRE D'OUTILS");
        ui->labelPage->setText("3/10");
    }
    else if (currentPage.compare("qrc:/aide/page03fr.html") == 0)
    {
        _previous = "qrc:/aide/page02fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page04fr.html";
        ui->labelTitre->setText("ARBORESCENCE");
        ui->labelPage->setText("4/10");
    }
    else if (currentPage.compare("qrc:/aide/page04fr.html") == 0)
    {
        _previous = "qrc:/aide/page03fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page05fr.html";
        ui->labelTitre->setText("INFORMATIONS GENERALES");
        ui->labelPage->setText("5/10");
    }
    else if (currentPage.compare("qrc:/aide/page05fr.html") == 0)
    {
        _previous = "qrc:/aide/page04fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page06fr.html";
        ui->labelTitre->setText("SONS");
        ui->labelPage->setText("6/10");
    }
    else if (currentPage.compare("qrc:/aide/page06fr.html") == 0)
    {
        _previous = "qrc:/aide/page05fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page07fr.html";
        ui->labelTitre->setText("INSTRUMENTS");
        ui->labelPage->setText("7/10");
    }
    else if (currentPage.compare("qrc:/aide/page07fr.html") == 0)
    {
        _previous = "qrc:/aide/page06fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page08fr.html";
        ui->labelTitre->setText("PRESETS");
        ui->labelPage->setText("8/10");
    }
    else if (currentPage.compare("qrc:/aide/page08fr.html") == 0)
    {
        _previous = "qrc:/aide/page07fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(true);
        _next = "qrc:/aide/page09fr.html";
        ui->labelTitre->setText("OUTILS");
        ui->labelPage->setText("9/10");
    }
    else if (currentPage.compare("qrc:/aide/page09fr.html") == 0)
    {
        _previous = "qrc:/aide/page08fr.html";
        ui->pushPrevious->setEnabled(true);
        ui->pushNext->setEnabled(false);
        _next = "qrc:/aide/page09fr.html";
        ui->labelTitre->setText("PREFERENCES");
        ui->labelPage->setText("10/10");
    }
}
