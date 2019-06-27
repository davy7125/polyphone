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

#include "showsoundfonts.h"
#include "ui_showsoundfonts.h"
#include "urlreaderjson.h"
#include "repositorymanager.h"
#include "contextmanager.h"
#include "soundfontcell.h"
#include <QJsonArray>
#include <QResizeEvent>
#include <QDebug>

ShowSoundfonts::ShowSoundfonts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowSoundfonts),
    _urlReaderJson(nullptr),
    _soundfontListOk(false),
    _lastWidth(-1)
{
    // Interface
    ui->setupUi(this);
    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" +
                                 ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                 ";border-radius:5px;padding:5px}");

    // Initialization
    _urlReaderJson = new UrlReaderJson(RepositoryManager::BASE_URL + "day");
    connect(_urlReaderJson, SIGNAL(downloadCompleted(QString)), this, SLOT(dailyListAvailable(QString)), Qt::QueuedConnection);
}

ShowSoundfonts::~ShowSoundfonts()
{
    delete _urlReaderJson;
    delete ui;
}

void ShowSoundfonts::initialize()
{
    // Display the first panel with the waiting spinner
    ui->spinner->startAnimation();
    ui->stackedWidget->setCurrentIndex(0);

    // Two list required before being ready
    _soundfontListOk = false;
    _dailyListOk = false;

    // Start the download
    _urlReaderJson->download();
}

void ShowSoundfonts::dailyListAvailable(QString error)
{
    // Get an error if any
    if (error.isEmpty() && !_urlReaderJson->getErrorCode().isEmpty())
        error = _urlReaderJson->getErrorMessage() + " (" + _urlReaderJson->getErrorCode() + ")";

    // If everything is ok, load the daily ids
    if (error.isEmpty())
        error = loadDailyIds();
    else
        qCritical() << "ShowSoundfonts::dailyListAvailable() - received error" << error;
    _error1 = error;

    _mutex.lock();
    _dailyListOk = true;
    if (_soundfontListOk)
        populate();
    _mutex.unlock();
}

QString ShowSoundfonts::loadDailyIds()
{
    // Get the array of ids
    QJsonObject object = _urlReaderJson->getData();
    if (!object.value("dailyIds").isArray())
        return "bad data: dailyIds is not an array";
    QJsonArray dailyIds = object.value("dailyIds").toArray();

    // Load all ids
    _dailyIds.clear();
    foreach (QJsonValue value, dailyIds)
        if (value.isDouble())
            _dailyIds.append((int)value.toDouble());
    if (_dailyIds.isEmpty())
        return "empty daily list";

    // No error until here
    return "";
}

void ShowSoundfonts::soundfontListAvailable(QString error)
{
    _error2 = error;

    _mutex.lock();
    _soundfontListOk = true;
    if (_dailyListOk)
        populate();
    _mutex.unlock();
}

void ShowSoundfonts::populate()
{
    if (_error1.isEmpty() && _error2.isEmpty())
    {
        foreach (int dailyId, _dailyIds)
        {
            SoundfontInformation * si = RepositoryManager::getInstance()->getSoundfontInformation(dailyId);
            if (si != nullptr)
            {
                //qDebug() << "ShowSoundfonts::populate() - adding daily soundfont " << si->getTitle();
                SoundfontCell * cell = new SoundfontCell(si, this);
                connect(cell, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));

                QListWidgetItem * item = new QListWidgetItem();
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item, cell);
            } else
                qWarning() << "ShowSoundfonts::populate() - ID " << dailyId << "is null";
        }

        // Show soundfonts
        ui->stackedWidget->setCurrentIndex(1);
        updateCellHeight();
    }
    else
    {
        // Show the error
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void ShowSoundfonts::on_listWidget_itemSelectionChanged()
{
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        ((SoundfontCell*)ui->listWidget->itemWidget(item))->setActive(item->isSelected());
    }
}

void ShowSoundfonts::on_pushRetry_clicked()
{
    RepositoryManager::getInstance()->initialize();
}

void ShowSoundfonts::resizeEvent(QResizeEvent * event)
{
    _lastWidth = event->size().width();
    QWidget::resizeEvent(event);
    updateCellHeight();
}

void ShowSoundfonts::updateCellHeight()
{
    // Update size hints for all cells
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        SoundfontCell* cell = (SoundfontCell*)ui->listWidget->itemWidget(item);
        item->setSizeHint(QSize(0, cell->heightForWidth(_lastWidth == -1 ? ui->listWidget->width() : _lastWidth)));
    }
}

void ShowSoundfonts::keyPressEvent(QKeyEvent * event)
{
    // Key press event in the cell doesn't seem to work? So the code is here
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            QListWidgetItem * item = ui->listWidget->item(i);
            if (item->isSelected())
            {
                SoundfontCell* cell = (SoundfontCell*)ui->listWidget->itemWidget(item);
                RepositoryManager::getInstance()->openSoundfont(cell->getSoundfontId(), true);
            }
        }
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}
