#include "tooldialog.h"
#include "abstracttoolgui.h"

ToolDialog::ToolDialog(AbstractToolGui *toolGui, QWidget *parent) : QDialog(parent)
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);

    // Connection of the GUI
    connect(toolGui, SIGNAL(validated()), this, SIGNAL(validated()));
    connect(toolGui, SIGNAL(canceled()), this, SLOT(canceled()));

    toolGui->setParent(this);
}

void ToolDialog::canceled()
{
    this->close();
}
