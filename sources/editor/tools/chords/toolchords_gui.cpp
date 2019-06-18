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

#include "toolchords_gui.h"
#include "ui_toolchords_gui.h"
#include "contextmanager.h"

ToolChords_gui::ToolChords_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolChords_gui)
{
    ui->setupUi(this);
}

ToolChords_gui::~ToolChords_gui()
{
    delete ui;
}

void ToolChords_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolChords_parameters * params = (ToolChords_parameters *) parameters;
    
    // Instrument & samples parameters
    ui->lineName->setText(params->getInstrumentName());
    ui->comboFreq->setCurrentIndex(params->getDensityType());
    ui->checkLoop->setChecked(params->getLoopSample());
    ui->checkStereo->setChecked(params->getStereoSample());

    // Chord configuration
    ChordInfo ci = params->getChordConfiguration();
    int inversions = 0;
    int possibleInversions = 0;

    ui->comboType1->setCurrentIndex(ci.chordType1);
    ui->sliderType1->setValue(ci.chordType1Attenuation);

    ui->comboType3->setCurrentIndex(ci.chordType3);
    ui->sliderType3->setValue(ci.chordType3Attenuation);
    if (ci.chordType3Inversion)
        inversions++;
    if (ci.chordType3 != 0)
        possibleInversions++;

    ui->comboType5->setCurrentIndex(ci.chordType5);
    ui->sliderType5->setValue(ci.chordType5Attenuation);
    if (ci.chordType5Inversion)
        inversions++;
    if (ci.chordType5 != 0)
        possibleInversions++;

    ui->comboType7->setCurrentIndex(ci.chordType7);
    ui->sliderType7->setValue(ci.chordType7Attenuation);
    if (ci.chordType7Inversion)
        inversions++;
    if (ci.chordType7 != 0)
        possibleInversions++;

    ui->comboType9->setCurrentIndex(ci.chordType9);
    ui->sliderType9->setValue(ci.chordType9Attenuation);
    if (ci.chordType9Inversion)
        inversions++;
    if (ci.chordType9 != 0)
        possibleInversions++;

    ui->spinInversions->setMaximum(possibleInversions);
    ui->spinInversions->setValue(inversions);

    ui->sliderOctave->setValue(ci.octave);
}

void ToolChords_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolChords_parameters * params = (ToolChords_parameters *) parameters;
    
    // Instrument & samples parameters
    params->setInstrumentName(ui->lineName->text());
    params->setDensityType(ui->comboFreq->currentIndex());
    params->setLoopSample(ui->checkLoop->isChecked());
    params->setStereoSample(ui->checkStereo->isChecked());

    // Chord configuration
    ChordInfo ci;
    int inversions = ui->spinInversions->value();

    ci.chordType9 = ui->comboType9->currentIndex();
    ci.chordType9Attenuation = ui->sliderType9->value();
    if (inversions > 0)
    {
        ci.chordType9Inversion = true;
        inversions--;
    }
    else
        ci.chordType9Inversion = false;

    ci.chordType7 = ui->comboType7->currentIndex();
    ci.chordType7Attenuation = ui->sliderType7->value();
    if (inversions > 0)
    {
        ci.chordType7Inversion = true;
        inversions--;
    }
    else
        ci.chordType7Inversion = false;

    ci.chordType5 = ui->comboType5->currentIndex();
    ci.chordType5Attenuation = ui->sliderType5->value();
    if (inversions > 0)
    {
        ci.chordType5Inversion = true;
        inversions--;
    }
    else
        ci.chordType5Inversion = false;

    ci.chordType3 = ui->comboType3->currentIndex();
    ci.chordType3Attenuation = ui->sliderType3->value();
    if (inversions > 0)
    {
        ci.chordType3Inversion = true;
        inversions--;
    }
    else
        ci.chordType3Inversion = false;

    ci.chordType1 = ui->comboType1->currentIndex();
    ci.chordType1Attenuation = ui->sliderType1->value();

    ci.octave = ui->sliderOctave->value();

    params->setChordConfiguration(ci);
}

void ToolChords_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolChords_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
