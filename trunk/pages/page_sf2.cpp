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

#include "page_sf2.h"
#include "ui_page_sf2.h"
#include "mainwindow.h"
#include <QDateTime>

Page_Sf2::Page_Sf2(MainWindow * mainWindow, Tree * tree, QStackedWidget * qStackedWidget,
                   Pile_sf2 * sf2, Synth *synth, QWidget * parent) :
    Page(PAGE_SF2, parent), ui(new Ui::Page_Sf2)
{
    ui->setupUi(this);
    this->_mainWindow = mainWindow;
    this->_tree = tree;
    this->_qStackedWidget = qStackedWidget;
    this->_sf2 = sf2;
    this->_synth = synth;
}

Page_Sf2::~Page_Sf2()
{
    delete ui;
}

void Page_Sf2::afficher()
{
    // Préparation de l'affichage
    _preparation = 1;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    id.indexElt = 0;
    id.indexElt2 = 0;
    id.indexMod = 0;

    // Eléments non utilisés
    this->compte();

    // Mode 24 bits ?
    ui->check_24bits->setChecked(this->_sf2->get(id, champ_wBpsSave).wValue == 24);

    // Informations
    QString txt;
#ifndef Q_OS_MAC
    txt = " "; // Sinon le premier "/" n'est pas visible entièrement
#endif
    txt += this->_sf2->getQstr(id, champ_filename);
    ui->label_filename->setText(txt);
    ui->label_sfVersion->setText(QString("%1.%2")
                                 .arg(this->_sf2->get(id, champ_IFIL).sfVerValue.wMajor)
                                 .arg(this->_sf2->get(id, champ_IFIL).sfVerValue.wMinor, 2, 10, QChar('0')));
    ui->label_soundEngine->setText(this->_sf2->getQstr(id, champ_ISNG));
    txt = QString("%1.%2")
            .arg(this->_sf2->get(id, champ_IVER).sfVerValue.wMajor)
            .arg(this->_sf2->get(id, champ_IVER).sfVerValue.wMinor, 2, 10, QChar('0'));
    if (this->_sf2->getQstr(id, champ_IROM).isEmpty())
        ui->label_romVersion->setText("- (version " + txt + ")");
    else
        ui->label_romVersion->setText(this->_sf2->getQstr(id, champ_IROM) + " (version " + txt + ")");
    ui->label_software->setText(this->_sf2->getQstr(id, champ_ISFT));
    ui->lineEdit_name->setText(this->_sf2->getQstr(id, champ_name));
    ui->lineEdit_copyright->setText(this->_sf2->getQstr(id, champ_ICOP));
    ui->lineEdit_author->setText(this->_sf2->getQstr(id, champ_IENG));
    ui->lineEdit_date->setText(this->_sf2->getQstr(id, champ_ICRD));
    ui->lineEdit_product->setText(this->_sf2->getQstr(id, champ_IPRD));
    ui->textEdit_Com->setPlainText(this->_sf2->getQstr(id, champ_ICMT));
    // Basculement de l'affichage
    this->_qStackedWidget->setCurrentWidget(this);
    _preparation = 0;
}

void Page_Sf2::set24bits(int checked)
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Configuration d'un SF2 : mode 16 ou 24 bits
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    Valeur valTmp;
    if (checked == 2)
        valTmp.wValue = 24;
    else
        valTmp.wValue = 16;
    this->_sf2->set(id, champ_wBpsSave, valTmp);
    this->_mainWindow->updateDo();
}

void Page_Sf2::setName()
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->lineEdit_name->text().compare(this->_sf2->getQstr(id, champ_name)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_name, ui->lineEdit_name->text());
    this->_mainWindow->updateDo();
}
void Page_Sf2::setCopyright()
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->lineEdit_copyright->text().compare(this->_sf2->getQstr(id, champ_ICOP)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_ICOP, ui->lineEdit_copyright->text());
    this->_mainWindow->updateDo();
}
void Page_Sf2::setAuthor()
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->lineEdit_author->text().compare(this->_sf2->getQstr(id, champ_IENG)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_IENG, ui->lineEdit_author->text());
    this->_mainWindow->updateDo();
}
void Page_Sf2::setDate()
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->lineEdit_date->text().compare(this->_sf2->getQstr(id, champ_ICRD)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_ICRD, ui->lineEdit_date->text());
    this->_mainWindow->updateDo();
}
void Page_Sf2::setProduct()
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->lineEdit_product->text().compare(this->_sf2->getQstr(id, champ_IPRD)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_IPRD, ui->lineEdit_product->text());
    this->_mainWindow->updateDo();
}
void Page_Sf2::setCommentaire()
{
    if (_preparation) return;
    if (ui->textEdit_Com->toPlainText().size() > 65536)
        ui->textEdit_Com->setPlainText(ui->textEdit_Com->toPlainText().left(65536));
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    if (id.indexSf2 == -1 || ui->textEdit_Com->toPlainText().compare(this->_sf2->getQstr(id, champ_ICMT)) == 0) return;
    _sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->_tree->getFirstID();
    id.typeElement = elementSf2;
    this->_sf2->set(id, champ_ICMT, ui->textEdit_Com->toPlainText());
    this->_mainWindow->updateDo();
}

void Page_Sf2::setNow()
{
    QString date = QDateTime::currentDateTime().toString("dddd d MMMM yyyy, hh:mm:ss");
    ui->lineEdit_date->setText(date);
    setDate();
}

// METHODES PRIVEES

void Page_Sf2::compte()
{
    if (this->_tree->getSelectedItemsNumber() == 0) return;
    if (!this->_tree->isSelectedItemsSf2Unique()) return;

    // Compte des samples, instruments et presets utilisés et non utilisés
    int unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen;
    this->compte(unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen);

    // Affichage
    if (unusedSmpl)
    {
        if (unusedSmpl > 1)
            this->ui->label_nbSmpl->setText(QString::number(usedSmpl) +
                                            trUtf8(" (inutilisés : ") + QString::number(unusedSmpl) + ")");
        else
            this->ui->label_nbSmpl->setText(QString::number(usedSmpl) +
                                            trUtf8(" (inutilisé : ") + QString::number(unusedSmpl) + ")");
        this->ui->label_nbSmpl->setStyleSheet("QLabel { color : red; font-weight: bold;}");
    }
    else
    {
        this->ui->label_nbSmpl->setText(QString::number(usedSmpl));
        this->ui->label_nbSmpl->setStyleSheet("QLabel { color : black; font-weight: normal;}");
    }
    if (unusedInst)
    {
        if (unusedInst > 1)
            this->ui->label_nbInst->setText(QString::number(usedInst) +
                                            trUtf8(" (inutilisés : ") + QString::number(unusedInst) + ")");
        else
            this->ui->label_nbInst->setText(QString::number(usedInst) +
                                            trUtf8(" (inutilisé : ") + QString::number(unusedInst) + ")");
        this->ui->label_nbInst->setStyleSheet("QLabel { color : red; font-weight: bold;}");
    }
    else
    {
        this->ui->label_nbInst->setText(QString::number(usedInst));
        this->ui->label_nbInst->setStyleSheet("QLabel { color : black; font-weight: normal;}");
    }
    this->ui->label_nbPrst->setText(QString::number(usedPrst));

    if (instGen <= 65536)
    {
        this->ui->label_nbInstGen->setStyleSheet("QLabel{color:#008800;}");
        this->ui->label_nbInstGen->setText(QString::number(instGen) + QString::fromUtf8(" (≤ 65536)"));
    }
    else
    {
        this->ui->label_nbInstGen->setStyleSheet("QLabel{color:#FF0000;}");
        this->ui->label_nbInstGen->setText(QString::number(instGen) + " (> 65536)");
    }
    if (prstGen <= 65536)
    {
        this->ui->label_nbPrstGen->setStyleSheet("QLabel{color:#008800;}");
        this->ui->label_nbPrstGen->setText(QString::number(prstGen) + QString::fromUtf8(" (≤ 65536)"));
    }
    else
    {
        this->ui->label_nbPrstGen->setStyleSheet("QLabel{color:#FF0000;}");
        this->ui->label_nbPrstGen->setText(QString::number(prstGen) + " (> 65536)");
    }
}

QList<int> Page_Sf2::getListNotHidden(EltID id)
{
    QList<int> listElt;
    int iTmp = _sf2->count(id);
    for (int i = 0; i < iTmp; i++)
    {
        id.indexElt = i;
        if (!_sf2->get(id, champ_hidden).bValue)
            listElt << i;
    }
    return listElt;
}

void Page_Sf2::compte(int &unusedSmpl, int &unusedInst, int &usedSmpl, int &usedInst, int &usedPrst, int &instGen, int &prstGen)
{
    // Etablissement de la liste des samples, instruments et presets
    // Préparation de la liste des samples et instruments utilisés
    EltID id = this->_tree->getFirstID();
    id.typeElement = elementSmpl;
    QList<int> listSmpl = getListNotHidden(id);
    QList<int> listUsedSmpl;

    id.typeElement = elementInst;
    QList<int> listInst = getListNotHidden(id);
    QList<int> listUsedInst;

    id.typeElement = elementPrst;
    QList<int> listPrst = getListNotHidden(id);

    // Nombre de presets
    usedPrst = listPrst.size();

    // Nombre de gen dans les preset
    // En passant on met de côté les instruments utilisés
    prstGen = 0;
    int iTmp;
    foreach (int elementId, listPrst)
    {
        id.indexElt = elementId;
        id.typeElement = elementPrstGen;
        prstGen += _sf2->count(id);

        id.typeElement = elementPrstInst;
        int nbPrstInst = _sf2->count(id);
        for (int i = 0; i < nbPrstInst; i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                id.typeElement = elementPrstInstGen;
                prstGen += _sf2->count(id);
                id.typeElement = elementPrstInst;

                iTmp = _sf2->get(id, champ_instrument).wValue;
                if (listInst.contains(iTmp))
                {
                    listInst.removeOne(iTmp);
                    listUsedInst << iTmp;
                }
            }
        }
    }


    // Nombre de Gen dans les instruments utilisés
    // En passant on met de côté les samples utilisés
    instGen = 0;
    foreach (int elementId, listUsedInst)
    {
        id.indexElt = elementId;
        id.typeElement = elementInstGen;
        instGen += _sf2->count(id);

        id.typeElement = elementInstSmpl;
        int nbInstSmpl = _sf2->count(id);
        for (int i = 0; i < nbInstSmpl; i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                id.typeElement = elementInstSmplGen;
                instGen += _sf2->count(id);
                id.typeElement = elementInstSmpl;

                iTmp = _sf2->get(id, champ_sampleID).wValue;
                if (listSmpl.contains(iTmp))
                {
                    listSmpl.removeOne(iTmp);
                    listUsedSmpl << iTmp;
                }
            }
        }
    }

    // Nombre de Gen dans les instruments non utilisés
    foreach (int elementId, listInst)
    {
        id.indexElt = elementId;
        id.typeElement = elementInstGen;
        instGen += _sf2->count(id);

        id.typeElement = elementInstSmpl;
        int nbInstSmpl = _sf2->count(id);
        for (int i = 0; i < nbInstSmpl; i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                id.typeElement = elementInstSmplGen;
                instGen += _sf2->count(id);
                id.typeElement = elementInstSmpl;
            }
        }
    }

    // Nombre de samples et instruments utilisés / non utilisés
    unusedSmpl = listSmpl.size();
    unusedInst = listInst.size();
    usedSmpl = listUsedSmpl.size() + listSmpl.size();
    usedInst = listUsedInst.size() + listInst.size();
}
