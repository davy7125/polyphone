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

#include "configpanel.h"
#include "ui_configpanel.h"
#include "contextmanager.h"
#include <QScrollBar>

ConfigPanel::ConfigPanel(QWidget *parent) : Tab(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);

    // Style (same horizontal padding and no indent for the title and the headers, so that their texts are aligned)
    QString colors = "background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";" +
            "color: " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";";
    QString styleSheet = "QLabel{border-radius:2px;padding: 3px 10px;" + colors + "}";
    foreach (QLabel * label, QList<QLabel *>() << ui->interfaceLabel << ui->soundLabel << ui->keyboardLabel << ui->repositoryLabel)
    {
        label->setStyleSheet(styleSheet);
        label->setIndent(0);
    }
    ui->labelTitle->setStyleSheet("QLabel{padding: 0 10px;" + colors + "}");

    // Section headers
    _sectionLabels << ui->interfaceLabel << ui->soundLabel << ui->keyboardLabel;
#ifdef NO_SF2_REPOSITORY
    ui->repositoryLabel->hide();
    ui->repositoryWidget->hide();
#else
    _sectionLabels << ui->repositoryLabel;
#endif

    // Events
    connect(ui->widgetToc, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateTitle()));
    connect(ui->scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateTitle()));
    ui->scrollArea->viewport()->installEventFilter(this);
    ui->scrollAreaWidgetContents->installEventFilter(this);
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
#ifndef NO_SF2_REPOSITORY
    ui->repositoryWidget->initialize();
#endif
}

bool ConfigPanel::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::Resize)
    {
        updateTocPosition();
        updateBottomSpace();
        updateTitle();
    }
    return Tab::eventFilter(watched, event);
}

void ConfigPanel::onSectionClicked(int sectionNumber)
{
    if (sectionNumber < 0 || sectionNumber > _sectionLabels.count())
        return;

    // The header of the section is scrolled just under the title
    int position = 0;
    if (sectionNumber > 0)
    {
        QLabel * label = _sectionLabels[sectionNumber - 1];
        position = label->y() + label->height();
    }
    ui->scrollArea->verticalScrollBar()->setValue(position);
}

void ConfigPanel::updateTitle()
{
    // The title is the name of the last section whose header has been scrolled under it
    QString title = tr("General");
    int position = ui->scrollArea->verticalScrollBar()->value();
    foreach (QLabel * label, _sectionLabels)
        if (label->y() + label->height() <= position)
            title = label->text();
    ui->labelTitle->setText(title);

    // Align the title with the headers
    int indent = qMax(0, ui->interfaceLabel->mapTo(this, QPoint(0, 0)).x() - ui->labelTitle->x());
    if (indent != ui->labelTitle->indent())
        ui->labelTitle->setIndent(indent);
}

void ConfigPanel::updateTocPosition()
{
    // Same space on the left of the table of contents and on the right of the sections
    int leftMargin = ui->widgetToc->contentsMargins().left();
    int tocWidth = ui->widgetToc->sizeHint().width() - leftMargin;
    int tocLeftSpace = ui->widgetToc->layout()->contentsMargins().left();
    int scrollBarWidth = ui->scrollArea->width() - ui->scrollArea->viewport()->width();
    int sectionsRight = ui->interfaceLabel->x() + ui->interfaceLabel->width();
    int newLeftMargin = qMax(0, (this->width() - tocWidth - scrollBarWidth - sectionsRight - tocLeftSpace) / 2);
    if (newLeftMargin != leftMargin)
        ui->widgetToc->setContentsMargins(newLeftMargin, 0, 0, 0);
}

void ConfigPanel::updateBottomSpace()
{
    // Space below the last section, so that its header can be scrolled under the title
    QLabel * lastLabel = _sectionLabels.last();
    int bottomMargin;
    ui->horizontalLayout->getContentsMargins(nullptr, nullptr, nullptr, &bottomMargin);
    int height = qMax(0, lastLabel->y() + lastLabel->height() + ui->scrollArea->viewport()->height() -
                      ui->bottomSpacer->geometry().top() - bottomMargin);
    if (height != ui->bottomSpacer->sizeHint().height())
    {
        ui->bottomSpacer->changeSize(20, height, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        ui->verticalLayout->invalidate();
    }
}
