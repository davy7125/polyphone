#include "tooldialog.h"
#include "abstracttoolgui.h"
#include "abstracttool.h"
#include <QGridLayout>

ToolDialog::ToolDialog(AbstractToolGui *toolGui, AbstractTool * tool, QWidget *parent) : QDialog(parent)
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);

    // Connection of the GUI
    connect(toolGui, SIGNAL(validated()), this, SIGNAL(validated()));
    connect(toolGui, SIGNAL(canceled()), this, SLOT(canceled()));

    // Fill the dialog
    QGridLayout * layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(toolGui);
    this->setLayout(layout);

    // Title
    this->setWindowTitle(tool->getLabel());
}

void ToolDialog::canceled()
{
    this->close();
}
