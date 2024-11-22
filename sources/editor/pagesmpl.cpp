/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pagesmpl.h"
#include "ui_pagesmpl.h"
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>
#include <QProcess>
#include <QScreen>
#include "sound.h"
#include "graphicsfourier.h"
#include "sampleutils.h"
#include "contextmanager.h"

PageSmpl::PageSmpl(QWidget *parent) :
    Page(parent, "page:smpl"),
    ui(new Ui::PageSmpl),
    _currentPlayingToken(0)
{
    ui->setupUi(this);

    // Style
    ui->frameGraph->setStyleSheet("QFrame{border:0; border-bottom: 1px solid " + ContextManager::theme()->getColor(ThemeManager::BORDER).name() + "}");
    ui->framePlayArea->setStyleSheet("TransparentFrame{background-color: " +
                                     ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                     ";color: " + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() +
                                     ";border: 1px solid " + ContextManager::theme()->getColor(ThemeManager::BORDER).name() + ";border-radius: 3px;}");

    // Icons
    ui->pushFullLength->setIcon(ContextManager::theme()->getColoredSvg(":/icons/range.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));
    ui->pushAutoTune->setIcon(ContextManager::theme()->getColoredSvg(":/icons/left.svg", QSize(14, 14), ThemeManager::BUTTON_TEXT));

    // Initialize spin box for the key names
    ui->spinRootKey->setAlwaysShowKeyName(true);

    // Initialisation du graphique
    ui->waveDisplay->linkSliderX(ui->sliderGraphe);
    ui->waveDisplay->linkSpinBoxes(ui->spinStartLoop, ui->spinEndLoop);

    // Connections
    ui->waveDisplay->connect(_synth, SIGNAL(currentPosChanged(quint32)), SLOT(setCurrentSample(quint32)));
    this->connect(_synth, SIGNAL(readFinished(int)), SLOT(lecteurFinished(int)));
    connect(ui->waveDisplay, SIGNAL(cutOrdered(int,int)), this, SLOT(onCutOrdered(int,int)));

    // Play area over the wave display
    ui->checkLectureLien->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "stereo_playback", false).toBool());
    _synth->setStereo(ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "stereo_playback", false).toBool());
    ui->framePlayArea->setParent(ui->waveDisplay);
    ui->framePlayArea->move(5, 5);

    // Adapt the interface for small screens
    if (QGuiApplication::primaryScreen()->geometry().width() <= 800)
    {
        // Add a tabwidget in an existing layout
        QTabWidget * tabWidget = new QTabWidget(ui->frame_5);
        QGridLayout * layout = dynamic_cast<QGridLayout *>(ui->frame_5->layout());
        layout->addWidget(tabWidget, 0, 0, 0, 1);
        tabWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        tabWidget->tabBar()->setMinimumWidth(240);

        // Populate the tab widget
        QFont fontBold = tabWidget->font();
        fontBold.setBold(true);
        tabWidget->setFont(fontBold);
        fontBold.setBold(false);
        tabWidget->addTab(ui->frameLeft, ui->label_20->text());
        ui->frameLeft->setFont(fontBold);
        tabWidget->addTab(ui->frameRight, ui->label_21->text());
        ui->frameRight->setFont(fontBold);
        ui->frameRight->setContentsMargins(0, 6, 0, 0);

        // Remove labels
        delete ui->label_20;
        delete ui->label_21;
    }
}

PageSmpl::~PageSmpl()
{
    delete ui;
}

void PageSmpl::updateInterface(QString editingSource)
{
    if (editingSource == "page:smpl" || editingSource == "tree:remove") // tree:remove comes with a new selection
        return;

    IdList ids = _currentIds.getSelectedIds(elementSmpl);
    int nombreElements = ids.size();

    EltID id = ids.first();
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
    for (int i = 1; i < ids.count(); i++)
    {
        EltID idTmp = ids[i];
        if (sampleRate != _sf2->get(idTmp, champ_dwSampleRate).dwValue)
            sampleRate = 0;
        if (rootKey != _sf2->get(idTmp, champ_byOriginalPitch).bValue)
            rootKey = 0;
        if (correction != _sf2->get(idTmp, champ_chPitchCorrection).cValue)
            correction = 0;
        if (startLoop != static_cast<int>(_sf2->get(idTmp, champ_dwStartLoop).dwValue))
            startLoop = 0;
        if (endLoop != static_cast<int>(_sf2->get(idTmp, champ_dwEndLoop).dwValue))
            endLoop = 0;
        length = qMin(length, _sf2->get(idTmp, champ_dwLength).dwValue);
        if (typeLink != _sf2->get(idTmp, champ_sfSampleType).sfLinkValue)
            typeLink = linkInvalid;
    }

    // Remplissage des informations
    while (ui->comboSampleRate->count() > 7)
        ui->comboSampleRate->removeItem(7);
    int sampleRateIndex = ui->comboSampleRate->findText(QString::number(sampleRate));
    if (sampleRateIndex == -1)
    {
        if (sampleRate == 0)
            ui->comboSampleRate->addItem("-");
        else
            ui->comboSampleRate->addItem(QString::number(sampleRate));
        sampleRateIndex = 7;
    }
    ui->comboSampleRate->setCurrentIndex(sampleRateIndex);
    if (nombreElements == 1)
        ui->labelTaille->setText(QString::number(length) + " - " +
                                 QLocale::system().toString((double)length / sampleRate, 'f', 3) + " " + tr("s", "unit for seconds"));
    else
    {
        if (sampleRate > 0)
            ui->labelTaille->setText(tr("(min)", "minimum") + " " + QString::number(length) + " - " +
                                     QLocale::system().toString((double)length / sampleRate, 'f', 3) + " " + tr("s", "unit for seconds"));
        else
            ui->labelTaille->setText(tr("(min)", "minimum") + " " + QString::number(length));
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

    // Loop quality
    updateLoopQuality();

    // Graphics
    ui->grapheFourier->setSampleName(_sf2->getQstr(id, champ_name));
    ui->grapheFourier->setCurrentIds(_currentIds);
    ui->waveDisplay->setCurrentSample(0);
    if (nombreElements > 1)
    {
        ui->waveDisplay->displayMultipleSelection(true);
        ui->pushAutoTune->setEnabled(true);
    }
    else
    {
        ui->waveDisplay->displayMultipleSelection(false);
        QVector<float> baData = _sf2->getData(id);
        ui->waveDisplay->setData(baData, sampleRate); // prend du temps
        ui->waveDisplay->setStartLoop(startLoop, false);
        ui->waveDisplay->setEndLoop(endLoop, false);
        ui->waveDisplay->repaint();

        // Loop points in the Fourier graph
        ui->grapheFourier->setPos(startLoop, endLoop);

        int pitch, correction;
        float score;
        ui->grapheFourier->getEstimation(pitch, correction, score);
        ui->pushAutoTune->setEnabled(pitch > 0);
    }

    // Lecteur
    if (nombreElements == 1)
    {
        ui->checkLectureBoucle->setEnabled(ui->spinStartLoop->value() != ui->spinEndLoop->value());
        ui->checkLectureBoucle->setChecked(qAbs(ui->spinEndLoop->value() - ui->spinStartLoop->value()) > 10);
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
    ui->comboLink->setEnabled(nombreElements == 1 && !ui->pushLecture->isChecked());

    // Types possibles et sélections
    ui->comboType->addItem(tr("mono", "opposite to stereo"));
    if (typeLink == monoSample || typeLink == RomMonoSample)
    {
        ui->comboType->setCurrentIndex(0);
        ui->comboLink->setCurrentIndex(0);
        ui->checkLectureLien->setEnabled(false);
        ui->checkLectureLien->setChecked(false);
    }
    else
    {
        ui->comboType->addItem(tr("right"));
        ui->comboType->addItem(tr("left"));
        ui->comboType->addItem(tr("link"));
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
    ui->comboType->setEnabled(nombreElements == 1 && !ui->pushLecture->isChecked());

    // Reprise de la lecture
    _currentPlayingToken = 0;
    ui->checkLectureLien->blockSignals(true);
    ui->checkLectureLien->setChecked(_synth->isStereoEnabled());
    ui->checkLectureLien->blockSignals(false);
    if (ui->pushLecture->isChecked())
        this->lecture();
    updatePlayButton();

    // Initialize the equalizer
    ui->widgetEqualizer->setCurrentIds(ids);
    ui->widgetEqualizer->enableApply(!ui->pushLecture->isChecked() || nombreElements > 1);
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
    _sf2->endEditing(_editingSource);

    // Update the page
    ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
    updateLoopQuality();
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

        // Update the loop option
        ui->checkLectureBoucle->setEnabled(ui->spinStartLoop->value() != ui->spinEndLoop->value());
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
    _sf2->endEditing(_editingSource);

    // Update the page
    ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
    updateLoopQuality();
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

        // Update the loop option
        ui->checkLectureBoucle->setEnabled(ui->spinStartLoop->value() != ui->spinEndLoop->value());
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
    _sf2->endEditing(_editingSource);

    // Update the page
    ui->spinStartLoop->setValue(0);
    ui->spinStartLoop->setMaximum(displayedEndLoop);
    ui->spinEndLoop->setValue(displayedEndLoop);
    ui->spinEndLoop->setMinimum(0);
    ui->grapheFourier->setPos(ui->spinStartLoop->value(), ui->spinEndLoop->value());
    updateLoopQuality();

    if (triggersMessage)
        QMessageBox::information(this, tr("Information"),
                                 tr("Change successfully applied to the different samples"));
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
    _sf2->endEditing(_editingSource);
}

void PageSmpl::setRootKey(int val)
{
    // Send a signal for updating the keyboard
    emit rootKeyChanged(val);

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
                    if (val.cValue != _sf2->get(id2, champ_chPitchCorrection).cValue)
                        _sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }
    _sf2->endEditing(_editingSource);
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
            ui->comboType->addItem(tr("mono", "opposite to stereo"));
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

    _sf2->endEditing(_editingSource);
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
            ui->comboType->addItem(tr("mono"));
            ui->comboType->addItem(tr("right"));
            ui->comboType->addItem(tr("left"));
            ui->comboType->addItem(tr("link"));
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
        ui->comboType->addItem(tr("mono"));
        ui->comboLink->setCurrentIndex(0);
        ui->checkLectureLien->setEnabled(false);
        ui->checkLectureLien->setChecked(false);
    }
    _preparingPage = false;

    _sf2->endEditing(_editingSource);
}

void PageSmpl::setRate(int index)
{
    Q_UNUSED(index);
    if (_preparingPage)
        return;

    // Soundfont editing
    bool ok;
    quint32 echFinal = QLocale::system().toInt(ui->comboSampleRate->currentText(), &ok);
    if (echFinal == 0 || !ok)
        return;
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
    _sf2->endEditing(_editingSource + ":update");
}

void PageSmpl::setRateElt(EltID id, quint32 echFinal)
{
    // Modification échantillonnage
    quint32 echInit = _sf2->get(id, champ_dwSampleRate).dwValue;
    QVector<float> baData = _sf2->getData(id);
    baData = SampleUtils::resampleMono(baData, echInit, echFinal);
    _sf2->set(id, baData);
    AttributeValue val;
    val.dwValue = echFinal;
    _sf2->set(id, champ_dwSampleRate, val);

    // Ajustement de length, startLoop, endLoop
    val.dwValue = baData.size();
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
            ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool())
        id2.indexElt = _sf2->get(id, champ_wSampleLink).wValue;
    else
        id2.indexElt = -1;

    return id2;
}

void PageSmpl::lecture()
{
    _synth->play(EltID(), -1, -1, 0);
    if (ui->pushLecture->isChecked())
    {
        _synth->setLoopEnabled(ui->checkLectureBoucle->isChecked());

        QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
        if (listID.count() == 1)
        {
            _synth->setSmplEqValues(ui->widgetEqualizer->gatherEqVariables().data());
            _synth->activateSmplEq(ui->widgetEqualizer->isPreviewEnabled());
            _currentPlayingToken = _synth->play(listID[0], -1, -1, 127);
        }

        // Désactivations
        ui->comboLink->setEnabled(false);
        ui->comboType->setEnabled(false);
        ui->comboSampleRate->setEnabled(false);
        ui->widgetEqualizer->enableApply(false);
    }

    updatePlayButton();
    updateSinus();
}

void PageSmpl::lecteurFinished(int token)
{
    if (token != _currentPlayingToken)
        return;
    ui->waveDisplay->setCurrentSample(0);

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
    updatePlayButton();
}

void PageSmpl::setSinusEnabled(bool val)
{
    Q_UNUSED(val)
    updateSinus();
}

void PageSmpl::setGainSample(int val)
{
    // Modif synth
    _synth->setSampleGain((double)val);
}

void PageSmpl::updateSinus()
{
    _synth->setSinus(_currentIds.count() == 1 && ui->pushLecture->isChecked() && ui->checkLectureSinus->isChecked(),
                     ui->spinRootKey->value());
}

void PageSmpl::setStereo(bool val)
{
    // Modif synth
    ContextManager::configuration()->setValue(ConfManager::SECTION_AUDIO, "stereo_playback", val);
    _synth->setStereo(val);
}

void PageSmpl::on_pushAutoTune_clicked()
{
    bool firstValue = true;
    bool triggersMessage = false;

    QList<int> listeSamplesProcessed;
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (_sf2->isValid(id) && !listeSamplesProcessed.contains(id.indexElt))
        {
            if (firstValue)
                firstValue = false;
            else
                triggersMessage = true;

            listeSamplesProcessed << id.indexElt;

            // Modification pitch / correction
            int pitch1, correction1;
            int pitch2 = -1;
            int correction2 = 0;
            float score1;
            float score2 = -1;
            autoTune(id, pitch1, correction1, score1);

            // Sample associé ?
            EltID id2 = getRepercussionID(id);
            bool id2Valid = false;
            if (id2.indexElt != -1 && _sf2->isValid(id2) && !listeSamplesProcessed.contains(id2.indexElt))
            {
                id2Valid = true;
                listeSamplesProcessed << id2.indexElt;

                // Modification pitch / correction
                autoTune(id2, pitch2, correction2, score2);
            }

            // Use the pitch / correction associated to the best score
            if (score2 > score1)
            {
                pitch1 = pitch2;
                correction1 = correction2;
            }

            if (pitch1 != -1)
            {
                // Modification du pitch et de la correction
                AttributeValue val;
                val.wValue = pitch1;
                _sf2->set(id, champ_byOriginalPitch, val);
                val.wValue = correction1;
                _sf2->set(id, champ_chPitchCorrection, val);

                // Other sample?
                if (id2Valid)
                {
                    val.wValue = pitch1;
                    _sf2->set(id2, champ_byOriginalPitch, val);
                    val.wValue = correction1;
                    _sf2->set(id2, champ_chPitchCorrection, val);
                }
            }
        }
    }
    _sf2->endEditing(_editingSource + ":update");

    if (triggersMessage)
        QMessageBox::information(this, tr("Information"),
                                 tr("Change successfully applied to the different samples"));
}

void PageSmpl::autoTune(EltID id, int &pitch, int &correction, float &score)
{
    // Remplissage du graphique Fourier
    ui->grapheFourier->setCurrentIds(id);
    quint32 startLoop = _sf2->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = _sf2->get(id, champ_dwEndLoop).dwValue;
    ui->grapheFourier->setPos(startLoop, endLoop, false);

    // Hauteur de note et correction
    ui->grapheFourier->getEstimation(pitch, correction, score);
}

void PageSmpl::onSpacePressedInternal()
{
    if (_currentIds.getSelectedIds(elementSmpl).count() == 1)
    {
        ui->pushLecture->toggle();
        this->lecture();
    }
}

void PageSmpl::updatePlayButton()
{
    if (ui->pushLecture->isChecked())
    {
        ui->pushLecture->setToolTip(tr("Stop"));
        ui->pushLecture->setIcon(ContextManager::theme()->getColoredSvg(
                                     ":/icons/play.svg", QSize(36, 36), ThemeManager::HIGHLIGHTED_BACKGROUND));
    }
    else
    {
        ui->pushLecture->setToolTip(tr("Play"));
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
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setText(tr("Are you sure to cut the sample from <b>%1</b> to <b>%2</b>?").arg(start).arg(end));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::YesAll);
    msgBox.button(QMessageBox::Yes)->setText(tr("&Yes"));
    msgBox.button(QMessageBox::No)->setText(tr("&No"));
    msgBox.button(QMessageBox::YesAll)->setText(tr("Yes and &create another sample"));
    msgBox.setDefaultButton(QMessageBox::No);

    int dialogResult = msgBox.exec();
    if (dialogResult == QMessageBox::No)
        return;

    // Current sample id, with possibly its stereo sample id
    EltID id = _currentIds[0];
    EltID id2 = getRepercussionID(id);
    if (id2.indexElt != -1)
    {
        // The second sample must be long enough
        if (_sf2->get(id, champ_dwLength).dwValue <= static_cast<quint32>(end))
            id2.indexElt = -1;
    }

    EltID createdSmplId(elementUnknown);
    if (dialogResult == QMessageBox::YesAll)
        createdSmplId = id;

    // Cut between start and end
    bool updateNeeded = cutSample(id, static_cast<quint32>(start), static_cast<quint32>(end), createdSmplId);
    if (id2.indexElt != -1)
    {
        EltID createdSmplId2 = createdSmplId;
        updateNeeded |= cutSample(id2, static_cast<quint32>(start), static_cast<quint32>(end), createdSmplId2);
        if (createdSmplId.typeElement != elementUnknown && createdSmplId2.typeElement != elementUnknown)
        {
            // Link the created samples
            AttributeValue val = _sf2->get(id, champ_sfSampleType);
            _sf2->set(createdSmplId, champ_sfSampleType, val);
            val = _sf2->get(id2, champ_sfSampleType);
            _sf2->set(createdSmplId2, champ_sfSampleType, val);
            val.wValue = createdSmplId2.indexElt;
            _sf2->set(createdSmplId, champ_wSampleLink, val);
            val.wValue = createdSmplId.indexElt;
            _sf2->set(createdSmplId2, champ_wSampleLink, val);
        }
    }

    if (updateNeeded)
        _sf2->endEditing(_editingSource + ":update");
}

bool PageSmpl::cutSample(EltID id, quint32 start, quint32 end, EltID &createdSmplId)
{
    if (end > _sf2->get(id, champ_dwLength).dwValue)
    {
        createdSmplId.typeElement = elementUnknown;
        return false;
    }

    quint16 wBps = _sf2->get(id, champ_wBpsInit).wValue;
    if (wBps > 16)
        wBps = 24;
    else
        wBps = 16;
    QVector<float> vData = _sf2->getData(id);

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

    // Possibly create another sample with the removed part
    if (createdSmplId.typeElement != elementUnknown)
    {
        // Data
        createdSmplId.indexElt = _sf2->add(createdSmplId);
        QVector<float> midData = vData.mid(start, end - start);
        _sf2->set(createdSmplId, midData);
        val.dwValue = static_cast<quint32>(midData.size());
        _sf2->set(createdSmplId, champ_dwLength, val);
        val.wValue = wBps;
        _sf2->set(createdSmplId, champ_wBpsInit, val);
        val = _sf2->get(id, champ_dwSampleRate);
        _sf2->set(createdSmplId, champ_dwSampleRate, val);

        // Copy some attributes
        val = _sf2->get(id, champ_byOriginalPitch);
        _sf2->set(createdSmplId, champ_byOriginalPitch, val);
        val = _sf2->get(id, champ_chPitchCorrection);
        _sf2->set(createdSmplId, champ_chPitchCorrection, val);

        // Name
        _sf2->set(createdSmplId, champ_name, findDuplicateName(id));
    }

    // Remove data and update the sample
    vData.remove(start, end - start);
    _sf2->set(id, vData);
    val.dwValue = static_cast<quint32>(vData.size());
    _sf2->set(id, champ_dwLength, val);

    return true;
}

QString PageSmpl::findDuplicateName(EltID smplId)
{
    // Browse all names
    QStringList listName;
    QString currentName;
    EltID idTmp = smplId;
    foreach (int i, _sf2->getSiblings(smplId))
    {
        idTmp.indexElt = i;
        if (i == smplId.indexElt)
        {
            currentName = _sf2->getQstr(idTmp, champ_name);
            listName << currentName;
        }
        else
            listName << _sf2->getQstr(idTmp, champ_name);
    }

    // If the name ends with a suffix such as "-1", possibly remove it
    QRegularExpression regEx("-[0-9]+$");
    int suffixPos = currentName.indexOf(regEx);
    if (suffixPos >= 0)
    {
        // The name without the suffix must exist
        QString tmp = currentName.left(suffixPos);
        if (listName.contains(tmp))
            currentName = tmp;
    }

    int suffixNumber = 0;
    while (listName.contains(getName(currentName, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
    {
        suffixNumber++;
    }
    return getName(currentName, 20, suffixNumber);
}

QString PageSmpl::getName(QString name, int maxCharacters, int suffixNumber)
{
    if (suffixNumber == 0)
        return name.left(maxCharacters);
    QString suffix = QString::number(suffixNumber);
    int suffixSize = suffix.length();
    if (suffixSize > 3 || maxCharacters < suffixSize + 1)
        return name.left(maxCharacters);
    return name.left(maxCharacters - suffixSize - 1) + "-" + suffix;
}

void PageSmpl::on_checkLectureBoucle_clicked(bool checked)
{
    _synth->setLoopEnabled(checked);
}

void PageSmpl::updateLoopQuality()
{
    QList<EltID> ids = _currentIds.getSelectedIds(elementSmpl);
    if (ids.size() == 1 && ui->spinStartLoop->value() != ui->spinEndLoop->value())
    {
        QVector<float> vData = _sf2->getData(ids.first());
        float loopQuality = SampleUtils::computeLoopQuality(vData, ui->spinStartLoop->value(), ui->spinEndLoop->value(), 3, false);
        if (loopQuality < 0.05)
            ui->iconLoopWarning->setPixmap(QPixmap());
        else if (loopQuality < 0.15)
        {
            QMap<QString, QString> replacement;
            replacement["currentColor"] = ContextManager::theme()->getFixedColor(
                        ThemeManager::FixedColorType::YELLOW, ThemeManager::WINDOW_BACKGROUND).name();
            ui->iconLoopWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), replacement));
        }
        else
        {
            QMap<QString, QString> replacement;
            replacement["currentColor"] = ContextManager::theme()->getFixedColor(
                        ThemeManager::FixedColorType::RED, ThemeManager::WINDOW_BACKGROUND).name();
            ui->iconLoopWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), replacement));
        }
    }
    else
        ui->iconLoopWarning->setPixmap(QPixmap());
}
