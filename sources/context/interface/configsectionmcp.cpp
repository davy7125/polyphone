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

#include "configsectionmcp.h"
#include "ui_configsectionmcp.h"
#include "contextmanager.h"
#include "mcpserver.h"
#include "mcpfunctions.h"
#include <QClipboard>
#include <QApplication>

ConfigSectionMcp::ConfigSectionMcp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionMcp)
{
    ui->setupUi(this);
    ui->labelSubTitle->setStyleSheet("QLabel{margin: 20px 0;}");
    ui->treeFunctions->setStyleSheet(ContextManager::theme()->getTableTheme());
    connect(McpServer::getInstance(), SIGNAL(stateChanged()), this, SLOT(updateState()));
    fillFunctions();
}

ConfigSectionMcp::~ConfigSectionMcp()
{
    delete ui;
}

void ConfigSectionMcp::initialize()
{
    ui->checkEnabled->blockSignals(true);
    ui->checkEnabled->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "enabled", false).toBool());
    ui->checkEnabled->blockSignals(false);

    ui->spinPort->blockSignals(true);
    ui->spinPort->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "port", McpServer::DEFAULT_PORT).toInt());
    ui->spinPort->blockSignals(false);

    updateState();
}

void ConfigSectionMcp::on_checkEnabled_toggled(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MCP, "enabled", checked);
    McpServer::getInstance()->applyConfiguration();
}

void ConfigSectionMcp::on_spinPort_editingFinished()
{
    if (ui->spinPort->value() == ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "port", McpServer::DEFAULT_PORT).toInt())
        return;
    ContextManager::configuration()->setValue(ConfManager::SECTION_MCP, "port", ui->spinPort->value());
    McpServer::getInstance()->applyConfiguration();
}

void ConfigSectionMcp::on_pushCopy_clicked()
{
    QApplication::clipboard()->setText(ui->lineCommand->text());
}

void ConfigSectionMcp::updateState()
{
    McpServer * server = McpServer::getInstance();
    bool enabled = ui->checkEnabled->isChecked();
    ui->spinPort->setEnabled(enabled);
    ui->lineCommand->setText("claude mcp add --transport http polyphone " + server->getUrl());
    ui->lineCommand->setCursorPosition(0);

    if (server->isListening())
        ui->labelState->setText(tr("Server started, address: %1").arg(server->getUrl()));
    else if (enabled && !server->getError().isEmpty())
        ui->labelState->setText(tr("The server cannot start: %1").arg(server->getError()));
    else
        ui->labelState->setText(tr("Server stopped"));
}

void ConfigSectionMcp::fillFunctions()
{
    QMap<McpFunction::Category, QTreeWidgetItem *> categories;
    QMap<McpFunction::Category, QString> names;
    names[McpFunction::CATEGORY_BROWSING] = tr("Browsing");
    names[McpFunction::CATEGORY_EDITING] = tr("Editing");
    names[McpFunction::CATEGORY_TOOLS] = tr("Tools");
    names[McpFunction::CATEGORY_ANALYSIS] = tr("Sound analysis");

    foreach (McpFunction::Category category, names.keys())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(ui->treeFunctions, QStringList() << names[category]);
        QFont font = item->font(0);
        font.setBold(true);
        item->setFont(0, font);
        item->setFirstColumnSpanned(true);
        categories[category] = item;
    }

    // Descriptions of the functions are written for the AI, in English
    foreach (const McpFunction * function, McpFunctions::getAll())
    {
        QString description = function->getDescription();
        QTreeWidgetItem * item = new QTreeWidgetItem(categories[function->getCategory()],
                                                     QStringList() << function->getName() << description);
        item->setToolTip(1, "<p>" + description.toHtmlEscaped() + "</p>");
    }
    foreach (QTreeWidgetItem * item, categories.values())
        item->setText(0, item->text(0) + QString(" (%1)").arg(item->childCount()));
    ui->treeFunctions->expandAll();
    ui->treeFunctions->resizeColumnToContents(0);
}
