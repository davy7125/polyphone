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

#include "toolchangeattenuation_gui.h"
#include "ui_toolchangeattenuation_gui.h"
#include "toolchangeattenuation_parameters.h"
#include "soundfontmanager.h"

ToolChangeAttenuation_gui::ToolChangeAttenuation_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolChangeAttenuation_gui)
{
    ui->setupUi(this);
}

ToolChangeAttenuation_gui::~ToolChangeAttenuation_gui()
{
    delete ui;
}

void ToolChangeAttenuation_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolChangeAttenuation_parameters * params = (ToolChangeAttenuation_parameters *) parameters;
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Compute current attenuation range
    int minAttenuation = 1440;
    int maxAttenuation = _isInst ? 0 : -1440;
    foreach (EltID id, ids)
    {
        int defaultAttenuation = sm->isSet(id, champ_initialAttenuation) ? sm->get(id, champ_initialAttenuation).shValue : 0;

        // Browse each element linked
        EltID elementLinked(_isInst ? elementInstSmpl : elementPrstInst, id.indexSf2, id.indexElt);
        foreach (int i, sm->getSiblings(elementLinked))
        {
            elementLinked.indexElt2 = i;
            int attenuation = defaultAttenuation;
            if (sm->isSet(elementLinked, champ_initialAttenuation))
                attenuation = sm->get(elementLinked, champ_initialAttenuation).shValue;

            if (attenuation < minAttenuation)
                minAttenuation = attenuation;
            if (attenuation > maxAttenuation)
                maxAttenuation = attenuation;
        }
    }

    // Show it
    ui->labelRange->setText("[" + formatDouble(0.1 * minAttenuation) + ", " + formatDouble(0.1 * maxAttenuation) + "]");

    // Compute the possible offset and show it
    double minOffset = -0.1 * minAttenuation - (_isInst ? 0 : 144.);
    double maxOffset = 144. - 0.1 * maxAttenuation;
    ui->labelPossibleOffset->setText("[" + formatDouble(minOffset) + ", " + formatDouble(maxOffset) + "]");
    ui->doubleSpinValue->setMinimum(minOffset);
    ui->doubleSpinValue->setMaximum(maxOffset);

    // Recall the last value
    double previousValue = _isInst ? params->getInstValue() : params->getPrstValue();
    if (previousValue < minOffset)
        ui->doubleSpinValue->setValue(minOffset);
    else if (previousValue > maxOffset)
        ui->doubleSpinValue->setValue(maxOffset);
    else
        ui->doubleSpinValue->setValue(previousValue);
}

void ToolChangeAttenuation_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolChangeAttenuation_parameters * params = (ToolChangeAttenuation_parameters *) parameters;

    // Value
    if (_isInst)
        params->setInstValue(ui->doubleSpinValue->value());
    else
        params->setPrstValue(ui->doubleSpinValue->value());
}

void ToolChangeAttenuation_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolChangeAttenuation_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}

QString ToolChangeAttenuation_gui::formatDouble(double value)
{
    return (value > 0 ? "+" : "") + QString::number(value, 'f', 1);
}
