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

#include "editortoolbar.h"
#include "ui_editortoolbar.h"
#include "contextmanager.h"
#include "styledaction.h"
#include "soundfontmanager.h"
#include "toolmenu.h"
#include "sampleloader.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "tabmanager.h"
#include "outputfactory.h"
#include "dialognewelement.h"
#include "utils.h"
#include "tools/auto_distribution/toolautodistribution.h"
#include "extensionmanager.h"

bool EditorToolBar::s_recorderOpen = false;
bool EditorToolBar::s_keyboardOpen = false;
QMap<int, bool> EditorToolBar::s_midiExtensionOpen;
QList<EditorToolBar *> EditorToolBar::s_instances;

EditorToolBar::EditorToolBar(QWidget * parent) : QWidget(parent),
    ui(new Ui::EditorToolBar),
    _sf2Index(-1),
    _updatingDisplayOptions(false)
{
    ui->setupUi(this);

    // Style
    setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("EditorToolBar{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + "}");
    QString separatorStyleSheet = "QFrame{border: 1px solid " +
            ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
            ";margin: 7px 3px;}";
    ui->separator_1->setStyleSheet(separatorStyleSheet);
    ui->separator_2->setStyleSheet(separatorStyleSheet);
    ui->separator_3->setStyleSheet(separatorStyleSheet);
    ui->separator_3->setVisible(false);

    // Initialize the actions
    ui->pushAddSample->initialize(tr("Add a sample"), ":/icons/sample.svg");
    ui->pushAddInstrument->initialize(tr("Add an instrument"), ":/icons/instrument.svg");
    ui->pushAddPreset->initialize(tr("Add a preset"), ":/icons/preset.svg");
    ui->pushUndo->initialize(tr("Cancel"), ":/icons/edit-undo.svg");
    ui->pushRedo->initialize(tr("Redo"), ":/icons/edit-redo.svg");
    ui->pushSave->initialize(tr("Save"), ":/icons/document-save.svg");

    ui->pushToolBox->initialize(tr("Toolbox"), ":/icons/toolbox.svg");
    _toolMenu = new ToolMenu(ui->pushToolBox);
    ui->pushToolBox->setMenu(_toolMenu);
    ui->pushToolBox->setMinimumWidth(46);
    ui->pushToolBox->setStyleSheet("StyledAction{padding-right: 6px;border:0} StyledAction::menu-indicator {image: url(:/icons/arrow_down.svg); height: 12px; width: 12px;}");

    ui->pushShowRecorder->initialize(tr("Recorder"), ":/icons/recorder.svg");
    ui->pushShowRecorder->blockSignals(true);
    ui->pushShowRecorder->setChecked(s_recorderOpen);
    ui->pushShowRecorder->blockSignals(false);

    ui->pushShowKeyboard->initialize(tr("Virtual keyboard"), ":/icons/piano.svg");
    ui->pushShowKeyboard->blockSignals(true);
    ui->pushShowKeyboard->setChecked(s_keyboardOpen);
    ui->pushShowKeyboard->blockSignals(false);

    // Possible extensions
    for (int i = 0; i < ExtensionManager::midi()->count(); i++)
    {
        StyledAction * action = new StyledAction(this);
        action->setData(i);
        action->setCheckable(true);
        ui->horizontalLayout->addWidget(action);
        action->initialize(ExtensionManager::midi()->getTitle(i), ExtensionManager::midi()->getIconPath(i));
        if (!s_midiExtensionOpen.contains(i))
            s_midiExtensionOpen[i] = false;
        QDialog * dialog = ExtensionManager::midi()->getDialog(i);
        connect(dialog, SIGNAL(finished(int)), this, SLOT(onExtensionDialogClosed(int)));

        action->setChecked(s_midiExtensionOpen[i]);
        connect(action, SIGNAL(clicked()), this, SLOT(onMidiExtensionActionClicked()));
        _midiControllerActions << action;
    }

    s_instances << this;
}

EditorToolBar::~EditorToolBar()
{
    delete ui;
    s_instances.removeAll(this);
    if (s_instances.empty())
    {
        s_keyboardOpen = false;
        s_recorderOpen = false;

        for (int i = 0; i < ExtensionManager::midi()->count(); i++)
        {
            s_midiExtensionOpen[i] = false;
            QDialog * dialog = ExtensionManager::midi()->getDialog(i);
            dialog->setVisible(false);
        }
    }
}

void EditorToolBar::setSf2Index(int sf2index)
{
    _sf2Index = sf2index;
    updateActions();
}

void EditorToolBar::onSelectionChanged(IdList ids)
{
    _currentSelection = ids;
    _toolMenu->selectionChanged(ids);
    ui->pushToolBox->disable(_toolMenu->isEmpty());
}

void EditorToolBar::updateActions()
{
    ui->pushAddSample->disable(false);
    ui->pushAddInstrument->disable(false);
    ui->pushAddPreset->disable(false);

    SoundfontManager * sf2 = SoundfontManager::getInstance();
    ui->pushSave->disable(!sf2->isEdited(_sf2Index));
    ui->pushRedo->disable(!sf2->isRedoable(_sf2Index));
    ui->pushUndo->disable(!sf2->isUndoable(_sf2Index));
}

void EditorToolBar::disable()
{
    ui->pushAddSample->disable(true);
    ui->pushAddInstrument->disable(true);
    ui->pushAddPreset->disable(true);
    ui->pushToolBox->disable(true);
    ui->pushUndo->disable(true);
    ui->pushRedo->disable(true);
    ui->pushSave->disable(true);
}

void EditorToolBar::clearDisplayOptions()
{
    while (!_displayActions.isEmpty())
        delete _displayActions.takeFirst();
}

void EditorToolBar::setDisplayOptions(QList<Page *> possiblePages, Page *selectedPage)
{
    // First clear all display options
    this->clearDisplayOptions();

    // Then create new display options, only if there is more than one
    if (possiblePages.length() > 1)
    {
        ui->separator_3->show();
        for (int i = 0; i < possiblePages.length(); i++)
        {
            Page * page = possiblePages[i];
            StyledAction * action = new StyledAction(this);
            QString iconName = page->getIconName();
            if (iconName.isEmpty())
                iconName = ":/icons/pen.svg";
            action->initialize(page->getLabel(), iconName);
            action->setCheckable(true);
            action->setData(i);
            action->setChecked(page == selectedPage);
            connect(action, SIGNAL(clicked()), this, SLOT(onDisplayActionClicked()));
            ui->layoutDisplay->addWidget(action);
            _displayActions << action;
        }
    }
    else
        ui->separator_3->hide();
}

void EditorToolBar::onDisplayActionClicked()
{
    if (_updatingDisplayOptions)
        return;

    StyledAction * action = dynamic_cast<StyledAction *>(QObject::sender());
    selectDisplayOption(action->getData());
    emit(displayOptionChanged(action->getData()));
}

void EditorToolBar::disableDisplayOption(QList<int> disabledOptions)
{
    foreach (StyledAction * action, _displayActions)
        action->disable(disabledOptions.contains(action->getData()));
}

void EditorToolBar::selectDisplayOption(int displayOption)
{
    _updatingDisplayOptions = true;
    foreach (StyledAction * action, _displayActions) {
        action->setChecked(action->getData() == displayOption);
    }
    _updatingDisplayOptions = false;
}

void EditorToolBar::on_pushUndo_clicked()
{
    SoundfontManager::getInstance()->undo(_sf2Index);
}

void EditorToolBar::on_pushRedo_clicked()
{
    SoundfontManager::getInstance()->redo(_sf2Index);
}

void EditorToolBar::on_pushShowRecorder_clicked()
{
    bool isDisplayed = ui->pushShowRecorder->isChecked();
    updateRecorderButtonsState(isDisplayed);
    emit(recorderDisplayChanged(isDisplayed));
}

void EditorToolBar::on_pushShowKeyboard_clicked()
{
    bool isDisplayed = ui->pushShowKeyboard->isChecked();
    updateKeyboardButtonsState(isDisplayed);
    emit(keyboardDisplayChanged(isDisplayed));
}

void EditorToolBar::updateRecorderButtonsState(bool isChecked)
{
    s_recorderOpen = isChecked;
    foreach (EditorToolBar * toolBar, s_instances)
    {
        toolBar->blockSignals(true);
        toolBar->ui->pushShowRecorder->setChecked(s_recorderOpen);
        toolBar->blockSignals(false);
    }
}

void EditorToolBar::updateKeyboardButtonsState(bool isChecked)
{
    s_keyboardOpen = isChecked;
    foreach (EditorToolBar * toolBar, s_instances)
    {
        toolBar->blockSignals(true);
        toolBar->ui->pushShowKeyboard->setChecked(s_keyboardOpen);
        toolBar->blockSignals(false);
    }
}

void EditorToolBar::on_pushAddSample_clicked()
{
    // Other allowed format?
    QString ext = "";
    typedef QString (*MyPrototype)();
    MyPrototype myFunction = reinterpret_cast<MyPrototype>(QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extensions", "getExtensionFilter"));
    if (myFunction)
        ext = myFunction();

    // Display dialog
    QStringList strList = QFileDialog::getOpenFileNames(this, tr("Import an audio file"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE),
                                                        tr("Audio files") + " (*.wav *.flac *.ogg" + ext + ")");

    if (strList.count() == 0)
        return;

    // Add samples
    SampleLoader sl(this);
    int replace = 0;
    IdList smplList;
    while (!strList.isEmpty())
        smplList << sl.load(strList.takeAt(0), _sf2Index, &replace);
    SoundfontManager::getInstance()->endEditing("command:newSmpl");

    // Selection
    if (!smplList.isEmpty())
        selectionChanged(smplList);
}

void EditorToolBar::on_pushAddInstrument_clicked()
{
    EltID id(elementSf2, _sf2Index);
    if (!SoundfontManager::getInstance()->isValid(id))
        return;

    // Name of all selected samples
    IdList ids = _currentSelection.getSelectedIds(elementSmpl);
    QStringList names;
    foreach (EltID id, ids)
        names << SoundfontManager::getInstance()->getQstr(id, champ_name);

    // Open a dialog
    DialogNewElement * dial = new DialogNewElement(this);
    dial->initialize(false, !ids.isEmpty(), Utils::commonPart(names));
    connect(dial, SIGNAL(onOk(QString, bool)), this, SLOT(onNewInstClicked(QString, bool)));
    dial->show();
}

void EditorToolBar::onNewInstClicked(QString name, bool linkElements)
{
    if (name.isEmpty())
        return;

    // Add a new instrument
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, _sf2Index);
    id.typeElement = elementInst;
    id.indexElt = sm->add(id);
    sm->set(id, champ_name, name.left(20));

    if (linkElements)
    {
        // Link all selected samples
        IdList ids = _currentSelection.getSelectedIds(elementSmpl);
        EltID idLink = id;
        idLink.typeElement = elementInstSmpl;
        AttributeValue val;
        bool sampleLooped = true;
        bool samePitch = false;
        QMap<unsigned char, QPair<bool, bool> > pitches;
        foreach (EltID idSrc, ids)
        {
            // Create a division
            idLink.indexElt2 = sm->add(idLink);

            // Association of the sample in the instrument
            val.wValue = idSrc.indexElt;
            sm->set(idLink, champ_sampleID, val);

            // Pan
            SFSampleLink sampleType = sm->get(idSrc, champ_sfSampleType).sfLinkValue;
            if (sampleType == rightSample || sampleType == RomRightSample)
                val.shValue = 500;
            else if (sampleType == leftSample || sampleType == RomLeftSample)
                val.shValue = -500;
            else
                val.shValue = 0;
            sm->set(idLink, champ_pan, val);

            // Check the sample is looped
            if (sm->get(idSrc, champ_dwStartLoop).dwValue == sm->get(idSrc, champ_dwEndLoop).dwValue)
                sampleLooped = false;

            // Store the pitch
            int bPitch = sm->get(idSrc, champ_byOriginalPitch).bValue;
            if (!pitches.contains(bPitch))
            {
                pitches[bPitch].first = false;
                pitches[bPitch].second = false;
            }
            if (sampleType != rightSample && sampleType != RomRightSample)
            {
                if (pitches[bPitch].first)
                    samePitch = true;
                else
                    pitches[bPitch].first = true;
            }
            if (sampleType != leftSample && sampleType != RomLeftSample)
            {
                if (pitches[bPitch].second)
                    samePitch = true;
                else
                    pitches[bPitch].second = true;
            }
        }

        // If all samples are looped, enable it in the instrument
        if (!ids.empty() && sampleLooped)
        {
            val.wValue = 1;
            sm->set(id, champ_sampleModes, val);
        }

        // If there is more than 1 pitch and no more than 1 sample per pitch / side, distribute them
        if (!samePitch && pitches.count() > 1)
        {
            ToolAutoDistribution tool;
            tool.process(sm, id, NULL);
        }
    }

    sm->endEditing("command:newInst");

    // Selection
    selectionChanged(id);
}

void EditorToolBar::on_pushAddPreset_clicked()
{
    EltID id(elementSf2, _sf2Index);
    if (!SoundfontManager::getInstance()->isValid(id))
        return;

    // List of all selected instruments
    IdList ids = _currentSelection.getSelectedIds(elementInst);
    QStringList names;
    foreach (EltID id, ids)
        names << SoundfontManager::getInstance()->getQstr(id, champ_name);

    // Open a dialog
    DialogNewElement * dial = new DialogNewElement(this);
    dial->initialize(true, !ids.isEmpty(), Utils::commonPart(names));
    connect(dial, SIGNAL(onOk(QString, bool)), this, SLOT(onNewPrstClicked(QString, bool)));
    dial->show();
}

void EditorToolBar::onNewPrstClicked(QString name, bool linkElements)
{
    if (name.isEmpty())
        return;

    // Check that a preset is available
    int nPreset = -1;
    int nBank = -1;
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, _sf2Index);
    sm->firstAvailablePresetBank(id, nBank, nPreset);
    if (nBank < 0 || nPreset < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot create more presets."));
        return;
    }

    // Add a new preset
    id.typeElement = elementPrst;
    id.indexElt = sm->add(id);
    sm->set(id, champ_name, name.left(20));
    AttributeValue val;
    val.wValue = nPreset;
    sm->set(id, champ_wPreset, val);
    val.wValue = nBank;
    sm->set(id, champ_wBank, val);

    if (linkElements)
    {
        // Link all selected instruments
        IdList ids = _currentSelection.getSelectedIds(elementInst);
        EltID idLink = id;
        idLink.typeElement = elementPrstInst;
        AttributeValue val;
        foreach (EltID idSrc, ids)
        {
            // Create a division
            idLink.indexElt2 = sm->add(idLink);

            // Association of the sample in the instrument
            val.wValue = idSrc.indexElt;
            sm->set(idLink, champ_instrument, val);

            // Key range
            int keyMin = 127;
            int keyMax = 0;
            EltID idInstSmpl = idSrc;
            idInstSmpl.typeElement = elementInstSmpl;
            foreach (int i, sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                if (sm->isSet(idInstSmpl, champ_keyRange))
                {
                    keyMin = qMin(keyMin, (int)sm->get(idInstSmpl, champ_keyRange).rValue.byLo);
                    keyMax = qMax(keyMax, (int)sm->get(idInstSmpl, champ_keyRange).rValue.byHi);
                }
            }
            AttributeValue value;
            if (keyMin < keyMax)
            {
                value.rValue.byLo = keyMin;
                value.rValue.byHi = keyMax;
            }
            else
            {
                value.rValue.byLo = 0;
                value.rValue.byHi = 127;
            }
            sm->set(idLink, champ_keyRange, value);
        }
    }

    sm->endEditing("command:newPrst");

    // Selection
    selectionChanged(id);
}

void EditorToolBar::on_pushSave_clicked()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(TabManager::getInstance()->getCurrentSf2(), false);
}

void EditorToolBar::onMidiExtensionActionClicked()
{
    // Update the state of the other buttons
    StyledAction * action = dynamic_cast<StyledAction *>(QObject::sender());
    updateMidiExtensionButtonsState(action->getData(), action->isChecked());

    // Show or hide the dialog
    QDialog * dialog = ExtensionManager::midi()->getDialog(action->getData());
    dialog->setVisible(action->isChecked());
}

void EditorToolBar::onExtensionDialogClosed(int result)
{
    Q_UNUSED(result)

    QDialog * sender = dynamic_cast<QDialog *>(QObject::sender());
    for (int i = 0; i < ExtensionManager::midi()->count(); i++)
    {
        if (ExtensionManager::midi()->getDialog(i) == sender)
        {
            s_midiExtensionOpen[i] = false;
            this->blockSignals(true);
            this->_midiControllerActions[i]->setChecked(false);
            this->blockSignals(false);
            break;
        }
    }
}

void EditorToolBar::updateMidiExtensionButtonsState(int midiExtensionIndex, bool isChecked)
{
    s_midiExtensionOpen[midiExtensionIndex] = isChecked;
    foreach (EditorToolBar * toolBar, s_instances)
    {
        toolBar->blockSignals(true);
        toolBar->_midiControllerActions[midiExtensionIndex]->setChecked(isChecked);
        toolBar->blockSignals(false);
    }
}
