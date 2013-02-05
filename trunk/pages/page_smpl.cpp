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
****************************************************************************/

#include "page_smpl.h"
#include "ui_page_smpl.h"
#include "mainwindow.h"
#include "sound.h"
#include "dialog_sifflements.h"
#include <QProgressDialog>
#include <QInputDialog>


Page_Smpl::Page_Smpl(QWidget *parent) :
    Page(parent), ui(new Ui::Page_Smpl)
{
    ui->setupUi(this);
    // Initialisation des données
    this->preparation = 0;
    this->lectureEnCours = false;
    // Initialisation du graphique
    this->ui->graphe->linkSliderX(this->ui->sliderGraphe);
    this->ui->graphe->linkSpinBoxes(this->ui->spinStartLoop, this->ui->spinEndLoop);
    // Connexions
    this->ui->graphe->connect(this->synth, SIGNAL(currentPosChanged(int)), SLOT(setCurrentSample(int)));
    this->connect(this->synth, SIGNAL(readFinished()), SLOT(lecteurFinished()));
    connect(this->tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    // Couleur de fond du graphe Fourier
    this->ui->grapheFourier->setBackgroundColor(this->palette().background().color());
}

Page_Smpl::~Page_Smpl()
{
    delete ui;
}

void Page_Smpl::afficher()
{
    // Préparation de l'affichage
    preparation = true;
    EltID id = this->tree->getID(0);
    DWORD dwSmplRate = this->sf2->get(id, champ_dwSampleRate).dwValue;
    char T[50];
    sprintf(T, "%lu", dwSmplRate);
    this->ui->comboSampleRate->setCurrentIndex(this->ui->comboSampleRate->findText(T));
    // Remplissage du graphe et initialisation des données
    QByteArray baData = this->sf2->getData(id, champ_sampleData16);
    this->ui->graphe->setData(baData, dwSmplRate); // Prend du temps, thread séparé !!!
    qint32 posStart = this->sf2->get(id, champ_dwStartLoop).dwValue;
    qint32 posEnd = this->sf2->get(id, champ_dwEndLoop).dwValue;
    this->ui->graphe->setStartLoop(posStart);
    this->ui->graphe->setEndLoop(posEnd);
    // Remplissage des informations
    DWORD dwTmp = this->sf2->get(id, champ_dwLength).dwValue;
    sprintf(T, "%lu - %.3fs", dwTmp, (double)dwTmp / dwSmplRate);
    this->ui->labelTaille->setText(T);
    this->ui->spinStartLoop->setMaximum(posEnd);
    this->ui->spinEndLoop->setMaximum(dwTmp);
    this->ui->spinEndLoop->setMinimum(posStart);
    this->ui->spinStartLoop->setValue(posStart);
    this->ui->spinEndLoop->setValue(posEnd);
    if (this->ui->spinEndLoop->value() == this->ui->spinStartLoop->value())
    {
        this->ui->checkLectureBoucle->setEnabled(false);
        this->ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        this->ui->checkLectureBoucle->setEnabled(true);
        this->ui->checkLectureBoucle->setChecked(true);
    }
    this->ui->spinRootKey->setValue(this->sf2->get(id, champ_byOriginalPitch).bValue);
    this->ui->spinTune->setValue(this->sf2->get(id, champ_chPitchCorrection).cValue);
    // Type et lien
    EltID id2 = id;
    this->ui->comboLink->clear();
    this->ui->comboType->clear();
    // Liens possibles
    for (int i = 0; i < this->sf2->count(id2); i++)
    {
        if (i != id.indexElt)
        {
            id2.indexElt = i;
            if (!this->sf2->get(id2, champ_hidden).bValue)
                this->ui->comboLink->addItem(this->sf2->getQstr(id2, champ_name));
        }
    }
    this->ui->comboLink->model()->sort(0);
    this->ui->comboLink->insertItem(0, "-");
    // Types possibles et sélections
    SFSampleLink typeLink = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
    this->ui->comboType->addItem(tr("mono"));
    if (typeLink == monoSample || typeLink == RomMonoSample)
    {
        this->ui->comboType->setCurrentIndex(0);
        this->ui->comboLink->setCurrentIndex(0);
        this->ui->checkLectureLien->setEnabled(false);
        this->ui->checkLectureLien->setChecked(false);
    }
    else
    {
        this->ui->comboType->addItem(tr("droit"));
        this->ui->comboType->addItem(tr("gauche"));
        this->ui->comboType->addItem(tr("lien"));
        switch (typeLink)
        {
        case rightSample: case RomRightSample:
            this->ui->comboType->setCurrentIndex(1);
            break;
        case leftSample: case RomLeftSample:
            this->ui->comboType->setCurrentIndex(2);
            break;
        case linkedSample: case RomLinkedSample:
            this->ui->comboType->setCurrentIndex(3);
            break;
        default: break;
        }
        id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
        this->ui->comboLink->setCurrentIndex(this->ui->comboLink->findText(this->sf2->getQstr(id2, champ_name)));
        this->ui->checkLectureLien->setEnabled(true);
    }
    // Liste des instruments qui utilisent le sample
    int nbInst = 0;
    bool isFound;
    int j;
    QString qStr = "";
    id2 = id;
    id2.typeElement = elementInst;
    EltID id3 = id;
    id3.typeElement = elementInstSmpl;
    // Parcours de tous les instruments
    for (int i = 0; i < this->sf2->count(id2); i++)
    {
        id2.indexElt = i;
        id3.indexElt = i;
        // Parcours de tous les samples liés à l'instrument
        isFound = false;
        j = 0;
        while(j < this->sf2->count(id3) && !isFound)
        {
            id3.indexElt2 = j;
            if (!this->sf2->get(id3, champ_hidden).bValue)
            {
                if (this->sf2->get(id3, champ_sampleID).wValue == id.indexElt)
                {
                    // Ajout d'un instrument
                    if (nbInst)
                        qStr.append(", ");
                    qStr.append(this->sf2->getQstr(id2, champ_name).toStdString().c_str());
                    nbInst++;
                    isFound = true;
                }
            }
            j++;
        }
    }
    if (nbInst == 0)
        qStr = QString::fromUtf8(tr("<b>Sample lié à aucun instrument.</b>").toStdString().c_str());
    else if (nbInst == 1)
        qStr.prepend(QString::fromUtf8(tr("<b>Sample lié à l'instrument : </b>").toStdString().c_str()));
    else
        qStr.prepend(QString::fromUtf8(tr("<b>Sample lié aux instruments : </b>").toStdString().c_str()));
    this->ui->labelInst->setText(qStr);
    // Remplissage du graphe fourier
    this->ui->grapheFourier->setData(baData, posStart, posEnd, dwSmplRate); // Prend du temps, thread séparé !!!
    // Basculement affichage
    this->qStackedWidget->setCurrentWidget(this);
    preparation = false;
}

void Page_Smpl::setStartLoop()
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    if ((unsigned)this->ui->spinStartLoop->value() == this->sf2->get(id, champ_dwStartLoop).dwValue) return;
    this->ui->spinEndLoop->setMinimum(this->ui->spinStartLoop->value());
    sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->tree->getID(0);
    Valeur val;
    val.dwValue = this->ui->spinStartLoop->value();
    this->sf2->set(id, champ_dwStartLoop, val);
    if (this->ui->spinEndLoop->value() == this->ui->spinStartLoop->value())
    {
        this->ui->checkLectureBoucle->setEnabled(false);
        this->ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        this->ui->checkLectureBoucle->setEnabled(true);
        this->ui->checkLectureBoucle->setChecked(true);
    }
    this->mainWindow->updateDo();
    // Mise à jour graphe Fourier
    this->ui->grapheFourier->setPos(this->ui->spinStartLoop->value(), this->ui->spinEndLoop->value());
}
void Page_Smpl::setStartLoop(int val)
{
    // Modif synth
    this->synth->setStartLoop(val);
}
void Page_Smpl::setEndLoop()
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    if ((unsigned)this->ui->spinEndLoop->value() == this->sf2->get(id, champ_dwEndLoop).dwValue) return;
    this->ui->spinStartLoop->setMaximum(this->ui->spinEndLoop->value());
    sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->tree->getID(0);
    Valeur val;
    val.dwValue = this->ui->spinEndLoop->value();
    this->sf2->set(id, champ_dwEndLoop, val);
    if (this->ui->spinEndLoop->value() == this->ui->spinStartLoop->value())
    {
        this->ui->checkLectureBoucle->setEnabled(false);
        this->ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        this->ui->checkLectureBoucle->setEnabled(true);
        this->ui->checkLectureBoucle->setChecked(true);
    }
    this->mainWindow->updateDo();
    // Mise à jour graphe Fourier
    this->ui->grapheFourier->setPos(this->ui->spinStartLoop->value(), this->ui->spinEndLoop->value());
}
void Page_Smpl::setEndLoop(int val)
{
    // Modif synth
    this->synth->setEndLoop(val);
}
void Page_Smpl::setRootKey()
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    if ((unsigned)this->ui->spinRootKey->value() == this->sf2->get(id, champ_byOriginalPitch).bValue) return;
    sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->tree->getID(0);
    Valeur val;
    val.bValue = this->ui->spinRootKey->value();
    // Sample associé ?
    SFSampleLink typeLien = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample)
    {
        EltID id2 = id;
        id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
        if ((unsigned)this->ui->spinRootKey->value() != this->sf2->get(id2, champ_byOriginalPitch).bValue)
            this->sf2->set(id2, champ_byOriginalPitch, val);
    }
    this->sf2->set(id, champ_byOriginalPitch, val);
    this->mainWindow->updateDo();
}
void Page_Smpl::setRootKey(int val)
{
    // Modif synth
    this->synth->setRootKey(val);
}
void Page_Smpl::setTune()
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    if (this->ui->spinTune->value() == this->sf2->get(id, champ_chPitchCorrection).cValue) return;
    sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->tree->getID(0);
    Valeur val;
    val.cValue = this->ui->spinTune->value();
    // Sample associé ?
    SFSampleLink typeLien = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample)
    {
        EltID id2 = id;
        id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
        if ((unsigned)this->ui->spinTune->value() != this->sf2->get(id2, champ_chPitchCorrection).bValue)
            this->sf2->set(id2, champ_chPitchCorrection, val);
    }
    this->sf2->set(id, champ_chPitchCorrection, val);
    this->mainWindow->updateDo();
}
void Page_Smpl::setTune(int val)
{
    // Modif synth
    this->synth->setPitchCorrection(val);
}
void Page_Smpl::setType(int index)
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    // Ancien et nouveau types
    SFSampleLink ancienType = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
    SFSampleLink nouveauType = monoSample;
    switch (index)
    {
    case 0: nouveauType = monoSample; break;
    case 1: nouveauType = rightSample; break;
    case 2: nouveauType = leftSample; break;
    case 3: nouveauType = linkedSample; break;
    }
    if (ancienType == RomMonoSample) ancienType = monoSample;
    if (ancienType == RomRightSample) ancienType = rightSample;
    if (ancienType == RomLeftSample) ancienType = leftSample;
    if (ancienType == RomLinkedSample) ancienType = linkedSample;
    if (ancienType == nouveauType) return;
    sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->tree->getID(0);
    this->preparation = true;
    // Modification du type du sample
    Valeur val;
    val.sfLinkValue = nouveauType;
    this->sf2->set(id, champ_sfSampleType, val);
    if (ancienType != monoSample)
    {
        // Sample lié
        EltID id2 = id;
        id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
        switch (nouveauType)
        {
        case monoSample:
            // Suppression du lien vers le sample
            val.wValue = 0;
            this->sf2->set(id, champ_wSampleLink, val);
            // Suppression du lien du sample anciennement lié
            this->sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            this->sf2->set(id2, champ_sfSampleType, val);
            // Mise à jour combobox
            this->ui->comboType->clear();
            this->ui->comboType->addItem(tr("mono"));
            this->ui->comboLink->setCurrentIndex(0);
            this->ui->checkLectureLien->setEnabled(false);
            this->ui->checkLectureLien->setChecked(false);
            break;
        case rightSample:
            val.sfLinkValue = leftSample;
            this->sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        case leftSample:
            val.sfLinkValue = rightSample;
            this->sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        case linkedSample:
            val.sfLinkValue = linkedSample;
            this->sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        default: break;
        }
    }
    this->preparation = false;
    this->mainWindow->updateDo();
}
void Page_Smpl::setLinkedSmpl(int index)
{
    if (preparation) return;
    EltID id = this->tree->getID(0);
    // Ancien et nouveau samples liés
    EltID idLinkedOld = id;
    if (this->sf2->get(id, champ_sfSampleType).sfLinkValue != monoSample && \
            this->sf2->get(id, champ_sfSampleType).sfLinkValue != RomMonoSample)
        idLinkedOld.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    EltID idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = this->ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = this->sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!this->sf2->get(id2, champ_hidden).bValue)
            {
                if (!this->sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
                    idLinkedNew.indexElt = pos;
            }
            pos++;
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    // Ancien et nouveau sont égaux ?
    if (idLinkedNew.indexElt == idLinkedOld.indexElt) return;
    sf2->prepareNewActions();
    // Reprise des adresses si modification
    id = this->tree->getID(0);
    if (this->sf2->get(id, champ_sfSampleType).sfLinkValue != monoSample && \
            this->sf2->get(id, champ_sfSampleType).sfLinkValue != RomMonoSample)
        idLinkedOld.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = this->ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = this->sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!this->sf2->get(id2, champ_hidden).bValue)
            {
                if (!this->sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
                    idLinkedNew.indexElt = pos;
            }
            pos++;
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    this->preparation = true;
    // Suppression du lien du sample anciennement lié, le cas échéant
    Valeur val;
    if (idLinkedOld.indexElt > -1)
    {
        val.sfLinkValue = monoSample;
        this->sf2->set(idLinkedOld, champ_sfSampleType, val);
        val.wValue = 0;
        this->sf2->set(idLinkedOld, champ_wSampleLink, val);
    }
    // Nouveau sample à lier ?
    if (idLinkedNew.indexElt > -1)
    {
        // Le nouveau sample à lier est-il déjà lié ?
        if (this->sf2->get(idLinkedNew, champ_sfSampleType).sfLinkValue != monoSample && \
                this->sf2->get(idLinkedNew, champ_sfSampleType).sfLinkValue != RomMonoSample)
        {
            // On délie le sample lié au sample que l'on veut lier
            EltID id2 = id;
            id2.indexElt = this->sf2->get(idLinkedNew, champ_wSampleLink).wValue;
            val.wValue = 0;
            this->sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            this->sf2->set(id2, champ_sfSampleType, val);
        }
        // Lien vers le nouveau sample
        val.wValue = idLinkedNew.indexElt;
        this->sf2->set(id, champ_wSampleLink, val);
        val.sfLinkValue = linkedSample;
        this->sf2->set(id, champ_sfSampleType, val);
        // Lien du nouveau sample lié
        this->sf2->set(idLinkedNew, champ_sfSampleType, val);
        val.wValue = id.indexElt;
        this->sf2->set(idLinkedNew, champ_wSampleLink, val);
        // Ajustement rootKey et tune du sample lié
        val = this->sf2->get(id, champ_byOriginalPitch);
        this->sf2->set(idLinkedNew, champ_byOriginalPitch, val);
        val = this->sf2->get(id, champ_chPitchCorrection);
        this->sf2->set(idLinkedNew, champ_chPitchCorrection, val);
        // Mise à jour combobox
        this->ui->comboType->clear();
        this->ui->comboType->addItem(tr("mono"));
        this->ui->comboType->addItem(tr("droit"));
        this->ui->comboType->addItem(tr("gauche"));
        this->ui->comboType->addItem(tr("lien"));
        this->ui->comboType->setCurrentIndex(3);
        this->ui->checkLectureLien->setEnabled(true);
    }
    else
    {
        // Plus de lien
        val.wValue = 0;
        this->sf2->set(id, champ_wSampleLink, val);
        val.sfLinkValue = monoSample;
        this->sf2->set(id, champ_sfSampleType, val);
        // Mise à jour combobox
        this->ui->comboType->clear();
        this->ui->comboType->addItem(tr("mono"));
        this->ui->comboLink->setCurrentIndex(0);
        this->ui->checkLectureLien->setEnabled(false);
        this->ui->checkLectureLien->setChecked(false);
    }
    this->preparation = false;
    this->mainWindow->updateDo();
}
void Page_Smpl::setRate(int index)
{
    if (preparation) return;
    // Modification de l'échantillonnage
    Q_UNUSED(index);
    EltID id = this->tree->getID(0);
    DWORD echFinal = this->ui->comboSampleRate->currentText().toInt();
    DWORD echInit = this->sf2->get(id, champ_dwSampleRate).dwValue;
    if (echInit == echFinal) return;
    sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->tree->getID(0);
    // Sample associé ?
    SFSampleLink typeLien = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample)
    {
        EltID id2 = id;
        id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
        if (echFinal != this->sf2->get(id2, champ_dwSampleRate).dwValue)
            setRateElt(id2, echFinal);
    }
    setRateElt(id, echFinal);
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::setRateElt(EltID id, DWORD echFinal)
{
    // Modification échantillonnage
    DWORD echInit = this->sf2->get(id, champ_dwSampleRate).dwValue;
    QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
    double d = 0;
    baData = Sound::resampleMono(baData, echInit, echFinal, 24, 0, d, false);
    this->sf2->set(id, champ_sampleDataFull24, baData);
    Valeur val;
    val.dwValue = echFinal;
    this->sf2->set(id, champ_dwSampleRate, val);
    // Ajustement de length, startLoop, endLoop
    val.dwValue = baData.size()/3;
    this->sf2->set(id, champ_dwLength, val);
    DWORD dwTmp = this->sf2->get(id, champ_dwStartLoop).dwValue;
    dwTmp = (dwTmp * echFinal) / echInit;
    val.dwValue = dwTmp;
    this->sf2->set(id, champ_dwStartLoop, val);
    dwTmp = this->sf2->get(id, champ_dwEndLoop).dwValue;
    dwTmp = (dwTmp * echFinal) / echInit;
    val.dwValue = dwTmp;
    this->sf2->set(id, champ_dwEndLoop, val);
}

// Outils
void Page_Smpl::normalisation()
{
    if (preparation) return;
    sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                double db = 0;
                QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
                baData = Sound::normaliser(baData, 0.9, 24, db);
                if (db != 0)
                    this->sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::enleveBlanc()
{
    if (preparation) return;
    sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveBlanc(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::enleveBlanc(EltID id)
{
    quint32 pos = 0;
    QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
    baData = Sound::enleveBlanc(baData, 0.001, 24, pos);
    if (pos != 0)
    {
        this->sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length, startloop, endLoop
        Valeur val;
        val.dwValue = baData.size()/3;
        this->sf2->set(id, champ_dwLength, val);
        if (this->sf2->get(id, champ_dwStartLoop).dwValue > pos)
            val.dwValue = this->sf2->get(id, champ_dwStartLoop).dwValue - pos;
        else
            val.dwValue = 0;
        this->sf2->set(id, champ_dwStartLoop, val);
        if (this->sf2->get(id, champ_dwEndLoop).dwValue > pos)
            val.dwValue = this->sf2->get(id, champ_dwEndLoop).dwValue - pos;
        else
            val.dwValue = 0;
        this->sf2->set(id, champ_dwEndLoop, val);
    }
}
void Page_Smpl::enleveFin()
{
    if (preparation) return;
    sf2->prepareNewActions();

    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveFin(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::enleveFin(EltID id)
{
    QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
    // Nombre de valeurs à enlever
    qint32 size = baData.size() / 3;
    qint32 endLoop = this->sf2->get(id, champ_dwEndLoop).dwValue;
    if (endLoop == 0) return;
    if (endLoop < size - 8)
    {
        qint32 pos = size - (8 + endLoop);
        // Rognage
        baData = baData.left(baData.size() - 3 * pos);
        this->sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length
        Valeur val;
        val.dwValue = baData.size() / 3;
        this->sf2->set(id, champ_dwLength, val);
    }
}
void Page_Smpl::bouclage()
{
    if (preparation) return;
    sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                // Récupération des données, échantillonnage, startloop et endloop
                QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
                DWORD dwSmplRate = this->sf2->get(id, champ_dwSampleRate).dwValue;
                qint32 startLoop = this->sf2->get(id, champ_dwStartLoop).dwValue;
                qint32 endLoop = this->sf2->get(id, champ_dwEndLoop).dwValue;
                // Bouclage
                baData = Sound::bouclage(baData, dwSmplRate, startLoop, endLoop, 24);
                if (!baData.isEmpty())
                {
                    // Modification des données, de length, startloop et endloop
                    this->sf2->set(id, champ_sampleDataFull24, baData);
                    Valeur val;
                    val.dwValue = startLoop;
                    this->sf2->set(id, champ_dwStartLoop, val);
                    val.dwValue = endLoop;
                    this->sf2->set(id, champ_dwEndLoop, val);
                    val.dwValue = baData.size() / 3;
                    this->sf2->set(id, champ_dwLength, val);
                }
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::filtreMur()
{
    if (preparation) return;
    // Fréquence de filtre
    bool ok;
    double rep = QInputDialog::getDouble(this, tr("Question"),
                                         QString::fromUtf8(tr("Fréquence de coupure :").toStdString().c_str()),
                                         15000, 20, 96000, 2, &ok);
    if (!ok) return;
    sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                // Récupération des données et de l'échantillonnage
                QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
                DWORD dwSmplRate = this->sf2->get(id, champ_dwSampleRate).dwValue;
                // Filtre passe bas
                baData = Sound::bandFilter(baData, 24, dwSmplRate, rep, 0, -1);
                this->sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::reglerBalance()
{
    if (preparation) return;
    this->sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                // Sample lié ?
                SFSampleLink typeLien = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
                if (typeLien != monoSample && typeLien != RomMonoSample)
                    nbEtapes++;
            }
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl && !this->sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié ?
            SFSampleLink typeLien = this->sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                EltID id2 = id;
                id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
                // Récupération des données des samples
                QByteArray baData1 = this->sf2->getData(id, champ_sampleDataFull24);
                QByteArray baData2 = this->sf2->getData(id2, champ_sampleDataFull24);
                // Régimes permanents
                qint32 debut1, debut2, fin1, fin2;
                if (this->sf2->get(id, champ_dwStartLoop).dwValue == this->sf2->get(id, champ_dwEndLoop).dwValue)
                {
                    Sound::regimePermanent(baData1, this->sf2->get(id, champ_dwSampleRate).dwValue,
                                           24, debut1, fin1);
                }
                else
                {
                    debut1 = this->sf2->get(id, champ_dwStartLoop).dwValue;
                    fin1 = this->sf2->get(id, champ_dwEndLoop).dwValue;
                }
                if (this->sf2->get(id2, champ_dwStartLoop).dwValue == this->sf2->get(id2, champ_dwEndLoop).dwValue)
                {
                    Sound::regimePermanent(baData2, this->sf2->get(id2, champ_dwSampleRate).dwValue,
                                           24, debut2, fin2);
                }
                else
                {
                    debut2 = this->sf2->get(id2, champ_dwStartLoop).dwValue;
                    fin2 = this->sf2->get(id2, champ_dwEndLoop).dwValue;
                }
                // Calcul des intensités
                double intensite1 = Sound::moyenneCarre(baData1.mid(debut1*3, (fin1-debut1)*3), 24);
                double intensite2 = Sound::moyenneCarre(baData2.mid(debut2*3, (fin2-debut2)*3), 24);
                // Intensité moyenne
                double intensiteMoy = sqrt(intensite1 * intensite2);
                // Ajustement des données
                double gain1, gain2;
                baData1 = Sound::multiplier(baData1, intensiteMoy/intensite1, 24, gain1);
                baData2 = Sound::multiplier(baData2, intensiteMoy/intensite2, 24, gain2);
                // Stockage
                this->sf2->set(id, champ_sampleDataFull24, baData1);
                this->sf2->set(id2, champ_sampleDataFull24, baData2);
                // Avancement barre de progression
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::sifflements()
{
    if (preparation) return;
    DialogSifflements * dialogSifflements = new DialogSifflements(this);
    dialogSifflements->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogSifflements, SIGNAL(accepted(int,int,double)),
                  SLOT(sifflements(int,int,double)));
    dialogSifflements->show();
}
void Page_Smpl::sifflements(int freq1, int freq2, double raideur)
{
    // Elimination des sifflements
    this->sf2->prepareNewActions();
    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
                DWORD dwSmplRate = this->sf2->get(id, champ_dwSampleRate).dwValue;
                baData = Sound::sifflements(baData, dwSmplRate, 24, freq1, freq2, raideur);
                this->sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Mise à jour
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::transposer()
{
    if (preparation) return;
    // Ecart en demi-tons
    bool ok;
    double rep = QInputDialog::getDouble(this, tr("Question"),
                                         QString::fromUtf8(tr("Écart en demi-tons :").toStdString().c_str()),
                                         12, -36, 36, 2,
                                         &ok);
    if (!ok) return;
    sf2->prepareNewActions();

    EltID id;
    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;
    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", tr("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
    {
        id = this->tree->getID(i);
        if (id.typeElement == elementSmpl)
        {
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                // Récupération des données et de l'échantillonnage
                QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
                DWORD echFinal = this->sf2->get(id, champ_dwSampleRate).dwValue;
                // Calcul de l'échantillonnage fictif de départ
                double echInit = (double)echFinal * pow(2, rep/12);
                // Rééchantillonnage
                double d = 0;
                baData = Sound::resampleMono(baData, echInit, echFinal, 24, 0, d, false);
                this->sf2->set(id, champ_sampleDataFull24, baData);
                // Ajustement de length, startLoop, endLoop
                Valeur val;
                val.dwValue = baData.size()/3;
                this->sf2->set(id, champ_dwLength, val);
                DWORD dwTmp = this->sf2->get(id, champ_dwStartLoop).dwValue;
                dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
                val.dwValue = dwTmp;
                this->sf2->set(id, champ_dwStartLoop, val);
                dwTmp = this->sf2->get(id, champ_dwEndLoop).dwValue;
                dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
                val.dwValue = dwTmp;
                this->sf2->set(id, champ_dwEndLoop, val);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}

// Egaliseur
void Page_Smpl::applyEQ()
{
    if (preparation) return;
    if (this->ui->verticalSlider_1->value() == 0 &&
        this->ui->verticalSlider_2->value() == 0 &&
        this->ui->verticalSlider_3->value() == 0 &&
        this->ui->verticalSlider_4->value() == 0 &&
        this->ui->verticalSlider_5->value() == 0 &&
        this->ui->verticalSlider_6->value() == 0 &&
        this->ui->verticalSlider_7->value() == 0 &&
        this->ui->verticalSlider_8->value() == 0 &&
        this->ui->verticalSlider_9->value() == 0 &&
        this->ui->verticalSlider_10->value() == 0)
        return;
    sf2->prepareNewActions();
    EltID id = this->tree->getID(0);
    QByteArray baData = this->sf2->getData(id, champ_sampleDataFull24);
    // Traitement
    baData = Sound::EQ(baData, this->sf2->get(id, champ_dwSampleRate).dwValue, 24,
                       this->ui->verticalSlider_1->value(),
                       this->ui->verticalSlider_2->value(),
                       this->ui->verticalSlider_3->value(),
                       this->ui->verticalSlider_4->value(),
                       this->ui->verticalSlider_5->value(),
                       this->ui->verticalSlider_6->value(),
                       this->ui->verticalSlider_7->value(),
                       this->ui->verticalSlider_8->value(),
                       this->ui->verticalSlider_9->value(),
                       this->ui->verticalSlider_10->value());
    this->sf2->set(id, champ_sampleDataFull24, baData);
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::initEQ()
{
    this->ui->verticalSlider_1->setValue(0);
    this->ui->verticalSlider_2->setValue(0);
    this->ui->verticalSlider_3->setValue(0);
    this->ui->verticalSlider_4->setValue(0);
    this->ui->verticalSlider_5->setValue(0);
    this->ui->verticalSlider_6->setValue(0);
    this->ui->verticalSlider_7->setValue(0);
    this->ui->verticalSlider_8->setValue(0);
    this->ui->verticalSlider_9->setValue(0);
    this->ui->verticalSlider_10->setValue(0);
}

// Lecture
void Page_Smpl::lecture()
{
    if (this->ui->pushLecture->isChecked())
    {
        this->noteChanged(-1, 127);
        // Désactivations
        this->ui->comboLink->setEnabled(false);
        this->ui->comboType->setEnabled(false);
        this->ui->comboSampleRate->setEnabled(false);
        this->ui->verticalSlider_1->setEnabled(false);
        this->ui->verticalSlider_2->setEnabled(false);
        this->ui->verticalSlider_3->setEnabled(false);
        this->ui->verticalSlider_4->setEnabled(false);
        this->ui->verticalSlider_5->setEnabled(false);
        this->ui->verticalSlider_6->setEnabled(false);
        this->ui->verticalSlider_7->setEnabled(false);
        this->ui->verticalSlider_8->setEnabled(false);
        this->ui->verticalSlider_9->setEnabled(false);
        this->ui->verticalSlider_10->setEnabled(false);
        this->ui->pushEgaliser->setEnabled(false);
        this->ui->pushEgalRestore->setEnabled(false);
        // Désactivation outils sample
        this->mainWindow->desactiveOutilsSmpl();
        this->lectureEnCours = true;
    }
    else
    {
        this->lectureEnCours = false;
        this->noteChanged(-1, 0);
    }
}
void Page_Smpl::lecteurFinished()
{
    this->ui->graphe->setCurrentSample(0);
    // Réinitialisation des boutons
    this->ui->comboLink->setEnabled(true);
    this->ui->comboType->setEnabled(true);
    this->ui->comboSampleRate->setEnabled(true);
    this->ui->verticalSlider_1->setEnabled(true);
    this->ui->verticalSlider_2->setEnabled(true);
    this->ui->verticalSlider_3->setEnabled(true);
    this->ui->verticalSlider_4->setEnabled(true);
    this->ui->verticalSlider_5->setEnabled(true);
    this->ui->verticalSlider_6->setEnabled(true);
    this->ui->verticalSlider_7->setEnabled(true);
    this->ui->verticalSlider_8->setEnabled(true);
    this->ui->verticalSlider_9->setEnabled(true);
    this->ui->verticalSlider_10->setEnabled(true);
    this->ui->pushEgaliser->setEnabled(true);
    this->ui->pushEgalRestore->setEnabled(true);
    // Réactivation outils sample
    if (this->qStackedWidget->currentWidget() == this)
        this->mainWindow->activeOutilsSmpl();
    if (this->ui->pushLecture->isChecked())
    {
        this->ui->pushLecture->blockSignals(true);
        this->ui->pushLecture->setChecked(false);
        this->ui->pushLecture->blockSignals(false);
    }
    this->lectureEnCours = false;
    this->mainWindow->activeOutilsSmpl();
}
void Page_Smpl::selectionChanged()
{
    if (this->lectureEnCours)
        this->noteChanged(-1, 0);
}

bool Page_Smpl::isPlaying() {return this->lectureEnCours;}
void Page_Smpl::setLoopEnabled(int val)
{
    // Modif synth
    this->synth->setLoopEnabled(val);
}
void Page_Smpl::setSinusEnabled(int val)
{
    // Modif synth
    this->synth->setSinusEnabled(val);
}
void Page_Smpl::setGainSample(int val)
{
    // Modif synth
    this->synth->setGainSample(val);
}
void Page_Smpl::setStereo(int val)
{
    // Modif synth
    this->synth->setStereo(val);
}

///////////////////////////////////////////////////////////////////////
////////////////////////////// GRAPHIQUE //////////////////////////////
///////////////////////////////////////////////////////////////////////

// Constructeur
Graphique::Graphique(QWidget * parent) : QCustomPlot(parent)
{
    zoomFlag = false;
    dragFlag = false;
    zoomX = 1; zoomXinit = 1;
    zoomY = 1; zoomYinit = 1;
    posX = 0.5; posXinit = 0.5;
    posY = 0.5; posYinit = 0.5;
    bFromExt = false;
    qScrollX = NULL;
    spinStart = NULL;
    spinEnd = NULL;
    this->installEventFilter(this);
    // Création des graphes et définition des pinceaux
    // Graphe des données
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(0, 0, 255));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setAntialiased(false);
    // Graphe contenant startLoop
    this->addGraph();
    graphPen.setColor(QColor(100, 255, 100));
    graphPen.setWidthF(2);
    this->graph(1)->setPen(graphPen);
    // Graphe contenant endLoop
    this->addGraph();
    graphPen.setColor(QColor(255, 0, 0));
    graphPen.setWidthF(2);
    this->graph(2)->setPen(graphPen);
    // Graphe contenant le curseur de lecture
    this->addGraph();
    graphPen.setColor(QColor(255, 255, 255));
    graphPen.setWidthF(1);
    this->graph(3)->setPen(graphPen);
    // Graphe pour la ligne de zoom
    this->addGraph();
    graphPen.setColor(QColor(255, 255, 255));
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DashLine);
    this->graph(4)->setPen(graphPen);
    // Axes
    this->xAxis2->setRange(0, 1);
    this->yAxis2->setRange(0, 1);
    this->yAxis->setRange(-1, 1);
    graphPen.setColor(QColor(60, 60, 100));
    graphPen.setWidthF(1);
    this->yAxis->setZeroLinePen(graphPen);
    graphPen.setStyle(Qt::DotLine);
    this->yAxis->setGridPen(graphPen);
    this->xAxis->setVisible(false);
    this->yAxis->setTicks(false);
    // Marges
    this->setAutoMargin(false);
    this->setMargin(0, 0, 0, 0);
    // Couleur de fond
    this->setColor(QColor(0, 0, 0));
}

// Méthodes publiques
void Graphique::setData(QByteArray baData, DWORD dwSmplRate)
{
    // Réinitialisation du graphe
    this->graph(0)->clearData();
    this->graph(1)->clearData();
    this->graph(2)->clearData();
    this->graph(3)->clearData();
    this->graph(4)->clearData();
    // Ajout des données
    this->dwSmplRate = dwSmplRate;
    DWORD size_x = baData.size() / 2;
    this->sizeX = ((double)size_x - 1) / dwSmplRate;
    qint16 * data = (qint16 *)baData.data();
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i=0; i < size_x; i++)
    {
        x[i] = this->sizeX * i/((double)size_x - 1);
        y[i] = (double)data[i]/32768; // normalisation entre -1 et 1
    }
    this->graph(0)->setData(x, y);
    this->zoomDrag();
    this->replot();
}
void Graphique::linkSliderX(QScrollBar * qScrollX)
{
    this->qScrollX = qScrollX;
}
void Graphique::linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd)
{
    this->spinStart = spinStart;
    this->spinEnd = spinEnd;
}
void Graphique::setPosX(int posX)
{
    if (this->qScrollX)
    {
        bFromExt = true;
        if (this->qScrollX->maximum() > 0)
            this->posX = (double)posX / this->qScrollX->maximum();
        else
            this->posX = 0.5;
        this->zoomDrag();
        bFromExt = false;
    }
}
void Graphique::setStartLoop(int pos)
{
    this->graph(1)->clearData();
    if (pos >= 0)
    {
        QVector<double> x(2), y(2);
        x[0] = (double)pos / dwSmplRate;
        x[1] = (double)pos / dwSmplRate;
        y[0] = -1;
        y[1] = 1;
        this->graph(1)->setData(x, y);
    }
    this->replot();
}
void Graphique::setEndLoop(int pos)
{
    this->graph(2)->clearData();
    if (pos >= 0)
    {
        QVector<double> x(2), y(2);
        x[0] = (double)pos / dwSmplRate;
        x[1] = (double)pos / dwSmplRate;
        y[0] = -1;
        y[1] = 1;
        this->graph(2)->setData(x, y);
    }
    this->replot();
}
void Graphique::setCurrentSample(int pos)
{
    this->graph(3)->clearData();
    if (pos >= 0)
    {
        QVector<double> x(2), y(2);
        x[0] = (double)pos / dwSmplRate;
        x[1] = (double)pos / dwSmplRate;
        y[0] = -1;
        y[1] = 1;
        this->graph(3)->setData(x, y);
    }
    this->replot();
}

// Méthodes privées
void Graphique::zoom(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->yInit;
    // Modification zoom & drag
    this->zoomX = zoomXinit * pow(2, 25.0 * decX);
    this->zoomY = zoomYinit * pow(2,  5.0 * decY);
    // Ajustement posX et posY
    if (zoomX > 1)
    {
        posX = (zoomX * posXinit * (zoomXinit - 1) + \
                xInit*(zoomX - zoomXinit)) / \
                (zoomXinit*(zoomX - 1));
    }
    if (zoomY > 1)
    {
        posY = (zoomY * posYinit * (zoomYinit - 1) + \
                yInit*(zoomY - zoomYinit)) / \
                (zoomYinit*(zoomY - 1));
    }
    // Mise à jour
    this->zoomDrag();
}
void Graphique::drag(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->yInit;
    // Modification posX et posY
    if (zoomXinit > 1)
        posX = posXinit - decX / (zoomXinit - 1);
    if (zoomYinit > 1)
        posY = posYinit - decY / (zoomYinit - 1);
    // Mise à jour
    this->zoomDrag();
}
void Graphique::zoomDrag()
{
    // Bornes des paramètres d'affichage
    if (this->zoomX < 1)
        this->zoomX = 1;
    else if (this->zoomX > sizeX * 200)
        this->zoomX = sizeX * 200;
    if (this->zoomY < 1)
        this->zoomY = 1;
    else if (this->zoomY > 50)
        this->zoomY = 50;
    if (this->posX < 0)
        this->posX = 0;
    else if (this->posX > 1)
        this->posX = 1;
    if (this->posY < 0)
        this->posY = 0;
    else if (this->posY > 1)
        this->posY = 1;
    posY = 0.5; // blocage sur Y
    // Application du drag et zoom
    double etendueX = sizeX / zoomX;
    double offsetX = (sizeX - etendueX) * posX;
    this->xAxis->setRange(offsetX, offsetX + etendueX);
    double etendueY = 2 / zoomY;
    double offsetY = (2 - etendueY) * posY - 1;
    this->yAxis->setRange(offsetY, offsetY + etendueY);
    // Mise à jour
    this->replot();
    // Envoi signal
    if (!bFromExt && qScrollX)
    {
        // Mise à jour du scrollbar
        int valMax = (zoomX-1.0)*5.0*sizeX;
        qScrollX->blockSignals(true);
        qScrollX->setRange(0, valMax);
        qScrollX->setValue(valMax*posX);
        qScrollX->blockSignals(false);
    }
}
void Graphique::setZoomLine(double x1, double y1, double x2, double y2)
{
    this->graph(4)->clearData();
    if (x1 >= 0)
    {
        // Conversion
        QVector<double> x(2), y(2);
        x[0] = sizeX * (x1 + posX * (zoomX - 1)) / zoomX;
        x[1] = sizeX * (x2 + posX * (zoomX - 1)) / zoomX;
        y[0] = 2 * (y1 + posY * (zoomY - 1)) / zoomY - 1;
        y[1] = 2 * (y2 + posY * (zoomY - 1)) / zoomY - 1;
        this->graph(4)->setData(x, y);
    }
    this->replot();
}


///////////////////////////////////////////////////////////////////////
////////////////////////// GRAPHIQUE FOURIER //////////////////////////
///////////////////////////////////////////////////////////////////////

// Constructeur
GraphiqueFourier::GraphiqueFourier(QWidget * parent) : QCustomPlot(parent)
{

    // Configuration du graphe
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(45, 45, 45));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);
    // Axes
    this->xAxis->setRange(0, 1);
    this->yAxis->setRange(0, 1.05);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    // Marges
    this->setAutoMargin(false);
    this->setMargin(0, 0, 0, 0);
    // Texte
    text1 = new QCPItemText(this);
    text1->position->setType(QCPItemPosition::ptAxisRectRatio);
    text1->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text1->position->setCoords(1.0, 0);
    text1->setTextAlignment(Qt::AlignRight);
    text1->setFont(QFont(font().family(), 8, QFont::Bold));
    text1->setColor(QColor(0, 0, 0, 180));
    this->addItem(text1);
    text2 = new QCPItemText(this);
    text2->position->setType(QCPItemPosition::ptAxisRectRatio);
    text2->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text2->setTextAlignment(Qt::AlignRight);
    text2->setFont(QFont(font().family(), 8));
    text2->setColor(QColor(0, 0, 0, 75));
    this->addItem(text2);
    text3 = new QCPItemText(this);
    text3->position->setType(QCPItemPosition::ptAxisRectRatio);
    text3->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text3->setTextAlignment(Qt::AlignRight);
    text3->setFont(QFont(font().family(), 8));
    text3->setColor(QColor(0, 0, 150, 150));
    this->addItem(text3);
    text4 = new QCPItemText(this);
    text4->position->setType(QCPItemPosition::ptAxisRectRatio);
    text4->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text4->setTextAlignment(Qt::AlignRight);
    text4->setFont(QFont(font().family(), 8));
    text4->setColor(QColor(0, 0, 150, 150));
    this->addItem(text4);
    text5 = new QCPItemText(this);
    text5->position->setType(QCPItemPosition::ptAxisRectRatio);
    text5->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text5->setTextAlignment(Qt::AlignRight);
    text5->setFont(QFont(font().family(), 8));
    text5->setColor(QColor(0, 0, 150, 150));
    this->addItem(text5);
    // Filtre
    this->installEventFilter(this);
}

// Méthodes publiques
void GraphiqueFourier::setBackgroundColor(QColor color)
{
    // Modification de la couleur de fond
    this->setColor(color);
}
void GraphiqueFourier::setData(QByteArray baData, qint32 posStart, qint32 posEnd, DWORD dwSmplRate)
{
    this->baData = baData;
    this->dwSmplRate = dwSmplRate;
    this->setPos(posStart, posEnd);
}
void GraphiqueFourier::setPos(qint32 posStart, qint32 posEnd)
{
    // Détermination du régime permanent pour transformée de Fourier et corrélation
    if (posStart == posEnd)
        Sound::regimePermanent(this->baData, dwSmplRate, 16, posStart, posEnd);
    if (posEnd > (qint32)(posStart + dwSmplRate/2))
    {
        qint32 offset = (posEnd - posStart - dwSmplRate/2) / 2;
        posStart += offset;
        posEnd -= offset;
    }
    // Extraction des données du régime permanent
    QByteArray baData2 = baData.mid(posStart * 2, (posEnd - posStart) * 2);
    // Corrélation du signal de 20 à 20000Hz
    qint32 dMin;
    QByteArray baCorrel = Sound::correlation(baData2, dwSmplRate, 16, 20, 20000, dMin);
    // Transformée de Fourier du signal
    unsigned long size = 0;
    Complex * cpxData = Sound::fromBaToComplex(baData2, 16, size);
    Complex * fc_sortie_fft = Sound::FFT(cpxData, size);
    delete [] cpxData;
    QByteArray baFourier;
    baFourier.resize(size * 2);
    qint32 * data = (qint32 *)baFourier.data();
    for (unsigned int i = 0; i < size/2; i++)
    {
        data[i] = qSqrt(fc_sortie_fft[i].real() * fc_sortie_fft[i].real() +
                fc_sortie_fft[i].imag() * fc_sortie_fft[i].imag());
        data[i] += qSqrt(fc_sortie_fft[size-i-1].real() * fc_sortie_fft[size-i-1].real() +
                        fc_sortie_fft[size-i-1].imag() * fc_sortie_fft[size-i-1].imag());
    }
    delete fc_sortie_fft;

    // Recherche des corrélations minimales (= plus grandes similitudes)
    quint32 * posMinCor = Sound::findMins(baCorrel, 32, 10, 0.9);

    // Recherche des intensités maximales
    int nbPic = 50;
    quint32 * posMaxFFT = Sound::findMax(baFourier, 32, nbPic, 0.05);

    // Portion de fourier contenant le 1er minimum de la corrélation
    quint32 iMin, iMax;
    iMin = (size - 1) / (posMinCor[0]+dMin+1) - 1;
    if (iMin < 1) iMin = 1;
    iMax = (size - 1) / (posMinCor[0]+dMin-1) + 1;
    if (iMax > size/2) iMax = size/2;
    // Un pic s'y trouve-t-il ?
    bool rep = false;
    int numeroPic = 0;
    while (rep == false && numeroPic < nbPic)
    {
        rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
        if (!rep) numeroPic++;
    }

    double freq = 0;
    if (!rep)
    {
        // Recherche à l'octave au dessus
        iMin *= 2;
        iMax *= 2;
        if (iMin >= size/2) iMin = size/2;
        if (iMax >= size/2) iMax = size/2;
        // Un pic s'y trouve-t-il ?
        numeroPic = 0;
        while (rep == false && numeroPic < nbPic)
        {
            rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
            if (!rep) numeroPic++;
        }
        if (!rep)
        {
            // On prend le plus grand pic de Fourier
            numeroPic = posMaxFFT[0];
        }
    }
    freq = (double)(posMaxFFT[numeroPic] * dwSmplRate) / (size - 1);

    // Numéro de la note correspondant à cette fréquence
    double note3 = 12*log2(freq)-36.3763;
    if (note3 < 0) note3 = 0;
    else if (note3 > 128) note3 = 128;

    // note la plus proche
    int note = note3 + 0.5;
    int correction = (note - note3) * 100 + 0.5;

    // Affichage
    QString qStr1 = "";
    qStr1.sprintf("%s %d, %s %d (%s)", tr("note").toStdString().c_str(), note,
                  tr("correction").toStdString().c_str(), correction, tr("estimation").toStdString().c_str());
    this->text1->setText(qStr1);
    QString qStr2 = "";
    QString qStr3 = "";
    QString qStr4 = "";
    QString qStr5 = "";
    QString qTmp;
    for (int i = 0; i < 10; i++)
    {
        if (posMaxFFT[i] != 0)
        {
            // intensité
            qTmp.sprintf("%.2f\n", (double)data[posMaxFFT[i]] / data[posMaxFFT[0]]);
            qStr2.append(qTmp);
            // fréquence
            double freq = (double)(posMaxFFT[i] * dwSmplRate) / (size - 1);
            qTmp.sprintf("%.2f Hz\n", freq);
            qStr3.append(qTmp);
            // note la plus proche
            double note = 12*log2(freq)-36.3763;
            if (note < 0) note = 0;
            else if (note > 128) note = 128;
            int note2 = note + .5;
            int correction = (note2 - note) * 100 + 0.5;
            qTmp.sprintf("%d\n", note2);
            qStr4.append(qTmp);
            qTmp.sprintf("%d\n", correction);
            qStr5.append(qTmp);
        }
    }
    this->text2->setText(qStr2);
    this->text3->setText(qStr3);
    this->text4->setText(qStr4);
    this->text5->setText(qStr5);
    // Initialisation du graphe
    this->graph(0)->clearData();
    // Ajout des données
    DWORD size_x = ((long int)baFourier.size() / 4 * 40000) / this->dwSmplRate;
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i=0; i < size_x; i++)
    {
        x[i] = (double)i/(size_x - 1);
        if (i < (unsigned)baFourier.size() / 4)
            y[i] = (double)data[i]/data[posMaxFFT[0]]; // normalisation entre 0 et 1
        else
            y[i] = 0;
    }
    this->graph(0)->setData(x, y);
    this->replot();
    // Destruction de la liste des mini maxi
    delete [] posMaxFFT;
    delete [] posMinCor;
}
void GraphiqueFourier::resized()
{
    // Repositionnement du texte
    QSize size = this->size();
    double y = 1.0 + (double)(14.0 - size.height()) / size.height();
    text2->position->setCoords((double)(size.width() - 108) / size.width(), y);
    text3->position->setCoords((double)(size.width() - 45) / size.width(), y);
    text4->position->setCoords((double)(size.width() - 20) / size.width(), y);
    text5->position->setCoords((double)(size.width() - 1) / size.width(), y);
}
