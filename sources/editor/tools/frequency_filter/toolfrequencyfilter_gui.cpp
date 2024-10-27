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
    ui(new Ui::ToolFrequencyFilter_gui)
{
    ui->setupUi(this);
}

ToolFrequencyFilter_gui::~ToolFrequencyFilter_gui()
{
    delete ui;
}

void ToolFrequencyFilter_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    ToolFrequencyFilter_parameters * params = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters);

    // Filter used
    ui->graphFilterFrequencies->setValues(params->getCurve());

    // Number of Fourier transforms to add
    int nbFourier = qMin(ids.count(), 6);
    ui->graphFilterFrequencies->clearFourierTransforms();

    // Add the fourier transforms of the first samples in the graph
    SoundfontManager * sm = SoundfontManager::getInstance();
    for (int i = 0; i < nbFourier; i++)
    {
        EltID id = ids[i];
        quint32 sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
        QVector<float> vData = sm->getData(id);

        // Get the Fourier transform of the sample
        QVector<float> fData = SampleUtils::getFourierTransform(vData);

        // Display it
        ui->graphFilterFrequencies->addFourierTransform(fData, sampleRate);
    }
}

void ToolFrequencyFilter_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolFrequencyFilter_parameters * params = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters);

    // Save parameters
    params->setCurve(ui->graphFilterFrequencies->getValues());
}

void ToolFrequencyFilter_gui::on_pushCancel_clicked()
{
    emit this->canceled();
}

void ToolFrequencyFilter_gui::on_pushOk_clicked()
{
    emit this->validated();
}
