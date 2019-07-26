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

#include "filtertag.h"
#include "ui_filtertag.h"
#include <QCompleter>
#include "flowlayout.h"
#include "filtertagcell.h"
#include <QMouseEvent>

FilterTag::FilterTag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterTag),
    _completer(nullptr),
    _canCreate(false)
{
    ui->setupUi(this);
    ui->tagArea->setLayout(new FlowLayout(0));
}

FilterTag::~FilterTag()
{
    delete ui;
    delete _completer;
}

void FilterTag::setPossibleTags(QStringList tags, bool canCreate)
{
    _canCreate = canCreate;
    _possibleTags = tags;
    if (_completer != nullptr)
    {
        ui->lineEdit->setCompleter(nullptr);
        delete _completer;
    }

    // Prepare a QCompleter
    _completer = new QCompleter(_possibleTags);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setFilterMode(Qt::MatchContains);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    connect(_completer, SIGNAL(activated(QString)), this, SLOT(onCompletionSelected(QString)), Qt::QueuedConnection);
    ui->lineEdit->setCompleter(_completer);
}

void FilterTag::select(QStringList tags)
{
    this->blockSignals(true);

    // Delete everything
    _tags.clear();
    while (QWidget* w = ui->tagArea->findChild<QWidget*>())
        delete w;

    // Add new tags
    foreach (QString tag, tags)
        if (_possibleTags.contains(tag) || _canCreate)
            onCompletionSelected(tag);

    this->blockSignals(false);
}

QStringList FilterTag::getSelection()
{
    return _tags;
}

void FilterTag::onCompletionSelected(QString completion)
{
    ui->lineEdit->clear();
    if (!_tags.contains(completion))
    {
        // Update the tag list
        _tags.append(completion);

        // Create a cell
        FilterTagCell * cell = new FilterTagCell(this);
        cell->setTagName(completion);
        connect(cell, SIGNAL(onRemoveClicked()), this, SLOT(onCellDelete()));
        ui->tagArea->layout()->addWidget(cell);

        // Notify about the change
        emit(selectionChanged());
    }
}

void FilterTag::on_lineEdit_returnPressed()
{
    QString tagToSelect = ui->lineEdit->text();
    if (_possibleTags.contains(tagToSelect) || _canCreate)
        onCompletionSelected(tagToSelect);
}

void FilterTag::onCellDelete()
{
    // Delete the cell and update the tag list
    FilterTagCell * cell = dynamic_cast<FilterTagCell *>(QObject::sender());
    _tags.removeAll(cell->getTagName());
    cell->deleteLater();

    // Notify about the change
    emit(selectionChanged());
}
