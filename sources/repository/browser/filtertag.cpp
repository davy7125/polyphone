#include "filtertag.h"
#include "ui_filtertag.h"
#include <QCompleter>
#include "flowlayout.h"
#include "filtertagcell.h"

FilterTag::FilterTag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterTag),
    _completer(NULL)
{
    ui->setupUi(this);
    ui->tagArea->setLayout(new FlowLayout(0));
}

FilterTag::~FilterTag()
{
    delete ui;
}

void FilterTag::setPossibleTags(QStringList tags)
{
    _possibleTags = tags;
    if (_completer != NULL)
    {
        ui->lineEdit->setCompleter(NULL);
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
        if (_possibleTags.contains(tag))
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
    if (_possibleTags.contains(ui->lineEdit->text()))
        onCompletionSelected(ui->lineEdit->text());
}

void FilterTag::onCellDelete()
{
    // Delete the cell and update the tag list
    FilterTagCell * cell = (FilterTagCell *)QObject::sender();
    _tags.removeAll(cell->getTagName());
    cell->deleteLater();

    // Notify about the change
    emit(selectionChanged());
}
