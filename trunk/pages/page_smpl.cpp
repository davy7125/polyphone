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

#include "page_smpl.h"
#include "ui_page_smpl.h"
#include "mainwindow.h"
#include "sound.h"
#include "dialog_sifflements.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include "graphique.h"
#include "graphiquefourier.h"

Page_Smpl::Page_Smpl(QWidget *parent) :
    Page(PAGE_SMPL, parent),
    ui(new Ui::Page_Smpl),
    lectureEnCours(false),
    preventStop(0)
{
    ui->setupUi(this);

    // Initialisation du graphique
    this->ui->graphe->linkSliderX(this->ui->sliderGraphe);
    this->ui->graphe->linkSpinBoxes(this->ui->spinStartLoop, this->ui->spinEndLoop);

    // Connexions
    this->ui->graphe->connect(this->_synth, SIGNAL(currentPosChanged(int)), SLOT(setCurrentSample(int)));
    this->connect(this->_synth, SIGNAL(readFinished()), SLOT(lecteurFinished()));
    connect(this->_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

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
    _preparation = true;

    // Valeurs à afficher
    QList<EltID> ids = _tree->getAllIDs();
    int nombreElements = ids.size();

    EltID id = ids.takeFirst();
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    int rootKey = _sf2->get(id, champ_byOriginalPitch).bValue;
    int correction = _sf2->get(id, champ_chPitchCorrection).cValue;
    int startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    if (startLoop < 0)
        startLoop = 0;
    int endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    if (endLoop < 0)
        endLoop = 0;
    quint32 length = _sf2->get(id, champ_dwLength).dwValue;
    SFSampleLink typeLink = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    while (!ids.isEmpty())
    {
        EltID idTmp = ids.takeFirst();

        if (sampleRate != _sf2->get(idTmp, champ_dwSampleRate).dwValue)
            sampleRate = -1;
        if (rootKey != _sf2->get(idTmp, champ_byOriginalPitch).bValue)
            rootKey = -1;
        if (correction != _sf2->get(idTmp, champ_chPitchCorrection).cValue)
            correction = 0;
        if (startLoop != (int)_sf2->get(idTmp, champ_dwStartLoop).dwValue)
            startLoop = 0;
        if (endLoop != (int)_sf2->get(idTmp, champ_dwEndLoop).dwValue)
            endLoop = 0;
        length = qMin(length, _sf2->get(idTmp, champ_dwLength).dwValue);
        if (typeLink != _sf2->get(idTmp, champ_sfSampleType).sfLinkValue)
            typeLink = linkInvalid;
    }

    // Initialisation clavier
    _mainWindow->clearKeyboardCustomisation();
    _mainWindow->setRangeAndRootKey(rootKey, 0, 127);
    if (rootKey == -1)
        rootKey = 0;

    // Remplissage des informations
    ui->comboSampleRate->setCurrentIndex(ui->comboSampleRate->findText(QString::number(sampleRate)));
    if (nombreElements == 1)
        ui->labelTaille->setText(QString::number(length) + " - " +
                                 QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds"));
    else
    {
        if (sampleRate > 0)
            ui->labelTaille->setText(trUtf8("(min) ") + QString::number(length) + " - " +
                                     QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds"));
        else
            ui->labelTaille->setText(trUtf8("(min) ") + QString::number(length));
    }
    ui->spinStartLoop->blockSignals(true);
    ui->spinEndLoop->blockSignals(true);
    ui->spinStartLoop->setMaximum(endLoop);
    ui->spinEndLoop->setMaximum(length-1);
    ui->spinEndLoop->setMinimum(startLoop);
    ui->spinStartLoop->setValue(startLoop);
    ui->spinEndLoop->setValue(endLoop);
    ui->spinStartLoop->blockSignals(false);
    ui->spinEndLoop->blockSignals(false);
    ui->spinRootKey->setValue(rootKey);
    ui->spinTune->setValue(correction);

    // Graphiques
    ui->graphe->clearAll();
    ui->grapheFourier->setSampleName(_sf2->getQstr(id, champ_name));
    if (nombreElements > 1)
    {
        ui->graphe->displayMultipleSelection(true);
        ui->grapheFourier->setData(QByteArray(), 0);
        ui->grapheFourier->setPos(0, 0);
    }
    else
    {
        ui->graphe->displayMultipleSelection(false);
        QByteArray baData = this->_sf2->getData(id, champ_sampleData16);
        ui->graphe->setData(baData); // prend du temps
        ui->graphe->setStartLoop(startLoop, false);
        ui->graphe->setEndLoop(endLoop, false);
        ui->graphe->zoomDrag();

        // Remplissage du graphe fourier
        ui->grapheFourier->setData(baData, sampleRate);
        ui->grapheFourier->setPos(startLoop, endLoop);
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
        for (int i = 0; i < this->_sf2->count(id2); i++)
        {
            if (i != id.indexElt)
            {
                id2.indexElt = i;
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                    this->ui->comboLink->addItem(this->_sf2->getQstr(id2, champ_name));
            }
        }
    }
    ui->comboLink->model()->sort(0);
    ui->comboLink->insertItem(0, "-");
    ui->comboLink->setEnabled(nombreElements == 1 && !lectureEnCours);

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
            id2.indexElt = this->_sf2->get(id, champ_wSampleLink).wValue;
            ui->comboLink->setCurrentIndex(ui->comboLink->findText(_sf2->getQstr(id2, champ_name)));
        }
        else
            ui->comboLink->setCurrentIndex(-1);

        ui->checkLectureLien->setEnabled(nombreElements == 1);
    }
    ui->comboType->setEnabled(nombreElements == 1 && !lectureEnCours);

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
        for (int i = 0; i < this->_sf2->count(id2); i++)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // Parcours de tous les samples liés à l'instrument
            isFound = false;
            j = 0;
            while(j < this->_sf2->count(id3) && !isFound)
            {
                id3.indexElt2 = j;
                if (!this->_sf2->get(id3, champ_hidden).bValue)
                {
                    if (this->_sf2->get(id3, champ_sampleID).wValue == id.indexElt)
                    {
                        // Ajout d'un instrument
                        if (nbInst)
                            qStr.append(", ");
                        qStr.append(this->_sf2->getQstr(id2, champ_name).toStdString().c_str());
                        nbInst++;
                        isFound = true;
                    }
                }
                j++;
            }
        }
        if (nbInst == 0)
            qStr = trUtf8("<b>Échantillon lié à aucun instrument.</b>");
        else if (nbInst == 1)
            qStr.prepend(trUtf8("<b>Échantillon lié à l'instrument : </b>"));
        else
            qStr.prepend(trUtf8("<b>Échantillon lié aux instruments : </b>"));
        this->ui->labelInst->setText(qStr);
    }

    if (!ui->pushLecture->isChecked())
        ui->pushLecture->setText(trUtf8("Lecture"));

    // Basculement affichage
    this->_qStackedWidget->setCurrentWidget(this); // prend du temps
    _preparation = false;
}

void Page_Smpl::updateColors()
{
    this->ui->graphe->updateStyle();
}

void Page_Smpl::setStartLoop()
{
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    Valeur val;
    val.dwValue = ui->spinStartLoop->value();
    ui->spinEndLoop->setMinimum(val.dwValue);
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_dwStartLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwStartLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.dwValue != _sf2->get(id2, champ_dwStartLoop).dwValue &&
                            val.dwValue <= _sf2->get(id2, champ_dwLength).dwValue)
                        _sf2->set(id2, champ_dwStartLoop, val);
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
        ui->checkLectureBoucle->setEnabled(_tree->getSelectedItemsNumber() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    this->_mainWindow->updateDo();
    if (_tree->getSelectedItemsNumber() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}

void Page_Smpl::setStartLoop(int val)
{
    if (_tree->getSelectedItemsNumber() == 1)
    {
        EltID id2 = getRepercussionID(_tree->getFirstID());
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinStartLoop->value() > _sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        this->_synth->setStartLoop(val, id2.indexElt != -1);
    }
}

void Page_Smpl::setEndLoop()
{
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    Valeur val;
    val.dwValue = this->ui->spinEndLoop->value();
    ui->spinStartLoop->setMaximum(val.dwValue);
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_dwEndLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwEndLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.dwValue != _sf2->get(id2, champ_dwEndLoop).dwValue &&
                            val.dwValue <= _sf2->get(id2, champ_dwLength).dwValue)
                        _sf2->set(id2, champ_dwEndLoop, val);
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
        ui->checkLectureBoucle->setEnabled(_tree->getSelectedItemsNumber() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    _mainWindow->updateDo();
    if (_tree->getSelectedItemsNumber() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}

void Page_Smpl::setEndLoop(int val)
{
    if (_tree->getSelectedItemsNumber() == 1)
    {
        EltID id2 = getRepercussionID(_tree->getFirstID());
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinEndLoop->value() >= _sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        this->_synth->setEndLoop(val, id2.indexElt != -1);
    }
}

void Page_Smpl::on_pushFullLength_clicked()
{
    _sf2->prepareNewActions();
    Valeur val;

    quint32 displayedEndLoop = 0;
    bool firstValue = true;
    bool triggersMessage = false;

    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            // Début de la boucle à 0
            val.dwValue = 0;
            if (_sf2->get(id, champ_dwStartLoop).dwValue != 0)
                _sf2->set(id, champ_dwStartLoop, val);

            // Fin de la boucle correspondant à la fin de l'échantillon - 1
            quint32 length = _sf2->get(id, champ_dwLength).dwValue - 1;
            val.dwValue = length;
            if (_sf2->get(id, champ_dwEndLoop).dwValue != length)
                _sf2->set(id, champ_dwEndLoop, val);

            if (firstValue)
            {
                firstValue = false;
                displayedEndLoop = length;
            }
            else if (displayedEndLoop != length)
            {
                displayedEndLoop = 0;
                triggersMessage = true;
            }

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    // Début de la boucle à 0
                    val.dwValue = 0;
                    if (_sf2->get(id2, champ_dwStartLoop).dwValue != 0)
                        _sf2->set(id2, champ_dwStartLoop, val);

                    // Fin de la boucle correspondant à la fin de l'échantillon
                    length = _sf2->get(id2, champ_dwLength).dwValue - 1;
                    val.dwValue = length;
                    if (_sf2->get(id2, champ_dwEndLoop).dwValue != length)
                        _sf2->set(id2, champ_dwEndLoop, val);
                }
            }
        }
    }

    ui->spinStartLoop->setValue(0);
    ui->spinStartLoop->setMaximum(displayedEndLoop);
    ui->spinEndLoop->setValue(displayedEndLoop);
    ui->spinEndLoop->setMinimum(0);

    // Mise à jour fenêtre et graphe Fourier
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(_tree->getSelectedItemsNumber() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    this->_mainWindow->updateDo();
    if (_tree->getSelectedItemsNumber() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());

    if (triggersMessage)
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Modification appliquée avec succès aux différents échantillons"));
}

void Page_Smpl::setRootKey()
{
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    Valeur val;
    val.bValue = ui->spinRootKey->value();
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_byOriginalPitch).bValue != val.bValue)
                _sf2->set(id, champ_byOriginalPitch, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.bValue != _sf2->get(id2, champ_byOriginalPitch).bValue)
                        _sf2->set(id2, champ_byOriginalPitch, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    this->_mainWindow->updateDo();
}

void Page_Smpl::setRootKey(int val)
{
    _mainWindow->clearKeyboardCustomisation();
    _mainWindow->setRangeAndRootKey(val, 0, 127);

    // Modif synth
    if (_tree->getSelectedItemsNumber() == 1)
        _synth->setRootKey(val);
}

void Page_Smpl::setTune()
{
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    Valeur val;
    val.cValue = ui->spinTune->value();
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_chPitchCorrection).cValue != val.cValue)
                _sf2->set(id, champ_chPitchCorrection, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.cValue != _sf2->get(id2, champ_chPitchCorrection).bValue)
                        _sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    this->_mainWindow->updateDo();
}

void Page_Smpl::setTune(int val)
{
    // Modif synth
    if (_tree->getSelectedItemsNumber() == 1)
        this->_synth->setPitchCorrection(val, getRepercussionID(_tree->getFirstID()).indexElt != -1);
}

void Page_Smpl::setType(int index)
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    // Ancien et nouveau types
    SFSampleLink ancienType = this->_sf2->get(id, champ_sfSampleType).sfLinkValue;
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
    _sf2->prepareNewActions();
    // Reprise de l'adresse si modification
    id = this->_tree->getFirstID();
    this->_preparation = true;
    // Modification du type du sample
    Valeur val;
    val.sfLinkValue = nouveauType;
    this->_sf2->set(id, champ_sfSampleType, val);
    if (ancienType != monoSample)
    {
        // Sample lié
        EltID id2 = id;
        id2.indexElt = this->_sf2->get(id, champ_wSampleLink).wValue;
        switch (nouveauType)
        {
        case monoSample:
            // Suppression du lien vers le sample
            val.wValue = 0;
            this->_sf2->set(id, champ_wSampleLink, val);
            // Suppression du lien du sample anciennement lié
            this->_sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            this->_sf2->set(id2, champ_sfSampleType, val);
            // Mise à jour combobox
            this->ui->comboType->clear();
            this->ui->comboType->addItem(trUtf8("mono"));
            this->ui->comboLink->setCurrentIndex(0);
            this->ui->checkLectureLien->setEnabled(false);
            this->ui->checkLectureLien->setChecked(false);
            break;
        case rightSample:
            val.sfLinkValue = leftSample;
            this->_sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        case leftSample:
            val.sfLinkValue = rightSample;
            this->_sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        case linkedSample:
            val.sfLinkValue = linkedSample;
            this->_sf2->set(id2, champ_sfSampleType, val);
            this->ui->checkLectureLien->setEnabled(true);
            break;
        default: break;
        }
    }
    this->_preparation = false;
    this->_mainWindow->updateDo();
}

void Page_Smpl::setLinkedSmpl(int index)
{
    if (_preparation) return;
    EltID id = this->_tree->getFirstID();
    // Ancien et nouveau samples liés
    EltID idLinkedOld = id;
    SFSampleLink type = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (type != monoSample && type != RomMonoSample)
        idLinkedOld.indexElt = this->_sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    EltID idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = this->ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = this->_sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!this->_sf2->get(id2, champ_hidden).bValue)
            {
                if (!this->_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
                    idLinkedNew.indexElt = pos;
            }
            pos++;
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    // Ancien et nouveau sont égaux ?
    if (idLinkedNew.indexElt == idLinkedOld.indexElt) return;
    _sf2->prepareNewActions();
    // Reprise des adresses si modification
    id = this->_tree->getFirstID();
    type = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (type != monoSample && type != RomMonoSample)
        idLinkedOld.indexElt = this->_sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = this->ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = this->_sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!this->_sf2->get(id2, champ_hidden).bValue)
            {
                if (!this->_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
                    idLinkedNew.indexElt = pos;
            }
            pos++;
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    this->_preparation = true;
    // Suppression du lien du sample anciennement lié, le cas échéant
    Valeur val;
    if (idLinkedOld.indexElt > -1)
    {
        val.sfLinkValue = monoSample;
        this->_sf2->set(idLinkedOld, champ_sfSampleType, val);
        val.wValue = 0;
        this->_sf2->set(idLinkedOld, champ_wSampleLink, val);
    }
    // Nouveau sample à lier ?
    if (idLinkedNew.indexElt > -1)
    {
        // Le nouveau sample à lier est-il déjà lié ?
        SFSampleLink typeLinkedNew = _sf2->get(idLinkedNew, champ_sfSampleType).sfLinkValue;
        if (typeLinkedNew != monoSample && typeLinkedNew != RomMonoSample)
        {
            // On délie le sample lié au sample que l'on veut lier
            EltID id2 = id;
            id2.indexElt = this->_sf2->get(idLinkedNew, champ_wSampleLink).wValue;
            val.wValue = 0;
            this->_sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            this->_sf2->set(id2, champ_sfSampleType, val);
        }

        // Lien vers le nouveau sample
        val.wValue = idLinkedNew.indexElt;
        this->_sf2->set(id, champ_wSampleLink, val);

        // Lien du nouveau sample lié
        val.wValue = id.indexElt;
        this->_sf2->set(idLinkedNew, champ_wSampleLink, val);

        // Restauration des types de liens
        bool keepTypeLink = (type == rightSample && typeLinkedNew == leftSample) ||
                (type == RomRightSample && typeLinkedNew == RomLeftSample) ||
                (type == leftSample && typeLinkedNew == rightSample) ||
                (type == RomLeftSample && typeLinkedNew == RomRightSample);
        if (!keepTypeLink)
        {
            val.sfLinkValue = linkedSample;
            this->_sf2->set(id, champ_sfSampleType, val);
            this->_sf2->set(idLinkedNew, champ_sfSampleType, val);

            // Mise à jour combobox
            this->ui->comboType->clear();
            this->ui->comboType->addItem(trUtf8("mono"));
            this->ui->comboType->addItem(trUtf8("droit"));
            this->ui->comboType->addItem(trUtf8("gauche"));
            this->ui->comboType->addItem(trUtf8("lien"));
            this->ui->comboType->setCurrentIndex(3);
        }

        // Ajustement rootKey et tune du sample lié
        val = this->_sf2->get(id, champ_byOriginalPitch);
        this->_sf2->set(idLinkedNew, champ_byOriginalPitch, val);
        val = this->_sf2->get(id, champ_chPitchCorrection);
        this->_sf2->set(idLinkedNew, champ_chPitchCorrection, val);

        this->ui->checkLectureLien->setEnabled(true);
    }
    else
    {
        // Plus de lien
        val.wValue = 0;
        this->_sf2->set(id, champ_wSampleLink, val);
        val.sfLinkValue = monoSample;
        this->_sf2->set(id, champ_sfSampleType, val);
        // Mise à jour combobox
        this->ui->comboType->clear();
        this->ui->comboType->addItem(trUtf8("mono"));
        this->ui->comboLink->setCurrentIndex(0);
        this->ui->checkLectureLien->setEnabled(false);
        this->ui->checkLectureLien->setChecked(false);
    }
    this->_preparation = false;
    this->_mainWindow->updateDo();
}

void Page_Smpl::setRate(int index)
{
    Q_UNUSED(index);
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    quint32 echFinal = ui->comboSampleRate->currentText().toInt();
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
            if (echInit != echFinal)
                setRateElt(id, echFinal);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue)
                {
                    if (echFinal != _sf2->get(id2, champ_dwSampleRate).dwValue)
                        setRateElt(id2, echFinal);
                }
            }
        }
    }

    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::setRateElt(EltID id, quint32 echFinal)
{
    // Modification échantillonnage
    quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
    QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
    baData = Sound::resampleMono(baData, echInit, echFinal, 24);
    _sf2->set(id, champ_sampleDataFull24, baData);
    Valeur val;
    val.dwValue = echFinal;
    _sf2->set(id, champ_dwSampleRate, val);

    // Ajustement de length, startLoop, endLoop
    val.dwValue = baData.size()/3;
    _sf2->set(id, champ_dwLength, val);
    quint32 dwTmp = this->_sf2->get(id, champ_dwStartLoop).dwValue;
    dwTmp = ((quint64)dwTmp * (quint64)echFinal) / (quint64)echInit;
    val.dwValue = dwTmp;
    _sf2->set(id, champ_dwStartLoop, val);
    dwTmp = _sf2->get(id, champ_dwEndLoop).dwValue;
    dwTmp = ((quint64)dwTmp * (quint64)echFinal) / (quint64)echInit;
    val.dwValue = dwTmp;
    _sf2->set(id, champ_dwEndLoop, val);
}

EltID Page_Smpl::getRepercussionID(EltID id)
{
    EltID id2 = id;

    // Recherche du sample associé, le cas échéant et si la répercussion est activée
    SFSampleLink typeLien = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample && Config::getInstance()->getRepercussionStereo())
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        id2.indexElt = -1;
    return id2;
}

// Outils
void Page_Smpl::normalisation()
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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

    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                double db = 0;
                QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
                baData = Sound::normaliser(baData, 0.9, 24, db);
                if (db != 0)
                    this->_sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::enleveBlanc()
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveBlanc(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::enleveBlanc(EltID id)
{
    quint32 pos = 0;
    QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
    baData = Sound::enleveBlanc(baData, 0.001, 24, pos);
    if (pos != 0)
    {
        this->_sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length, startloop, endLoop
        Valeur val;
        val.dwValue = baData.size()/3;
        this->_sf2->set(id, champ_dwLength, val);
        if (this->_sf2->get(id, champ_dwStartLoop).dwValue > pos)
            val.dwValue = this->_sf2->get(id, champ_dwStartLoop).dwValue - pos;
        else
            val.dwValue = 0;
        this->_sf2->set(id, champ_dwStartLoop, val);
        if (this->_sf2->get(id, champ_dwEndLoop).dwValue > pos)
            val.dwValue = this->_sf2->get(id, champ_dwEndLoop).dwValue - pos;
        else
            val.dwValue = 0;
        this->_sf2->set(id, champ_dwEndLoop, val);
    }
}

void Page_Smpl::enleveFin()
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveFin(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::enleveFin(EltID id)
{
    QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
    // Nombre de valeurs à enlever
    qint32 size = baData.size() / 3;
    qint32 endLoop = this->_sf2->get(id, champ_dwEndLoop).dwValue;
    if (endLoop == 0) return;
    if (endLoop < size - 8)
    {
        qint32 pos = size - (8 + endLoop);
        // Rognage
        baData = baData.left(baData.size() - 3 * pos);
        this->_sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length
        Valeur val;
        val.dwValue = baData.size() / 3;
        this->_sf2->set(id, champ_dwLength, val);
    }
}

void Page_Smpl::bouclage()
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    QStringList samplesNotLooped;
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();

                // Récupération des données, échantillonnage, startloop et endloop
                QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
                quint32 dwSmplRate = this->_sf2->get(id, champ_dwSampleRate).dwValue;
                qint32 startLoop = this->_sf2->get(id, champ_dwStartLoop).dwValue;
                qint32 endLoop = this->_sf2->get(id, champ_dwEndLoop).dwValue;

                // Bouclage
                baData = Sound::bouclage(baData, dwSmplRate, startLoop, endLoop, 24);
                if (!baData.isEmpty())
                {
                    // Modification des données, de length, startloop et endloop
                    this->_sf2->set(id, champ_sampleDataFull24, baData);
                    Valeur val;
                    val.dwValue = startLoop;
                    this->_sf2->set(id, champ_dwStartLoop, val);
                    val.dwValue = endLoop;
                    this->_sf2->set(id, champ_dwEndLoop, val);
                    val.dwValue = baData.size() / 3;
                    this->_sf2->set(id, champ_dwLength, val);
                }
                else
                    samplesNotLooped << _sf2->getQstr(id, champ_name);

                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    if (!samplesNotLooped.isEmpty())
    {
        QString txt;
        if (samplesNotLooped.size() == 1)
            txt = trUtf8("L'échantillon « ") + samplesNotLooped.first() + trUtf8(" » n'a pas pu être bouclé.") + "<br/>";
        else
        {
            txt = trUtf8("Les échantillons suivants n'ont pas pu être bouclés :") + "<ul>";
            for (int i = 0; i < samplesNotLooped.size(); i++)
                txt += "<li>" + samplesNotLooped.at(i) + "</li>";
            txt += "</ul>";
        }
        txt += trUtf8("Causes possibles : trop court ou trop turbulent.");
        QMessageBox::warning(this, trUtf8("Attention"), txt);
    }

    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::filtreMur()
{
    if (_preparation) return;
    // Fréquence de filtre
    bool ok;
    Config * conf = Config::getInstance();
    double rep = QInputDialog::getDouble(this, trUtf8("Question"),
                                         trUtf8("Fréquence de coupure :"),
                                         conf->getTools_s_mur_coupure(), 20, 96000, 2, &ok);
    if (!ok) return;
    conf->setTools_s_mur_coupure(rep);
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                // Récupération des données et de l'échantillonnage
                QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
                quint32 dwSmplRate = this->_sf2->get(id, champ_dwSampleRate).dwValue;
                // Filtre passe bas
                baData = Sound::bandFilter(baData, 24, dwSmplRate, rep, 0, -1);
                this->_sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::reglerBalance()
{
    if (_preparation) return;
    this->_sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                // Sample lié ?
                SFSampleLink typeLien = this->_sf2->get(id, champ_sfSampleType).sfLinkValue;
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !this->_sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié ?
            SFSampleLink typeLien = this->_sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                EltID id2 = id;
                id2.indexElt = this->_sf2->get(id, champ_wSampleLink).wValue;

                // Récupération des données des samples
                QByteArray baData1 = this->_sf2->getData(id, champ_sampleDataFull24);
                QByteArray baData2 = this->_sf2->getData(id2, champ_sampleDataFull24);

                // Régimes permanents
                qint32 debut1, debut2, fin1, fin2;
                if (this->_sf2->get(id, champ_dwStartLoop).dwValue == this->_sf2->get(id, champ_dwEndLoop).dwValue)
                    Sound::regimePermanent(baData1, this->_sf2->get(id, champ_dwSampleRate).dwValue, 24, debut1, fin1);
                else
                {
                    debut1 = this->_sf2->get(id, champ_dwStartLoop).dwValue;
                    fin1 = this->_sf2->get(id, champ_dwEndLoop).dwValue;
                }
                if (this->_sf2->get(id2, champ_dwStartLoop).dwValue == this->_sf2->get(id2, champ_dwEndLoop).dwValue)
                {
                    Sound::regimePermanent(baData2, this->_sf2->get(id2, champ_dwSampleRate).dwValue,
                                           24, debut2, fin2);
                }
                else
                {
                    debut2 = this->_sf2->get(id2, champ_dwStartLoop).dwValue;
                    fin2 = this->_sf2->get(id2, champ_dwEndLoop).dwValue;
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
                this->_sf2->set(id, champ_sampleDataFull24, baData1);
                this->_sf2->set(id2, champ_sampleDataFull24, baData2);

                // Avancement barre de progression
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::sifflements()
{
    if (_preparation) return;
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
    this->_sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
                quint32 dwSmplRate = this->_sf2->get(id, champ_dwSampleRate).dwValue;
                baData = Sound::sifflements(baData, dwSmplRate, 24, freq1, freq2, raideur);
                this->_sf2->set(id, champ_sampleDataFull24, baData);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Mise à jour
    this->_mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::transposer()
{
    if (_preparation) return;
    // Ecart en demi-tons
    bool ok;
    Config * conf = Config::getInstance();
    double rep = QInputDialog::getDouble(this, trUtf8("Transposition"),
                                         trUtf8("Écart en demi-tons :"),
                                         conf->getTools_s_transpo_ton(), -36, 36, 2,
                                         &ok);
    if (!ok) return;
    conf->setTools_s_transpo_ton(rep);
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
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
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!this->_sf2->get(id, champ_hidden).bValue)
            {
                QString name = this->_sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                // Récupération des données et de l'échantillonnage
                QByteArray baData = this->_sf2->getData(id, champ_sampleDataFull24);
                quint32 echFinal = this->_sf2->get(id, champ_dwSampleRate).dwValue;
                // Calcul de l'échantillonnage fictif de départ
                double echInit = (double)echFinal * pow(2, rep/12);
                // Rééchantillonnage
                baData = Sound::resampleMono(baData, echInit, echFinal, 24);
                this->_sf2->set(id, champ_sampleDataFull24, baData);
                // Ajustement de length, startLoop, endLoop
                Valeur val;
                val.dwValue = baData.size()/3;
                this->_sf2->set(id, champ_dwLength, val);
                quint32 dwTmp = this->_sf2->get(id, champ_dwStartLoop).dwValue;
                dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
                val.dwValue = dwTmp;
                this->_sf2->set(id, champ_dwStartLoop, val);
                dwTmp = this->_sf2->get(id, champ_dwEndLoop).dwValue;
                dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
                val.dwValue = dwTmp;
                this->_sf2->set(id, champ_dwEndLoop, val);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    this->_mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    this->_mainWindow->updateDo();
    this->afficher();
}

// Egaliseur
void Page_Smpl::applyEQ()
{
    if (_preparation)
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

    _sf2->prepareNewActions();
    QList<EltID> listprocessedID;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!_sf2->get(id, champ_hidden).bValue && !listprocessedID.contains(id))
        {
            listprocessedID << id;
            QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
            baData = Sound::EQ(baData, _sf2->get(id, champ_dwSampleRate).dwValue, 24,
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
            _sf2->set(id, champ_sampleDataFull24, baData);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue && !listprocessedID.contains(id2))
                {
                    listprocessedID << id2;
                    QByteArray baData = _sf2->getData(id2, champ_sampleDataFull24);
                    baData = Sound::EQ(baData, _sf2->get(id2, champ_dwSampleRate).dwValue, 24,
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
                    _sf2->set(id2, champ_sampleDataFull24, baData);
                }
            }
        }
    }

    // Actualisation
    _mainWindow->updateDo();
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
        ui->pushLecture->setText(trUtf8("Arrêt"));
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
        this->_mainWindow->desactiveOutilsSmpl();
        this->lectureEnCours = true;
    }
    else
    {
        ui->pushLecture->setText(trUtf8("Lecture"));
        this->lectureEnCours = false;
        this->noteChanged(-1, 0);
    }
}

void Page_Smpl::lecteurFinished()
{
    if (preventStop)
    {
        preventStop--;
        return;
    }

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
    if (this->_qStackedWidget->currentWidget() == this)
        this->_mainWindow->activeOutilsSmpl();
    if (ui->pushLecture->isChecked())
    {
        ui->pushLecture->blockSignals(true);
        ui->pushLecture->setChecked(false);
        ui->pushLecture->setText(trUtf8("Lecture"));
        ui->pushLecture->blockSignals(false);
    }
    this->lectureEnCours = false;
    this->_mainWindow->activeOutilsSmpl();
}

void Page_Smpl::selectionChanged()
{
    if (this->lectureEnCours)
    {
        this->noteChanged(-1, 0);
        if (this->isVisible())
        {
            ui->pushLecture->setChecked(true);
            this->lecture();
            preventStop++;
        }
    }
}

bool Page_Smpl::isPlaying() { return this->lectureEnCours; }

void Page_Smpl::pushPlayPause()
{
    ui->pushLecture->toggle();
    this->lecture();
}

void Page_Smpl::on_checkLectureBoucle_stateChanged(int arg1)
{
    // Modif synth
    this->_synth->setLoopEnabled(arg1);
}

void Page_Smpl::setSinusEnabled(bool val)
{
    // Modif synth
    this->_synth->setSinusEnabled(val);
}
void Page_Smpl::setGainSample(int val)
{
    // Modif synth
    this->_synth->setGainSample(val);
}
void Page_Smpl::setStereo(bool val)
{
    // Modif synth
    this->_synth->setStereo(val);
}

void Page_Smpl::getPeakFrequencies(EltID id, QList<double> &frequencies, QList<double> &factors,
                                   QList<int> &keys, QList<int> &corrections)
{
    GraphiqueFourier graphTmp(NULL);
    graphTmp.setData(_sf2->getData(id, champ_sampleData16), _sf2->get(id, champ_dwSampleRate).dwValue);
    frequencies.clear();
    factors.clear();
    keys.clear();
    corrections.clear();
    graphTmp.setPos(_sf2->get(id, champ_dwStartLoop).dwValue, _sf2->get(id, champ_dwEndLoop).dwValue,
                    frequencies, factors, keys, corrections, false);
}

void Page_Smpl::on_pushAutoTune_clicked()
{
    _sf2->prepareNewActions();


    int displayedPitch = -1000, displayedCorrection = -1000;
    bool firstValue = true;
    bool triggersMessage = false;

    QList<int> listeSamplesProcessed;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!this->_sf2->get(id, champ_hidden).bValue && !listeSamplesProcessed.contains(id.indexElt))
        {
            listeSamplesProcessed << id.indexElt;

            // Modification pitch / correction
            int pitch, correction;
            autoTune(id, pitch, correction);

            if (firstValue)
            {
                firstValue = false;
                displayedPitch = pitch;
                displayedCorrection = correction;
            }
            else if (displayedPitch != pitch || displayedCorrection != correction)
                triggersMessage = true;

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!this->_sf2->get(id2, champ_hidden).bValue && !listeSamplesProcessed.contains(id2.indexElt))
                {
                    listeSamplesProcessed << id2.indexElt;

                    // Modification pitch / correction
                    autoTune(id2, pitch, correction);
                    if (displayedPitch != pitch || displayedCorrection != correction)
                        triggersMessage = true;
                }
            }
        }
    }

    // Mise à jour interface
    this->_mainWindow->updateDo();
    this->afficher();

    if (triggersMessage)
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Modification appliquée avec succès aux différents échantillons"));
}

void Page_Smpl::autoTune(EltID id, int &pitch, int &correction)
{
    // Récupération des données
    QByteArray baData = this->_sf2->getData(id, champ_sampleData16);
    quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;

    // Remplissage du graphique Fourier
    ui->grapheFourier->setData(baData, sampleRate);
    ui->grapheFourier->setPos(startLoop, endLoop, false);

    // Hauteur de note et correction
    ui->grapheFourier->getEstimation(pitch, correction);

    if (pitch != -1)
    {
        // Modification du pitch et de la correction
        Valeur val;
        val.wValue = pitch;
        _sf2->set(id, champ_byOriginalPitch, val);
        val.wValue = correction;
        _sf2->set(id, champ_chPitchCorrection, val);
    }
    else
    {
        pitch = _sf2->get(id, champ_byOriginalPitch).wValue;
        correction = _sf2->get(id, champ_chPitchCorrection).wValue;
    }
}
