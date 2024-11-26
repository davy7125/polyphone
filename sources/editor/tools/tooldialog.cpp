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

#include "tooldialog.h"
#include "abstracttoolgui.h"
#include "abstracttool.h"
#include <QGridLayout>
#include <QIcon>
#include "contextmanager.h"

ToolDialog::ToolDialog(AbstractToolGui *toolGui, AbstractTool * tool, QWidget *parent) : QDialog(parent),
    _tool(tool)
{
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
    this->setWindowModality(Qt::WindowModal);
    this->setWindowIcon(QIcon(":/misc/polyphone.png"));

    // Connection of the GUI
    connect(toolGui, SIGNAL(validated()), this, SIGNAL(validated()));
    connect(toolGui, SIGNAL(canceled()), this, SLOT(canceled()));

    // Fill the dialog
    QGridLayout * layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolGui);
    this->setLayout(layout);

    // Title
    this->setWindowTitle(_tool->getLabel());
}

void ToolDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // Restore the geometry
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_TOOLS, _tool->getIdentifier(true), "dialogGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

void ToolDialog::closeEvent(QCloseEvent *event)
{
    // Save the geometry
    ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, _tool->getIdentifier(true), "dialogGeometry", this->saveGeometry());

    QDialog::closeEvent(event);
}

void ToolDialog::canceled()
{
    this->close();
}
