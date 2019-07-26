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

#include "soundfontviewer.h"
#include "ui_soundfontviewer.h"
#include "contextmanager.h"
#include "repositorymanager.h"
#include "detailsmanager.h"
#include "soundfontfilter.h"
#include "soundfontdetails.h"

SoundfontViewer::SoundfontViewer(QWidget *parent) : QWidget(parent),
    ui(new Ui::SoundfontViewer)
{
    ui->setupUi(this);

    // Colors
    QColor highlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor border = this->palette().dark().color();
    QColor highlightedHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    QColor buttonBackgroundHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);

    // Style - top
    ui->frameTitle->setStyleSheet("QFrame{background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    ui->frameTitle_2->setStyleSheet("QFrame{background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    ui->frameTitle_2->setMinimumHeight(ui->frameTitle->height());

    ui->pushEdit->setIcon(ContextManager::theme()->getColoredSvg(":/icons/pen.svg", QSize(22, 22), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
    ui->pushEdit->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                "color:" + highlightedText.name() + "}" +
                                "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushSave->setIcon(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
    ui->pushSave->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                "color:" + highlightedText.name() + "}" +
                                "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushCancel->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
    ui->pushCancel->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                  "color:" + highlightedText.name() + "}" +
                                  "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushEdit->hide();

    // Style - left
    QString tmp = QString("QPushButton{background-color:%1; color:%2;border:0px;padding:5px;border-radius:4px;}") +
            "QPushButton:hover{ background-color:%3;}";
    ui->scrollArea->setStyleSheet(tmp.arg(highlighted.name()).arg(highlightedText.name()).arg(buttonBackgroundHover.name()));
    ui->scrollArea_3->setStyleSheet(tmp.arg(highlighted.name()).arg(highlightedText.name()).arg(buttonBackgroundHover.name()));

    // Style - center
    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" + highlighted.name() + ";border-radius:5px;padding:5px}");
    ui->stackedWidget->setStyleSheet("QStackedWidget#stackedWidget{border:1px solid " + border.name() + ";border-top:0;border-right:0;border-bottom:0}");
    ui->stackedWidget_2->setStyleSheet("QStackedWidget#stackedWidget_2{border:1px solid " + border.name() + ";border-top:0;border-right:0;border-bottom:0}");
    ui->scrollMainArea->setStyleSheet("QWidget#scrollMainArea{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                      ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    ui->scrollMainArea_2->setStyleSheet("QWidget#scrollMainArea_2{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                        ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    ui->pageFailed->setStyleSheet("QWidget#pageFailed{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                  ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    ui->pageWait->setStyleSheet("QWidget#pageWait{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");

    // Connection
    connect(ui->viewerLeft, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
}

SoundfontViewer::~SoundfontViewer()
{
    delete ui;
}

void SoundfontViewer::initialize(int soundfontId)
{
    ui->spinner->startAnimation();
    ui->stackedWidget->setCurrentIndex(0);
    _soundfontId = soundfontId;

    // Fill basic information
    SoundfontInformation * soundfontInfo = RepositoryManager::getInstance()->getSoundfontInformation(soundfontId);
    if (soundfontInfo == nullptr)
        return; // Should not be possible
    ui->viewerTop->initialize(soundfontInfo);
    ui->viewerLeft->initialize(soundfontInfo);

    // Ask for the details
    DetailsManager * dm = DetailsManager::getInstance();
    connect(dm, SIGNAL(detailsReady(int)), this, SLOT(onDetailsReady(int)));
    connect(dm, SIGNAL(detailsFailed(int,QString)), this, SLOT(onDetailsFailed(int,QString)));
    dm->askForSoundfontDetails(_soundfontId);
}

void SoundfontViewer::onDetailsReady(int soundfontId)
{
    if (soundfontId != _soundfontId)
        return;

    // Get the details
    SoundfontDetails * sd = DetailsManager::getInstance()->getDetails(_soundfontId);
    if (sd != nullptr)
    {
        ui->viewerCenter->initialize(sd);
        ui->stackedWidget->setCurrentIndex(1);
        ui->pushEdit->show();
    }
    else
        ui->stackedWidget->setCurrentIndex(2);
}

void SoundfontViewer::onDetailsFailed(int soundfontId, QString error)
{
    Q_UNUSED(error)

    if (soundfontId != _soundfontId)
        return;

    ui->stackedWidget->setCurrentIndex(2);
}

void SoundfontViewer::on_pushRetry_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    DetailsManager::getInstance()->askForSoundfontDetails(_soundfontId);
}

void SoundfontViewer::on_pushEdit_clicked()
{
    // Initialize the editor
    SoundfontInformation * soundfontInfo = RepositoryManager::getInstance()->getSoundfontInformation(_soundfontId);
    SoundfontDetails * sd = DetailsManager::getInstance()->getDetails(_soundfontId);
    if (soundfontInfo == nullptr || sd == nullptr)
        return; // Should not be possible
    ui->editorTop->initialize(soundfontInfo);
    ui->editorLeft->initialize(soundfontInfo);
    ui->editorCenter->initialize(sd);

    // Switch the view
    ui->stackWidgetMain->setCurrentIndex(1);
}

void SoundfontViewer::on_pushCancel_clicked()
{
    // Nothing special
    ui->stackWidgetMain->setCurrentIndex(0);
}

void SoundfontViewer::on_pushSave_clicked()
{
    // Save the changes on the website
    /// TODO

    ui->stackWidgetMain->setCurrentIndex(0);
}
