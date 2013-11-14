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

#include "page_smpl.h"
#include "ui_page_smpl.h"
#include "mainwindow.h"
#include "sound.h"
#include "dialog_sifflements.h"
#include <QProgressDialog>
#include <QInputDialog>

Page_Smpl::Page_Smpl(QWidget *parent) :
    Page(PAGE_SMPL, parent), ui(new Ui::Page_Smpl)
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

    // Adaptation petits écrans
    if (QApplication::desktop()->width() <= 800)
    {
        QTabWidget * tabWidget = new QTabWidget(this);
        QFont fontBold = tabWidget->font();
        fontBold.setBold(true);
        tabWidget->setFont(fontBold);
        fontBold.setBold(false);
        QFont font = fontBold;
        ui->label->setFont(font);
        ui->label_2->setFont(font);
        ui->label_3->setFont(font);
        ui->label_4->setFont(font);
        ui->label_5->setFont(font);
        ui->label_6->setFont(font);
        ui->label_7->setFont(font);
        ui->label_8->setFont(font);
        ui->label_9->setFont(font);
        ui->label_10->setFont(font);
        ui->label_11->setFont(font);
        ui->label_12->setFont(font);
        ui->label_13->setFont(font);
        ui->label_14->setFont(font);
        ui->label_15->setFont(font);
        ui->label_16->setFont(font);
        ui->label_17->setFont(font);
        ui->labelTaille->setFont(font);
        ui->comboLink->setFont(font);
        ui->comboSampleRate->setFont(font);
        ui->comboType->setFont(font);
        ui->spinEndLoop->setFont(font);
        ui->spinRootKey->setFont(font);
        ui->spinStartLoop->setFont(font);
        ui->spinTune->setFont(font);
        ui->pushEgaliser->setFont(font);
        ui->pushEgalRestore->setFont(font);
        tabWidget->addTab(ui->frame_2, trUtf8("Informations"));
        tabWidget->addTab(ui->frame_7, trUtf8("Égaliseur (±15 dB)"));
        delete ui->label_20;
        delete ui->label_21;
        QGridLayout * layout = (QGridLayout *) ui->frame_5->layout();
        layout->addWidget(tabWidget, 0, 0, 0, 1);
    }
}
Page_Smpl::~Page_Smpl()
{
    delete ui;
}

void Page_Smpl::afficher()
{
    // Préparation de l'affichage
    preparation = true;

    // Valeurs à afficher
    int nombreElements = tree->getSelectedItemsNumber();
    EltID id = tree->getID(0);
    DWORD sampleRate = sf2->get(id, champ_dwSampleRate).dwValue;
    int rootKey = sf2->get(id, champ_byOriginalPitch).bValue;
    int correction = sf2->get(id, champ_chPitchCorrection).cValue;
    DWORD startLoop = sf2->get(id, champ_dwStartLoop).dwValue;
    DWORD endLoop = sf2->get(id, champ_dwEndLoop).dwValue;
    DWORD length = sf2->get(id, champ_dwLength).dwValue;
    SFSampleLink typeLink = sf2->get(id, champ_sfSampleType).sfLinkValue;

    for (int i = 1; i < nombreElements; i++)
    {
        EltID idTmp = tree->getID(i);

        if (sampleRate != sf2->get(idTmp, champ_dwSampleRate).dwValue)
            sampleRate = -1;
        if (rootKey != sf2->get(idTmp, champ_byOriginalPitch).bValue)
            rootKey = 0;
        if (correction != sf2->get(idTmp, champ_chPitchCorrection).cValue)
            correction = 0;
        if (startLoop != sf2->get(idTmp, champ_dwStartLoop).dwValue)
            startLoop = 0;
        if (endLoop != sf2->get(idTmp, champ_dwEndLoop).dwValue)
            endLoop = 0;
        if (length > sf2->get(idTmp, champ_dwLength).dwValue)
            length = sf2->get(idTmp, champ_dwLength).dwValue;
        if (typeLink != sf2->get(idTmp, champ_sfSampleType).sfLinkValue)
            typeLink = linkInvalid;
    }

    // Remplissage des informations
    ui->comboSampleRate->setCurrentIndex(ui->comboSampleRate->findText(QString::number(sampleRate)));
    if (nombreElements == 1)
        ui->labelTaille->setText(QString::number(length) + " - " +
                                 QString::number((double)length / sampleRate, 'f', 3) + "s");
    else
    {
        if (sampleRate > 0)
            ui->labelTaille->setText(trUtf8("(min) ") + QString::number(length) + " - " +
                                     QString::number((double)length / sampleRate, 'f', 3) + "s");
        else
            ui->labelTaille->setText(trUtf8("(min) ") + QString::number(length));
    }
    ui->spinStartLoop->blockSignals(true);
    ui->spinEndLoop->blockSignals(true);
    ui->spinStartLoop->setMaximum(endLoop);
    ui->spinEndLoop->setMaximum(length);
    ui->spinEndLoop->setMinimum(startLoop);
    ui->spinStartLoop->setValue(startLoop);
    ui->spinEndLoop->setValue(endLoop);
    ui->spinStartLoop->blockSignals(false);
    ui->spinEndLoop->blockSignals(false);
    ui->spinRootKey->setValue(rootKey);
    ui->spinTune->setValue(correction);

    // Graphiques
    ui->graphe->clearAll();
    if (nombreElements > 1)
    {
        ui->graphe->displayMultipleSelection(true);
        ui->grapheFourier->setData(QByteArray(), 0, 0, 0);
    }
    else
    {
        ui->graphe->displayMultipleSelection(false);
        QByteArray baData = this->sf2->getData(id, champ_sampleData16);
        ui->graphe->setData(baData); // prend du temps
        ui->graphe->setStartLoop(startLoop, false);
        ui->graphe->setEndLoop(endLoop, false);
        ui->graphe->zoomDrag();

        // Remplissage du graphe fourier
        ui->grapheFourier->setData(baData, startLoop, endLoop, sampleRate);
    }

    // Lecteur
    if (nombreElements == 1)
    {
        if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
        {
            ui->checkLectureBoucle->setEnabled(false);
            ui->checkLectureBoucle->setChecked(false);
        }
        else
        {
            ui->checkLectureBoucle->setEnabled(true);
            ui->checkLectureBoucle->setChecked(true);
        }
    }
    else
        ui->checkLectureBoucle->setEnabled(false);
    ui->checkLectureSinus->setEnabled(nombreElements == 1);
    ui->pushLecture->setEnabled(nombreElements == 1);
    ui->sliderLectureVolume->setEnabled(nombreElements == 1);


    // Type et lien
    EltID id2 = id;
    this->ui->comboLink->clear();
    this->ui->comboType->clear();

    // Liens possibles
    if (nombreElements == 1)
    {
        for (int i = 0; i < this->sf2->count(id2); i++)
        {
            if (i != id.indexElt)
            {
                id2.indexElt = i;
                if (!this->sf2->get(id2, champ_hidden).bValue)
                    this->ui->comboLink->addItem(this->sf2->getQstr(id2, champ_name));
            }
        }
    }
    ui->comboLink->model()->sort(0);
    ui->comboLink->insertItem(0, "-");
    ui->comboLink->setEnabled(nombreElements == 1);

    // Types possibles et sélections
    this->ui->comboType->addItem(trUtf8("mono"));
    if (typeLink == monoSample || typeLink == RomMonoSample)
    {
        this->ui->comboType->setCurrentIndex(0);
        this->ui->comboLink->setCurrentIndex(0);
        this->ui->checkLectureLien->setEnabled(false);
        this->ui->checkLectureLien->setChecked(false);
    }
    else
    {
        this->ui->comboType->addItem(trUtf8("droit"));
        this->ui->comboType->addItem(trUtf8("gauche"));
        this->ui->comboType->addItem(trUtf8("lien"));
        switch (typeLink)
        {
        case rightSample: case RomRightSample:
            ui->comboType->setCurrentIndex(1);
            break;
        case leftSample: case RomLeftSample:
            ui->comboType->setCurrentIndex(2);
            break;
        case linkedSample: case RomLinkedSample:
            ui->comboType->setCurrentIndex(3);
            break;
        default:
            ui->comboType->setCurrentIndex(-1);
        }
        if (nombreElements == 1)
        {
            id2.indexElt = this->sf2->get(id, champ_wSampleLink).wValue;
            ui->comboLink->setCurrentIndex(ui->comboLink->findText(sf2->getQstr(id2, champ_name)));
        }
        else
            ui->comboLink->setCurrentIndex(-1);

        ui->checkLectureLien->setEnabled(nombreElements == 1);
    }
    ui->comboType->setEnabled(nombreElements == 1);

    // Liste des instruments qui utilisent le sample
    if (nombreElements > 1)
        this->ui->labelInst->setText("-");
    else
    {
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
            qStr = trUtf8("<b>Sample lié à aucun instrument.</b>");
        else if (nbInst == 1)
            qStr.prepend(trUtf8("<b>Sample lié à l'instrument : </b>"));
        else
            qStr.prepend(trUtf8("<b>Sample lié aux instruments : </b>"));
        this->ui->labelInst->setText(qStr);
    }

    // Basculement affichage
    this->qStackedWidget->setCurrentWidget(this); // prend du temps
    preparation = false;
}
void Page_Smpl::updateColors()
{
    this->ui->graphe->updateStyle();
}

void Page_Smpl::setStartLoop()
{
    if (preparation)
        return;

    sf2->prepareNewActions();
    Valeur val;
    val.dwValue = ui->spinStartLoop->value();
    ui->spinEndLoop->setMinimum(val.dwValue);
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = tree->getID(i);
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->get(id, champ_dwStartLoop).dwValue != val.dwValue)
                sf2->set(id, champ_dwStartLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.dwValue != sf2->get(id2, champ_dwStartLoop).dwValue &&
                            val.dwValue <= sf2->get(id2, champ_dwLength).dwValue)
                        sf2->set(id2, champ_dwStartLoop, val);
                }
            }
        }
    }

    // Mise à jour fenêtre et graphe Fourier
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(tree->getSelectedItemsNumber() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    this->mainWindow->updateDo();
    if (tree->getSelectedItemsNumber() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}
void Page_Smpl::setStartLoop(int val)
{
    if (tree->getSelectedItemsNumber() == 1)
    {
        EltID id2 = getRepercussionID();
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinStartLoop->value() > sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        this->synth->setStartLoop(val, id2.indexElt != -1);
    }
}
void Page_Smpl::setEndLoop()
{
    if (preparation)
        return;

    sf2->prepareNewActions();
    Valeur val;
    val.dwValue = this->ui->spinEndLoop->value();
    ui->spinStartLoop->setMaximum(val.dwValue);
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = this->tree->getID(i);
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->get(id, champ_dwEndLoop).dwValue != val.dwValue)
                sf2->set(id, champ_dwEndLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.dwValue != sf2->get(id2, champ_dwEndLoop).dwValue &&
                            val.dwValue <= sf2->get(id2, champ_dwLength).dwValue)
                        sf2->set(id2, champ_dwEndLoop, val);
                }
            }
        }
    }

    // Mise à jour fenêtre et graphe Fourier
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(tree->getSelectedItemsNumber() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    mainWindow->updateDo();
    if (tree->getSelectedItemsNumber() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}
void Page_Smpl::setEndLoop(int val)
{
    if (tree->getSelectedItemsNumber() == 1)
    {
        EltID id2 = getRepercussionID();
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinEndLoop->value() > sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        this->synth->setEndLoop(val, id2.indexElt != -1);
    }
}
void Page_Smpl::setRootKey()
{
    if (preparation)
        return;

    sf2->prepareNewActions();
    Valeur val;
    val.bValue = this->ui->spinRootKey->value();
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = tree->getID(i);
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->get(id, champ_byOriginalPitch).bValue != val.bValue)
                sf2->set(id, champ_byOriginalPitch, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.bValue != sf2->get(id2, champ_byOriginalPitch).bValue)
                        sf2->set(id2, champ_byOriginalPitch, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    this->mainWindow->updateDo();
}
void Page_Smpl::setRootKey(int val)
{
    // Modif synth
    if (tree->getSelectedItemsNumber() == 1)
        synth->setRootKey(val);
}
void Page_Smpl::setTune()
{
    if (preparation)
        return;

    sf2->prepareNewActions();
    Valeur val;
    val.cValue = ui->spinTune->value();
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = this->tree->getID(i);
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->get(id, champ_chPitchCorrection).cValue != val.cValue)
                sf2->set(id, champ_chPitchCorrection, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.cValue != sf2->get(id2, champ_chPitchCorrection).bValue)
                        sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    this->mainWindow->updateDo();
}
void Page_Smpl::setTune(int val)
{
    // Modif synth
    if (tree->getSelectedItemsNumber() == 1)
        this->synth->setPitchCorrection(val, getRepercussionID().indexElt != -1);
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
            this->ui->comboType->addItem(trUtf8("mono"));
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
        this->ui->comboType->addItem(trUtf8("mono"));
        this->ui->comboType->addItem(trUtf8("droit"));
        this->ui->comboType->addItem(trUtf8("gauche"));
        this->ui->comboType->addItem(trUtf8("lien"));
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
    Q_UNUSED(index);
    if (preparation)
        return;

    sf2->prepareNewActions();
    DWORD echFinal = ui->comboSampleRate->currentText().toInt();
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = this->tree->getID(i);
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            DWORD echInit = sf2->get(id, champ_dwSampleRate).dwValue;
            if (echInit != echFinal)
                setRateElt(id, echFinal);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (echFinal != sf2->get(id2, champ_dwSampleRate).dwValue)
                        setRateElt(id2, echFinal);
                }
            }
        }
    }

    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Smpl::setRateElt(EltID id, DWORD echFinal)
{
    // Modification échantillonnage
    DWORD echInit = sf2->get(id, champ_dwSampleRate).dwValue;
    QByteArray baData = sf2->getData(id, champ_sampleDataFull24);
    baData = Sound::resampleMono(baData, echInit, echFinal, 24);
    sf2->set(id, champ_sampleDataFull24, baData);
    Valeur val;
    val.dwValue = echFinal;
    sf2->set(id, champ_dwSampleRate, val);

    // Ajustement de length, startLoop, endLoop
    val.dwValue = baData.size()/3;
    sf2->set(id, champ_dwLength, val);
    DWORD dwTmp = this->sf2->get(id, champ_dwStartLoop).dwValue;
    dwTmp = ((quint64)dwTmp * (quint64)echFinal) / (quint64)echInit;
    val.dwValue = dwTmp;
    sf2->set(id, champ_dwStartLoop, val);
    dwTmp = sf2->get(id, champ_dwEndLoop).dwValue;
    dwTmp = ((quint64)dwTmp * (quint64)echFinal) / (quint64)echInit;
    val.dwValue = dwTmp;
    sf2->set(id, champ_dwEndLoop, val);
}
EltID Page_Smpl::getRepercussionID(int num)
{
    EltID id = tree->getID(num);
    EltID id2 = id;

    // Recherche du sample associé, le cas échéant et si la répercussion est activée
    SFSampleLink typeLien = sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample && Config::getInstance()->getRepercussionStereo())
        id2.indexElt = sf2->get(id, champ_wSampleLink).wValue;
    else
        id2.indexElt = -1;
    return id2;
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    Config * conf = Config::getInstance();
    double rep = QInputDialog::getDouble(this, trUtf8("Question"),
                                         trUtf8("Fréquence de coupure :"),
                                         conf->getTools_s_mur_coupure(), 20, 96000, 2, &ok);
    if (!ok) return;
    conf->setTools_s_mur_coupure(rep);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    // Sauvegarde des valeurs
    Config * conf = Config::getInstance();
    conf->setTools_s_sifflements_debut(freq1);
    conf->setTools_s_sifflements_fin(freq2);
    conf->setTools_s_sifflements_raideur(raideur);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
    Config * conf = Config::getInstance();
    double rep = QInputDialog::getDouble(this, trUtf8("Transposition"),
                                         trUtf8("Écart en demi-tons :"),
                                         conf->getTools_s_transpo_ton(), -36, 36, 2,
                                         &ok);
    if (!ok) return;
    conf->setTools_s_transpo_ton(rep);
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
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
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
                baData = Sound::resampleMono(baData, echInit, echFinal, 24);
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
    if (preparation)
        return;

    if (ui->verticalSlider_1->value() == 0 &&
        ui->verticalSlider_2->value() == 0 &&
        ui->verticalSlider_3->value() == 0 &&
        ui->verticalSlider_4->value() == 0 &&
        ui->verticalSlider_5->value() == 0 &&
        ui->verticalSlider_6->value() == 0 &&
        ui->verticalSlider_7->value() == 0 &&
        ui->verticalSlider_8->value() == 0 &&
        ui->verticalSlider_9->value() == 0 &&
        ui->verticalSlider_10->value() == 0)
        return;

    sf2->prepareNewActions();
    QList<EltID> listeID;
    for (unsigned int i = 0; i < tree->getSelectedItemsNumber(); i++)
    {
        EltID id = this->tree->getID(i);
        if (!sf2->get(id, champ_hidden).bValue && !listeID.contains(id))
        {
            listeID << id;
            QByteArray baData = sf2->getData(id, champ_sampleDataFull24);
            baData = Sound::EQ(baData, sf2->get(id, champ_dwSampleRate).dwValue, 24,
                               ui->verticalSlider_1->value(),
                               ui->verticalSlider_2->value(),
                               ui->verticalSlider_3->value(),
                               ui->verticalSlider_4->value(),
                               ui->verticalSlider_5->value(),
                               ui->verticalSlider_6->value(),
                               ui->verticalSlider_7->value(),
                               ui->verticalSlider_8->value(),
                               ui->verticalSlider_9->value(),
                               ui->verticalSlider_10->value());
            sf2->set(id, champ_sampleDataFull24, baData);

            // Sample associé ?
            EltID id2 = getRepercussionID(i);
            if (id2.indexElt != -1)
            {
                if (!sf2->get(id2, champ_hidden).bValue && !listeID.contains(id2))
                {
                    listeID << id2;
                    QByteArray baData = sf2->getData(id2, champ_sampleDataFull24);
                    baData = Sound::EQ(baData, sf2->get(id2, champ_dwSampleRate).dwValue, 24,
                                       ui->verticalSlider_1->value(),
                                       ui->verticalSlider_2->value(),
                                       ui->verticalSlider_3->value(),
                                       ui->verticalSlider_4->value(),
                                       ui->verticalSlider_5->value(),
                                       ui->verticalSlider_6->value(),
                                       ui->verticalSlider_7->value(),
                                       ui->verticalSlider_8->value(),
                                       ui->verticalSlider_9->value(),
                                       ui->verticalSlider_10->value());
                    sf2->set(id2, champ_sampleDataFull24, baData);
                }
            }
        }
    }

    // Actualisation
    mainWindow->updateDo();
    afficher();
}
void Page_Smpl::initEQ()
{
    ui->verticalSlider_1->setValue(0);
    ui->verticalSlider_2->setValue(0);
    ui->verticalSlider_3->setValue(0);
    ui->verticalSlider_4->setValue(0);
    ui->verticalSlider_5->setValue(0);
    ui->verticalSlider_6->setValue(0);
    ui->verticalSlider_7->setValue(0);
    ui->verticalSlider_8->setValue(0);
    ui->verticalSlider_9->setValue(0);
    ui->verticalSlider_10->setValue(0);
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
    ui->comboLink->setEnabled(true);
    ui->comboType->setEnabled(true);
    ui->comboSampleRate->setEnabled(true);
    ui->verticalSlider_1->setEnabled(true);
    ui->verticalSlider_2->setEnabled(true);
    ui->verticalSlider_3->setEnabled(true);
    ui->verticalSlider_4->setEnabled(true);
    ui->verticalSlider_5->setEnabled(true);
    ui->verticalSlider_6->setEnabled(true);
    ui->verticalSlider_7->setEnabled(true);
    ui->verticalSlider_8->setEnabled(true);
    ui->verticalSlider_9->setEnabled(true);
    ui->verticalSlider_10->setEnabled(true);
    ui->pushEgaliser->setEnabled(true);
    ui->pushEgalRestore->setEnabled(true);
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
Graphique::Graphique(QWidget * parent) : QCustomPlot(parent),
    zoomFlag(false),
    dragFlag(false),
    zoomX(1),
    zoomY(1),
    posX(.5),
    posY(.5),
    zoomXinit(1),
    zoomYinit(1),
    posXinit(.5),
    posYinit(.5),
    bFromExt(false),
    qScrollX(NULL),
    spinStart(NULL),
    spinEnd(NULL),
    filterEventEnabled(true),
    textMultipleSelection(NULL)
{
    this->installEventFilter(this);

    // Graphe des données
    this->addGraph();
    this->graph(0)->setAntialiased(false);

    // Graphes contenant startLoop, endLoop
    this->addGraph();
    this->addGraph();

    // Graphe pour la ligne de zoom
    this->addGraph();

    // Graphes contenant l'overlay
    this->addGraph();
    this->addGraph();

    // Axes
    this->xAxis2->setRange(0, 1);
    this->yAxis2->setRange(0, 1);
    this->yAxis->setRange(-1, 1);
    this->xAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->setAutoMargin(false);
    this->setMargin(0, 0, 0, 0);

    // Message "sélection multiple"
    textMultipleSelection = new QCPItemText(this);
    textMultipleSelection->position->setType(QCPItemPosition::ptAxisRectRatio);
    textMultipleSelection->setPositionAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    textMultipleSelection->position->setCoords(.5, .5);
    textMultipleSelection->setTextAlignment(Qt::AlignHCenter);
    textMultipleSelection->setFont(QFont(font().family(), 16, QFont::Bold));
    textMultipleSelection->setText(trUtf8("Sélection multiple"));
    addItem(textMultipleSelection);

    // Paramétrage des couleurs
    this->updateStyle();
}

// Méthodes publiques
void Graphique::updateStyle()
{
    QList<QColor> colors = Config::getInstance()->getColors();
    if (colors.size() != 6)
        return;
    // Couleur de fond
    this->setColor(colors.at(0));
    QPen graphPen;
    graphPen.setColor(colors.at(0));
    this->yAxis->setBasePen(graphPen);
    // Couleur de l'onde
    graphPen.setColor(colors.at(1));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);
    // Couleur début de boucle
    graphPen.setColor(colors.at(3));
    graphPen.setWidthF(2);
    this->graph(1)->setPen(graphPen);
    // Couleur fin de boucle
    graphPen.setColor(colors.at(4));
    graphPen.setWidthF(2);
    this->graph(2)->setPen(graphPen);
    // Ligne de zoom
    graphPen.setColor(colors.at(5));
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DashLine);
    this->graph(3)->setPen(graphPen);
    // Overlay
    QColor colorTmp = colors.at(3);
    colorTmp.setAlpha(175);
    graphPen.setColor(colorTmp);
    graphPen.setWidth(2);
    graphPen.setStyle(Qt::DotLine);
    this->graph(4)->setPen(graphPen);
    colorTmp = colors.at(4);
    colorTmp.setAlpha(175);
    graphPen.setColor(colorTmp);
    this->graph(5)->setPen(graphPen);
    // Couleur de la grille
    graphPen.setColor(colors.at(2));
    graphPen.setWidthF(1);
    this->yAxis->setZeroLinePen(graphPen);
    graphPen.setStyle(Qt::DotLine);
    this->yAxis->setGridPen(graphPen);
    // Curseur de lecture et message (sélection multiple)
    penLecture.setColor(colors.at(5));
    penLecture.setWidthF(1);
    colorTmp = colors.at(5);
    colorTmp.setAlpha(180);
    textMultipleSelection->setColor(colorTmp);

    this->replot();
}

void Graphique::clearAll()
{
    this->graph(0)->clearData();
    this->graph(1)->clearData();
    this->graph(2)->clearData();
    this->graph(3)->clearData();
    this->graph(4)->clearData();
    this->graph(5)->clearData();
}
void Graphique::setData(QByteArray baData)
{
    // Ajout des données
    DWORD size_x = baData.size() / 2;
    this->sizeX = ((double)size_x - 1);
    qint16 * data = (qint16 *)baData.data();
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i = 0; i < size_x; i++)
    {
        x[i] = i;
        y[i] = (double)data[i] / 32768; // normalisation entre -1 et 1
    }
    this->graph(0)->setData(x, y);
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
void Graphique::setStartLoop(int pos, bool replot)
{
    if (filterEventEnabled)
    {
        if (pos >= 0)
        {
            QVector<double> x(2), y(2);
            x[0] = pos;
            x[1] = pos;
            y[0] = -1;
            y[1] = 1;
            this->graph(1)->setData(x, y);
        }
        this->plotOverlay();
        if (replot)
            this->replot();
    }
}
void Graphique::setEndLoop(int pos, bool replot)
{
    if (filterEventEnabled)
    {
        if (pos >= 0)
        {
            QVector<double> x(2), y(2);
            x[0] = pos;
            x[1] = pos;
            y[0] = -1;
            y[1] = 1;
            this->graph(2)->setData(x, y);
        }
        this->plotOverlay();
        if (replot)
            this->replot();
    }
}
void Graphique::setCurrentSample(int pos)
{
    m_currentPos = this->xAxis->coordToPixel(pos);
    this->repaint();
    this->update();
}
void Graphique::paintEvent(QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);
    // Ajout du trait de lecture
    if (m_currentPos > 0)
    {
        QPainter painter(this);
        painter.setPen(penLecture);
        painter.drawLine(QPointF(m_currentPos, -10000), QPointF(m_currentPos, 10000));
    }
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
    double etendueY = 2. / zoomY;
    double offsetY = (2. - etendueY) * posY - 1;
    this->yAxis->setRange(offsetY, offsetY + etendueY);
    // Mise à jour
    this->replot();
    if (!bFromExt && qScrollX)
    {
        // Mise à jour du scrollbar
        double valMax = ((zoomX-1.)*sizeX) / 10000;
        qScrollX->blockSignals(true);
        qScrollX->setRange(0, valMax);
        qScrollX->setValue(valMax*posX);
        qScrollX->blockSignals(false);
    }
}
void Graphique::displayMultipleSelection(bool isOn)
{
    qScrollX->setEnabled(!isOn);
    filterEventEnabled = !isOn;
    textMultipleSelection->setVisible(isOn);
    if (isOn)
        zoomDrag();
    else
        qScrollX->setRange(0, 0);
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
void Graphique::setZoomLine(double x1, double y1, double x2, double y2)
{
    if (x1 >= 0)
    {
        // Conversion
        QVector<double> x(2), y(2);
        x[0] = sizeX * (x1 + posX * (zoomX - 1)) / zoomX;
        x[1] = sizeX * (x2 + posX * (zoomX - 1)) / zoomX;
        y[0] = 2 * (y1 + posY * (zoomY - 1)) / zoomY - 1;
        y[1] = 2 * (y2 + posY * (zoomY - 1)) / zoomY - 1;
        this->graph(3)->setData(x, y);
    }
    else
        this->graph(3)->clearData();
    this->replot();
}
void Graphique::plotOverlay()
{
    int sizeOverlay = 20;
    qint32 posDebut = this->spinStart->value();
    qint32 posFin = this->spinEnd->value();
    if (posDebut > 0 && posFin > 0 && posDebut != posFin)
    {
        QVector<double> x1(2 * sizeOverlay), y1(2 * sizeOverlay),
                x2(2 * sizeOverlay), y2(2 * sizeOverlay);
        for (int i = 0; i < 2 * sizeOverlay; i++)
        {
            // Partie droite recopiée à gauche
            x1[i]                       = posDebut - sizeOverlay + i;
            x1[2 * sizeOverlay - i - 1] = posDebut + sizeOverlay - i;
            y1[i]                       = this->graph(0)->data()->value(posFin - sizeOverlay + i).value;
            y1[2 * sizeOverlay - i - 1] = this->graph(0)->data()->value(posFin + sizeOverlay - i).value;
            // Partie gauche recopiée à droite
            x2[i]                       = posFin - sizeOverlay + i;
            x2[2 * sizeOverlay - i - 1] = posFin + sizeOverlay - i;
            y2[i]                       = this->graph(0)->data()->value(posDebut - sizeOverlay + i).value;
            y2[2 * sizeOverlay - i - 1] = this->graph(0)->data()->value(posDebut + sizeOverlay - i).value;
        }
        this->graph(4)->setData(x1, y1);
        this->graph(5)->setData(x2, y2);
    }
    else
    {
        this->graph(4)->clearData();
        this->graph(5)->clearData();
    }
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
    if (this->baData.size() == 0)
    {
        text1->setText("");
        text2->setText("");
        text3->setText("");
        text4->setText("");
        text5->setText("");
        graph(0)->clearData();
        replot();
        return;
    }

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
    QByteArray baCorrel = Sound::correlation(baData2.left(qMin(baData2.size(), 8000)), dwSmplRate, 16, 20, 20000, dMin);

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

    // Recherche des corrélations minimales (= plus grandes similitudes) et intensités maximales
    quint32 * posMinCor = Sound::findMins(baCorrel, 16, 10, 0.9);
    int nbPic = 50;
    quint32 * posMaxFFT = Sound::findMax(baFourier, 32, nbPic, 0.05);

    // Portion de fourier contenant le 1er minimum de la corrélation
    quint32 iMin, iMax;
    iMin = (size - 1) / (posMinCor[0]+dMin+1) - 1;
    if (iMin < 1) iMin = 1;
    if (!(posMinCor[0] + dMin - 1))
        iMax = 0;
    else
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
            //qDebug() << "iMin" << iMin << "iMax" << iMax << "numeroPic" << numeroPic << "posMaxFFT" << posMaxFFT[numeroPic];
            rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
            if (!rep) numeroPic++;
        }
        if (!rep)
        {
            // On prend le plus grand pic de Fourier
            numeroPic = 0;
        }
    }

    freq = (double)((double)posMaxFFT[numeroPic] * (double)dwSmplRate) / (double)(size - 1);

    // Numéro de la note correspondant à cette fréquence
    double note3 = 12 * log2(freq) - 36.3763;

    // note la plus proche
    int note = note3 + 0.5;
    int correction = (note - note3) * 100 + 0.5;

    // Affichage
    QString qStr1 = "";
    QString qStr2 = "";
    QString qStr3 = "";
    QString qStr4 = "";
    QString qStr5 = "";
    QString qTmp;
    if (note >= 0 && note <= 128)
    {
        qStr1.sprintf("%s %d, %s %d (%s)", tr("note").toStdString().c_str(), note,
                      tr("correction").toStdString().c_str(), correction, tr("estimation").toStdString().c_str());
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
                double note = 12 * log2(freq) - 36.3763;
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
    }
    text1->setText(qStr1);
    text2->setText(qStr2);
    text3->setText(qStr3);
    text4->setText(qStr4);
    text5->setText(qStr5);

    // Initialisation du graphe
    this->graph(0)->clearData();

    // Ajout des données
    DWORD size_x = ((long int)baFourier.size() / 4 * 40000) / this->dwSmplRate;
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i=0; i < size_x; i++)
    {
        x[i] = (double)i/(size_x - 1);
        if (i < (unsigned)baFourier.size() / 4 && posMaxFFT[0] != 0)
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
