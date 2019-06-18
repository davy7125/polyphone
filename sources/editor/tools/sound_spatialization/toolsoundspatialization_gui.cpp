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

#include "toolsoundspatialization_gui.h"
#include "ui_toolsoundspatialization_gui.h"
#include "toolsoundspatialization_parameters.h"
#include "soundfontmanager.h"

ToolSoundSpatialization_gui::ToolSoundSpatialization_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolSoundSpatialization_gui)
{
    ui->setupUi(this);
}

ToolSoundSpatialization_gui::~ToolSoundSpatialization_gui()
{
    delete ui;
}

void ToolSoundSpatialization_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *) parameters;
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Find maximum key range to display
    _minKey = 127;
    _maxKey = 0;
    foreach (EltID id, ids.getSelectedIds(_isInst ? elementInst : elementPrst))
    {
        if (_isInst)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt2 = i;
            if (sm->isSet(id, champ_keyRange))
            {
                AttributeValue amount = sm->get(id, champ_keyRange);
                if (amount.rValue.byLo < _minKey)
                    _minKey = amount.rValue.byLo;
                if (amount.rValue.byHi > _maxKey)
                    _maxKey = amount.rValue.byHi;
            }
        }
    }
    if (_minKey > _maxKey)
    {
        _minKey = 0;
        _maxKey = 127;
    }

    // Recall parameters
    ui->comboPattern->blockSignals(true);
    ui->comboPattern->setCurrentIndex(_isInst ? params->getInstPattern() : params->getPrstPattern());
    ui->comboPattern->blockSignals(false);

    ui->spinNbDivision->blockSignals(true);
    ui->spinNbDivision->setValue(_isInst ? params->getInstDivisionNumber() : params->getPrstDivisionNumber());
    ui->spinNbDivision->blockSignals(false);

    ui->spinSpreading->blockSignals(true);
    ui->spinSpreading->setValue(_isInst ? params->getInstSpreading() : params->getPrstSpreading());
    ui->spinSpreading->blockSignals(false);

    ui->spinFilling->blockSignals(true);
    ui->spinFilling->setValue(_isInst ? params->getInstFilling() : params->getPrstFilling());
    ui->spinFilling->blockSignals(false);

    ui->spinOffset->blockSignals(true);
    ui->spinOffset->setValue(_isInst ? params->getInstOffset() : params->getPrstOffset());
    ui->spinOffset->blockSignals(false);

    ui->checkFlip->blockSignals(true);
    ui->checkFlip->setChecked(_isInst ? params->getInstFlip() : params->getPrstFlip());
    ui->checkFlip->blockSignals(false);

    ui->checkFlop->blockSignals(true);
    ui->checkFlop->setChecked(_isInst ? params->getInstFlop() : params->getPrstFlop());
    ui->checkFlop->blockSignals(false);

    // Activation des renversements et remplissage du graphique
    ui->spinOffset->setEnabled(ui->spinSpreading->value() != 100);
    on_comboPattern_currentIndexChanged(ui->comboPattern->currentIndex());
}

void ToolSoundSpatialization_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *) parameters;

    // Save configuration
    if (_isInst)
    {
        params->setInstPattern(ui->comboPattern->currentIndex());
        params->setInstDivisionNumber(ui->spinNbDivision->value());
        params->setInstSpreading(ui->spinSpreading->value());
        params->setInstFilling(ui->spinFilling->value());
        params->setInstOffset(ui->spinOffset->value());
        params->setInstFlip(ui->checkFlip->isChecked());
        params->setInstFlop(ui->checkFlop->isChecked());
    }
    else
    {
        params->setPrstPattern(ui->comboPattern->currentIndex());
        params->setPrstDivisionNumber(ui->spinNbDivision->value());
        params->setPrstSpreading(ui->spinSpreading->value());
        params->setPrstFilling(ui->spinFilling->value());
        params->setPrstOffset(ui->spinOffset->value());
        params->setPrstFlip(ui->checkFlip->isChecked());
        params->setPrstFlop(ui->checkFlop->isChecked());
    }

    // Store the curve
    QVector<double> x;
    QVector<int> y;
    computeData(x, y);
    QMap<int, double> mapPan;
    for (int i = 0; i < x.size(); i++)
        mapPan.insert(y.at(i), x.at(i));
    params->setMapPan(mapPan);
}

void ToolSoundSpatialization_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolSoundSpatialization_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

void ToolSoundSpatialization_gui::on_comboPattern_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateRenversements();
    updateGraph();
}

void ToolSoundSpatialization_gui::on_spinNbDivision_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateRenversements();
    updateGraph();
}

void ToolSoundSpatialization_gui::on_spinSpreading_valueChanged(int arg1)
{
    ui->spinOffset->setEnabled(arg1 != 100);
    updateGraph();
}

void ToolSoundSpatialization_gui::on_spinFilling_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateGraph();
}

void ToolSoundSpatialization_gui::on_spinOffset_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateGraph();
}

void ToolSoundSpatialization_gui::on_checkFlip_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateGraph();
}

void ToolSoundSpatialization_gui::on_checkFlop_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    updateGraph();
}

void ToolSoundSpatialization_gui::updateRenversements()
{
    int nbDivisions = ui->spinNbDivision->value();
    switch (ui->comboPattern->currentIndex())
    {
    case 0: case 1:
        ui->checkFlip->setEnabled(nbDivisions > 1);
        ui->checkFlop->setEnabled(false);
        break;
    case 2: case 3:
        ui->checkFlip->setEnabled(nbDivisions > 1);
        ui->checkFlop->setEnabled(true);
        break;
    default:
        ui->checkFlip->setEnabled(false);
        ui->checkFlop->setEnabled(false);
        break;
    }
}

void ToolSoundSpatialization_gui::updateGraph()
{
    QVector<double> x;
    QVector<int> y;
    computeData(x, y);
    ui->grapheSpace->setData(x, y);
}

void ToolSoundSpatialization_gui::computeData(QVector<double> &x, QVector<int> &y)
{
    // Calcul du numéro de note en fonction du positionnement dans l'espace
    int nbNotes = _maxKey - _minKey + 1;
    x.resize(nbNotes);
    y.resize(nbNotes);
    for (int i = 0; i < nbNotes; i++)
        y[i] = _minKey + i;

    int motif = ui->comboPattern->currentIndex();
    switch (motif)
    {
    case 0: case 1:
        for (int i = 0; i < nbNotes; i++)
            x[i] = spaceLinear(i + _minKey,
                               ui->spinNbDivision->value(),
                               ui->spinSpreading->value(),
                               ui->spinFilling->value(),
                               ui->spinOffset->value(),
                               ui->checkFlip->isChecked(),
                               motif == 0);
        break;
    case 2: case 3:
        for (int i = 0; i < nbNotes; i++)
            x[i] = spaceCurve(i + _minKey,
                              ui->spinNbDivision->value(),
                              ui->spinSpreading->value(),
                              ui->spinFilling->value(),
                              ui->spinOffset->value(),
                              ui->checkFlip->isChecked(),
                              ui->checkFlop->isChecked(),
                              motif == 2);
        break;
    case 4:{
        // Mélange des notes
        uint seed = 0;
        QList<int> listTmp;
        for (int i = 0; i < nbNotes; i++)
            listTmp << i;

        for (int i = 0; i < nbNotes; i++)
        {
            seed += QTime(0, 0).msecsTo(QTime::currentTime());
            qsrand(seed);
            x[listTmp.takeAt(qrand() % listTmp.size())] = spaceLinear(i + _minKey,
                               ui->spinNbDivision->value(),
                               ui->spinSpreading->value(),
                               ui->spinFilling->value(),
                               ui->spinOffset->value(),
                               ui->checkFlip->isChecked(),
                               motif == 0);
        }
        }break;
    }
}

double ToolSoundSpatialization_gui::spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending)
{
    // Ajustement note
    int noteRelative = isAscending ? note - _minKey : _maxKey - note;

    // Détermination de l'axe autour duquel se trouve la note
    double posAxe = getAxe(noteRelative, nbDiv, sens);

    // Calcul de l'écart à l'axe
    double e = 1. / (_maxKey - _minKey + nbDiv - 1); // écart unitaire
    double ne_axe = (((double)(_maxKey - _minKey) / 2) - noteRelative) / nbDiv;
    double ecart_axe = e * ne_axe;

    // Prise en compte de l'occupation
    double mult = (double)(nbDiv) / (nbDiv - 1 + (double)occupation / 100); // dilatation future
    ecart_axe *= (double)occupation / 100;

    // Synthèse
    return (double)etalement / 100        * ((double)mult * (posAxe + ecart_axe - 0.5) + 0.5) +
           (1. - (double)etalement / 100) * (double)offset / 100;
}

double ToolSoundSpatialization_gui::spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow)
{
    // Ajustement note
    int noteRelative = 0;
    if (isHollow)
    {
        // Calcul décalage
        int resteNotes = (_maxKey - _minKey + 1) % (2 * nbDiv);
        int decalage = 0;
        if (resteNotes < nbDiv)
            decalage = nbDiv - resteNotes;
        else if (resteNotes > nbDiv)
            decalage = 3 * nbDiv - resteNotes;
        noteRelative = _maxKey - note + decalage;
    }
    else
        noteRelative = note - _minKey;

    // Détermination de l'axe autour duquel se trouve la note
    double posAxe = getAxe(noteRelative, nbDiv, sens);

    // Calcul de l'écart à l'axe
    double e = 1.0 / (ceil((double)(_maxKey - _minKey + 1 - nbDiv) / (2 * nbDiv)) * (2 * nbDiv) + nbDiv); // écart unitaire
    int ne_axe = (noteRelative + nbDiv) / (2 * nbDiv);
    double ecart_axe = e * ne_axe;

    // Prise en compte de l'occupation
    double mult = 1; // dilation future
    if (isHollow)
        ecart_axe = (double)(ecart_axe * occupation + (double)(100 - occupation) / (2*nbDiv)) / 100;
    else
    {
        ecart_axe *= (double)occupation / 100;
        mult = (double)(nbDiv) / (nbDiv - 1 + (double)occupation / 100);
    }

    // Côté de l'axe
    int cote = noteRelative % (2 * nbDiv);
    if (cote >= nbDiv)
        ecart_axe = -ecart_axe;
    if (sens2 != 0) ecart_axe = -ecart_axe;

    // Synthèse
    return (double)etalement / 100        * ((double)mult * (posAxe + ecart_axe - 0.5) + 0.5) +
           (1. - (double)etalement / 100) * (double)offset / 100;
}

double ToolSoundSpatialization_gui::getAxe(int note, int nbDiv, bool sens)
{
    int axe = note % nbDiv;
    if (axe % 2 == 0)
        axe = axe / 2;
    else
        axe = nbDiv - (axe + 1) / 2;
    if (sens)
        axe = nbDiv - 1 - axe;
    return (double)(2 * axe + 1) / (2 * nbDiv);
}
