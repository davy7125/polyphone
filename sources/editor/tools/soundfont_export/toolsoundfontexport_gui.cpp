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

#include "toolsoundfontexport_gui.h"
#include "ui_toolsoundfontexport_gui.h"
#include "toolsoundfontexport_parameters.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "utils.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QShortcut>
#include <QKeyEvent>

ToolSoundfontExport_gui::ToolSoundfontExport_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolSoundfontExport_gui),
    _clickedItem(nullptr),
    _clickedItemState(Qt::Unchecked),
    _ignoreNextRelease(false)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("&Export"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
    ui->pushFolder->setIcon(ContextManager::theme()->getColoredSvg(":/icons/folder.svg", QSize(24, 24), ThemeManager::WINDOW_TEXT));

    // Filter field, focused with Ctrl+F
    QShortcut * shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    connect(shortcut, &QShortcut::activated, this, [this]() {
        ui->lineSearch->selectAll();
        ui->lineSearch->setFocus();
    });
    ui->lineSearch->installEventFilter(this);
    ui->listPresets->installEventFilter(this);
    ui->listPresets->viewport()->installEventFilter(this);
    connect(ui->listPresets, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(updateCheckedCount()));
}

ToolSoundfontExport_gui::~ToolSoundfontExport_gui()
{
    delete ui;
}

void ToolSoundfontExport_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)

    // Populate the tree
    ui->listPresets->clear();
    _clickedItem = nullptr;
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idSf2(elementSf2);
    foreach (int i, sm->getSiblings(idSf2))
    {
        EltID id(elementSf2, i);
        QString title = sm->getQstr(id, champ_name);
        if (title.isEmpty())
            title = sm->getQstr(id, champ_filenameInitial).split(QRegularExpression("(/|\\\\)")).last();
        if (title.isEmpty())
            title = tr("Untitled");
        QTreeWidgetItem * rootItem = new QTreeWidgetItem(QStringList(title));
        rootItem->setFlags(rootItem->flags() & ~Qt::ItemIsSelectable);
        ui->listPresets->addTopLevelItem(rootItem);

        id.typeElement = elementPrst;
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt = i;
            QTreeWidgetItem * item = new QTreeWidgetItem(
                        QStringList() << QString("%1:%2   %3").arg(QString::number(sm->get(id, champ_wBank).wValue), 3, QChar('0'))
                        .arg(QString::number(sm->get(id, champ_wPreset).wValue), 3, QChar('0'))
                        .arg(sm->getQstr(id, champ_name)));
            item->setCheckState(0, Qt::Checked);
            item->setData(0, Qt::UserRole, QPoint(id.indexSf2, id.indexElt));
            item->setData(0, Qt::UserRole + 1, Utils::simplifySpaces(sm->getQstr(id, champ_nameSort)));
            rootItem->addChild(item);
            rootItem->setExpanded(true);
        }

        // Tri
        rootItem->sortChildren(0, Qt::AscendingOrder);
    }
    ui->listPresets->resizeColumnToContents(0);

    // Empty filter
    ui->lineSearch->clear();
    ui->lineSearch->setFocus();
    updateCheckedCount();

    // Load parameters
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;
    int exportType = params->getFormat();
    if (exportType < 0 || exportType >= ui->comboFormat->count())
        exportType = 0;
    ui->comboFormat->setCurrentIndex(exportType);
    on_comboFormat_currentIndexChanged(exportType);

    ui->lineFolder->setText(params->getDirectory());
    ui->checkBank->setChecked(params->getBankDirectory());
    ui->checkPreset->setChecked(params->getPresetPrefix());
    ui->checkGM->setChecked(params->getGmSort());
    ui->checkFilePreset->setChecked(params->getFilePreset());
    ui->checkRawValues->setChecked(params->getCsvRaw());

    int exportQuality = params->getQuality();
    if (exportQuality < 0 || exportQuality >= ui->comboQuality->count())
        exportQuality = 1;
    ui->comboQuality->setCurrentIndex(2 - exportQuality);
}

void ToolSoundfontExport_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;

    // Save parameters
    params->setDirectory(ui->lineFolder->text());
    params->setFormat(ui->comboFormat->currentIndex());
    params->setQuality(2 - ui->comboQuality->currentIndex());
    params->setPresetPrefix(ui->checkPreset->isChecked());
    params->setBankDirectory(ui->checkBank->isChecked());
    params->setGmSort(ui->checkGM->isChecked());
    params->setFilePreset(ui->checkFilePreset->isChecked());
    params->setCsvRaw(ui->checkRawValues->isChecked());

    // Preset list
    params->setSelectedPresets(_presetList);
}

void ToolSoundfontExport_gui::on_pushTick_clicked()
{
    // Select all presets displayed by the filter
    setCheckState(getVisiblePresets(), Qt::Checked);
}

void ToolSoundfontExport_gui::on_pushUntick_clicked()
{
    // Untick all presets displayed by the filter
    setCheckState(getVisiblePresets(), Qt::Unchecked);
}

QList<QTreeWidgetItem *> ToolSoundfontExport_gui::getVisiblePresets()
{
    QList<QTreeWidgetItem *> items;
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            if (!rootItem->child(j)->isHidden())
                items << rootItem->child(j);
    }
    return items;
}

void ToolSoundfontExport_gui::setCheckState(QList<QTreeWidgetItem *> items, Qt::CheckState state)
{
    // Count updated once at the end
    ui->listPresets->blockSignals(true);
    foreach (QTreeWidgetItem * item, items)
        item->setCheckState(0, state);
    ui->listPresets->blockSignals(false);
    updateCheckedCount();
}

void ToolSoundfontExport_gui::on_lineSearch_textChanged(const QString &text)
{
    // Presets whose numbers or name contain the filter (whatever the number of spaces), soundfonts without matches are hidden
    QString filter = Utils::simplifySpaces(Utils::removeAccents(text).toLower());
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        bool rootVisible = filter.isEmpty();
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
        {
            bool visible = rootItem->child(j)->data(0, Qt::UserRole + 1).toString().contains(filter);
            rootItem->child(j)->setHidden(!visible);
            if (!visible)
                rootItem->child(j)->setSelected(false);
            rootVisible |= visible;
        }
        rootItem->setHidden(!rootVisible);
    }

    // Checked presets may now be hidden
    updateCheckedCount();
}

void ToolSoundfontExport_gui::on_listPresets_itemDoubleClicked(QTreeWidgetItem * item, int column)
{
    Q_UNUSED(column)

    // A double click anywhere on the row of a preset ticks or unticks it (soundfonts are expanded / collapsed),
    // unless its first click already did it (on the check box)
    if (item->parent() == nullptr || (item == _clickedItem && item->checkState(0) != _clickedItemState))
        return;
    item->setCheckState(0, item->checkState(0) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

void ToolSoundfontExport_gui::updateCheckedCount()
{
    int nbPresets = 0;
    int nbChecked = 0;
    int nbHiddenChecked = 0;
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
        {
            nbPresets++;
            if (rootItem->child(j)->checkState(0) == Qt::Checked)
            {
                nbChecked++;
                if (rootItem->child(j)->isHidden())
                    nbHiddenChecked++;
            }
        }
    }

    if (nbHiddenChecked > 0)
        ui->labelCount->setText(tr("Checked presets: %1 / %2 (hidden: %n)", "", nbHiddenChecked).arg(nbChecked).arg(nbPresets));
    else
        ui->labelCount->setText(tr("Checked presets: %1 / %2").arg(nbChecked).arg(nbPresets));
}

bool ToolSoundfontExport_gui::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == ui->listPresets->viewport())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            // State before the first click of a possible double click
            _clickedItem = ui->listPresets->itemAt(static_cast<QMouseEvent *>(event)->position().toPoint());
            if (_clickedItem != nullptr)
                _clickedItemState = _clickedItem->checkState(0);
            _ignoreNextRelease = false;
        }
        else if (event->type() == QEvent::MouseButtonDblClick && _clickedItem != nullptr &&
                 _clickedItem->checkState(0) != _clickedItemState)
        {
            // Double click on the check box, already changed by the first click: the tree would change it again
            // when the button is released
            _ignoreNextRelease = true;
        }
        else if (event->type() == QEvent::MouseButtonRelease && _ignoreNextRelease)
        {
            _ignoreNextRelease = false;
            return true;
        }
    }

    if (event->type() != QEvent::KeyPress)
        return AbstractToolGui::eventFilter(obj, event);

    QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
    bool isEnter = (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
                   !(keyEvent->modifiers() & Qt::ControlModifier);
    if (obj == ui->lineSearch && (keyEvent->key() == Qt::Key_Down || isEnter))
    {
        // Go to the first preset displayed (if any)
        // Enter doesn't start the export, which is still possible with Ctrl+Enter
        QTreeWidgetItem * item = nullptr;
        for (int i = 0; i < ui->listPresets->topLevelItemCount() && item == nullptr; i++)
            if (!ui->listPresets->topLevelItem(i)->isHidden())
                item = ui->listPresets->topLevelItem(i);
        while (item != nullptr && item->parent() == nullptr)
            item = ui->listPresets->itemBelow(item);
        if (item != nullptr)
        {
            ui->listPresets->setFocus();
            ui->listPresets->setCurrentItem(item);
        }
        return true;
    }

    if (obj == ui->listPresets && keyEvent->key() == Qt::Key_Space && keyEvent->modifiers() == Qt::NoModifier)
    {
        // A preset hidden by the filter cannot be changed
        QTreeWidgetItem * currentItem = ui->listPresets->currentItem();
        if (currentItem != nullptr && currentItem->isHidden())
            return true;

        // The opposite state of the current preset is applied to all selected presets
        if (currentItem != nullptr && currentItem->parent() != nullptr && currentItem->isSelected())
        {
            QList<QTreeWidgetItem *> items;
            foreach (QTreeWidgetItem * item, ui->listPresets->selectedItems())
                if (item->parent() != nullptr)
                    items << item;
            setCheckState(items, currentItem->checkState(0) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
            return true;
        }
    }

    return AbstractToolGui::eventFilter(obj, event);
}

void ToolSoundfontExport_gui::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, tr("Select the destination directory"),
                                                     ui->lineFolder->text());
    if (!qDir.isEmpty())
        ui->lineFolder->setText(qDir);
}

void ToolSoundfontExport_gui::on_buttonBox_rejected()
{
    emit this->canceled();
}

void ToolSoundfontExport_gui::on_buttonBox_accepted()
{
    // Check the directory
    if (ui->lineFolder->text().isEmpty() || !QDir(ui->lineFolder->text()).exists())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Invalid directory."));
        return;
    }

    // Update the preset list by soundfont
    _presetList.clear();
    for (int i = 0; i < ui->listPresets->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        for (int j = 0; j < rootItem->childCount(); j++)
        {
            QTreeWidgetItem * item = rootItem->child(j);
            if (item->checkState(0) == Qt::Checked)
            {
                QPoint dataPoint = item->data(0, Qt::UserRole).toPoint();
                _presetList[dataPoint.x()] << dataPoint.y();
            }
        }
    }

    // Remove empty lists
    QList<int> soundfonts = _presetList.keys();
    foreach (int soundfont, soundfonts)
        if (_presetList[soundfont].empty())
            _presetList.remove(soundfont);

    // Check on the number of selected elements
    if (_presetList.empty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("At least one preset must be selected."));
        return;
    }

    // Limits when several soundfonts are merged in a single file:
    // each soundfont is a bank (0-127) and each of its presets a preset number (0-127)
    bool filePreset = !ui->checkFilePreset->isHidden() && ui->checkFilePreset->isChecked();
    if (_presetList.count() > 1 && !filePreset)
    {
        if (_presetList.count() > 128)
        {
            QMessageBox::warning(this, tr("Warning"), tr("The maximal number of soundfont to export is 128."));
            return;
        }

        int maxNumberOfPresets = 0;
        foreach (int i, _presetList.keys())
            maxNumberOfPresets = qMax(maxNumberOfPresets, _presetList[i].count());
        if (maxNumberOfPresets > 128)
        {
            QMessageBox::warning(this, tr("Warning"), tr("In the case where several soundfonts are exported, the maximal number of presets per soundfonts is 128."));
            return;
        }
    }

    emit this->validated();
}

void ToolSoundfontExport_gui::on_comboFormat_currentIndexChanged(int index)
{
    // Options for sf3
    ui->labelQuality->setVisible(index == 1);
    ui->comboQuality->setVisible(index == 1);

    // Options for sf2, sf3 and dls
    ui->checkFilePreset->setVisible(index == 0 || index == 1 || index == 4);

    // Options for sfz
    ui->checkBank->setVisible(index == 2);
    ui->checkGM->setVisible(index == 2);
    ui->checkPreset->setVisible(index == 2);

    // Options for csv
    ui->checkRawValues->setVisible(index == 3);
}
