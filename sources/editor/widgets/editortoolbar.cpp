/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "editortoolbar.h"
#include "contextmanager.h"
#include "styledaction.h"
#include "soundfontmanager.h"
#include "toolmenu.h"
#include "sampleloader.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "windowmanager.h"
#include "outputfactory.h"
#include "dialognewelement.h"
#include "utils.h"

bool EditorToolBar::s_recorderOpen = false;
bool EditorToolBar::s_keyboardOpen = false;
QList<EditorToolBar *> EditorToolBar::s_instances;

EditorToolBar::EditorToolBar(QWidget * parent) : QToolBar(parent),
    _sf2Index(-1),
    _updatingDisplayOptions(false)
{
    // Configuration
    this->setContextMenuPolicy(Qt::PreventContextMenu); // no "close" option

    // Style
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    this->setStyleSheet("QToolBar{border:0;background-color:" + highlightColor.name() + "}");

    // Create the actions
    _actionAddSample = new StyledAction(tr("Add a sample"), ":/icons/sample.svg", this);
    connect(_actionAddSample, SIGNAL(clicked()), this, SLOT(onNewSmplClicked()));
    this->addWidget(_actionAddSample);

    _actionAddInstrument = new StyledAction(tr("Add an instrument"), ":/icons/instrument.svg", this);
    connect(_actionAddInstrument, SIGNAL(clicked()), this, SLOT(onNewInstClicked()));
    this->addWidget(_actionAddInstrument);

    _actionAddPreset = new StyledAction(tr("Add a preset"), ":/icons/preset.svg", this);
    connect(_actionAddPreset, SIGNAL(clicked()), this, SLOT(onNewPrstClicked()));
    this->addWidget(_actionAddPreset);

    _actionToolBox = new StyledAction(tr("Toolbox"), ":/icons/toolbox.svg", this);
    this->addWidget(_actionToolBox);
    _toolMenu = new ToolMenu(_actionToolBox);
    _actionToolBox->setMenu(_toolMenu);
    _actionToolBox->setPopupMode(QToolButton::InstantPopup);

    this->addSeparator();
    _actionUndo = new StyledAction(tr("Cancel"), ":/icons/edit-undo.svg", this);
    connect(_actionUndo, SIGNAL(clicked()), this, SLOT(onUndo()));
    this->addWidget(_actionUndo);

    _actionRedo = new StyledAction(tr("Redo"), ":/icons/edit-redo.svg", this);
    connect(_actionRedo, SIGNAL(clicked()), this, SLOT(onRedo()));
    this->addWidget(_actionRedo);
    this->addSeparator();

    _actionSave = new StyledAction(tr("Save"), ":/icons/document-save.svg", this);
    connect(_actionSave, SIGNAL(clicked()), this, SLOT(onSaveClicked()));
    this->addWidget(_actionSave);

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->addWidget(empty);
    _displayActionSeparator = this->addSeparator();
    _displayActionSeparator->setVisible(false);

    _actionShowRecorder = new StyledAction(tr("Recorder"), ":/icons/recorder.svg", this);
    _actionShowRecorder->setCheckable(true);
    _actionShowRecorder->setChecked(s_recorderOpen);
    connect(_actionShowRecorder, SIGNAL(clicked()), this, SLOT(onRecorderActionClicked()));
    this->addWidget(_actionShowRecorder);

    _actionShowKeyboard = new StyledAction(tr("Virtual keyboard"), ":/icons/piano.svg", this);
    _actionShowKeyboard->setCheckable(true);
    _actionShowKeyboard->setChecked(s_keyboardOpen);
    connect(_actionShowKeyboard, SIGNAL(clicked()), this, SLOT(onKeyboardActionClicked()));
    this->addWidget(_actionShowKeyboard);

    s_instances << this;
}

EditorToolBar::~EditorToolBar()
{
    s_instances.removeAll(this);
    if (s_instances.empty())
    {
        s_keyboardOpen = false;
        s_recorderOpen = false;
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
    _actionToolBox->disable(_toolMenu->isEmpty());
}

void EditorToolBar::updateActions()
{
    _actionAddSample->disable(false);
    _actionAddInstrument->disable(false);
    _actionAddPreset->disable(false);

    SoundfontManager * sf2 = SoundfontManager::getInstance();
    _actionSave->disable(!sf2->isEdited(_sf2Index));
    _actionRedo->disable(!sf2->isRedoable(_sf2Index));
    _actionUndo->disable(!sf2->isUndoable(_sf2Index));
}

void EditorToolBar::disable()
{
    _actionAddSample->disable(true);
    _actionAddInstrument->disable(true);
    _actionAddPreset->disable(true);
    _actionToolBox->disable(true);
    _actionUndo->disable(true);
    _actionRedo->disable(true);
    _actionSave->disable(true);
}

void EditorToolBar::onUndo()
{
    SoundfontManager::getInstance()->undo(_sf2Index);
}

void EditorToolBar::onRedo()
{
    SoundfontManager::getInstance()->redo(_sf2Index);
}

void EditorToolBar::setDisplayOptions(QList<Page::DisplayOption> displayOptions)
{
    // First clear all display options
    while (!_displayActions.isEmpty())
        delete _displayActions.takeFirst();

    // Then create new display options
    _displayActionSeparator->setVisible(!displayOptions.isEmpty());
    foreach (Page::DisplayOption displayOption, displayOptions)
    {
        StyledAction * action = new StyledAction(displayOption._label, displayOption._iconName, this);
        action->setCheckable(true);
        action->setData(displayOption._id);
        action->setEnabled(displayOption._isEnabled);
        action->setChecked(displayOption._isSelected);
        connect(action, SIGNAL(clicked()), this, SLOT(onDisplayActionClicked()));
        this->insertWidget(_displayActionSeparator, action);
        _displayActions << action;
    }
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


void EditorToolBar::onRecorderActionClicked()
{
    bool isDisplayed = _actionShowRecorder->isChecked();
    updateRecorderButtonsState(isDisplayed);
    emit(recorderDisplayChanged(isDisplayed));
}

void EditorToolBar::onKeyboardActionClicked()
{
    bool isDisplayed = _actionShowKeyboard->isChecked();
    updateKeyboardButtonsState(isDisplayed);
    emit(keyboardDisplayChanged(isDisplayed));
}

void EditorToolBar::updateRecorderButtonsState(bool isChecked)
{
    s_recorderOpen = isChecked;
    foreach (EditorToolBar * toolBar, s_instances)
    {
        toolBar->blockSignals(true);
        toolBar->_actionShowRecorder->setChecked(s_recorderOpen);
        toolBar->blockSignals(false);
    }
}

void EditorToolBar::updateKeyboardButtonsState(bool isChecked)
{
    s_keyboardOpen = isChecked;
    foreach (EditorToolBar * toolBar, s_instances)
    {
        toolBar->blockSignals(true);
        toolBar->_actionShowKeyboard->setChecked(s_keyboardOpen);
        toolBar->blockSignals(false);
    }
}

void EditorToolBar::onNewSmplClicked()
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
                                                        tr("Audio files") + " (*.wav *.flac" + ext + ")");

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
    selectionChanged(smplList);
}

void EditorToolBar::onNewInstClicked()
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
        foreach (EltID idSrc, ids)
        {
            // Create a division
            idLink.indexElt2 = sm->add(idLink);

            // Association of the sample in the instrument
            val.wValue = idSrc.indexElt;
            sm->set(idLink, champ_sampleID, val);

            // Pan
            if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample ||
                    sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
                val.shValue = 500;
            else if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample ||
                     sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomLeftSample)
                val.shValue = -500;
            else
                val.shValue = 0;
            sm->set(idLink, champ_pan, val);
        }
    }

    sm->endEditing("command:newInst");

    // Selection
    selectionChanged(id);
}

void EditorToolBar::onNewPrstClicked()
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
    sm->endEditing("command:newPrst");

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

    // Selection
    selectionChanged(id);
}

void EditorToolBar::onSaveClicked()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(WindowManager::getInstance()->getCurrentSf2(), false);
}
