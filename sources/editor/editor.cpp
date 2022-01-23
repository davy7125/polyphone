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

#include "editor.h"
#include "ui_editor.h"
#include "contextmanager.h"
#include "treesortfilterproxy.h"
#include "soundfontmanager.h"
#include <QFileInfo>
#include "treemodel.h"
#include "abstractinputparser.h"
#include "treesplitter.h"
#include "solomanager.h"

Editor::Editor(QWidget *parent) : QWidget(parent),
    ui(new Ui::Editor),
    _sf2Index(-1)
{
    ui->setupUi(this);

    // QSplitter for being able to resize the tree
    TreeSplitter * splitter = new TreeSplitter(this, ui->leftPart, ui->stackedWidget);
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
    ui->widgetBottom->setStyleSheet("QWidget{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                    ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                                    "}");
    ui->widgetBottom2->setStyleSheet(ui->widgetBottom->styleSheet());

    // Propagation of the selection
    connect(ui->pageGeneral, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pageAllSmpl, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pageAllInst, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pageAllPrst, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pageSmpl, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pageInst, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->pagePrst, SIGNAL(selectedIdsChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));
    connect(ui->treeView, SIGNAL(selectionChanged(IdList)), this, SLOT(onSelectionChanged(IdList)));
    connect(ui->treeView, SIGNAL(selectionChanged(IdList)), ui->toolBar, SLOT(onSelectionChanged(IdList)));

    // Midi event
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), ui->pageSmpl, SLOT(keyPlayed(int,int)));
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), ui->pageInst, SLOT(keyPlayed(int,int)));
    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), ui->pagePrst, SLOT(keyPlayed(int,int)));

    // Toolbar connections
    connect(ui->toolBar, SIGNAL(displayOptionChanged(int)), this, SLOT(displayOptionChanged(int)));
    connect(ui->toolBar, SIGNAL(keyboardDisplayChanged(bool)), this, SIGNAL(keyboardDisplayChanged(bool)));
    connect(ui->toolBar, SIGNAL(recorderDisplayChanged(bool)), this, SIGNAL(recorderDisplayChanged(bool)));
    connect(ui->toolBar, SIGNAL(selectionChanged(IdList)), ui->treeView, SLOT(onSelectionChanged(IdList)));

    // Tree connection
    connect(ui->treeView, SIGNAL(sampleOnOff()), ui->pageSmpl, SLOT(onSampleOnOff()));
    connect(ui->treeView, SIGNAL(focusOnSearch()), ui->editFilter, SLOT(setFocus()));
}

Editor::~Editor()
{
    delete ui;
}

void Editor::initialize(AbstractInputParser * input)
{
    ui->toolBar->disable();
    ui->rotatingSpinner->startAnimation();
    ui->labelFileName->setText(input->getFileName());
    ui->labelFileName2->setText(input->getFileName());
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
        ui->stackedWidget->setCurrentWidget(ui->pageError);
    }

    delete input;
}

void Editor::onSelectionChanged(IdList ids)
{
    // Sample play is muted
    ContextManager::audio()->getSynth()->play(EltID(), -1, 0);

    // Update the solo status
    SoundfontManager::getInstance()->solo()->selectionChanged(ids);

    // At least one id must be selected
    if (ids.empty())
        return;

    // Find the view to display and update it
    Page * currentPage = nullptr;
    switch (ids[0].typeElement)
    {
    case elementSf2:
        currentPage = ui->pageGeneral;
        break;
    case elementRootSmpl:
        currentPage = ui->pageAllSmpl;
        break;
    case elementRootInst:
        currentPage = ui->pageAllInst;
        break;
    case elementRootPrst:
        currentPage = ui->pageAllPrst;
        break;
    case elementSmpl:
        currentPage = ui->pageSmpl;
        break;
    case elementInst: case elementInstSmpl:
        currentPage = ui->pageInst;
        break;
    case elementPrst: case elementPrstInst:
        currentPage = ui->pagePrst;
        break;
    default:
        break;
    }

    if (currentPage != nullptr)
    {
        if ((void *)currentPage == (void *)ui->stackedWidget->currentWidget())
            currentPage->preparePage("command:selection", ids);
        else
            currentPage->preparePage("command:display", ids);
        ui->toolBar->setDisplayOptions(currentPage->getDisplayOptions());

        // Display the page
        ui->stackedWidget->setCurrentWidget(currentPage);
    }
}

void Editor::update(QString editingSource)
{
    // State of the toolbar
    ui->toolBar->updateActions();

    // Update the current page
    Page * currentPage = dynamic_cast<Page*>(ui->stackedWidget->currentWidget());
    currentPage->preparePage(editingSource);

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
    // Update the current page
    Page * currentPage = dynamic_cast<Page*>(ui->stackedWidget->currentWidget());
    currentPage->setDisplayOption(displayOption);
}
