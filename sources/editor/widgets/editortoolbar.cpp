#include "editortoolbar.h"
#include "contextmanager.h"
#include "styledaction.h"
#include "soundfontmanager.h"
#include "toolmenu.h"

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
    _actionAddSample = new StyledAction(trUtf8("Ajouter un échantillon"), ":/icons/sample.svg", this);
    this->addWidget(_actionAddSample);

    _actionAddInstrument = new StyledAction(trUtf8("Ajouter un instrument"), ":/icons/instrument.svg", this);
    this->addWidget(_actionAddInstrument);

    _actionAddPreset = new StyledAction(trUtf8("Ajouter un preset"), ":/icons/preset.svg", this);
    this->addWidget(_actionAddPreset);

    _actionToolBox = new StyledAction(trUtf8("Boîte à outils"), ":/icons/toolbox.svg", this);
    this->addWidget(_actionToolBox);
    _toolMenu = new ToolMenu(_actionToolBox);
    _actionToolBox->setMenu(_toolMenu);
    _actionToolBox->setPopupMode(QToolButton::InstantPopup);

    this->addSeparator();
    _actionUndo = new StyledAction(trUtf8("Annuler"), ":/icons/edit-undo.svg", this);
//    _actionUndo->setShortcut(Qt::CTRL + Qt::Key_Z);
    connect(_actionUndo, SIGNAL(clicked()), this, SLOT(onUndo()));
    this->addWidget(_actionUndo);

    _actionRedo = new StyledAction(trUtf8("Rétablir"), ":/icons/edit-redo.svg", this);
    connect(_actionRedo, SIGNAL(clicked()), this, SLOT(onRedo()));
    this->addWidget(_actionRedo);
    this->addSeparator();

    _actionSave = new StyledAction(trUtf8("Sauvegarder"), ":/icons/document-save.svg", this);
    this->addWidget(_actionSave);

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->addWidget(empty);
    _displayActionSeparator = this->addSeparator();
    _displayActionSeparator->setVisible(false);

    _actionShowRecorder = new StyledAction(trUtf8("Magnétophone"), ":/icons/recorder.svg", this);
    _actionShowRecorder->setCheckable(true);
    _actionShowRecorder->setChecked(s_recorderOpen);
    connect(_actionShowRecorder, SIGNAL(clicked()), this, SLOT(onRecorderActionClicked()));
    this->addWidget(_actionShowRecorder);

    _actionShowKeyboard = new StyledAction(trUtf8("Clavier virtuel"), ":/icons/piano.svg", this);
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

    StyledAction * action = (StyledAction *)QObject::sender();
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
