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

#include "soundfonteditorcenter.h"
#include "ui_soundfonteditorcenter.h"
#include "contextmanager.h"
#include "soundfontdetails.h"

SoundfontEditorCenter::SoundfontEditorCenter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontEditorCenter)
{
    ui->setupUi(this);

    // Colors
    QColor highlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor border = this->palette().dark().color();
    QColor highlightedHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);

    // Style
    QString styleSheetTitle = "QLabel { background-color:" + highlighted.name() +
            "; color: " + highlightedText.name() + ";border-radius: 2px; padding: 0 5px; }";
    ui->labelDescription->setStyleSheet(styleSheetTitle);
    ui->frameEditDownload->setStyleSheet("QFrame{background-color:" + highlighted.name() +
                                         "; color: " + highlightedText.name() + ";border:0;border-radius: 2px; }");
    ui->pushAddFile->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                   "color:" + highlightedText.name() + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                   "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushAddFile->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(12, 12), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
}

SoundfontEditorCenter::~SoundfontEditorCenter()
{
    delete ui;
}

void SoundfontEditorCenter::initialize(SoundfontDetails *details)
{
    ui->editorDescription->initialize(details->getDescription());
    ui->editorFiles->initialize(details->getDownloads());
}
