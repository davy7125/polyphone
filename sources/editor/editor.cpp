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
#include "contextmanager.h"
#include "treesortfilterproxy.h"
#include "soundfontmanager.h"
#include <QFileInfo>
#include <QMessageBox>
#include "treemodel.h"
#include "abstractinputparser.h"
#include "treesplitter.h"
#include "solomanager.h"
#include "pageselector.h"
#include "pianokeybdcustom.h"

Editor::Editor(QWidget *parent) : QWidget(parent),
    ui(new Ui::Editor),
    _sf2Index(-1),
    _pageSelector(new PageSelector()),
    _currentElementType(elementUnknown)
{
    ui->setupUi(this);

    // QSplitter for being able to resize the tree
    TreeSplitter * splitter = new TreeSplitter(this, ui->leftPart, ui->rightPart);
    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(this->layout());
    layout->addWidget(splitter);

    // General style
    ui->editFilter->setStyleSheet("QLineEdit{border: 0}");
    ui->frameSearch->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + "}");
    ui->treeView->setStyleSheet("TreeView{border:1px solid " +
                                ContextManager::theme()->getColor(ThemeManager::BORDER).name() +
                                ";border-top:0;border-left:0;border-bottom:0}");
    ui->iconWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(64, 64), ThemeManager::WINDOW_TEXT));

    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    ui->iconLogo->setPixmap(QPixmap(":/misc/polyphone.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString resetHoverColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->stackedFooter->setStyleSheet("QStackedWidget, QLabel{background-color:" +
                                     ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                     ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                                     "QPushButton{background-color:transparent;color:" +
                                     ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                                     ";border:0;padding:0px 5px}" +
                                     "QPushButton:hover{color:" + resetHoverColor + "}");

    // List of possible pages
    QList<Page *> pages = _pageSelector->getAllPages();
    foreach (Page * page, pages)
    {
        ui->stackedMain->addWidget(page);

        // Reaction when the selection changed in the tree
        connect(page, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));

        // Reaction to MIDI events
        connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), page, SLOT(onKeyPlayed(int,int)));

        // Reaction when "space" is pressed in the tree
        connect(ui->treeView, SIGNAL(spacePressed()), page, SLOT(onSpacePressed()));
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
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), this, SLOT(onKeyPlayed(int,int)));
    connect(SoundfontManager::getInstance(), SIGNAL(parameterForCustomizingKeyboardChanged()), this, SLOT(customizeKeyboard()));
    connect(SoundfontManager::getInstance(), SIGNAL(editingDone(QString, QList<int>)), this, SLOT(onEditingDone(QString, QList<int>)));
    connect(SoundfontManager::getInstance(), SIGNAL(errorEncountered(QString)), this, SLOT(onErrorEncountered(QString)));
}

Editor::~Editor()
{
    delete ui;
    delete _pageSelector;
}

void Editor::initialize(AbstractInputParser * input)
{
    ui->toolBar->disable();
    ui->rotatingSpinner->startAnimation();
    ui->labelFileName->setText(input->getFileName());
    connect(input, SIGNAL(finished()), this, SLOT(inputProcessed()));
    input->process(true);
}

void Editor::inputProcessed()
{
    // Get information from the input
    AbstractInputParser * input = dynamic_cast<AbstractInputParser *>(QObject::sender());
    if (input->isSuccess())
    {
        // Index of the opened soundfont
        _sf2Index = input->getSf2Index();

        // Prepare the tree
        TreeModel * model = dynamic_cast<TreeModel*>(SoundfontManager::getInstance()->getModel(_sf2Index));
        TreeSortFilterProxy * proxy = new TreeSortFilterProxy(_sf2Index, ui->treeView, model);
        connect(ui->editFilter, SIGNAL(textChanged(QString)), proxy, SLOT(filterChanged(QString)));
        connect(model, SIGNAL(saveExpandedState()), ui->treeView, SLOT(saveExpandedState()));
        connect(model, SIGNAL(restoreExpandedState()), ui->treeView, SLOT(restoreExpandedState()));

        // Prepare the actions
        ui->toolBar->setSf2Index(_sf2Index);

        // Tab title and filepath
        updateTitleAndPath();
    }
    else
    {
        // Display the error
        ui->labelReason->setText(input->getError());
        ui->stackedMain->setCurrentWidget(ui->pageError);
    }

    delete input;
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

void Editor::update(QString editingSource)
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

    // Tab title and filepath
    updateTitleAndPath();
}

void Editor::updateTitleAndPath()
{
    // Title
    SoundfontManager * sm = SoundfontManager::getInstance();
    QString title = sm->getQstr(EltID(elementSf2, _sf2Index), champ_name);
    if (title.isEmpty())
        title = sm->getQstr(EltID(elementSf2, _sf2Index), champ_filenameInitial).split(QRegularExpression("(/|\\\\)")).last();
    if (title.isEmpty())
        title = tr("Untitled");

    emit(tabTitleChanged((sm->isEdited(_sf2Index) ? "*" : "") + title));

    // Path
    emit(filePathChanged(sm->getQstr(EltID(elementSf2, _sf2Index), champ_filenameInitial)));
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

    ContextManager::midi()->keyboard()->clearCustomization();
    if (_currentIds.isEmpty())
        return;

    // If only one global division is in the list, exclude the rest
    // If more than one global divison is in the list, don't customize
    IdList ids = _currentIds;
    for (int i = 0; i < ids.count(); i++)
    {
        EltID id = ids[i];
        if (id.typeElement == elementInst || id.typeElement == elementPrst)
        {
            for (int j = i + 1; j < ids.count(); j++)
                if (ids[j].typeElement == elementInst || ids[j].typeElement == elementPrst)
                    return;
            ids.clear();
            ids << id;
            break;
        }
    }

    // Affichage des étendues et rootkeys des divisions sélectionnées
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (EltID id, ids)
    {
        if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
        {
            int rootKey = -1;
            if (id.typeElement == elementInstSmpl)
            {
                if (sf2->isSet(id, champ_overridingRootKey))
                    rootKey = sf2->get(id, champ_overridingRootKey).wValue;
                else
                {
                    EltID idSmpl = id;
                    idSmpl.typeElement = elementSmpl;
                    idSmpl.indexElt = sf2->get(id, champ_sampleID).wValue;
                    rootKey = sf2->get(idSmpl, champ_byOriginalPitch).bValue;
                }
            }

            RangesType keyRange;
            keyRange.byLo = 0;
            keyRange.byHi = 127;
            if (sf2->isSet(id, champ_keyRange))
                keyRange = sf2->get(id, champ_keyRange).rValue;
            else
            {
                if (id.typeElement == elementInstSmpl)
                    id.typeElement = elementInst;
                else
                    id.typeElement = elementPrst;
                if (sf2->isSet(id, champ_keyRange))
                    keyRange = sf2->get(id, champ_keyRange).rValue;
            }
            ContextManager::midi()->keyboard()->addRangeAndRootKey(rootKey, keyRange.byLo, keyRange.byHi);
        }
        else if (id.typeElement == elementInst || id.typeElement == elementPrst)
        {
            RangesType defaultKeyRange;
            if (sf2->isSet(id, champ_keyRange))
                defaultKeyRange = sf2->get(id, champ_keyRange).rValue;
            else
            {
                defaultKeyRange.byLo = 0;
                defaultKeyRange.byHi = 127;
            }
            if (id.typeElement == elementInst)
                id.typeElement = elementInstSmpl;
            else
                id.typeElement = elementPrstInst;
            foreach (int i, sf2->getSiblings(id))
            {
                id.indexElt2 = i;
                RangesType keyRange;
                if (sf2->isSet(id, champ_keyRange))
                    keyRange = sf2->get(id, champ_keyRange).rValue;
                else
                    keyRange = defaultKeyRange;
                ContextManager::midi()->keyboard()->addRangeAndRootKey(-1, keyRange.byLo, keyRange.byHi);
            }
        }
        else if (id.typeElement == elementSmpl)
        {
            int rootKey = sf2->get(id, champ_byOriginalPitch).bValue;
            ContextManager::midi()->keyboard()->addRangeAndRootKey(rootKey, 0, 127);
        }
    }
}

void Editor::onKeyPlayed(int key, int vel)
{
    // This editor may not be the one that is displayed
    if (!this->isVisible())
        return;

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
                    ContextManager::midi()->keyboard()->addCurrentRange(key, keyMin, keyMax);
                }
            }

            // Possibly update the current selection
            if (!currentIds.isEmpty() && (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) != 0)
                ui->treeView->onSelectionChanged(currentIds);
        }
        else
            ContextManager::midi()->keyboard()->removeCurrentRange(key);
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
}

void Editor::onErrorEncountered(QString text)
{
    QMessageBox::warning(this, tr("Warning"), text);
}
