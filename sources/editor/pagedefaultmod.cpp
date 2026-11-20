/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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
**           Author: Davy Triponney & Others                              **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 18.11.2026                                           **
***************************************************************************/


#include "pagedefaultmod.h"
#include "ui_pagedefaultmod.h"
#include "modulatorcell.h"
#include "contextmanager.h"
#include <QMessageBox>

PageDefaultMod::PageDefaultMod(QWidget *parent) :
    Page(parent, "page:defaultmod"),
    ui(new Ui::PageDefaultMod)
{
    ui->setupUi(this);
    ui->modulatorEditor->initializeForDefaultModulators();

    // Border under the text, same color as the one separating the buttons of the modulator editor from the modulators
    ui->frameHeader->setStyleSheet("QFrame#frameHeader{border:0;border-bottom:1px solid " +
                                   ContextManager::theme()->getColor(ThemeManager::BORDER).name() + ";}");
}

PageDefaultMod::~PageDefaultMod()
{
    delete ui;
}

void PageDefaultMod::updateInterface(QString editingSource)
{
    Q_UNUSED(editingSource)
    if (_currentIds.isEmpty())
        return;

    int sf2Index = _currentIds[0].indexSf2;
    bool isCustom = _sf2->get(EltID(elementSf2, sf2Index), champ_customDefaultMods).wValue != 0;
    ui->pushCustomize->setVisible(!isCustom);
    ui->pushGroup->setVisible(!isCustom);
    ui->pushReset->setVisible(isCustom);
    ui->pushMove->setVisible(isCustom);
    if (isCustom)
    {
        ui->label->setText(tr("This soundfont has its own default modulators, applied to every instrument instead of the ones "
                              "of the SoundFont format. They are saved in the sf2 and sf3 files (DMOD chunk) and need a "
                              "synthesizer that supports them."));
        ui->modulatorEditor->setIds(IdList(EltID(elementSf2, sf2Index)));
        ui->stackedWidget->setCurrentWidget(ui->pageCustom);
        return;
    }

    // Default modulators of the sf2 format, read only
    ui->label->setText(tr("This soundfont uses the default modulators of the SoundFont format, applied to every instrument. "
                          "Customize them to edit, add or remove default modulators in this soundfont."));
    ui->listWidget->clear();
    QList<ModulatorData> modulators = _sf2->getDefaultModulators(sf2Index);
    for (int i = 0; i < modulators.count(); i++)
    {
        ModulatorCell * cell = new ModulatorCell(modulators[i], i);
        QListWidgetItem * item = new QListWidgetItem();
        item->setSizeHint(cell->size());
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, cell);
    }
    ui->stackedWidget->setCurrentWidget(ui->pageStandard);
}

void PageDefaultMod::on_pushCustomize_clicked()
{
    setCustom(true);
}

void PageDefaultMod::on_pushReset_clicked()
{
    setCustom(false);
}

void PageDefaultMod::on_pushGroup_clicked()
{
    if (_currentIds.isEmpty())
        return;
    if (_sf2->groupCommonModulatorsAsDefaults(_currentIds[0].indexSf2) == 0)
    {
        QMessageBox::information(this, tr("Information"),
                                 tr("No modulator has the same value in the global division of all instruments."));
        return;
    }
    _sf2->endEditing(_editingSource);
    updateInterface(_editingSource);
}

void PageDefaultMod::on_pushMove_clicked()
{
    if (_currentIds.isEmpty())
        return;
    _sf2->moveDefaultModulatorsToInstruments(_currentIds[0].indexSf2);
    _sf2->endEditing(_editingSource);
    updateInterface(_editingSource);
}

void PageDefaultMod::setCustom(bool isCustom)
{
    if (_currentIds.isEmpty())
        return;
    _sf2->customizeDefaultModulators(_currentIds[0].indexSf2, isCustom);
    _sf2->endEditing(_editingSource);
    updateInterface(_editingSource);
}
