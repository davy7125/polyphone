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

#include "tab.h"
#include "abstractinputparser.h"
#include "soundfontmanager.h"
#include "contextmanager.h"

Tab::Tab(QWidget *parent) : QWidget(parent),
    _sf2Index(-1)
{
    ContextManager::midi()->addListener(this, 0);
}

Tab::~Tab()
{
    ContextManager::midi()->removeListener(this);
}

void Tab::initialize(AbstractInputParser * input, bool async)
{
    tabInitializing(input->getFileName());
    connect(input, SIGNAL(finished()), this, SLOT(inputProcessed()));
    input->process(async);
}

void Tab::initializeWithSoundfontIndex(int indexSf2)
{
    // File already loaded
    _sf2Index = indexSf2;
    tabInitialized(indexSf2);
    updateTitleAndPath();
}

void Tab::inputProcessed()
{
    // Get information from the input
    AbstractInputParser * input = dynamic_cast<AbstractInputParser *>(QObject::sender());
    if (input->isSuccess())
    {
        // Index of the opened soundfont
        _sf2Index = input->getSf2Index();
        tabInitialized(_sf2Index);

        // Tab title and filepath
        updateTitleAndPath();
    }
    else
        tabInError(input->getError());

    delete input;
}

void Tab::update(QString editingSource)
{
    tabUpdate(editingSource);
    updateTitleAndPath();
}

void Tab::updateTitleAndPath()
{
    // Title
    SoundfontManager * sm = SoundfontManager::getInstance();
    QString title = sm->getQstr(EltID(elementSf2, _sf2Index), champ_name);
    static QRegularExpression reg("(/|\\\\)");
    if (title.isEmpty())
        title = sm->getQstr(EltID(elementSf2, _sf2Index), champ_filenameInitial).split(reg).last();
    if (title.isEmpty())
        title = tr("Untitled");
    emit tabTitleChanged((sm->isEdited(_sf2Index) && !ContextManager::s_playerMode ? "*" : "") + getTabTitlePrefix() + title);

    // Path
    emit filePathChanged(sm->getQstr(EltID(elementSf2, _sf2Index), champ_filenameInitial));
}
