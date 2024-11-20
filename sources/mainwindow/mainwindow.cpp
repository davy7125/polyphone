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

#include <QToolButton>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QWindow>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repositorymanager.h"
#include "usermanager.h"
#include "tabmanager.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "dialogchangelog.h"
#include "dialogkeyboard.h"
#include "dialogrecorder.h"
#include "editortoolbar.h"
#include "outputfactory.h"
#include "inputfactory.h"
#include "extensionmanager.h"
#include "utils.h"
#include "playeroptions.h"

const int MainWindow::RESIZE_BORDER_WIDTH = 5;

MainWindow::MainWindow(bool playerMode, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    _recorder(new DialogRecorder(this)),
    _dialogAbout(this)
{
    ///////////
    /// GUI ///
    ///////////

    ContextManager::s_playerMode = playerMode;
    ui->setupUi(this);
    this->setWindowTitle(tr("Polyphone SoundFont Editor"));
    this->setWindowIcon(QIcon(":/misc/polyphone.png"));

    // Possibly remove the window borders
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "window_borders", false).toBool())
    {
        ui->gridLayoutTop->setContentsMargins(
            ui->gridLayoutTop->contentsMargins().right(), // Left is smaller, right value is used
            ui->gridLayoutTop->contentsMargins().top(),
            ui->gridLayoutTop->contentsMargins().right(),
            ui->gridLayoutTop->contentsMargins().bottom());
    }
    else
    {
        this->setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        QApplication::instance()->installEventFilter(this);
        ui->stackedWidget->setStyleSheet(
            QString("MainStackedWidget{border:1px solid %1;border-top: 0}")
                .arg(ContextManager::theme()->getColor(ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
                                                           ThemeManager::LIST_BACKGROUND : ThemeManager::LIST_TEXT).name())
            );
    }

#ifdef NO_SF2_CREATION
    ui->pushButtonNew->hide();
#else
    // Can still be hidden with the player mode
    if (playerMode)
        ui->pushButtonNew->hide();
#endif

    // Window state
    restoreGeometry(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "windowGeometry", QByteArray()).toByteArray());

    // Icons
    QSize iconSize(36, 36);
    ui->pushButtonNew->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonDocumentation->setIcon(ContextManager::theme()->getColoredSvg(":/icons/book.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonForum->setIcon(ContextManager::theme()->getColoredSvg(":/icons/forum.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSettings->setIcon(ContextManager::theme()->getColoredSvg(":/icons/settings.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSearch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/search.svg", iconSize, ThemeManager::BUTTON_TEXT));
    ui->pushButtonSoundfonts->setIcon(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", iconSize, ThemeManager::BUTTON_TEXT));

    // Top right widget
    connect(ui->topRightWidget, SIGNAL(newClicked()), this, SLOT(on_pushButtonNew_clicked()));
    connect(ui->topRightWidget, SIGNAL(openClicked()), this, SLOT(on_pushButtonOpen_clicked()));
    connect(ui->topRightWidget, SIGNAL(openSettingsClicked()), this, SLOT(on_pushButtonSettings_clicked()));
    connect(ui->topRightWidget, SIGNAL(onlineHelpClicked()), this, SLOT(on_pushButtonDocumentation_clicked()));
    connect(ui->topRightWidget, SIGNAL(aboutClicked()), this, SLOT(onAboutClicked()));
    connect(ui->topRightWidget, SIGNAL(closeFileClicked()), this, SLOT(onCloseFile()));
    connect(ui->topRightWidget, SIGNAL(closeClicked()), this, SLOT(close()));
    connect(ui->topRightWidget, SIGNAL(minimizeClicked()), this, SLOT(showMinimized()));
    connect(ui->topRightWidget, SIGNAL(save()), this, SLOT(onSave()));
    connect(ui->topRightWidget, SIGNAL(saveAs()), this, SLOT(onSaveAs()));
    connect(ui->topRightWidget, SIGNAL(fullScreenTriggered()), this, SLOT(fullScreenTriggered()));
    connect(ui->topRightWidget, SIGNAL(userClicked()), this, SLOT(onUserClicked()));

    //////////////////////
    /// INITIALIZATION ///
    //////////////////////

    // Global keyboard
    _keyboard = playerMode ? nullptr : new DialogKeyboard(this);

    // Tab manager
    ui->stackedWidget->setControls(ui->pushHome, ui->tabBar);
    _tabManager = TabManager::prepareInstance(_keyboard, ui->stackedWidget);
    connect(ui->widgetShowSoundfonts, SIGNAL(itemClicked(SoundfontFilter*)), _tabManager, SLOT(openRepository(SoundfontFilter*)));
    connect(_tabManager, SIGNAL(keyboardDisplayChanged(bool,bool)), this, SLOT(onKeyboardDisplayChange(bool,bool)));
    connect(_tabManager, SIGNAL(recorderDisplayChanged(bool,bool)), this, SLOT(onRecorderDisplayChange(bool,bool)));
    connect(_tabManager, SIGNAL(tabOpen(bool)), ui->topRightWidget, SLOT(onTabOpen(bool)));

#ifdef NO_SF2_REPOSITORY
    ui->widgetRepo->hide();
    ui->gridLayout_2->setColumnMinimumWidth(1, 0);
    ui->gridLayout_2->setColumnStretch(1, 0);
#else
    // Initialize the repository
    RepositoryManager * rm = RepositoryManager::getInstance();
    connect(rm, SIGNAL(initializing()), ui->widgetShowSoundfonts, SLOT(initialize()));
    connect(rm, SIGNAL(ready(QString)), ui->widgetShowSoundfonts, SLOT(soundfontListAvailable(QString)), Qt::QueuedConnection);
    connect(rm, SIGNAL(openSoundfont(int)), _tabManager, SLOT(openRepositorySoundfont(int)));
    rm->initialize();

    // Possibly initialize the user (must be done after the window manager creation)
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "auto_connect", false).toBool())
        UserManager::getInstance()->login();
#endif

    // Recent files
    connect(ContextManager::recentFile(), SIGNAL(recentSf2Changed()), this, SLOT(recentSf2Changed()));
    connect(ui->widgetShowHistory, SIGNAL(openFile(QString)), this, SLOT(openFiles(QString)));
    recentSf2Changed();

    // Utils
    Utils::prepareStaticVariables();

    // Show changelog if one of the 2 first version number is not identical
    QStringList lastVersion = ContextManager::configuration()->getValue(
                                                                 ConfManager::SECTION_NONE, "last_version_installed", "0.0.0").toString().split('.');
    QStringList currentVersion = QString(SOFT_VERSION).split('.');
    if (lastVersion.size() < 2 || lastVersion[0] != currentVersion[0] || lastVersion[1] != currentVersion[1])
    {
        DialogChangeLog * dialog = new DialogChangeLog(this);
        QTimer::singleShot(500, dialog, SLOT(show()));
    }
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "last_version_installed", SOFT_VERSION);
    ui->widgetShowHistory->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _recorder;
    TabManager::kill();
    _tabManager = nullptr;
    delete _keyboard;
    SoundfontManager::kill();
    RepositoryManager::kill();
    ExtensionManager::kill();
    ContextManager::kill();
    InputFactory::clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->setFocus();

    // Save state
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "windowGeometry", saveGeometry());

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
                name = sm->getQstr(id, champ_filenameInitial).split(QRegularExpression("(/|\\\\)")).last();
            if (name.isEmpty())
                name = tr("Untitled");
            fileNames << name;
        }
    }

    if (!fileNames.empty() && !ContextManager::s_playerMode)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText("<b>" + tr("Save before exiting?") + "</b>");
        if (fileNames.count() > 1)
        {
            QString txt = tr("The following files have been modified:") + "<ul>";
            foreach (QString filename, fileNames)
                txt += "<li>" + filename + "</li>";
            txt += "</ul>";
            msgBox.setInformativeText(txt);
        }
        else
            msgBox.setInformativeText(tr("File \"%1\" has been modified.").arg(fileNames[0]));

        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(tr("&Save"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("&Cancel"));
        msgBox.button(QMessageBox::Discard)->setText(tr("Do&n't save"));
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

    if (_keyboard)
        _keyboard->updateState(false);
    _recorder->updateState(false);
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
    QDesktopServices::openUrl(QUrl("https://www.polyphone.io/documentation"));
}

void MainWindow::on_pushButtonForum_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone.io/forum"));
}

void MainWindow::on_pushButtonSettings_clicked()
{
    _tabManager->openConfiguration();
}

void MainWindow::on_pushButtonSearch_clicked()
{
    SoundfontFilter * sf = new SoundfontFilter(); // Will be deleted in the windowManager
    sf->setSearchText(ui->lineSearch->text());
    _tabManager->openRepository(sf);
}

void MainWindow::on_pushButtonSoundfonts_clicked()
{
    _tabManager->openRepository(nullptr);
}

void MainWindow::on_pushButtonOpen_clicked()
{
    // Open files
    QStringList strList = QFileDialog::getOpenFileNames(this, tr("Opening files"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT),
                                                        InputFactory::getFileFilter());
    openFiles(strList.join('|'));
}

void MainWindow::on_pushButtonNew_clicked()
{
    _tabManager->openNewSoundfont();
}

void MainWindow::openFiles(QString fileNames)
{
    // Possible player options
    PlayerOptions playerOptions;
    QStringList split = fileNames.split("||");
    if (split.size() > 1)
        playerOptions.parse(split[1]);

    QStringList files = split[0].split('|', Qt::SkipEmptyParts);
    foreach (QString file, files)
        _tabManager->openSoundfont(file, &playerOptions);
}

void MainWindow::onKeyboardDisplayChange(bool isDisplayed, bool propagate)
{
    if (_keyboard != nullptr)
    {
        if (!propagate)
            _keyboard->updateState(false);
        _keyboard->setVisible(isDisplayed);
        _keyboard->updateState(true);
    }
}

void MainWindow::onRecorderDisplayChange(bool isDisplayed, bool propagate)
{
    if (!propagate)
        _recorder->updateState(false);
    _recorder->setVisible(isDisplayed);
    _recorder->updateState(true);
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        switch (event->key())
        {
        case Qt::Key_F: // Search
            if (ui->lineSearch->isVisible())
                ui->lineSearch->setFocus();
            event->accept();
            break;
        case Qt::Key_H: // Go to home
            _tabManager->showHome();
            event->accept();
            break;
        case Qt::Key_K: // Display / focus keyboard
            if (_keyboard != nullptr)
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
            }
            event->accept();
            break;
        case Qt::Key_Y: { // Redo
            int currentSf2 = _tabManager->getCurrentSf2();
            if (currentSf2 != -1)
                SoundfontManager::getInstance()->redo(currentSf2);
            event->accept();
        } break;
        case Qt::Key_Z: { // Undo
            int currentSf2 = _tabManager->getCurrentSf2();
            if (currentSf2 != -1)
                SoundfontManager::getInstance()->undo(currentSf2);
            event->accept();
        } break;
        case Qt::Key_Tab: case Qt::Key_PageDown: // Go to next tab
            _tabManager->setCurrentWidget(ui->tabBar->getNextWidget());
            event->accept();
            break;
        case Qt::Key_PageUp: // Go to previous tab
            _tabManager->setCurrentWidget(ui->tabBar->getPreviousWidget());
            event->accept();
            break;
        default:
            break;
        }
    }
    else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        switch (event->key())
        {
        case Qt::Key_Z: { // Redo
            int currentSf2 = _tabManager->getCurrentSf2();
            if (currentSf2 != -1)
                SoundfontManager::getInstance()->redo(currentSf2);
            event->accept();
        } break;
        case Qt::Key_Backtab: // Go to previous tab
            _tabManager->setCurrentWidget(ui->tabBar->getPreviousWidget());
            event->accept();
            break;
        default:
            break;
        }
    }

    QWidget::keyPressEvent(event);
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
    if (_tabManager->getCurrentSf2() != -1)
        _tabManager->closeCurrentTab();
}

void MainWindow::onSave()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(_tabManager->getCurrentSf2(), false);
}

void MainWindow::onSaveAs()
{
    // Remove the focus from the interface (so that all changes are taken into account)
    this->setFocus();

    OutputFactory::save(_tabManager->getCurrentSf2(), true);
}

void MainWindow::onUserClicked()
{
    _tabManager->openUser();
}

void MainWindow::on_lineSearch_returnPressed()
{
    if (!ui->lineSearch->text().isEmpty())
        on_pushButtonSearch_clicked();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    if (event->type() == QEvent::MouseMove && !this->isMaximized())
    {
        QWidget *w = qobject_cast<QWidget*>(object);
        if (w == nullptr || w->window() != this)
        {
            this->unsetCursor();
            return false;
        }

        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() != Qt::NoButton)
            return false;

        _mousePositionEdges = QFlags<Qt::Edge>();
#if QT_VERSION < 0x060000
        QPoint pos = this->mapFromGlobal(mouseEvent->globalPos());
#else
        QPoint pos = this->mapFromGlobal(mouseEvent->globalPosition().toPoint());
#endif
        if (pos.x() >= 0 && pos.x() <= RESIZE_BORDER_WIDTH)
            _mousePositionEdges |= Qt::LeftEdge;
        else if (pos.x() >= this->width() - RESIZE_BORDER_WIDTH && pos.x() <= this->width())
            _mousePositionEdges |= Qt::RightEdge;
        if (pos.y() >= 0 && pos.y() <= RESIZE_BORDER_WIDTH)
            _mousePositionEdges |= Qt::TopEdge;
        else if (pos.y() >= this->height() - RESIZE_BORDER_WIDTH && pos.y() <= this->height())
            _mousePositionEdges |= Qt::BottomEdge;

        if ((int)_mousePositionEdges > 0)
        {
            switch (_mousePositionEdges)
            {
            case Qt::LeftEdge:
                this->setCursor(Qt::SizeHorCursor);
                break;
            case Qt::LeftEdge | Qt::TopEdge:
                this->setCursor(Qt::SizeFDiagCursor);
                break;
            case Qt::LeftEdge | Qt::BottomEdge:
                this->setCursor(Qt::SizeBDiagCursor);
                break;
            case Qt::RightEdge:
                this->setCursor(Qt::SizeHorCursor);
                break;
            case Qt::RightEdge | Qt::TopEdge:
                this->setCursor(Qt::SizeBDiagCursor);
                break;
            case Qt::RightEdge | Qt::BottomEdge:
                this->setCursor(Qt::SizeFDiagCursor);
                break;
            case Qt::TopEdge:
                this->setCursor(Qt::SizeVerCursor);
                break;
            case Qt::BottomEdge:
                this->setCursor(Qt::SizeVerCursor);
                break;
            default:
                this->unsetCursor();
                return false;
            }
            return true;
        }

        this->unsetCursor();
        return false;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton && (int)_mousePositionEdges > 0)
        {
            QWindow * w = this->topLevelWidget()->windowHandle();
            if (w != nullptr)
                w->startSystemResize(_mousePositionEdges);
            return true;
        }
        return false;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        _mousePositionEdges = QFlags<Qt::Edge>();
        this->unsetCursor();
    }

    return false;
}
