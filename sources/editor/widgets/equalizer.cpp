/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "equalizer.h"
#include "ui_equalizer.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

Equalizer::Equalizer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Equalizer),
    _synth(ContextManager::audio()->getSynth())
{
    ui->setupUi(this);

    // Style
    ui->pushEgaliser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushEgalRestore->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reset_eq.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    // Initialize EQ
    _initialization = true;
    loadEQ();
    _initialization = false;
    _synth->setSmplEqValues(gatherEqVariables());
}

Equalizer::~Equalizer()
{
    delete ui;
}

void Equalizer::on_pushEgalRestore_clicked()
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
    _synth->setSmplEqValues(gatherEqVariables());
}

void Equalizer::on_pushEgaliser_clicked()
{
    if (_initialization)
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

    // Soundfont editing
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<EltID> listprocessedID;
    QList<EltID> listID = _currentIds.getSelectedIds(elementSmpl);
    foreach (EltID id, listID)
    {
        if (sm->isValid(id) && !listprocessedID.contains(id))
        {
            listprocessedID << id;
            QByteArray baData = sm->getData(id, champ_sampleDataFull24);
            baData = SampleUtils::EQ(baData, sm->get(id, champ_dwSampleRate).dwValue, 24,
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
            sm->set(id, champ_sampleDataFull24, baData);

            // Sample associé ?
            EltID id2 = PageSmpl::getRepercussionID(id);
            if (id2.indexElt != -1)
            {
                if (sm->isValid(id2) && !listprocessedID.contains(id2))
                {
                    listprocessedID << id2;
                    QByteArray baData = sm->getData(id2, champ_sampleDataFull24);
                    baData = SampleUtils::EQ(baData, sm->get(id2, champ_dwSampleRate).dwValue, 24,
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
                    sm->set(id2, champ_sampleDataFull24, baData);
                }
            }
        }
    }
    sm->endEditing("equalizer");
    ui->checkEqualizerPreview->setChecked(false);
}

void Equalizer::loadEQ()
{
    ui->verticalSlider_1->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "01", 0).toInt());
    ui->verticalSlider_2->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "02", 0).toInt());
    ui->verticalSlider_3->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "03", 0).toInt());
    ui->verticalSlider_4->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "04", 0).toInt());
    ui->verticalSlider_5->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "05", 0).toInt());
    ui->verticalSlider_6->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "06", 0).toInt());
    ui->verticalSlider_7->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "07", 0).toInt());
    ui->verticalSlider_8->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "08", 0).toInt());
    ui->verticalSlider_9->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "09", 0).toInt());
    ui->verticalSlider_10->setValue(ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "10", 0).toInt());
}

void Equalizer::saveEQ()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "01", ui->verticalSlider_1->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "02", ui->verticalSlider_2->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "03", ui->verticalSlider_3->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "04", ui->verticalSlider_4->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "05", ui->verticalSlider_5->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "06", ui->verticalSlider_6->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "07", ui->verticalSlider_7->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "08", ui->verticalSlider_8->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "09", ui->verticalSlider_9->value());
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "EQ", "10", ui->verticalSlider_10->value());
}

void Equalizer::enableApply(bool isEnabled)
{
    ui->pushEgaliser->setEnabled(isEnabled);
}

QVector<int> Equalizer::gatherEqVariables()
{
    QVector<int> result(10);
    result[0] = ui->verticalSlider_1->value();
    result[1] = ui->verticalSlider_2->value();
    result[2] = ui->verticalSlider_3->value();
    result[3] = ui->verticalSlider_4->value();
    result[4] = ui->verticalSlider_5->value();
    result[5] = ui->verticalSlider_6->value();
    result[6] = ui->verticalSlider_7->value();
    result[7] = ui->verticalSlider_8->value();
    result[8] = ui->verticalSlider_9->value();
    result[9] = ui->verticalSlider_10->value();
    return result;
}

bool Equalizer::isPreviewEnabled()
{
    return ui->checkEqualizerPreview->isChecked();
}

void Equalizer::on_checkEqualizerPreview_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    _synth->activateSmplEq(ui->checkEqualizerPreview->isChecked());
}

void Equalizer::on_verticalSlider_1_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_2_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_3_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_4_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_5_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_6_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_7_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_8_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_9_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}

void Equalizer::on_verticalSlider_10_sliderMoved(int position)
{
    Q_UNUSED(position)
    _synth->setSmplEqValues(gatherEqVariables());
    saveEQ();
}
