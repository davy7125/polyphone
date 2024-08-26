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

#include "editor.h"
#include "ui_editor.h"
#include <QFileInfo>
#include <QMessageBox>
#include "contextmanager.h"
#include "treesortfilterproxy.h"
#include "soundfontmanager.h"
#include "treemodel.h"
#include "abstractinputparser.h"
#include "treesplitter.h"
#include "solomanager.h"
#include "pageselector.h"
#include "dialogkeyboard.h"
#include "pianokeybdcustom.h"

Editor::Editor(DialogKeyboard *dialogKeyboard) : Tab(nullptr),
    _dialogKeyboard(dialogKeyboard),
    ui(new Ui::Editor),
    _pageSelector(new PageSelector()),
    _currentElementType(elementUnknown)
{
    ui->setupUi(this);

    // QSplitter so that the tree is resizable
    TreeSplitter * splitter = new TreeSplitter(this, ui->leftPart, ui->rightPart);
    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(this->layout());
    layout->addWidget(splitter);

    // General style
    QString highlightColorBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightColorText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();

    ui->editFilter->setStyleSheet("QLineEdit{border: 0}");
    ui->frameSearch->setStyleSheet("QFrame{background-color:" +
                                   highlightColorBackground + "}");
    ui->editFilter->setStyleSheet("QLineEdit{background-color:" + highlightColorBackground + ";color:" + highlightColorText + ";}");
    ui->treeView->setStyleSheet("TreeView{border:1px solid " +
                                ContextManager::theme()->getColor(ThemeManager::BORDER).name() +
                                ";border-top:0;border-left:0;border-bottom:0}");
    ui->iconWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(64, 64), ThemeManager::WINDOW_TEXT));

    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name();
    replacement["secondColor"] = highlightColorBackground;
    ui->iconLogo->setPixmap(QPixmap(":/misc/polyphone.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString resetHoverColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->stackedFooter->setStyleSheet("QStackedWidget, QLabel{background-color:" +
                                     highlightColorBackground + ";color:" +
                                     highlightColorText + "}" +
                                     "QPushButton{background-color:transparent;color:" +
                                     highlightColorText +
                                     ";border:0;padding:0px 5px}" +
                                     "QPushButton:hover{color:" + resetHoverColor + "}");

    // List of possible pages
    QList<Page *> pages = _pageSelector->getAllPages();
    foreach (Page * page, pages)
    {
        ui->stackedMain->addWidget(page);

        // Reaction when the selection changed in the tree
        connect(page, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));

        // Reaction when "space" is pressed in the tree
        connect(ui->treeView, SIGNAL(spacePressed()), page, SLOT(onSpacePressed()));

        // Reaction when the rootkey of a sample changed
        connect(page, SIGNAL(rootKeyChanged(int)), this, SLOT(onRootKeyChanged(int)));

        // Reaction when a page is hidden
        connect(page, SIGNAL(pageHidden()), this, SLOT(onPageHidden()));
    }

    // Propagation of the selection
    connect(ui->footerSf2, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->footerOverview, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->footerLinkedTo, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->footerPrst, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->treeView, SIGNAL(selectionChanged(IdList)), this, SLOT(onSelectionChanged(IdList)));
    connect(ui->treeView, SIGNAL(selectionChanged(IdList)), ui->toolBar, SLOT(onSelectionChanged(IdList)));

    // Toolbar connections
    connect(ui->toolBar, SIGNAL(displayOptionChanged(int)), this, SLOT(displayOptionChanged(int)));
    connect(ui->toolBar, SIGNAL(keyboardDisplayChanged(bool)), this, SIGNAL(keyboardDisplayChanged(bool)));
    connect(ui->toolBar, SIGNAL(recorderDisplayChanged(bool)), this, SIGNAL(recorderDisplayChanged(bool)));
    connect(ui->toolBar, SIGNAL(selectionChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));

    // Other
    connect(ui->treeView, SIGNAL(focusOnSearch()), ui->editFilter, SLOT(setFocus()));
    connect(SoundfontManager::getInstance(), SIGNAL(parameterForCustomizingKeyboardChanged()), this, SLOT(customizeKeyboard()));
    connect(SoundfontManager::getInstance(), SIGNAL(editingDone(QString, QList<int>)), this, SLOT(onEditingDone(QString, QList<int>)));
    connect(SoundfontManager::getInstance(), SIGNAL(errorEncountered(QString)), this, SLOT(onErrorEncountered(QString)));
}

Editor::~Editor()
{
    delete ui;
    delete _pageSelector;
}

void Editor::tabInitializing(QString filename)
{
    ui->toolBar->disable();
    ui->rotatingSpinner->startAnimation();
    ui->labelFileName->setText(filename);
}

void Editor::tabInError(QString errorMessage)
{
    // Display the error
    ui->labelReason->setText(errorMessage);
    ui->stackedMain->setCurrentWidget(ui->pageError);
}

void Editor::tabInitialized(int indexSf2)
{
    // Prepare the tree
    TreeModel * model = dynamic_cast<TreeModel*>(SoundfontManager::getInstance()->getModel(indexSf2));
    TreeSortFilterProxy * proxy = new TreeSortFilterProxy(indexSf2, ui->treeView, model);
    connect(ui->editFilter, SIGNAL(textChanged(QString)), proxy, SLOT(filterChanged(QString)));
    connect(model, SIGNAL(saveExpandedState()), ui->treeView, SLOT(saveExpandedState()));
    connect(model, SIGNAL(restoreExpandedState()), ui->treeView, SLOT(restoreExpandedState()));

    // Prepare the actions
    ui->toolBar->setSf2Index(indexSf2);
}

void Editor::tabUpdate(QString editingSource)
{
    // State of the toolbar
    ui->toolBar->updateActions();

    // Update the current page
    if (ui->stackedMain->currentIndex() > 2)
    {
        Page * currentPage = dynamic_cast<Page *>(ui->stackedMain->currentWidget());
        currentPage->preparePage(editingSource);
    }

    // Update the footer
    if (!editingSource.startsWith("footer") && ui->stackedFooter->currentIndex() > 0)
    {
        AbstractFooter * absFooter = dynamic_cast<AbstractFooter *>(ui->stackedFooter->currentWidget());
        absFooter->updateInterface();
    }
}

void Editor::onSelectionChanged(IdList ids)
{
    _currentIds = ids;

    // Sample play is muted
    ContextManager::audio()->getSynth()->play(EltID(), -1, -1, 0);

    // Update the solo status
    SoundfontManager::getInstance()->solo()->selectionChanged(ids);

    // View and footer to display
    Page * currentPage = nullptr;
    QWidget * footer = nullptr;
    if (ids.empty())
        _currentElementType = elementUnknown;
    else
    {
        _currentElementType = ids[0].typeElement;
        currentPage = _pageSelector->getLastPage(_currentElementType);
        switch (ids[0].typeElement)
        {
        case elementSf2:
            footer = ui->footerSf2;
            break;
        case elementRootSmpl: case elementRootInst: case elementRootPrst:
            footer = ui->footerOverview;
            break;
        case elementSmpl: case elementInst: case elementInstSmpl:
            footer = ui->footerLinkedTo;
            break;
        case elementPrst: case elementPrstInst:
            footer = ui->footerPrst;
            break;
        default:
            break;
        }
    }

    // Display the view corresponding to the element type
    if (currentPage == nullptr)
    {
        ui->toolBar->clearDisplayOptions();
        ui->stackedMain->setCurrentWidget(ui->pageLogo);
    }
    else
    {
        if ((void *)currentPage == (void *)ui->stackedMain->currentWidget())
            currentPage->preparePage("command:selection", ids);
        else
            currentPage->preparePage("command:display", ids);
        ui->toolBar->setDisplayOptions(_pageSelector->getPages(_currentElementType), currentPage);

        // Display the page
        ui->stackedMain->setCurrentWidget(currentPage);
    }

    // Display the footer corresponding to the element type
    if (footer == nullptr)
        ui->stackedFooter->setCurrentWidget(ui->footerDefault);
    else
    {
        AbstractFooter * absFooter = dynamic_cast<AbstractFooter *>(footer);
        absFooter->setCurrentIds(ids);
        absFooter->updateInterface();
        ui->stackedFooter->setCurrentWidget(footer);
    }

    // Update the keyboard
    this->customizeKeyboard();
}

void Editor::displayOptionChanged(int displayOption)
{
    // Get the page to display
    QList<Page *> possiblePages = _pageSelector->getPages(_currentElementType);
    if (displayOption >= possiblePages.count())
        ui->stackedMain->setCurrentWidget(ui->pageLogo);
    else
    {
        Page * pageToDisplay = possiblePages[displayOption];
        if ((void *)pageToDisplay != (void *)ui->stackedMain->currentWidget())
        {
            pageToDisplay->preparePage("command:display", _currentIds);
            _pageSelector->setLastPage(_currentElementType, pageToDisplay);
            ui->stackedMain->setCurrentWidget(pageToDisplay);
        }
    }

    // Customize the keyboard
    this->customizeKeyboard();
}

void Editor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    customizeKeyboard();
}

void Editor::onEditingDone(QString editingSource, QList<int> sf2Indexes)
{
    Q_UNUSED(sf2Indexes)
    if (editingSource == "command:undo" || editingSource == "command:redo")
        customizeKeyboard();
}

void Editor::customizeKeyboard()
{
    // This editor may not be the one that is displayed
    if (!this->isVisible())
        return;

    _dialogKeyboard->getKeyboard()->clearCustomization();
    if (_currentIds.isEmpty())
        return;

    // Browse all ids
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    IdList ids  = _currentIds;
    RangesType defaultKeyRange;
    defaultKeyRange.byLo = 0;
    defaultKeyRange.byHi = 127;
    bool defaultDefined = false;
    bool displayRootKeys = true;
    for (int i = 0; i < _currentIds.count(); i++)
    {
        EltID id = _currentIds[i];

        // Nothing to customize for headers
        if (id.typeElement == elementSf2 || id.typeElement == elementRootSmpl ||
            id.typeElement == elementRootInst || id.typeElement == elementRootPrst)
            return;

        if (id.typeElement == elementSmpl)
        {
            // Gather all sample root keys
            QList<int> rootKeys;
            bool uniqueSample = true;
            rootKeys << sf2->get(id, champ_byOriginalPitch).bValue;
            for (int j = i + 1; j < _currentIds.count(); j++)
            {
                if (_currentIds[j].typeElement == elementSmpl)
                {
                    uniqueSample = false;
                    int rootKey = sf2->get(_currentIds[j], champ_byOriginalPitch).bValue;
                    if (!rootKeys.contains(rootKey))
                        rootKeys << rootKey;
                }
            }

            // Show the rootkeys and enable the piano if there is only one sample
            foreach (int rootKey, rootKeys)
                _dialogKeyboard->getKeyboard()->addRangeAndRootKey(rootKey, uniqueSample ? 0 : -1, uniqueSample ? 127 : -1);
            return;
        }

        if (id.typeElement == elementInst || id.typeElement == elementPrst)
        {
            // If more than one global divison are in the list, don't customize
            for (int j = i + 1; j < _currentIds.count(); j++)
                if (_currentIds[j].typeElement == elementInst || _currentIds[j].typeElement == elementPrst)
                    return;

            // Default key range of the divisions
            if (sf2->isSet(id, champ_keyRange))
            {
                defaultKeyRange = sf2->get(id, champ_keyRange).rValue;
                defaultDefined = true;
            }

            // Only one instrument or preset: all related divisions are selected
            ids.clear();
            id.typeElement = id.typeElement == elementInst ? elementInstSmpl : elementPrstInst;
            foreach (int j, sf2->getSiblings(id))
            {
                id.indexElt2 = j;
                ids << id;
            }

            // No root keys
            displayRootKeys = false;
            break;
        }

        // Default key range
        if (!defaultDefined)
        {
            if (id.typeElement == elementInstSmpl)
                id.typeElement = elementInst;
            else
                id.typeElement = elementPrst;
            if (sf2->isSet(id, champ_keyRange))
                defaultKeyRange = sf2->get(id, champ_keyRange).rValue;
            defaultDefined = true;
        }

        // No root keys if a division of a preset is selected
        if (id.typeElement == elementPrstInst)
            displayRootKeys = false;
    }

    // Display the divisions
    int rootKey = -1;
    QMap<int, QVector<bool> > rangeByInst;
    foreach (EltID id, ids)
    {
        if (id.typeElement == elementInstSmpl)
        {
            // Corresponding root key, if needed
            if (displayRootKeys)
            {
                if (sf2->isSet(id, champ_overridingRootKey))
                    rootKey = sf2->get(id, champ_overridingRootKey).wValue;
                else
                {
                    EltID idSmpl(elementSmpl, id.indexSf2, sf2->get(id, champ_sampleID).wValue);
                    rootKey = sf2->get(idSmpl, champ_byOriginalPitch).bValue;
                }
            }

            // Add the segment to the keyboard
            RangesType keyRange = defaultKeyRange;
            if (sf2->isSet(id, champ_keyRange))
                keyRange = sf2->get(id, champ_keyRange).rValue;
            _dialogKeyboard->getKeyboard()->addRangeAndRootKey(rootKey, keyRange.byLo, keyRange.byHi);
        }
        else // elementPrstInst
        {
            // Maximum key range of this division
            RangesType maxRange = defaultKeyRange;
            if (sf2->isSet(id, champ_keyRange))
                maxRange = sf2->get(id, champ_keyRange).rValue;

            // Instrument target by this division
            int instId = sf2->get(id, champ_instrument).wValue;
            if (!rangeByInst.contains(instId))
                rangeByInst[instId] = getEnabledKeysForInstrument(EltID(elementInst, id.indexSf2, instId));

            // Find the instrument ranges within the max range
            int startKey = -1;
            for (int key = maxRange.byLo; key <= maxRange.byHi; key++)
            {
                if (rangeByInst[instId][key])
                {
                    // Possibly start a range
                    if (startKey == -1)
                        startKey = key;
                }
                else
                {
                    if (startKey != -1)
                    {
                        // Close a range
                        _dialogKeyboard->getKeyboard()->addRangeAndRootKey(-1, startKey, key - 1);
                        startKey = -1;
                    }
                }
            }

            // Final range
            if (startKey != -1)
                _dialogKeyboard->getKeyboard()->addRangeAndRootKey(-1, startKey, maxRange.byHi);
        }
    }
}

QVector<bool> Editor::getEnabledKeysForInstrument(EltID idInst)
{
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    QVector<bool> result(128);
    result.fill(false);

    // Default range of this instrument
    RangesType defInstRange;
    if (sf2->isSet(idInst, champ_keyRange))
        defInstRange = sf2->get(idInst, champ_keyRange).rValue;
    else
    {
        defInstRange.byLo = 0;
        defInstRange.byHi = 127;
    }

    // Browse all divisions inside the instrument
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    foreach (int j, sf2->getSiblings(idInstSmpl))
    {
        idInstSmpl.indexElt2 = j;
        RangesType instDivRange = defInstRange;
        if (sf2->isSet(idInstSmpl, champ_keyRange))
            instDivRange = sf2->get(idInstSmpl, champ_keyRange).rValue;

        // And set all used keys to "true"
        for (unsigned int key = instDivRange.byLo; key <= instDivRange.byHi; key++)
            result[key] = true;
    }

    return result;
}

void Editor::onErrorEncountered(QString text)
{
    QMessageBox::warning(this, tr("Warning"), text);
}

void Editor::onRootKeyChanged(int rootKey)
{
    _dialogKeyboard->getKeyboard()->clearCustomization();
    _dialogKeyboard->getKeyboard()->addRangeAndRootKey(rootKey, 0, 127);
}

void Editor::onPageHidden()
{
    ContextManager::audio()->getSynth()->play(EltID(), -1, -1, 0);
    _dialogKeyboard->getKeyboard()->clearCustomization();
}

bool Editor::processKey(int channel, int key, int vel)
{
    if (channel != -1)
        return false;

    // This editor may not be the one that is displayed
    if (!this->isVisible())
        return false;

    // SAMPLE

    IdList ids = _currentIds.getSelectedIds(elementSmpl);
    if (ids.count() == 1)
        ContextManager::audio()->getSynth()->play(ids[0], -1, key, vel);

    // INSTRUMENT

    ids = _currentIds.getSelectedIds(elementInst);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    if (ids.count() == 1)
    {
        ContextManager::audio()->getSynth()->play(ids[0], -1, key, vel);

        if (vel > 0)
        {
            // Emphasize the related ranges
            EltID idInst = ids[0];
            idInst.typeElement = elementInst;
            RangesType defaultKeyRange, defaultVelRange;
            if (sf2->isSet(idInst, champ_keyRange))
                defaultKeyRange = sf2->get(idInst, champ_keyRange).rValue;
            else
            {
                defaultKeyRange.byLo = 0;
                defaultKeyRange.byHi = 127;
            }
            if (sf2->isSet(idInst, champ_velRange))
                defaultVelRange = sf2->get(idInst, champ_velRange).rValue;
            else
            {
                defaultVelRange.byLo = 0;
                defaultVelRange.byHi = 127;
            }

            EltID idInstSmpl = ids[0];
            idInstSmpl.typeElement = elementInstSmpl;
            IdList currentIds;
            foreach (int i, sf2->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                int keyMin, keyMax, velMin, velMax;
                if (sf2->isSet(idInstSmpl, champ_keyRange))
                {
                    RangesType rangeTmp = sf2->get(idInstSmpl, champ_keyRange).rValue;
                    keyMin = rangeTmp.byLo;
                    keyMax = rangeTmp.byHi;
                }
                else
                {
                    keyMin = defaultKeyRange.byLo;
                    keyMax = defaultKeyRange.byHi;
                }
                if (sf2->isSet(idInstSmpl, champ_velRange))
                {
                    RangesType rangeTmp = sf2->get(idInstSmpl, champ_velRange).rValue;
                    velMin = rangeTmp.byLo;
                    velMax = rangeTmp.byHi;
                }
                else
                {
                    velMin = defaultVelRange.byLo;
                    velMax = defaultVelRange.byHi;
                }
                if (keyMin <= key && keyMax >= key && velMin <= vel && velMax >= vel)
                {
                    currentIds << idInstSmpl;
                    _dialogKeyboard->getKeyboard()->addCurrentRange(key, keyMin, keyMax);
                }
            }

            // Possibly update the current selection
            if (!currentIds.isEmpty() && (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) != 0)
                ui->treeView->onSelectionChanged(currentIds);
        }
        else
            _dialogKeyboard->getKeyboard()->removeCurrentRange(key);
    }

    // PRESET
    ids = _currentIds.getSelectedIds(elementPrst);
    if (ids.count() == 1)
    {
        ContextManager::audio()->getSynth()->play(ids[0], -1, key, vel);

        // Possibly update the current selection
        if (vel > 0 && (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) != 0)
        {
            EltID idPrst = ids[0];
            idPrst.typeElement = elementPrst;
            RangesType defaultKeyRange, defaultVelRange;
            if (sf2->isSet(idPrst, champ_keyRange))
                defaultKeyRange = sf2->get(idPrst, champ_keyRange).rValue;
            else
            {
                defaultKeyRange.byLo = 0;
                defaultKeyRange.byHi = 127;
            }
            if (sf2->isSet(idPrst, champ_velRange))
                defaultVelRange = sf2->get(idPrst, champ_velRange).rValue;
            else
            {
                defaultVelRange.byLo = 0;
                defaultVelRange.byHi = 127;
            }

            EltID idPrstInst = ids[0];
            idPrstInst.typeElement = elementPrstInst;
            IdList currentIds;
            foreach (int i, sf2->getSiblings(idPrstInst))
            {
                idPrstInst.indexElt2 = i;
                int keyMin, keyMax, velMin, velMax;
                if (sf2->isSet(idPrstInst, champ_keyRange))
                {
                    RangesType rangeTmp = sf2->get(idPrstInst, champ_keyRange).rValue;
                    keyMin = rangeTmp.byLo;
                    keyMax = rangeTmp.byHi;
                }
                else
                {
                    keyMin = defaultKeyRange.byLo;
                    keyMax = defaultKeyRange.byHi;
                }
                if (sf2->isSet(idPrstInst, champ_velRange))
                {
                    RangesType rangeTmp = sf2->get(idPrstInst, champ_velRange).rValue;
                    velMin = rangeTmp.byLo;
                    velMax = rangeTmp.byHi;
                }
                else
                {
                    velMin = defaultVelRange.byLo;
                    velMax = defaultVelRange.byHi;
                }
                if (keyMin <= key && keyMax >= key && velMin <= vel && velMax >= vel)
                    currentIds << idPrstInst;
            }

            if (!currentIds.isEmpty())
                ui->treeView->onSelectionChanged(currentIds);
        }
    }

    // Inner page
    _pageSelector->getLastPage(_currentElementType)->onKeyPlayed(key, vel);

    return false;
}
