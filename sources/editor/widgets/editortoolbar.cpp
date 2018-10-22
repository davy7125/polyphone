#include "editortoolbar.h"
#include "contextmanager.h"
#include "styledaction.h"
#include "soundfontmanager.h"
#include "toolmenu.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "trim_start/tooltrimstart.h"
#include "trim_end/tooltrimend.h"

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
    connect(_actionAddSample, SIGNAL(clicked()), this, SLOT(onNewSmplClicked()));
    this->addWidget(_actionAddSample);

    _actionAddInstrument = new StyledAction(trUtf8("Ajouter un instrument"), ":/icons/instrument.svg", this);
    connect(_actionAddInstrument, SIGNAL(clicked()), this, SLOT(onNewInstClicked()));
    this->addWidget(_actionAddInstrument);

    _actionAddPreset = new StyledAction(trUtf8("Ajouter un preset"), ":/icons/preset.svg", this);
    connect(_actionAddPreset, SIGNAL(clicked()), this, SLOT(onNewPrstClicked()));
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
    connect(_actionSave, SIGNAL(clicked()), this, SLOT(onSaveClicked()));
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

void EditorToolBar::onNewSmplClicked()
{
    // Other allowed format?
    QString ext = "";
    typedef QString (*MyPrototype)();
    MyPrototype myFunction = (MyPrototype) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extensions", "getExtensionFilter");
    if (myFunction)
        ext = myFunction();

    // Display dialog
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Importer un fichier audio"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE),
                                                        trUtf8("Fichier .wav") + " (*.wav)" + ext);

    if (strList.count() == 0)
        return;

    // Add samples
    EltID id(elementSf2, _sf2Index);
    int replace = 0;
    IdList smplList;
    while (!strList.isEmpty())
        smplList << this->addSmpl(strList.takeAt(0), id, &replace);
    SoundfontManager::getInstance()->endEditing("smpl:new");

    // Selection
    selectionChanged(smplList);
}

void EditorToolBar::onNewInstClicked()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, _sf2Index);
    if (!sm->isValid(id))
        return;

    bool ok;
    QString name = QInputDialog::getText(this, trUtf8("Créer un nouvel instrument"), trUtf8("Nom du nouvel instrument :"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty())
    {
        // Add a new instrument
        id.typeElement = elementInst;
        id.indexElt = sm->add(id);
        sm->set(id, champ_name, name.left(20));
        sm->endEditing("inst:new");

        // Selection
        selectionChanged(id);
    }
}

void EditorToolBar::onNewPrstClicked()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, _sf2Index);
    if (!sm->isValid(id))
        return;

    // Check that a preset is available
    int nPreset = -1;
    int nBank = -1;
    sm->firstAvailablePresetBank(id, nBank, nPreset);
    if (nPreset == -1)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucun preset n'est disponible."));
        return;
    }

    // Default name of the preset
    QString text = "";
    if (_currentSelection.getSelectedIds(elementInst).count() == 1)
    {
        EltID selectedInst = _currentSelection.getSelectedIds(elementInst)[0];
        text = sm->getQstr(selectedInst, champ_name);
    }

    bool ok;
    QString name = QInputDialog::getText(this, trUtf8("Créer un nouveau preset"), trUtf8("Nom du nouveau preset :"), QLineEdit::Normal, text, &ok);
    if (ok && !name.isEmpty())
    {
        // Add a new preset
        id.typeElement = elementPrst;
        id.indexElt = sm->add(id);
        sm->set(id, champ_name, name.left(20));
        AttributeValue val;
        val.wValue = nPreset;
        sm->set(id, champ_wPreset, val);
        val.wValue = nBank;
        sm->set(id, champ_wBank, val);
        sm->endEditing("prst:new");

        // Selection
        selectionChanged(id);
    }
}

IdList EditorToolBar::addSmpl(QString path, EltID id, int * replace)
{
    id.typeElement = elementSmpl;
    IdList addedSmpl;

    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, path);
    QFileInfo qFileInfo = path;

    // Get information about a sample
    Sound * son = new Sound(path);
    int nChannels = son->get(champ_wChannels);

    // Add a sample
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    QString nom = qFileInfo.completeBaseName();

    // Replacement ?
    int indexL = -1;
    int indexR = -1;
    QString qStr3 = "";
    if (*replace != -1)
    {
        foreach (int j, sm->getSiblings(id))
        {
            id.indexElt = j;
            if (nChannels == 2)
            {
                if (sm->getQstr(id, champ_name).compare(nom.left(19).append("L")) == 0)
                {
                    indexL = j;
                    qStr3 = trUtf8("L'échantillon « ") + nom.left(19).toUtf8() +
                            trUtf8("L » existe déjà.<br />Que faire ?");
                }
                else if (sm->getQstr(id, champ_name).compare(nom.left(19).append("R")) == 0)
                {
                    indexR = j;
                    qStr3 = trUtf8("L'échantillon « ") + nom.left(19).toUtf8() +
                            trUtf8("R » existe déjà.<br />Que faire ?");
                }
            }
            else
            {
                if (sm->getQstr(id, champ_name).compare(nom.left(20)) == 0)
                {
                    indexL = j;
                    qStr3 = trUtf8("L'échantillon « ") + nom.left(20).toUtf8() +
                            trUtf8(" » existe déjà.<br />Que faire ?");
                }
            }
        }
        if (*replace != 2 && *replace != 4 && (indexL != -1 || indexR != -1))
        {
            // Remplacement ?
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(qStr3);
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save |
                                      QMessageBox::NoAll | QMessageBox::No);
            msgBox.button(QMessageBox::Yes)->setText(trUtf8("&Remplacer"));
            msgBox.button(QMessageBox::YesAll)->setText(trUtf8("R&emplacer tout"));
            msgBox.button(QMessageBox::Save)->setText(trUtf8("&Dupliquer"));
            msgBox.button(QMessageBox::SaveAll)->setText(trUtf8("D&upliquer tout"));
            msgBox.button(QMessageBox::No)->setText(trUtf8("&Ignorer"));
            msgBox.button(QMessageBox::NoAll)->setText(trUtf8("I&gnorer tout"));
            msgBox.setDefaultButton(QMessageBox::YesAll);
            switch (msgBox.exec())
            {
            case QMessageBox::NoAll:    *replace =  4; break;
            case QMessageBox::No:       *replace =  3; break;
            case QMessageBox::YesAll:   *replace =  2; break;
            case QMessageBox::Yes:      *replace =  1; break;
            case QMessageBox::Save:     *replace =  0; break;
            case QMessageBox::SaveAll:  *replace = -1; break;
            }
        }
    }

    // Ajustement du nom, s'il est déjà utilisé
    if (*replace == 0 || *replace == -1)
    {
        QStringList listSampleName;
        foreach (int j, sm->getSiblings(id))
        {
            id.indexElt = j;
            listSampleName << sm->getQstr(id, champ_name);
        }

        int suffixNumber = 0;
        if (nChannels == 1)
        {
            while (listSampleName.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 20, suffixNumber);
        }
        else
        {
            while ((listSampleName.contains(getName(nom, 19, suffixNumber) + "L", Qt::CaseInsensitive) ||
                    listSampleName.contains(getName(nom, 19, suffixNumber) + "R", Qt::CaseInsensitive)) &&
                   suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 19, suffixNumber);
        }
    }

    for (int j = 0; j < nChannels; j++)
    {
        if (*replace < 3 || (nChannels == 2 && j == 0 && indexL == -1) ||
                (nChannels == 2 && j == 1 && indexR == -1) ||
                (nChannels == 1 && indexL == -1)) // Si pas ignorer
        {
            if (((nChannels == 2 && j == 0 && indexL != -1) ||
                 (nChannels == 2 && j == 1 && indexR != -1) ||
                 (nChannels == 1 && indexL != -1)) && (*replace == 2 || *replace == 1))
            {
                if ((nChannels == 2 && j == 0 && indexL != -1) || (nChannels == 1 && indexL != -1))
                    id.indexElt = indexL;
                else
                    id.indexElt = indexR;

                // Mise à jour des données
                AttributeValue valTmp;
                valTmp.wValue = j;
                son->set(champ_wChannel, valTmp);
                QByteArray data = son->getData(24);
                sm->set(id, champ_sampleDataFull24, data);
            }
            else
            {
                // Ajout d'un sample
                id.indexElt = sm->add(id);
                addedSmpl << id;
                if (nChannels == 2)
                {
                    if (j == 0)
                    {
                        // Left
                        sm->set(id, champ_name, nom.left(19).append("L"));
                        val.wValue = id.indexElt + 1;
                        sm->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = leftSample;
                        sm->set(id, champ_sfSampleType, val);
                    }
                    else
                    {
                        // Right
                        sm->set(id, champ_name, nom.left(19).append("R"));
                        val.wValue = id.indexElt - 1;
                        sm->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = rightSample;
                        sm->set(id, champ_sfSampleType, val);
                    }
                }
                else
                {
                    sm->set(id, champ_name, QString(nom.left(20)));
                    val.wValue = 0;
                    sm->set(id, champ_wSampleLink, val);
                    val.sfLinkValue = monoSample;
                    sm->set(id, champ_sfSampleType, val);
                }
                sm->set(id, champ_filenameForData, path);
                val.dwValue = son->get(champ_dwStart16);
                sm->set(id, champ_dwStart16, val);
                val.dwValue = son->get(champ_dwStart24);
                sm->set(id, champ_dwStart24, val);
            }

            // Configuration du sample
            val.wValue = j;
            sm->set(id, champ_wChannel, val);
            val.dwValue = son->get(champ_dwLength);
            sm->set(id, champ_dwLength, val);
            val.dwValue = son->get(champ_dwSampleRate);
            sm->set(id, champ_dwSampleRate, val);
            val.dwValue = son->get(champ_dwStartLoop);
            sm->set(id, champ_dwStartLoop, val);
            val.dwValue = son->get(champ_dwEndLoop);
            sm->set(id, champ_dwEndLoop, val);
            val.bValue = (quint8)son->get(champ_byOriginalPitch);
            sm->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)son->get(champ_chPitchCorrection);
            sm->set(id, champ_chPitchCorrection, val);

            // Automatically remove leading blank?
            if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool())
                ToolTrimStart::trim(id);

            // Automatically trim to loop?
            if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool())
                ToolTrimEnd::trim(id);
        }
    }
    delete son;

    return addedSmpl;
}

QString EditorToolBar::getName(QString name, int maxCharacters, int suffixNumber)
{
    if (suffixNumber == 0)
        return name.left(maxCharacters);
    QString suffix = QString::number(suffixNumber);
    int suffixSize = suffix.length();
    if (suffixSize > 3 || maxCharacters < suffixSize + 1)
        return name.left(maxCharacters);
    return name.left(maxCharacters - suffixSize - 1) + "-" + suffix;
}

void EditorToolBar::onSaveClicked()
{

}
