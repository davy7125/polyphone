#include "toolmenu.h"
#include "toolfactory.h"
#include "abstracttool.h"
#include "contextmanager.h"

ToolMenu::ToolMenu(QWidget * parent) : QMenu(parent),
    _toolFactory(new ToolFactory(parent))
{
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
                this->addSection(currentCategory);
        }

        // Insert the tool
        QAction * action = this->addAction(
                    ContextManager::theme()->getColoredSvg(tool->getIconName(), QSize(24, 24), ThemeManager::LIST_TEXT),
                    tool->getLabel());
        _currentActions[action] = tool;
    }
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
