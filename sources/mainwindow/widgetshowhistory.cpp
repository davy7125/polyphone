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

#include "widgetshowhistory.h"
#include "ui_widgetshowhistory.h"
#include "widgetshowhistorycell.h"
#include "contextmanager.h"
#include <QDateTime>
#include <QPainter>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

const int WidgetShowHistory::SIZE = 900;
const int WidgetShowHistory::OFFSET_X = -120;
const int WidgetShowHistory::OFFSET_Y = -140;
const double WidgetShowHistory::COLOR_RATIO = 0.1;

WidgetShowHistory::WidgetShowHistory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetShowHistory)
{
    ui->setupUi(this);
    ui->listWidget->viewport()->setAutoFillBackground(false);

    // Decoration
    _withDecoration = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "decoration", true).toBool();
    if (_withDecoration)
    {
        QMap<QString, QString> replacements;
        replacements["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                                         ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                         COLOR_RATIO).name();
        _decoration = ContextManager::theme()->getColoredSvg(":/misc/decoration.svg", QSize(SIZE, SIZE), replacements);

        // Flip the image
        QMatrix matrix;
        matrix = matrix.scale(-1, 1);
        _decoration = _decoration.transformed(matrix);
    }

    // Drag & drop
    this->setAcceptDrops(true);
}

WidgetShowHistory::~WidgetShowHistory()
{
    delete ui;
}

void WidgetShowHistory::clear()
{
    ui->listWidget->clear();
    ui->listWidget->hide();
    ui->frameNoHistory->show();
}

void WidgetShowHistory::addFile(QString path, QDateTime datetime)
{
    WidgetShowHistoryCell * cell = new WidgetShowHistoryCell(ui->listWidget);
    cell->setLink(path);
    cell->setDateTime(datetime);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(cell->width(), cell->height()));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, cell);

    ui->frameNoHistory->hide();
    ui->listWidget->show();
}

void WidgetShowHistory::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // Corresponding widget
    WidgetShowHistoryCell * cell = (WidgetShowHistoryCell*)ui->listWidget->itemWidget(item);
    emit(openFile(cell->getLink()));
}

void WidgetShowHistory::on_listWidget_itemSelectionChanged()
{
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        ((WidgetShowHistoryCell*)ui->listWidget->itemWidget(item))->setActive(item->isSelected());
    }
}

void WidgetShowHistory::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(opt.rect, ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND));
    if (_withDecoration)
        p.drawPixmap(opt.rect.right() - OFFSET_X - _decoration.width(), opt.rect.bottom() - _decoration.height() - OFFSET_Y, _decoration);

    QWidget::paintEvent(event);
}

void WidgetShowHistory::dragEnterEvent(QDragEnterEvent * event)
{
    event->acceptProposedAction();
}

void WidgetShowHistory::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->source() == nullptr)
    {
        for (int i = 0; i < event->mimeData()->urls().count(); i++)
        {
            QString path = QUrl::fromPercentEncoding(event->mimeData()->urls().at(i).toEncoded());
            if (!path.isEmpty())
            {
                // Fix path
                if (path.startsWith("file://"))
                    path = path.mid(7);
#ifdef Q_OS_WIN
                if (path.startsWith("/"))
                    path = path.mid(1);
#endif
                openFile(path);
            }
        }
    }
}
