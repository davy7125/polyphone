#include "toolmenu.h"
#include "toolfactory.h"
#include "abstracttool.h"
#include "contextmanager.h"
#include <QLabel>
#include <QWidgetAction>

ToolMenu::ToolMenu(QWidget * parent) : QMenu(parent),
    _toolFactory(new ToolFactory(parent))
{
    // Colors for the separators
    _separatorTextColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name();
    _separatorBackgroundColor = ContextManager::theme()->mix(
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
    return tool1->getCategory() + tool1->getLabel() < tool2->getCategory() + tool2->getLabel();
}

void ToolMenu::onTriggered(QAction * action)
{
    if (_currentActions.contains(action))
        _currentActions[action]->run();
}
