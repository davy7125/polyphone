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

#include "configpanel.h"
#include "ui_configpanel.h"
#include "contextmanager.h"
#include <QScrollBar>

ConfigPanel::ConfigPanel(QWidget *parent) : QWidget(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);

    // Style
    QString styleSheet = QString("QLabel{border-radius:2px;padding: 3px 5px;") +
            "background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";" +
            "color: " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";
    ui->generalLabel->setStyleSheet(styleSheet);
    ui->interfaceLabel->setStyleSheet(styleSheet);
    ui->soundLabel->setStyleSheet(styleSheet);
    ui->keyboardLabel->setStyleSheet(styleSheet);
    ui->repositoryLabel->setStyleSheet(styleSheet);

    // Events
    connect(ui->widgetToc, SIGNAL(sectionClicked(int)), this, SLOT(goToSection(int)));
}

ConfigPanel::~ConfigPanel()
{
    delete ui;
}

void ConfigPanel::initializeInterface()
{
    ui->generalWidget->initialize();
    ui->interfaceWidget->initialize();
    ui->soundWidget->initialize();
    ui->keyboardWidget->initialize();
    ui->repositoryWidget->initialize();
}

void ConfigPanel::goToSection(int sectionNumber)
{
    QWidget * widget;
    switch (sectionNumber)
    {
    case 0: widget = ui->generalLabel; break;
    case 1: widget = ui->interfaceLabel; break;
    case 2: widget = ui->soundLabel; break;
    case 3: widget = ui->keyboardLabel; break;
    case 4: widget = ui->repositoryLabel; break;
    default:
        return;
    }

    const QPoint p = widget->mapTo(ui->scrollArea, QPoint(0,0));
    ui->scrollArea->verticalScrollBar()->setValue(p.y() + ui->scrollArea->verticalScrollBar()->value() - ui->horizontalLayout->margin());
}
