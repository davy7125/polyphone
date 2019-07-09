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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repositorymanager.h"
#include "usermanager.h"
#include "windowmanager.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "dialogchangelog.h"
#include "dialogkeyboard.h"
#include "dialogrecorder.h"
#include "toprightwidget.h"
#include "editortoolbar.h"
#include <QToolButton>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "outputfactory.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _keyboard(new DialogKeyboard(this)),
    _recorder(new DialogRecorder(this)),
    _dialogAbout(this)
{
    ///////////
    /// GUI ///
    ///////////

    ui->setupUi(this);

    // Window state
    restoreGeometry(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "windowGeometry", QByteArray()).toByteArray());
    restoreState(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "windowState", QByteArray()).toByteArray());

    // Background
    this->setStyleSheet(QString("#centralwidget{background-image: url(:/misc/") +
                        (ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
                             "background_dark.png" : "background.png") + ");}");

    // Icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    ui->tabWidget->setTabIcon(0, ContextManager::theme()->getColoredSvg(":/icons/home.svg", ColoredTabWidget::TAB_ICON_SIZE, replacement));
    QSize iconSize(36, 36);
    ui->pushButtonNew->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonDocumentation->setIcon(ContextManager::theme()->getColoredSvg(":/icons/book.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonForum->setIcon(ContextManager::theme()->getColoredSvg(":/icons/forum.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSettings->setIcon(ContextManager::theme()->getColoredSvg(":/icons/settings.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSearch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/search.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSoundfonts->setIcon(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", iconSize, ThemeManager::BUTTON_TEXT));

    // Top right widget
    TopRightWidget * trw = new TopRightWidget(this);
    ui->tabWidget->setCornerWidget(trw, Qt::Corner::TopRightCorner);
    connect(trw, SIGNAL(newClicked()), this, SLOT(on_pushButtonNew_clicked()));
    connect(trw, SIGNAL(openClicked()), this, SLOT(on_pushButtonOpen_clicked()));
    connect(trw, SIGNAL(openSettingsClicked()), this, SLOT(on_pushButtonSettings_clicked()));
    connect(trw, SIGNAL(onlineHelpClicked()), this, SLOT(on_pushButtonDocumentation_clicked()));
    connect(trw, SIGNAL(aboutClicked()), this, SLOT(onAboutClicked()));
    connect(trw, SIGNAL(closeFileClicked()), this, SLOT(onCloseFile()));
    connect(trw, SIGNAL(closeClicked()), this, SLOT(close()));
    connect(trw, SIGNAL(save()), this, SLOT(onSave()));
    connect(trw, SIGNAL(saveAs()), this, SLOT(onSaveAs()));
    connect(trw, SIGNAL(fullScreenTriggered()), this, SLOT(fullScreenTriggered()));
    connect(trw, SIGNAL(userClicked()), this, SLOT(onUserClicked()));

    // Remove the close button of the first tab "home"
    ui->tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->deleteLater();
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);


    //////////////////////
    /// INITIALIZATION ///
    //////////////////////

    // Window manager
    _windowManager = WindowManager::getInstance(ui->tabWidget);
    connect(ui->widgetShowSoundfonts, SIGNAL(itemClicked(SoundfontFilter*)), _windowManager, SLOT(openRepository(SoundfontFilter*)));
    connect(_windowManager, SIGNAL(keyboardDisplayChanged(bool)), this, SLOT(onKeyboardDisplayChange(bool)));
    connect(_windowManager, SIGNAL(recorderDisplayChanged(bool)), this, SLOT(onRecorderDisplayChange(bool)));
    connect(_windowManager, SIGNAL(editorOpen(bool)), trw, SLOT(onEditorOpen(bool)));

    // Initialize the repository
    RepositoryManager * rm = RepositoryManager::getInstance();
    connect(rm, SIGNAL(initializing()), ui->widgetShowSoundfonts, SLOT(initialize()));
    connect(rm, SIGNAL(ready(QString)), ui->widgetShowSoundfonts, SLOT(soundfontListAvailable(QString)), Qt::QueuedConnection);
    connect(rm, SIGNAL(openSoundfont(int)), _windowManager, SLOT(openRepositorySoundfont(int)));
    rm->initialize();

    // Possibly initialize the user (must be done after the window manager creation)
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "auto_connect", false).toBool())
        UserManager::getInstance()->login();

    // Recent files
    connect(ContextManager::recentFile(), SIGNAL(recentSf2Changed()), this, SLOT(recentSf2Changed()));
    connect(ui->widgetShowHistory, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    recentSf2Changed();

    // Show changelog?
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "last_version_installed", "0.0").toString() != SOFT_VERSION)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "last_version_installed", SOFT_VERSION);
        DialogChangeLog * dialog = new DialogChangeLog(this);
        QTimer::singleShot(500, dialog, SLOT(show()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _recorder;
    WindowManager::kill();
    _windowManager = nullptr;
    delete _keyboard;
    SoundfontManager::kill();
    RepositoryManager::kill();
    ContextManager::kill();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->setFocus();

    // Save state
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "windowGeometry", saveGeometry());
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "windowState", saveState());

    // Number of files not saved
    EltID id(elementSf2);
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<int> nbSf2 = sm->getSiblings(id);
    QStringList fileNames;
    foreach (int i, nbSf2)
    {
        id.indexSf2 = i;
        if (sm->isEdited(i))
        {
            QString name = sm->getQstr(id, champ_name);
            if (name.isEmpty())
                fileNames << trUtf8("untitled");
            else
                fileNames << sm->getQstr(id, champ_name);
        }
    }

    if (!fileNames.empty())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(trUtf8("Warning"));
        msgBox.setText("<b>" + trUtf8("Save before exiting?") + "</b>");
        if (fileNames.count() > 1)
        {
            QString txt = trUtf8("The following files have been modified:") + "<ul>";
            foreach (QString filename, fileNames)
                txt += "<li>" + filename + "</li>";
            txt += "</ul>";
            msgBox.setInformativeText(txt);
        }
        else
            msgBox.setInformativeText(trUtf8("File \"%1\" has been modified.").arg(fileNames[0]));

        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Save"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Cancel"));
        msgBox.button(QMessageBox::Discard)->setText(trUtf8("Do&n't save"));
        msgBox.button(QMessageBox::Save)->setIcon(QIcon::fromTheme("filesave"));
        msgBox.setDefaultButton(QMessageBox::Save);
        switch (msgBox.exec())
        {
        case QMessageBox::Cancel:
            event->ignore();
            return;
        case QMessageBox::Save:
            foreach (int i, nbSf2)
            {
                id.indexSf2 = i;
                if (sm->isEdited(i) && !OutputFactory::save(i, false))
                {
                    event->ignore();
                    return;
                }
            }
            break;
        }
    }
}

void MainWindow::slotCloseTab(int index)
{
    delete ui->tabWidget->widget(index);
}

void MainWindow::recentSf2Changed()
{
    ui->widgetShowHistory->clear();
    for (int i = 0; i < RecentFileManager::MAX_SF2_FILES; i++)
    {
        QString filePath = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_SOUNDFONT, i);
        if (!filePath.isEmpty())
        {
            QDateTime dateTime = ContextManager::recentFile()->getLastFileDateTime(RecentFileManager::FILE_TYPE_SOUNDFONT, i);
            ui->widgetShowHistory->addFile(filePath, dateTime);
        }
    }
}

void MainWindow::on_pushButtonDocumentation_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/" + trUtf8("en/documentation", "path for the documentation online")));
}

void MainWindow::on_pushButtonForum_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/" + trUtf8("en/forum", "path for the forum")));
}

void MainWindow::on_pushButtonSettings_clicked()
{
    _windowManager->openConfiguration();
}

void MainWindow::on_pushButtonSearch_clicked()
{
    SoundfontFilter * sf = new SoundfontFilter(); // Will be deleted in the windowManager
    sf->setSearchText(ui->lineSearch->text());
    _windowManager->openRepository(sf);
}

void MainWindow::on_pushButtonSoundfonts_clicked()
{
    _windowManager->openRepository(nullptr);
}

void MainWindow::on_pushButtonOpen_clicked()
{
    // Ouverture de fichiers
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Opening files"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT),
                                                        trUtf8("Soundfonts") + " (*.sf2 *.sf3 *.sfz *.sfArk);;" +
                                                        trUtf8("Sf2 files") + " (*.sf2);;" +
                                                        trUtf8("Sf3 files") + " (*.sf3);;" +
                                                        trUtf8("Sfz files") + " (*.sfz);;" +
                                                        trUtf8("sfArk archives") + " (*.sfArk)");

    foreach (QString file, strList)
        openFile(file);
}

void MainWindow::on_pushButtonNew_clicked()
{
    _windowManager->openNewSoundfont();
}

void MainWindow::openFile(QString fileName)
{
    _windowManager->openSoundfont(fileName);
}

void MainWindow::onKeyboardDisplayChange(bool isDisplayed)
{
    _keyboard->setVisible(isDisplayed);
}

void MainWindow::onRecorderDisplayChange(bool isDisplayed)
{
    _recorder->setVisible(isDisplayed);
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_K)
    {
        if (!_keyboard->isVisible())
        {
            _keyboard->show();
            _keyboard->activateWindow();
            EditorToolBar::updateKeyboardButtonsState(true);
        }
        else
        {
            _keyboard->activateWindow();
            _keyboard->glow();
        }
        event->accept();
    }
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z)
    {
        // Undo
        int currentSf2 = _windowManager->getCurrentSf2();
        if (currentSf2 != -1)
            SoundfontManager::getInstance()->undo(currentSf2);
        event->accept();
    }
    else if ((event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Y) ||
             (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_Z))
    {
        // Redo
        int currentSf2 = _windowManager->getCurrentSf2();
        if (currentSf2 != -1)
            SoundfontManager::getInstance()->redo(currentSf2);
        event->accept();
    }
    else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F)
    {
        // Search
        if (ui->lineSearch->isVisible())
            ui->lineSearch->setFocus();
        event->accept();
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::fullScreenTriggered()
{
    this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::onAboutClicked()
{
    _dialogAbout.show();
}

void MainWindow::onCloseFile()
{
    if (_windowManager->getCurrentSf2() != -1)
        _windowManager->closeCurrentTab();
}

void MainWindow::onSave()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(_windowManager->getCurrentSf2(), false);
}

void MainWindow::onSaveAs()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(_windowManager->getCurrentSf2(), true);
}

void MainWindow::onUserClicked()
{
    _windowManager->openUser();
}

void MainWindow::on_lineSearch_returnPressed()
{
    if (!ui->lineSearch->text().isEmpty())
        on_pushButtonSearch_clicked();
}
