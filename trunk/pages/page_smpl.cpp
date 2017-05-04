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

#include "page_smpl.h"
#include "ui_page_smpl.h"
#include "mainwindow.h"
#include "sound.h"
#include "dialog_command.h"
#include "dialog_filter_frequencies.h"
#include "dialog_change_volume.h"
#include "graphique.h"
#include "graphiquefourier.h"
#include "confmanager.h"
#include "externalcommandrunner.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QProcess>

Page_Smpl::Page_Smpl(QWidget *parent) :
    Page(PAGE_SMPL, parent),
    ui(new Ui::Page_Smpl),
    lectureEnCours(false),
    preventStop(0)
{
    ui->setupUi(this);

    // Initialisation du graphique
    ui->graphe->linkSliderX(ui->sliderGraphe);
    ui->graphe->linkSpinBoxes(ui->spinStartLoop, ui->spinEndLoop);

    // Connexions
    ui->graphe->connect(_synth, SIGNAL(currentPosChanged(int)), SLOT(setCurrentSample(int)));
    this->connect(_synth, SIGNAL(readFinished()), SLOT(lecteurFinished()));
    connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    // Couleur de fond du graphe Fourier
    ui->grapheFourier->setBackgroundColor(this->palette().background().color());

    // Initialize EQ
    loadEQ();

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
        QByteArray baData = _sf2->getData(id, champ_sampleData16);
        ui->graphe->setData(baData, sampleRate); // prend du temps
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
    ui->comboLink->clear();
    ui->comboType->clear();

    // Liens possibles
    if (nombreElements == 1)
    {
        for (int i = 0; i < _sf2->count(id2); i++)
        {
            if (i != id.indexElt)
            {
                id2.indexElt = i;
                if (!_sf2->get(id2, champ_hidden).bValue)
                    ui->comboLink->addItem(_sf2->getQstr(id2, champ_name));
            }
        }
    }
    ui->comboLink->model()->sort(0);
    ui->comboLink->insertItem(0, "-");
    ui->comboLink->setEnabled(nombreElements == 1 && !lectureEnCours);

    // Types possibles et sélections
    ui->comboType->addItem(trUtf8("mono"));
    if (typeLink == monoSample || typeLink == RomMonoSample)
    {
        ui->comboType->setCurrentIndex(0);
        ui->comboLink->setCurrentIndex(0);
        ui->checkLectureLien->setEnabled(false);
        ui->checkLectureLien->setChecked(false);
    }
    else
    {
        ui->comboType->addItem(trUtf8("droit"));
        ui->comboType->addItem(trUtf8("gauche"));
        ui->comboType->addItem(trUtf8("lien"));
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
            id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
            ui->comboLink->setCurrentIndex(ui->comboLink->findText(_sf2->getQstr(id2, champ_name)));
        }
        else
            ui->comboLink->setCurrentIndex(-1);

        ui->checkLectureLien->setEnabled(nombreElements == 1);
    }
    ui->comboType->setEnabled(nombreElements == 1 && !lectureEnCours);

    // Liste des instruments qui utilisent le sample
    if (nombreElements > 1)
        ui->labelInst->setText("-");
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
        for (int i = 0; i < _sf2->count(id2); i++)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // Parcours de tous les samples liés à l'instrument
            isFound = false;
            j = 0;
            while(j < _sf2->count(id3) && !isFound)
            {
                id3.indexElt2 = j;
                if (!_sf2->get(id3, champ_hidden).bValue)
                {
                    if (_sf2->get(id3, champ_sampleID).wValue == id.indexElt)
                    {
                        // Ajout d'un instrument
                        if (nbInst)
                            qStr.append(", ");
                        qStr.append(_sf2->getQstr(id2, champ_name).toStdString().c_str());
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
        ui->labelInst->setText(qStr);
    }

    if (!ui->pushLecture->isChecked())
        ui->pushLecture->setText(trUtf8("Lecture"));

    // Basculement affichage
    _qStackedWidget->setCurrentWidget(this); // prend du temps
    _preparation = false;
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
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_dwStartLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwStartLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue)
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
    _mainWindow->updateDo();
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
        _synth->setStartLoop(val, id2.indexElt != -1);
    }
}

void Page_Smpl::setEndLoop()
{
    if (_preparation)
        return;

    _sf2->prepareNewActions();
    Valeur val;
    val.dwValue = ui->spinEndLoop->value();
    ui->spinStartLoop->setMaximum(val.dwValue);
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
    {
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_dwEndLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwEndLoop, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue)
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
        _synth->setEndLoop(val, id2.indexElt != -1);
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
        if (!_sf2->get(id, champ_hidden).bValue)
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
                if (!_sf2->get(id2, champ_hidden).bValue)
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
    _mainWindow->updateDo();
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
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_byOriginalPitch).bValue != val.bValue)
                _sf2->set(id, champ_byOriginalPitch, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.bValue != _sf2->get(id2, champ_byOriginalPitch).bValue)
                        _sf2->set(id2, champ_byOriginalPitch, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    _mainWindow->updateDo();
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
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            if (_sf2->get(id, champ_chPitchCorrection).cValue != val.cValue)
                _sf2->set(id, champ_chPitchCorrection, val);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue)
                {
                    if (val.cValue != _sf2->get(id2, champ_chPitchCorrection).bValue)
                        _sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }

    // Mise à jour fenêtre
    _mainWindow->updateDo();
}

void Page_Smpl::setTune(int val)
{
    // Modif synth
    if (_tree->getSelectedItemsNumber() == 1)
        _synth->setPitchCorrection(val, getRepercussionID(_tree->getFirstID()).indexElt != -1);
}

void Page_Smpl::setType(int index)
{
    if (_preparation) return;
    EltID id = _tree->getFirstID();
    // Ancien et nouveau types
    SFSampleLink ancienType = _sf2->get(id, champ_sfSampleType).sfLinkValue;
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
    id = _tree->getFirstID();
    _preparation = true;
    // Modification du type du sample
    Valeur val;
    val.sfLinkValue = nouveauType;
    _sf2->set(id, champ_sfSampleType, val);
    if (ancienType != monoSample)
    {
        // Sample lié
        EltID id2 = id;
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
        switch (nouveauType)
        {
        case monoSample:
            // Suppression du lien vers le sample
            val.wValue = 0;
            _sf2->set(id, champ_wSampleLink, val);
            // Suppression du lien du sample anciennement lié
            _sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            _sf2->set(id2, champ_sfSampleType, val);
            // Mise à jour combobox
            ui->comboType->clear();
            ui->comboType->addItem(trUtf8("mono"));
            ui->comboLink->setCurrentIndex(0);
            ui->checkLectureLien->setEnabled(false);
            ui->checkLectureLien->setChecked(false);
            break;
        case rightSample:
            val.sfLinkValue = leftSample;
            _sf2->set(id2, champ_sfSampleType, val);
            ui->checkLectureLien->setEnabled(true);
            break;
        case leftSample:
            val.sfLinkValue = rightSample;
            _sf2->set(id2, champ_sfSampleType, val);
            ui->checkLectureLien->setEnabled(true);
            break;
        case linkedSample:
            val.sfLinkValue = linkedSample;
            _sf2->set(id2, champ_sfSampleType, val);
            ui->checkLectureLien->setEnabled(true);
            break;
        default: break;
        }
    }
    _preparation = false;
    _mainWindow->updateDo();
}

void Page_Smpl::setLinkedSmpl(int index)
{
    if (_preparation) return;
    EltID id = _tree->getFirstID();
    // Ancien et nouveau samples liés
    EltID idLinkedOld = id;
    SFSampleLink type = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (type != monoSample && type != RomMonoSample)
        idLinkedOld.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    EltID idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = _sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!_sf2->get(id2, champ_hidden).bValue)
            {
                if (!_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
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
    id = _tree->getFirstID();
    type = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (type != monoSample && type != RomMonoSample)
        idLinkedOld.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        idLinkedOld.indexElt = -1; // Pas de lien initialement
    idLinkedNew = id;
    idLinkedNew.indexElt = -1;
    if (index > 0)
    {
        // Recherche à partir du nom
        QString qStr = ui->comboLink->itemText(index);
        int pos = 0;
        int nbSmpl = _sf2->count(id);
        EltID id2 = id;
        while (idLinkedNew.indexElt == -1 && pos < nbSmpl)
        {
            id2.indexElt = pos;
            if (!_sf2->get(id2, champ_hidden).bValue)
            {
                if (!_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
                    idLinkedNew.indexElt = pos;
            }
            pos++;
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    _preparation = true;
    // Suppression du lien du sample anciennement lié, le cas échéant
    Valeur val;
    if (idLinkedOld.indexElt > -1)
    {
        val.sfLinkValue = monoSample;
        _sf2->set(idLinkedOld, champ_sfSampleType, val);
        val.wValue = 0;
        _sf2->set(idLinkedOld, champ_wSampleLink, val);
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
            id2.indexElt = _sf2->get(idLinkedNew, champ_wSampleLink).wValue;
            val.wValue = 0;
            _sf2->set(id2, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            _sf2->set(id2, champ_sfSampleType, val);
        }

        // Lien vers le nouveau sample
        val.wValue = idLinkedNew.indexElt;
        _sf2->set(id, champ_wSampleLink, val);

        // Lien du nouveau sample lié
        val.wValue = id.indexElt;
        _sf2->set(idLinkedNew, champ_wSampleLink, val);

        // Restauration des types de liens
        bool keepTypeLink = (type == rightSample && typeLinkedNew == leftSample) ||
                (type == RomRightSample && typeLinkedNew == RomLeftSample) ||
                (type == leftSample && typeLinkedNew == rightSample) ||
                (type == RomLeftSample && typeLinkedNew == RomRightSample);
        if (!keepTypeLink)
        {
            val.sfLinkValue = linkedSample;
            _sf2->set(id, champ_sfSampleType, val);
            _sf2->set(idLinkedNew, champ_sfSampleType, val);

            // Mise à jour combobox
            ui->comboType->clear();
            ui->comboType->addItem(trUtf8("mono"));
            ui->comboType->addItem(trUtf8("droit"));
            ui->comboType->addItem(trUtf8("gauche"));
            ui->comboType->addItem(trUtf8("lien"));
            ui->comboType->setCurrentIndex(3);
        }

        // Ajustement rootKey et tune du sample lié
        val = _sf2->get(id, champ_byOriginalPitch);
        _sf2->set(idLinkedNew, champ_byOriginalPitch, val);
        val = _sf2->get(id, champ_chPitchCorrection);
        _sf2->set(idLinkedNew, champ_chPitchCorrection, val);

        ui->checkLectureLien->setEnabled(true);
    }
    else
    {
        // Plus de lien
        val.wValue = 0;
        _sf2->set(id, champ_wSampleLink, val);
        val.sfLinkValue = monoSample;
        _sf2->set(id, champ_sfSampleType, val);
        // Mise à jour combobox
        ui->comboType->clear();
        ui->comboType->addItem(trUtf8("mono"));
        ui->comboLink->setCurrentIndex(0);
        ui->checkLectureLien->setEnabled(false);
        ui->checkLectureLien->setChecked(false);
    }
    _preparation = false;
    _mainWindow->updateDo();
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
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
            if (echInit != echFinal)
                setRateElt(id, echFinal);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (!_sf2->get(id2, champ_hidden).bValue)
                {
                    if (echFinal != _sf2->get(id2, champ_dwSampleRate).dwValue)
                        setRateElt(id2, echFinal);
                }
            }
        }
    }

    // Actualisation
    _mainWindow->updateDo();
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
    quint32 dwTmp = _sf2->get(id, champ_dwStartLoop).dwValue;
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
    if (typeLien != monoSample && typeLien != RomMonoSample &&
            ConfManager::getInstance()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool())
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        id2.indexElt = -1;
    return id2;
}

// Outils
void Page_Smpl::changeVolume()
{
    QList<EltID> ids = _tree->getAllIDs();
    if (ids.isEmpty())
        return;

    // Create a dialog
    DialogChangeVolume * dial = new DialogChangeVolume(this);
    this->connect(dial, SIGNAL(accepted(int, double)), SLOT(changeVolume(int, double)));
    dial->setAttribute(Qt::WA_DeleteOnClose, true);

    // Display the dialog
    dial->show();
}

void Page_Smpl::changeVolume(int mode, double value)
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
            nbEtapes++;
    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();

    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
        {
            QString name = _sf2->getQstr(id, champ_name);
            progress.setLabelText(textProgress + name);
            QApplication::processEvents();

            QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);

            double db = 0;
            switch (mode)
            {
            case 0: // Add dB
                // Compute the factor
                value = qPow(10, value / 20.0);
                baData = Sound::multiplier(baData, value, 24, db);
                break;
            case 1: // Multiply by a factor
                baData = Sound::multiplier(baData, value, 24, db);
                break;
            case 2: // Normalize
                baData = Sound::normaliser(baData, value / 100, 24, db);
                break;
            }

            _sf2->set(id, champ_sampleDataFull24, baData);

            if (progress.wasCanceled())
                break;
            progress.setValue(progress.value() + 1);
        }
    }

    // Actualisation
    _mainWindow->updateDo();
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
            if (!_sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                QString name = _sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveBlanc(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    _mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::enleveBlanc(EltID id)
{
    quint32 pos = 0;
    QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
    baData = Sound::enleveBlanc(baData, 0.001, 24, pos);
    if (pos != 0)
    {
        _sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length, startloop, endLoop
        Valeur val;
        val.dwValue = baData.size()/3;
        _sf2->set(id, champ_dwLength, val);
        if (_sf2->get(id, champ_dwStartLoop).dwValue > pos)
            val.dwValue = _sf2->get(id, champ_dwStartLoop).dwValue - pos;
        else
            val.dwValue = 0;
        _sf2->set(id, champ_dwStartLoop, val);
        if (_sf2->get(id, champ_dwEndLoop).dwValue > pos)
            val.dwValue = _sf2->get(id, champ_dwEndLoop).dwValue - pos;
        else
            val.dwValue = 0;
        _sf2->set(id, champ_dwEndLoop, val);
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
            if (!_sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                QString name = _sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                this->enleveFin(id);
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    _mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }
    // Actualisation
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::enleveFin(EltID id)
{
    QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
    // Nombre de valeurs à enlever
    qint32 size = baData.size() / 3;
    qint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    if (endLoop == 0) return;
    if (endLoop < size - 8)
    {
        qint32 pos = size - (8 + endLoop);
        // Rognage
        baData = baData.left(baData.size() - 3 * pos);
        _sf2->set(id, champ_sampleDataFull24, baData);
        // Mise à jour length
        Valeur val;
        val.dwValue = baData.size() / 3;
        _sf2->set(id, champ_dwLength, val);
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
            if (!_sf2->get(id, champ_hidden).bValue)
                nbEtapes++;
        }
    }
    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    QStringList samplesNotLooped;
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl)
        {
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                QString name = _sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();

                // Récupération des données, échantillonnage, startloop et endloop
                QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
                quint32 dwSmplRate = _sf2->get(id, champ_dwSampleRate).dwValue;
                qint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
                qint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;

                // Bouclage
                baData = Sound::bouclage(baData, dwSmplRate, startLoop, endLoop, 24);
                if (!baData.isEmpty())
                {
                    // Modification des données, de length, startloop et endloop
                    _sf2->set(id, champ_sampleDataFull24, baData);
                    Valeur val;
                    val.dwValue = startLoop;
                    _sf2->set(id, champ_dwStartLoop, val);
                    val.dwValue = endLoop;
                    _sf2->set(id, champ_dwEndLoop, val);
                    val.dwValue = baData.size() / 3;
                    _sf2->set(id, champ_dwLength, val);
                }
                else
                    samplesNotLooped << _sf2->getQstr(id, champ_name);

                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    _mainWindow->updateDo();
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
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::filter()
{
    QList<EltID> ids = _tree->getAllIDs();
    if (ids.isEmpty())
        return;

    // Create a dialog
    DialogFilterFrequencies * dial = new DialogFilterFrequencies(this);
    this->connect(dial, SIGNAL(accepted(QVector<double>)), SLOT(filter(QVector<double>)));
    dial->setAttribute(Qt::WA_DeleteOnClose, true);

    // Add Fourier transforms
    int nbFourier = qMin(ids.count(), 6);
    dial->setNbFourier(nbFourier);
    for (int i = 0; i < nbFourier; i++)
    {
        EltID id = ids[i];
        quint32 sampleRate = _sf2->get(id, champ_dwSampleRate).dwValue;
        QByteArray baData = _sf2->getData(id, champ_sampleData16);
        dial->addFourierTransform(baData, sampleRate);
    }

    // Display the dialog
    dial->show();
}

void Page_Smpl::filter(QVector<double> dValues)
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Compute number of steps
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
            nbEtapes++;
    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
        {
            QString name = _sf2->getQstr(id, champ_name);
            progress.setLabelText(textProgress + name);
            QApplication::processEvents();

            // Récupération des données et de l'échantillonnage
            QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
            quint32 dwSmplRate = _sf2->get(id, champ_dwSampleRate).dwValue;

            // Filtre passe bas
            baData = Sound::cutFilter(baData, dwSmplRate, dValues, 24, 20000);
            _sf2->set(id, champ_sampleDataFull24, baData);

            if (!progress.wasCanceled())
                progress.setValue(progress.value() + 1);
            else
                break;
        }
    }

    // Actualisation
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::reglerBalance()
{
    if (_preparation) return;
    _sf2->prepareNewActions();

    // Calcul du nombre d'étapes
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    bool monoProcessed = false;
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié ?
            SFSampleLink typeLien = _sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
                nbEtapes++;
            else
                monoProcessed = true;
        }
    }


    if (monoProcessed)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Cet outil ne s'applique que sur des échantillons stéréo."));
        return;
    }

    if (nbEtapes == 0)
        return;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié ?
            SFSampleLink typeLien = _sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
            {
                QString name = _sf2->getQstr(id, champ_name);
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                EltID id2 = id;
                id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;

                // Récupération des données des samples
                QByteArray baData1 = _sf2->getData(id, champ_sampleDataFull24);
                QByteArray baData2 = _sf2->getData(id2, champ_sampleDataFull24);

                // Régimes permanents
                qint32 debut1, debut2, fin1, fin2;
                if (_sf2->get(id, champ_dwStartLoop).dwValue == _sf2->get(id, champ_dwEndLoop).dwValue)
                    Sound::regimePermanent(baData1, _sf2->get(id, champ_dwSampleRate).dwValue, 24, debut1, fin1);
                else
                {
                    debut1 = _sf2->get(id, champ_dwStartLoop).dwValue;
                    fin1 = _sf2->get(id, champ_dwEndLoop).dwValue;
                }
                if (_sf2->get(id2, champ_dwStartLoop).dwValue == _sf2->get(id2, champ_dwEndLoop).dwValue)
                {
                    Sound::regimePermanent(baData2, _sf2->get(id2, champ_dwSampleRate).dwValue,
                                           24, debut2, fin2);
                }
                else
                {
                    debut2 = _sf2->get(id2, champ_dwStartLoop).dwValue;
                    fin2 = _sf2->get(id2, champ_dwEndLoop).dwValue;
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
                _sf2->set(id, champ_sampleDataFull24, baData1);
                _sf2->set(id2, champ_sampleDataFull24, baData2);

                // Avancement barre de progression
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Mise à jour et sortie
                    _mainWindow->updateDo();
                    this->afficher();
                    return;
                }
            }
        }
    }

    // Actualisation
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::transposer()
{
    if (_preparation) return;

    // Shift in semi-tones
    bool ok;
    double rep = QInputDialog::getDouble(
                this, trUtf8("Transposition"),
                trUtf8("Écart en demi-tons :"),
                ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "transposition", "ton", 12.).toDouble(),
                -36, 36, 2, &ok);

    if (!ok) return;
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "transposition", "ton", rep);
    _sf2->prepareNewActions();

    // Compute how many steps
    int nbEtapes = 0;
    QList<EltID> listID = _tree->getAllIDs();
    foreach (EltID id, listID)
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
            nbEtapes++;
    if (nbEtapes == 0)
        return;

    // Open a progress bar
    QString textProgress = trUtf8("Traitement ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowFlags(progress.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    foreach (EltID id, listID)
    {
        if (id.typeElement == elementSmpl && !_sf2->get(id, champ_hidden).bValue)
        {
            QString name = _sf2->getQstr(id, champ_name);
            progress.setLabelText(textProgress + name);
            QApplication::processEvents();

            // Récupération des données et de l'échantillonnage
            QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
            quint32 echFinal = _sf2->get(id, champ_dwSampleRate).dwValue;

            // Calcul de l'échantillonnage fictif de départ
            double echInit = (double)echFinal * pow(2, rep/12);

            // Rééchantillonnage
            baData = Sound::resampleMono(baData, echInit, echFinal, 24);
            _sf2->set(id, champ_sampleDataFull24, baData);

            // Ajustement de length, startLoop, endLoop
            Valeur val;
            val.dwValue = baData.size()/3;
            _sf2->set(id, champ_dwLength, val);
            quint32 dwTmp = _sf2->get(id, champ_dwStartLoop).dwValue;
            dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
            val.dwValue = dwTmp;
            _sf2->set(id, champ_dwStartLoop, val);
            dwTmp = _sf2->get(id, champ_dwEndLoop).dwValue;
            dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
            val.dwValue = dwTmp;
            _sf2->set(id, champ_dwEndLoop, val);

            if (progress.wasCanceled())
                break;
            else
                progress.setValue(progress.value() + 1);
        }
    }

    // Actualisation
    _mainWindow->updateDo();
    this->afficher();
}

void Page_Smpl::command()
{
    if (_preparation) return;
    DialogCommand * dialog = new DialogCommand(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialog, SIGNAL(accepted(QString, bool, bool)), SLOT(command(QString, bool, bool)));
    dialog->show();
}

void Page_Smpl::command(QString command, bool stereo, bool replaceInfo)
{
    // About to store new actions
    _sf2->prepareNewActions();
    QList<EltID> listID = _tree->getAllIDs();
    for (int i = listID.count() - 1; i >= 0; i--)
        if (listID[i].typeElement != elementSmpl || _sf2->get(listID[i], champ_hidden).bValue)
            listID.removeAt(i);

    // Create a command runner and process the samples
    ExternalCommandRunner * commandRunner = new ExternalCommandRunner(_sf2, this);
    connect(commandRunner, SIGNAL(finished()), this, SLOT(commandFinished()));
    commandRunner->run(listID, command, stereo, replaceInfo); // Will destroy the object once all samples are processed
}

void Page_Smpl::commandFinished()
{
    // Update mainwindow
    _mainWindow->updateDo();
    this->afficher();
}

// Egaliseur
void Page_Smpl::applyEQ()
{
    if (_preparation)
        return;
    saveEQ();

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
    saveEQ();
}

// Lecture
void Page_Smpl::lecture()
{
    if (ui->pushLecture->isChecked())
    {
        ui->pushLecture->setText(trUtf8("Arrêt"));
        this->noteChanged(-1, 127);

        // Désactivations
        ui->comboLink->setEnabled(false);
        ui->comboType->setEnabled(false);
        ui->comboSampleRate->setEnabled(false);
        ui->verticalSlider_1->setEnabled(false);
        ui->verticalSlider_2->setEnabled(false);
        ui->verticalSlider_3->setEnabled(false);
        ui->verticalSlider_4->setEnabled(false);
        ui->verticalSlider_5->setEnabled(false);
        ui->verticalSlider_6->setEnabled(false);
        ui->verticalSlider_7->setEnabled(false);
        ui->verticalSlider_8->setEnabled(false);
        ui->verticalSlider_9->setEnabled(false);
        ui->verticalSlider_10->setEnabled(false);
        ui->pushEgaliser->setEnabled(false);
        ui->pushEgalRestore->setEnabled(false);

        // Désactivation outils sample
        _mainWindow->desactiveOutilsSmpl();
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

    ui->graphe->setCurrentSample(0);
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
    if (_qStackedWidget->currentWidget() == this)
        _mainWindow->activeOutilsSmpl();
    if (ui->pushLecture->isChecked())
    {
        ui->pushLecture->blockSignals(true);
        ui->pushLecture->setChecked(false);
        ui->pushLecture->setText(trUtf8("Lecture"));
        ui->pushLecture->blockSignals(false);
    }
    this->lectureEnCours = false;
    _mainWindow->activeOutilsSmpl();
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
    _synth->setLoopEnabled(arg1);
}

void Page_Smpl::setSinusEnabled(bool val)
{
    // Modif synth
    _synth->setSinusEnabled(val);
}
void Page_Smpl::setGainSample(int val)
{
    // Modif synth
    _synth->setGainSample(val);
}
void Page_Smpl::setStereo(bool val)
{
    // Modif synth
    _synth->setStereo(val);
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
        if (!_sf2->get(id, champ_hidden).bValue && !listeSamplesProcessed.contains(id.indexElt))
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
                if (!_sf2->get(id2, champ_hidden).bValue && !listeSamplesProcessed.contains(id2.indexElt))
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
    _mainWindow->updateDo();
    this->afficher();

    if (triggersMessage)
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Modification appliquée avec succès aux différents échantillons"));
}

void Page_Smpl::autoTune(EltID id, int &pitch, int &correction)
{
    // Récupération des données
    QByteArray baData = _sf2->getData(id, champ_sampleData16);
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

void Page_Smpl::saveEQ()
{
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "01", ui->verticalSlider_1->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "02", ui->verticalSlider_2->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "03", ui->verticalSlider_3->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "04", ui->verticalSlider_4->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "05", ui->verticalSlider_5->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "06", ui->verticalSlider_6->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "07", ui->verticalSlider_7->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "08", ui->verticalSlider_8->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "09", ui->verticalSlider_9->value());
    ConfManager::getInstance()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "10", ui->verticalSlider_10->value());
}

void Page_Smpl::loadEQ()
{
    ui->verticalSlider_1->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "01", 0).toInt());
    ui->verticalSlider_2->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "02", 0).toInt());
    ui->verticalSlider_3->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "03", 0).toInt());
    ui->verticalSlider_4->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "04", 0).toInt());
    ui->verticalSlider_5->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "05", 0).toInt());
    ui->verticalSlider_6->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "06", 0).toInt());
    ui->verticalSlider_7->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "07", 0).toInt());
    ui->verticalSlider_8->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "08", 0).toInt());
    ui->verticalSlider_9->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "09", 0).toInt());
    ui->verticalSlider_10->setValue(ConfManager::getInstance()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "10", 0).toInt());
}
