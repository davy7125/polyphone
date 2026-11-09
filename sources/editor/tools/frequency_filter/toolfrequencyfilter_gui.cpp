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

#include "toolfrequencyfilter_gui.h"
#include "ui_toolfrequencyfilter_gui.h"
#include "toolfrequencyfilter_parameters.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolFrequencyFilter_gui::ToolFrequencyFilter_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolFrequencyFilter_gui),
    _mode(GainCurve::MODE_CEILING),
    _noteTracking(false),
    _initialization(false)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
}

ToolFrequencyFilter_gui::~ToolFrequencyFilter_gui()
{
    delete ui;
}

void ToolFrequencyFilter_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    ToolFrequencyFilter_parameters * params = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters);

    // Curves of each mode and abscissa
    _mode = params->getMode();
    _noteTracking = params->isNoteTracking();
    for (int i = 0; i < 4; i++)
        _points[i] = params->getPoints(i >= 2 ? GainCurve::MODE_GAIN : GainCurve::MODE_CEILING, i % 2 == 1);

    // Fundamental frequencies of the samples
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<double> fundamentals;
    foreach (EltID id, ids)
        fundamentals << GainCurve::getFundamental(sm->get(id, champ_byOriginalPitch).bValue,
                                                  sm->get(id, champ_chPitchCorrection).cValue);
    ui->graphFilterFrequencies->setFundamentals(fundamentals);

    // Spectra of the first samples
    ui->graphFilterFrequencies->clearFourierTransforms();
    int nbFourier = qMin(ids.count(), 6);
    for (int i = 0; i < nbFourier; i++)
    {
        EltID id = ids[i];
        quint32 sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
        QVector<float> fData = SampleUtils::getFourierTransform(sm->getDataFloat(id));
        ui->graphFilterFrequencies->addFourierTransform(fData, sampleRate, fundamentals[i]);
    }

    _initialization = true;
    ui->comboMode->setCurrentIndex(_mode == GainCurve::MODE_GAIN ? 1 : 0);
    ui->checkNoteTracking->setChecked(_noteTracking);
    _initialization = false;
    displayCurve();
}

void ToolFrequencyFilter_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolFrequencyFilter_parameters * params = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters);

    // Save parameters
    _points[2 * _mode + (_noteTracking ? 1 : 0)] = ui->graphFilterFrequencies->getPoints();
    for (int i = 0; i < 4; i++)
        params->setPoints(i >= 2 ? GainCurve::MODE_GAIN : GainCurve::MODE_CEILING, i % 2 == 1, _points[i]);
    params->setMode(_mode);
    params->setNoteTracking(_noteTracking);
}

void ToolFrequencyFilter_gui::displayCurve()
{
    ui->label->setText(_mode == GainCurve::MODE_GAIN ?
                           tr("Draw the attenuation or the amplification of the frequencies:") :
                           tr("Mark with red color where you want to cut the frequencies:"));
    ui->graphFilterFrequencies->setMode(_mode, _noteTracking);
    ui->graphFilterFrequencies->setPoints(_points[2 * _mode + (_noteTracking ? 1 : 0)]);
}

void ToolFrequencyFilter_gui::on_comboMode_currentIndexChanged(int index)
{
    if (_initialization)
        return;
    _points[2 * _mode + (_noteTracking ? 1 : 0)] = ui->graphFilterFrequencies->getPoints();
    _mode = index == 1 ? GainCurve::MODE_GAIN : GainCurve::MODE_CEILING;
    displayCurve();
}

void ToolFrequencyFilter_gui::on_checkNoteTracking_toggled(bool checked)
{
    if (_initialization)
        return;
    _points[2 * _mode + (_noteTracking ? 1 : 0)] = ui->graphFilterFrequencies->getPoints();
    _noteTracking = checked;
    displayCurve();
}

void ToolFrequencyFilter_gui::on_buttonBox_rejected()
{
    emit this->canceled();
}

void ToolFrequencyFilter_gui::on_buttonBox_accepted()
{
    emit this->validated();
}
