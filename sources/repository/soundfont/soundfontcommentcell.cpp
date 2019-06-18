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

#include "soundfontcommentcell.h"
#include "ui_soundfontcommentcell.h"
#include "soundfontcommentdata.h"
#include "contextmanager.h"

SoundfontCommentCell::SoundfontCommentCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontCommentCell)
{
    ui->setupUi(this);
    QColor titleColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                0.5);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = titleColor.name();
    ui->labelUserIcon->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), replacement));
    ui->labelDateIcon->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), replacement));
    ui->labelDate->setStyleSheet("QLabel{color:" + titleColor.name() + ";}");
    ui->labelUser->setStyleSheet("QLabel{color:" + titleColor.name() + ";}");

    ui->textComment->setContentsMargins(0, 0, 0, 0);
    ui->textComment->document()->setDocumentMargin(0);
}

SoundfontCommentCell::~SoundfontCommentCell()
{
    delete ui;
}

void SoundfontCommentCell::initialize(SoundfontCommentData * data, int level)
{
    // Info
    ui->labelUser->setText(data->getUserName());
    ui->labelDate->setText(data->getCreationTime().toString(Qt::SystemLocaleShortDate));
    ui->textComment->setHtml(data->getText());

    // Level
    int maxLevel = 5;
    for (int i = 0; i < qMin(level, maxLevel); i++)
    {
        QWidget * verticalBar = new QWidget(this);
        verticalBar->setFixedWidth(22);
        verticalBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        QColor color = ThemeManager::mix(
                    ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
                    ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                    (double)i / maxLevel);

        verticalBar->setStyleSheet("QWidget{margin-left: 6px; margin-right: 14px;background-color: " + color.name() + ";}");
        ui->leftBars->addWidget(verticalBar);
    }
}

void SoundfontCommentCell::resizeEvent(QResizeEvent *event)
{
    // Compute the height of the text browser
    int height = ui->textComment->document()->size().height();
    ui->textComment->setMinimumHeight(height);
    ui->textComment->setMaximumHeight(height);

    if (event != nullptr)
        QWidget::resizeEvent(event);
}
