/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pagesmpl.h"
#include "ui_pagesmpl.h"
#include "sound.h"
#include "graphique.h"
#include "graphiquefourier.h"
#include "sampleutils.h"
#include "contextmanager.h"
#include "pianokeybdcustom.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QProcess>

PageSmpl::PageSmpl(QWidget *parent) :
    Page(parent, PAGE_SMPL, "page:smpl"),
    ui(new Ui::PageSmpl),
    _playingSmpl(false),
    preventStop(0)
{
    ui->setupUi(this);

    // Style
    QString resetHoverColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                                   "QPushButton{background-color:transparent;color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                                   ";border:0;padding:0px 5px}" +
                                   "QPushButton:hover{color:" + resetHoverColor + "}");
    ui->frameGraph->setStyleSheet("QFrame{border:0; border-bottom: 1px solid " + this->palette().dark().color().name() + "}");
    ui->framePlayArea->setStyleSheet(".QFrame{background-color: " +
                                     ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                     ";color: " + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() +
                                     ";border: 1px solid " + this->palette().dark().color().name() + ";border-radius: 3px;}");

    // Initialisation du graphique
    ui->graphe->linkSliderX(ui->sliderGraphe);
    ui->graphe->linkSpinBoxes(ui->spinStartLoop, ui->spinEndLoop);

    // Connections
    ui->graphe->connect(_synth, SIGNAL(currentPosChanged(quint32)), SLOT(setCurrentSample(quint32)));
    this->connect(_synth, SIGNAL(readFinished()), SLOT(lecteurFinished()));
    connect(ui->widgetLinkedTo, SIGNAL(itemClicked(EltID)), this, SLOT(onLinkClicked(EltID)));
    connect(ui->graphe, SIGNAL(cutOrdered(int,int)), this, SLOT(onCutOrdered(int,int)));

    // Couleur de fond du graphe Fourier
    ui->grapheFourier->setBackgroundColor(this->palette().background().color());

    // Adaptation petits écrans
    if (QApplication::desktop()->width() <= 800)
    {
        QTabWidget * tabWidget = new QTabWidget(this);
        QFont fontBold = tabWidget->font();
        fontBold.setBold(true);
        tabWidget->setFont(fontBold);
        fontBold.setBold(false);
        tabWidget->addTab(ui->frameLeft, trUtf8("Information"));
        tabWidget->addTab(ui->frameRight, trUtf8("Equalizer (±15 dB)"));
        delete ui->label_20;
        delete ui->label_21;
        QGridLayout * layout = dynamic_cast<QGridLayout *>(ui->frame_5->layout());
        layout->addWidget(tabWidget, 0, 0, 0, 1);
    }
}

PageSmpl::~PageSmpl()
{
    delete ui;
}

bool PageSmpl::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    Q_UNUSED(displayOption)

    if (editingSource == "page:smpl")
        return true;

    // Check the selection
    if (!selectedIds.isElementUnique(elementSf2))
        return false;
    _currentIds = selectedIds;
    IdList ids = _currentIds.getSelectedIds(elementSmpl);
    ui->widgetEqualizer->setCurrentIds(ids);
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
            rootKey = 0;
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

    // Remplissage des informations
    while (ui->comboSampleRate->count() > 7)
        ui->comboSampleRate->removeItem(7);
    int sampleRateIndex = ui->comboSampleRate->findText(QString::number(sampleRate));
    if (sampleRateIndex == -1) {
        ui->comboSampleRate->addItem(QString::number(sampleRate));
        sampleRateIndex = 7;
    }
    ui->comboSampleRate->setCurrentIndex(sampleRateIndex);
    if (nombreElements == 1)
        ui->labelTaille->setText(QString::number(length) + " - " +
                                 QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds"));
    else
    {
        if (sampleRate > 0)
            ui->labelTaille->setText(trUtf8("(min)", "minimum") + " " + QString::number(length) + " - " +
                                     QString::number((double)length / sampleRate, 'f', 3) + trUtf8("s", "unit for seconds"));
        else
            ui->labelTaille->setText(trUtf8("(min)", "minimum") + " " + QString::number(length));
    }
    ui->spinStartLoop->blockSignals(true);
    ui->spinEndLoop->blockSignals(true);
    ui->spinStartLoop->setMaximum(endLoop);
    ui->spinEndLoop->setMaximum(length - 1);
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
        foreach (int i, _sf2->getSiblings(id2))
        {
            if (i != id.indexElt)
            {
                id2.indexElt = i;
                ui->comboLink->addItem(_sf2->getQstr(id2, champ_name));
            }
        }
    }
    ui->comboLink->model()->sort(0);
    ui->comboLink->insertItem(0, "-");
    ui->comboLink->setEnabled(nombreElements == 1 && !_playingSmpl);

    // Types possibles et sélections
    ui->comboType->addItem(trUtf8("mono", "opposite to stereo"));
    if (typeLink == monoSample || typeLink == RomMonoSample)
    {
        ui->comboType->setCurrentIndex(0);
        ui->comboLink->setCurrentIndex(0);
        ui->checkLectureLien->setEnabled(false);
        ui->checkLectureLien->setChecked(false);
    }
    else
    {
        ui->comboType->addItem(trUtf8("right"));
        ui->comboType->addItem(trUtf8("left"));
        ui->comboType->addItem(trUtf8("link"));
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
    ui->comboType->setEnabled(nombreElements == 1 && !_playingSmpl);

    // Instruments that use the sample
    if (nombreElements > 1)
    {
        ui->widgetLinkedTo->clear();
        ui->labelLinkedTo->setText("-");
    }
    else
    {
        ui->widgetLinkedTo->initialize(id);
        int nbInst = ui->widgetLinkedTo->getLinkNumber();
        if (nbInst == 0)
            ui->labelLinkedTo->setText(trUtf8("Sample not linked to an instrument yet."));
        else if (nbInst == 1)
            ui->labelLinkedTo->setText(trUtf8("Sample linked to instrument:"));
        else
            ui->labelLinkedTo->setText(trUtf8("Sample linked to instruments:"));
    }

    // Reprise de la lecture
    if (this->_playingSmpl)
    {
        ui->pushLecture->setChecked(true);
        this->lecture();
        preventStop++;
    }
    updatePlayButton();

    return true;
}

void PageSmpl::setStartLoop()
{
    if (_preparingPage)
        return;

    // Soundfont editing
    AttributeValue val;
    val.dwValue = ui->spinStartLoop->value();
    ui->spinEndLoop->setMinimum(val.dwValue);
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
        {
            if (_sf2->get(id, champ_dwStartLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwStartLoop, val);

            // Linked sample?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    if (val.dwValue != _sf2->get(id2, champ_dwStartLoop).dwValue &&
                            val.dwValue <= _sf2->get(id2, champ_dwLength).dwValue)
                        _sf2->set(id2, champ_dwStartLoop, val);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource());

    // Update the page
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(_currentIds.count() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    if (_currentIds.count() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}

void PageSmpl::setStartLoop(int val)
{
    if (_currentIds.count() == 1)
    {
        EltID id2 = getRepercussionID(_currentIds.getFirstId(elementSmpl));
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinStartLoop->value() > _sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        _synth->setStartLoop(val, id2.indexElt != -1);
    }
}

void PageSmpl::setEndLoop()
{
    if (_preparingPage)
        return;

    // Soundfont editing
    AttributeValue val;
    val.dwValue = ui->spinEndLoop->value();
    ui->spinStartLoop->setMaximum(val.dwValue);
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
        {
            if (_sf2->get(id, champ_dwEndLoop).dwValue != val.dwValue)
                _sf2->set(id, champ_dwEndLoop, val);

            // Linked sample?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    if (val.dwValue != _sf2->get(id2, champ_dwEndLoop).dwValue &&
                            val.dwValue <= _sf2->get(id2, champ_dwLength).dwValue)
                        _sf2->set(id2, champ_dwEndLoop, val);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource());

    // Update the page
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(_currentIds.count() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }
    if (_currentIds.count() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
}

void PageSmpl::setEndLoop(int val)
{
    if (_currentIds.count() == 1)
    {
        EltID id2 = getRepercussionID(_currentIds.getFirstId(elementSmpl));
        if (id2.indexElt != -1)
        {
            if ((unsigned)ui->spinEndLoop->value() >= _sf2->get(id2, champ_dwLength).dwValue)
                id2.indexElt = -1;
        }

        // Modif synth
        _synth->setEndLoop(val, id2.indexElt != -1);
    }
}

void PageSmpl::on_pushFullLength_clicked()
{
    // Soundfont editing
    AttributeValue val;

    quint32 displayedEndLoop = 0;
    bool firstValue = true;
    bool triggersMessage = false;

    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
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

            // Linked sample associé?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    // Start loop
                    val.dwValue = 0;
                    if (_sf2->get(id2, champ_dwStartLoop).dwValue != 0)
                        _sf2->set(id2, champ_dwStartLoop, val);

                    // End loop is the end of the sample
                    length = _sf2->get(id2, champ_dwLength).dwValue - 1;
                    val.dwValue = length;
                    if (_sf2->get(id2, champ_dwEndLoop).dwValue != length)
                        _sf2->set(id2, champ_dwEndLoop, val);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource());

    // Update the page
    ui->spinStartLoop->setValue(0);
    ui->spinStartLoop->setMaximum(displayedEndLoop);
    ui->spinEndLoop->setValue(displayedEndLoop);
    ui->spinEndLoop->setMinimum(0);
    if (ui->spinEndLoop->value() == ui->spinStartLoop->value())
    {
        ui->checkLectureBoucle->setEnabled(false);
        ui->checkLectureBoucle->setChecked(false);
    }
    else
    {
        ui->checkLectureBoucle->setEnabled(_currentIds.count() == 1);
        ui->checkLectureBoucle->setChecked(true);
    }

    if (_currentIds.count() == 1)
        ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());

    if (triggersMessage)
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Change successfully applied to the different samples"));
}

void PageSmpl::setRootKey()
{
    if (_preparingPage)
        return;

    // Soundfont editing
    AttributeValue val;
    val.bValue = ui->spinRootKey->value();
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
        {
            if (_sf2->get(id, champ_byOriginalPitch).bValue != val.bValue)
                _sf2->set(id, champ_byOriginalPitch, val);

            // Linked sample?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    if (val.bValue != _sf2->get(id2, champ_byOriginalPitch).bValue)
                        _sf2->set(id2, champ_byOriginalPitch, val);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource());
}

void PageSmpl::setRootKey(int val)
{
    ContextManager::midi()->keyboard()->clearCustomization();
    ContextManager::midi()->keyboard()->addRangeAndRootKey(val, 0, 127);

    // Modif synth
    updateSinus();
}

void PageSmpl::setTune()
{
    if (_preparingPage)
        return;

    // Soundfont editing
    AttributeValue val;
    val.cValue = ui->spinTune->value();
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
        {
            if (_sf2->get(id, champ_chPitchCorrection).cValue != val.cValue)
                _sf2->set(id, champ_chPitchCorrection, val);

            // Linked sample?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    if (val.cValue != _sf2->get(id2, champ_chPitchCorrection).bValue)
                        _sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource());
}

void PageSmpl::setTune(int val)
{
    // Modif synth
    if (_currentIds.count() == 1)
        _synth->setPitchCorrection(val, getRepercussionID(_currentIds.getFirstId(elementSmpl)).indexElt != -1);
}

void PageSmpl::setType(int index)
{
    if (_preparingPage)
        return;

    EltID id = _currentIds.getFirstId(elementSmpl);

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
    if (ancienType == nouveauType)
        return;

    _preparingPage = true;
    // Modification du type du sample
    AttributeValue val;
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
            ui->comboType->addItem(trUtf8("mono", "opposite to stereo"));
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
    _preparingPage = false;

    _sf2->endEditing(getEditingSource());
}

void PageSmpl::setLinkedSmpl(int index)
{
    if (_preparingPage)
        return;

    EltID id = _currentIds.getFirstId(elementSmpl);

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
        EltID id2 = id;
        foreach (int pos, _sf2->getSiblings(id))
        {
            id2.indexElt = pos;
            if (!_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
            {
                idLinkedNew.indexElt = pos;
                break;
            }
        }
        if (idLinkedNew.indexElt < 0)
            return;
    }

    // Ancien et nouveau sont égaux ?
    if (idLinkedNew.indexElt == idLinkedOld.indexElt)
        return;

    // Soundfont editing
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
        EltID id2 = id;
        foreach (int pos, _sf2->getSiblings(id))
        {
            id2.indexElt = pos;
            if (!_sf2->getQstr(id2, champ_name).compare(qStr) && pos != id.indexElt)
            {
                idLinkedNew.indexElt = pos;
                break;
            }
        }
        if (idLinkedNew.indexElt < 0) return;
    }
    _preparingPage = true;

    // Suppression du lien du sample anciennement lié, le cas échéant
    AttributeValue val;
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
            ui->comboType->addItem(trUtf8("right"));
            ui->comboType->addItem(trUtf8("left"));
            ui->comboType->addItem(trUtf8("link"));
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
    _preparingPage = false;

    _sf2->endEditing(getEditingSource());
}

void PageSmpl::setRate(int index)
{
    Q_UNUSED(index);
    if (_preparingPage)
        return;

    // Soundfont editing
    quint32 echFinal = ui->comboSampleRate->currentText().toInt();
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id))
        {
            quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
            if (echInit != echFinal)
                setRateElt(id, echFinal);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (_sf2->isValid(id2))
                {
                    if (echFinal != _sf2->get(id2, champ_dwSampleRate).dwValue)
                        setRateElt(id2, echFinal);
                }
            }
        }
    }
    _sf2->endEditing(getEditingSource() + ":update");
}

void PageSmpl::setRateElt(EltID id, quint32 echFinal)
{
    // Modification échantillonnage
    quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
    QByteArray baData = _sf2->getData(id, champ_sampleDataFull24);
    baData = SampleUtils::resampleMono(baData, echInit, echFinal, 24);
    _sf2->set(id, champ_sampleDataFull24, baData);
    AttributeValue val;
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

EltID PageSmpl::getRepercussionID(EltID id)
{
    EltID id2 = id;

    // Recherche du sample associé, le cas échéant et si la répercussion est activée
    SFSampleLink typeLien = _sf2->get(id, champ_sfSampleType).sfLinkValue;
    if (typeLien != monoSample && typeLien != RomMonoSample &&
            ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool())
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        id2.indexElt = -1;

    return id2;
}

void PageSmpl::lecture()
{
    if (ui->pushLecture->isChecked())
    {
        QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
        if (listID.count() == 1)
        {
            _synth->activateSmplEq(ui->widgetEqualizer->isPreviewEnabled());
            _synth->play(listID[0], -1, 127);
        }

        // Désactivations
        ui->comboLink->setEnabled(false);
        ui->comboType->setEnabled(false);
        ui->comboSampleRate->setEnabled(false);
        ui->widgetEqualizer->enableApply(false);

        this->_playingSmpl = true;
    }
    else
    {
        this->_playingSmpl = false;
        _synth->play(EltID(), -1, 0);
    }

    updatePlayButton();
    updateSinus();
}

void PageSmpl::lecteurFinished()
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
    ui->widgetEqualizer->enableApply(true);

    if (ui->pushLecture->isChecked())
    {
        ui->pushLecture->blockSignals(true);
        ui->pushLecture->setChecked(false);
        ui->pushLecture->blockSignals(false);
        updateSinus();
        _synth->activateSmplEq(false);
    }
    _playingSmpl = false;
    updatePlayButton();
}

void PageSmpl::on_checkLectureBoucle_stateChanged(int arg1)
{
    // Modif synth
    _synth->setLoopEnabled(arg1);
}

void PageSmpl::setSinusEnabled(bool val)
{
    Q_UNUSED(val)
    updateSinus();
}

void PageSmpl::setGainSample(int val)
{
    // Modif synth
    _synth->setGainSample(val);
}

void PageSmpl::updateSinus()
{
     _synth->setSinus(_currentIds.count() == 1 && ui->pushLecture->isChecked() && ui->checkLectureSinus->isChecked(),
                      ui->spinRootKey->value());
}

void PageSmpl::setStereo(bool val)
{
    // Modif synth
    _synth->setStereo(val);
}

void PageSmpl::on_pushAutoTune_clicked()
{
    // Soundfont editing

    int displayedPitch = -1000, displayedCorrection = -1000;
    bool firstValue = true;
    bool triggersMessage = false;

    QList<int> listeSamplesProcessed;
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);;
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id) && !listeSamplesProcessed.contains(id.indexElt))
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
                if (_sf2->isValid(id2) && !listeSamplesProcessed.contains(id2.indexElt))
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
    _sf2->endEditing(getEditingSource() + ":update");

    if (triggersMessage)
        QMessageBox::information(this, trUtf8("Information"),
                                 trUtf8("Change successfully applied to the different samples"));
}

void PageSmpl::autoTune(EltID id, int &pitch, int &correction)
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
        AttributeValue val;
        val.wValue = pitch;
        _sf2->set(id, champ_byOriginalPitch, val);
        val.wValue = correction;
        _sf2->set(id, champ_chPitchCorrection, val);
    }
    else
    {
        pitch = _sf2->get(id, champ_byOriginalPitch).bValue;
        correction = _sf2->get(id, champ_chPitchCorrection).cValue;
    }
}

void PageSmpl::onLinkClicked(EltID id)
{
    emit(selectedIdsChanged(id));
}

void PageSmpl::keyPlayedInternal(int key, int velocity)
{
    IdList ids = _currentIds.getSelectedIds(elementSmpl);
    if (ids.count() == 1)
        ContextManager::audio()->getSynth()->play(ids[0], key, velocity);
}

void PageSmpl::onSampleOnOff()
{
    ui->pushLecture->toggle();
    this->lecture();
}

void PageSmpl::onShow()
{
    // Initialize keyboard
    QList<EltID> ids = _currentIds.getSelectedIds(elementSmpl);
    if (ids.count() == 1)
    {
        int rootKey = _sf2->get(ids.at(0), champ_byOriginalPitch).bValue;
        ContextManager::midi()->keyboard()->addRangeAndRootKey(rootKey, 0, 127);
    }
    else
        ContextManager::midi()->keyboard()->addRangeAndRootKey(-1, 0, 127);
}

void PageSmpl::updatePlayButton()
{
    if (_playingSmpl)
    {
        ui->pushLecture->setToolTip(trUtf8("Stop"));
        ui->pushLecture->setIcon(ContextManager::theme()->getColoredSvg(
                                     ":/icons/play.svg", QSize(36, 36), ThemeManager::HIGHLIGHTED_BACKGROUND));
    }
    else
    {
        ui->pushLecture->setToolTip(trUtf8("Play"));
        ui->pushLecture->setIcon(ContextManager::theme()->getColoredSvg(
                                     ":/icons/play.svg", QSize(36, 36), ThemeManager::WINDOW_TEXT));
    }
}

void PageSmpl::onCutOrdered(int start, int end)
{
    if (_currentIds.count() != 1 || start < 0 || end <= 0 || start >= end)
        return;

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(trUtf8("Warning"));
    msgBox.setText(trUtf8("Are you sure to cut the sample from <b>%0</b> to <b>%1</b>?").arg(start).arg(end));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(trUtf8("&Yes"));
    msgBox.button(QMessageBox::No)->setText(trUtf8("&No"));
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::No)
        return;

    // Current sample id, with possibly its stereo sample id
    EltID id = _currentIds[0];
    EltID id2 = getRepercussionID(id);
    if (id2.indexElt != -1)
    {
        // Both samples must have the same length
        if (_sf2->get(id, champ_dwLength).dwValue != _sf2->get(id2, champ_dwLength).dwValue)
            id2.indexElt = -1;
    }

    // Cut between start and end
    bool updateNeeded = cutSample(id, static_cast<quint32>(start), static_cast<quint32>(end));
    if (id2.indexElt != -1)
        updateNeeded |= cutSample(id2, static_cast<quint32>(start), static_cast<quint32>(end));

    if (updateNeeded)
        _sf2->endEditing(getEditingSource() + ":update");
}

bool PageSmpl::cutSample(EltID id, quint32 start, quint32 end)
{
    if (end > _sf2->get(id, champ_dwLength).dwValue)
        return false;

    quint16 wBps = _sf2->get(id, champ_wBpsInit).wValue;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;
    QByteArray baData = _sf2->getData(id, wBps == 24 ? champ_sampleDataFull24 : champ_sampleData16);

    // Possibly adapt start and end of loop
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    AttributeValue val;
    if (startLoop > end)
    {
        val.dwValue = startLoop - end + start;
        _sf2->set(id, champ_dwStartLoop, val);
    }
    else if (startLoop > start)
    {
        val.dwValue = 0;
        _sf2->set(id, champ_dwStartLoop, val);
    }
    if (endLoop > end)
    {
        val.dwValue = endLoop - end + start;
        _sf2->set(id, champ_dwEndLoop, val);
    }
    else if (endLoop > start)
    {
        val.dwValue = 0;
        _sf2->set(id, champ_dwStartLoop, val);
        _sf2->set(id, champ_dwEndLoop, val);
    }

    // Remove data and update the sample
    baData = baData.left(start * wBps / 8) + baData.right(baData.size() - static_cast<int>(end * wBps / 8));
    _sf2->set(id, wBps == 24 ? champ_sampleDataFull24 : champ_sampleData16, baData);
    val.dwValue = static_cast<quint32>(8 * baData.size()) / wBps;
    _sf2->set(id, champ_dwLength, val);

    return true;
}
