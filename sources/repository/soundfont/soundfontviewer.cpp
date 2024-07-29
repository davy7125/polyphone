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

#include "soundfontviewer.h"
#include "ui_soundfontviewer.h"
#include "contextmanager.h"
#include "repositorymanager.h"
#include "detailsmanager.h"
#include "soundfontfilter.h"
#include "soundfontdetails.h"
#include "soundfontdescriptiondata.h"
#include "usermanager.h"
#include "urlreader.h"
#include "uploadingdialog.h"
#include <QMessageBox>

SoundfontViewer::SoundfontViewer(QWidget *parent) : QWidget(parent),
    ui(new Ui::SoundfontViewer),
    _urlReader(new UrlReader(RepositoryManager::BASE_URL + "upload", false)),
    _waitingDialog(nullptr)
{
    ui->setupUi(this);

    // Colors
    QColor highlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor border = ContextManager::theme()->getColor(ThemeManager::BORDER);
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

    // Hide the edit button
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

    // Connections
    connect(ui->viewerLeft, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
    connect(_urlReader, SIGNAL(downloadCompleted(QString)), this, SLOT(postCompleted(QString)), Qt::QueuedConnection);
    connect(_urlReader, SIGNAL(progressChanged(int)), this, SLOT(postProgressChanged(int)));
    connect(RepositoryManager::getInstance(), SIGNAL(ready(QString)), this, SLOT(onRefreshReady(QString)));
    connect(DetailsManager::getInstance(), SIGNAL(detailsReady(int)), this, SLOT(onDetailsReady(int)));
    connect(DetailsManager::getInstance(), SIGNAL(detailsFailed(int,QString)), this, SLOT(onDetailsFailed(int,QString)));
}

SoundfontViewer::~SoundfontViewer()
{
    delete _waitingDialog;
    delete _urlReader;
    delete ui;
}

void SoundfontViewer::initialize(int soundfontId, bool forceReload)
{
    ui->spinner->startAnimation();
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackWidgetMain->setCurrentIndex(0);
    ui->pushEdit->hide();
    _soundfontId = soundfontId;

    // Fill basic information
    SoundfontInformation * soundfontInfo = RepositoryManager::getInstance()->getSoundfontInformation(soundfontId);
    if (soundfontInfo == nullptr) // Invalid id
    {
        delete _waitingDialog;
        _waitingDialog = nullptr;
        ui->stackedWidget->setCurrentIndex(2);
        return;
    }
    ui->viewerTop->initialize(soundfontInfo);
    ui->viewerLeft->initialize(soundfontInfo);

    // Ask for the details
    DetailsManager::getInstance()->askForSoundfontDetails(_soundfontId, forceReload);
}

void SoundfontViewer::onDetailsReady(int soundfontId)
{
    if (soundfontId != _soundfontId)
        return;

    // A dialog can be open
    delete _waitingDialog;
    _waitingDialog = nullptr;

    // Get the details
    SoundfontDetails * sd = DetailsManager::getInstance()->getDetails(_soundfontId);
    if (sd != nullptr)
    {
        ui->viewerCenter->initialize(sd);
        ui->stackedWidget->setCurrentIndex(1);

        // Check if it's possible for the user to edit the soundfont
        ui->pushEdit->setVisible(UserManager::getInstance()->getConnectionState() == UserManager::CONNECTED_ADMIN ||
                                 (sd->getDescription()->getCreatedBy() != -1 &&
                sd->getDescription()->getCreatedBy() == UserManager::getInstance()->getUserId()));
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
    DetailsManager::getInstance()->askForSoundfontDetails(_soundfontId, false);
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
    // Check the editing is valid
    QString error = ui->editorTop->getEditingError();
    if (!error.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), error);
        return;
    }
    error = ui->editorLeft->getEditingError();
    if (!error.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), error);
        return;
    }
    error = ui->editorCenter->getEditingError();
    if (!error.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), error);
        return;
    }

    // Get a full description of the editing
    QMap<QString, QString> arguments;
    ui->editorTop->fillArguments(arguments);
    ui->editorLeft->fillArguments(arguments);
    ui->editorCenter->fillArguments(arguments);
    QMap<QString, QString> files = ui->editorCenter->getFileArguments();

    // Save the changes on the website
    _urlReader->clearArguments();
    _urlReader->addArgument("id", QString::number(_soundfontId));
    _urlReader->addArgument("user", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "email", "").toString());
    _urlReader->addArgument("pass", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString());
    foreach (QString key, arguments.keys())
        _urlReader->addArgument(key, arguments[key]);
    foreach (QString key, files.keys())
        if (!files[key].isEmpty())
            _urlReader->addFileAsArgument(key, files[key]);
    _urlReader->download();

    // Open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;
    _waitingDialog = new UploadingDialog(100, this);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onUploadCancel()));
}

void SoundfontViewer::onUploadCancel()
{
    // Stop the query
    _urlReader->stop();

    // Delete the waiting dialog
    delete _waitingDialog;
    _waitingDialog = nullptr;
}

void SoundfontViewer::postProgressChanged(int progress)
{
    _waitingDialog->setValue(progress);
}

void SoundfontViewer::postCompleted(QString error)
{
    // Analyze the result
    int currentId = -1;
    if (error.isEmpty())
    {
        // The answer is either the soundfont id that has been updated / added or an error
        QString answer = _urlReader->getRawData();
        bool ok = false;
        currentId = answer.toInt(&ok);
        if (!ok)
        {
            currentId = -1;
            error = answer;
        }
    }

    if (error.isEmpty() && currentId >= 0)
    {
        // Successfully upload the soundfont: reload the soundfont list and the page
        _soundfontId = currentId;
        RepositoryManager::getInstance()->initialize();
    }
    else
    {
        // Delete the dialog and notify that the upload failed
        delete _waitingDialog;
        _waitingDialog = nullptr;
        QMessageBox::warning(this, tr("Warning"), tr("The upload failed: %1").arg(error));
    }
}

void SoundfontViewer::onRefreshReady(QString error)
{
    if (error.isEmpty())
    {
        // Refresh the page
        this->initialize(_soundfontId, true);
    }
    else
    {
        // The page cannot be refreshed
        delete _waitingDialog;
        _waitingDialog = nullptr;
        onDetailsFailed(_soundfontId, error);
    }
}
