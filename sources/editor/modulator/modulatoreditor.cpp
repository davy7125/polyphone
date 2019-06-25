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

#include "modulatoreditor.h"
#include "ui_modulatoreditor.h"
#include "contextmanager.h"
#include "modulatorcell.h"
#include "soundfontmanager.h"

QList<ModulatorEditor *> ModulatorEditor::s_instances;

ModulatorEditor::ModulatorEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorEditor)
{
    s_instances << this;
    ui->setupUi(this);

    // Icons
    ui->pushExpand->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_up.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushAdd->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushCopy->setIcon(ContextManager::theme()->getColoredSvg(":/icons/copy.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushPaste->setIcon(ContextManager::theme()->getColoredSvg(":/icons/paste.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushClone->setIcon(ContextManager::theme()->getColoredSvg(":/icons/clone.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushDelete->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushCollapse->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    // Style
    ui->frameButtons->setStyleSheet("QFrame{border:1px solid " +
                                    this->palette().dark().color().name() +
                                    ";border-top:0;border-bottom:0;border-left:0}");
    ui->labelNoModulators->setStyleSheet("QLabel{color:" + this->palette().dark().color().name() +
                                         ";background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                         "}");
    ui->labelSelectDivision->setStyleSheet("QLabel{color:" + this->palette().dark().color().name() +
                                           ";background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                           "}");

    // Initialize the expanded / collapsed state
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", false).toBool())
        on_pushCollapse_clicked();
    else
        on_pushExpand_clicked();

    // Buttons
    updateButtons(false);
}

ModulatorEditor::~ModulatorEditor()
{
    s_instances.removeOne(this);
    delete ui;
}

void ModulatorEditor::on_pushExpand_clicked()
{
    foreach (ModulatorEditor * elt, s_instances)
    {
        elt->ui->frameCollapsed->hide();
        elt->ui->frameExpanded->show();
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", false);
}

void ModulatorEditor::on_pushCollapse_clicked()
{
    foreach (ModulatorEditor * elt, s_instances)
    {
        elt->ui->frameExpanded->hide();
        elt->ui->frameCollapsed->show();
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", true);
}

void ModulatorEditor::setIds(IdList ids, QList<AttributeType> attributes)
{
    // Reset buttons
    ui->pushAdd->setEnabled(false);
    ui->pushClone->setEnabled(false);
    ui->pushCopy->setEnabled(false);
    ui->pushDelete->setEnabled(false);
    ui->pushPaste->setEnabled(false);

    if (ids.empty())
    {
        // Shouldn't happen
        _currentId.typeElement = elementUnknown;
        ui->stackedWidget->setCurrentIndex(0);
        ui->labelModSummary->setText("<b>" + trUtf8("Modulators:") + "</b> -");

    }
    else if (ids.count() > 1)
    {
        _currentId.typeElement = elementUnknown;
        ui->stackedWidget->setCurrentIndex(1);
        ui->labelModSummary->setText("<b>" + trUtf8("Modulators:") + "</b> -");
    }
    else
    {
        // Buttons
        ui->pushAdd->setEnabled(true);
        ui->pushCopy->setEnabled(true);
        ui->pushPaste->setEnabled(true);

        // Update the interface with the current division
        _currentId = ids[0];

        // Adapt the attributes to select
        if (attributes.contains(champ_startAddrsOffset))
            attributes << champ_startAddrsCoarseOffset;
        if (attributes.contains(champ_endAddrsOffset))
            attributes << champ_endAddrsCoarseOffset;
        if (attributes.contains(champ_startloopAddrsOffset))
            attributes << champ_startloopAddrsCoarseOffset;
        if (attributes.contains(champ_endloopAddrsOffset))
            attributes << champ_endloopAddrsCoarseOffset;

        // Update the interface
        updateInterface(attributes);
    }
}

void ModulatorEditor::updateInterface(QList<AttributeType> attributes)
{
    // List of mods associated to the current id
    EltID modId = _currentId;
    switch (_currentId.typeElement)
    {
    case elementInst:
        modId.typeElement = elementInstMod;
        break;
    case elementPrst:
        modId.typeElement = elementPrstMod;
        break;
    case elementInstSmpl:
        modId.typeElement = elementInstSmplMod;
        break;
    case elementPrstInst:
        modId.typeElement = elementPrstInstMod;
        break;
    default:
        modId.typeElement = elementUnknown;
        break;
    }

    // Clear the cell list
    ui->listWidget->clear();

    // Foreach modulator
    QStringList modTargets;
    int modCount = 0;
    foreach (int i, SoundfontManager::getInstance()->getSiblings(modId))
    {
        modId.indexMod = i;

        // Target list
        AttributeValue value = SoundfontManager::getInstance()->get(modId, champ_sfModDestOper);
        if (value.wValue < 99) // Link are rejected in the summary
            modTargets << Attribute::getDescription(value.sfGenValue, modId.typeElement == elementPrstMod || modId.typeElement == elementPrstInstMod);

        // Add a new cell
        ModulatorCell * cell = new ModulatorCell(modId);
        QListWidgetItem * item = new QListWidgetItem();
        item->setSizeHint(cell->size());
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, cell);

        // Selection
        item->setSelected(attributes.contains((AttributeType)value.wValue));

        // Modulator count
        modCount++;
    }
    ui->stackedWidget->setCurrentIndex(modCount == 0 ? 0 : 2);

    // Fill the summary
    QString summary = "<b>" + trUtf8("Modulators (%1): ").arg(modCount) + "</b> ";
    if (modTargets.count() == 0)
        summary += "-";
    else
        for (int i = 0; i < modTargets.count(); i++)
            summary += (i > 0 ? ", " : "") + modTargets[i];
    ui->labelModSummary->setText(summary);

    // Button visibility
    ui->pushClone->setEnabled(modCount > 0 && (_currentId.typeElement == elementInst || _currentId.typeElement == elementPrst));
    ui->pushCopy->setEnabled(modCount > 0);
}

void ModulatorEditor::on_listWidget_itemSelectionChanged()
{
    // Prepare the list of selected attributes
    QList<QListWidgetItem *> selection = ui->listWidget->selectedItems();
    QList<AttributeType> attributes;
    foreach (QListWidgetItem * item, selection)
    {
        ModulatorCell * cell = (ModulatorCell *)ui->listWidget->itemWidget(item);
        AttributeType attribute = cell->getTargetAttribute();
        if (attribute != champ_unknown && !attributes.contains(attribute))
            attributes << attribute;
    }

    // Notify the change
    emit(attributesSelected(attributes));

    // Adapt the button tooltips
    updateButtons(!selection.isEmpty());
}

void ModulatorEditor::updateButtons(bool withSelection)
{
    if (withSelection)
    {
        ui->pushClone->setToolTip(trUtf8("Duplicate the selection toward..."));
        ui->pushCopy->setToolTip(trUtf8("Copy the selected modulators"));
        ui->pushDelete->setEnabled(true);
    }
    else
    {
        ui->pushClone->setToolTip(trUtf8("Duplicate modulators toward..."));
        ui->pushCopy->setToolTip(trUtf8("Copy all modulators"));
        ui->pushDelete->setEnabled(false);
    }
}
