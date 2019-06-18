/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "toolmenu.h"
#include "toolfactory.h"
#include "abstracttool.h"
#include "contextmanager.h"
#include "utils.h"
#include <QLabel>
#include <QWidgetAction>

ToolMenu::ToolMenu(QWidget * parent) : QMenu(parent),
    _toolFactory(new ToolFactory())
{
    // Colors for the separators
    _separatorTextColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name();
    _separatorBackgroundColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.5).name();

    // Connection of the actions
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(onTriggered(QAction*)));
}

ToolMenu::~ToolMenu()
{
    delete _toolFactory;
    _currentActions.clear();
    this->clear();
}

void ToolMenu::selectionChanged(IdList ids)
{
    // First clear everything
    _currentActions.clear();
    this->clear();

    // Get the tools and sort them by category and labels
    QList<AbstractTool *> tools = _toolFactory->getTools(ids);
    qSort(tools.begin(), tools.end(), lessThan);

    // Then populate
    QString currentCategory = "";
    foreach (AbstractTool * tool, tools)
    {
        // Possibly insert the category name
        if (tool->getCategory() != currentCategory)
        {
            currentCategory = tool->getCategory();
            if (currentCategory != "")
                this->addCategory(currentCategory);
        }

        // Insert the tool
        QAction * action = this->addAction(
                    ContextManager::theme()->getColoredSvg(tool->getIconName(), QSize(24, 24), ThemeManager::LIST_TEXT),
                    tool->getLabel());
        _currentActions[action] = tool;
    }
}

void ToolMenu::addCategory(QString categoryName)
{
    // Create a styled label
    QLabel * label = new QLabel(categoryName);
    label->setStyleSheet(QString("background: %1; color: %2; padding: 3px")
                         .arg(_separatorBackgroundColor).arg(_separatorTextColor));

    // Add it as a separator
    QWidgetAction * separator = new QWidgetAction(this);
    separator->setDefaultWidget(label);
    this->addAction(separator);
}

bool ToolMenu::lessThan(const AbstractTool * tool1, const AbstractTool * tool2)
{
    return Utils::removeAccents(tool1->getCategory() + tool1->getLabel()).toLower() <
            Utils::removeAccents(tool2->getCategory() + tool2->getLabel()).toLower();
}

void ToolMenu::onTriggered(QAction * action)
{
    if (_currentActions.contains(action))
        _currentActions[action]->run();
}
