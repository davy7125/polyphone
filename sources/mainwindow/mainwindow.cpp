/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "repositorymanager.h"
#include "windowmanager.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "dialogchangelog.h"
#include "dialogkeyboard.h"
#include "dialogrecorder.h"
#include <QToolButton>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _keyboard(new DialogKeyboard(this)),
    _recorder(new DialogRecorder(this))
{
    ///////////
    /// GUI ///
    ///////////

    ui->setupUi(this);

    // Window state
    this->setMaximumSize(QApplication::desktop()->size());
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
    ui->tabWidget->setTabIcon(0, ContextManager::theme()->getColoredSvg(":/misc/logo.svg", QSize(32, 32), replacement));
    ui->pushButtonNew->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonOpen->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonDocumentation->setIcon(ContextManager::theme()->getColoredSvg(":/icons/book.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonForum->setIcon(ContextManager::theme()->getColoredSvg(":/icons/forum.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonSettings->setIcon(ContextManager::theme()->getColoredSvg(":/icons/settings.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonSearch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/search.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));
    ui->pushButtonSoundfonts->setIcon(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", QSize(48, 48), ThemeManager::BUTTON_TEXT));

    // Create a menu button what must be placed in tabs row
    QToolButton* tb = new QToolButton(this);
    tb->setText(trUtf8("Menu"));
    connect(tb, SIGNAL(clicked()), this, SLOT(slotAddTab()));
    ui->tabWidget->setCornerWidget(tb, Qt::Corner::TopRightCorner);

    // Remove the close button of the first tab "home"
    ui->tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->deleteLater();
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, NULL);


    //////////////////////
    /// INITIALIZATION ///
    //////////////////////

    // Window manager
    _windowManager = new WindowManager(ui->tabWidget);
    connect(ui->widgetShowSoundfonts, SIGNAL(itemClicked(SoundfontFilter*)), _windowManager, SLOT(openRepository(SoundfontFilter*)));
    connect(_windowManager, SIGNAL(keyboardDisplayChanged(bool)), this, SLOT(onKeyboardDisplayChange(bool)));
    connect(_windowManager, SIGNAL(recorderDisplayChanged(bool)), this, SLOT(onRecorderDisplayChange(bool)));

    // Initialize the repository
    RepositoryManager * rm = RepositoryManager::getInstance();
    connect(rm, SIGNAL(initializing()), ui->widgetShowSoundfonts, SLOT(initialize()));
    connect(rm, SIGNAL(ready(QString)), ui->widgetShowSoundfonts, SLOT(soundfontListAvailable(QString)), Qt::QueuedConnection);
    rm->initialize();

    // Recent files
    connect(ContextManager::recentFile(), SIGNAL(recentSf2Changed()), this, SLOT(recentSf2Changed()));
    connect(ui->widgetShowHistory, SIGNAL(openFile(QString)), this, SLOT(openFile(QString)));
    recentSf2Changed();

    // Show changelog?
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "last_version_installed", 0.).toDouble() != VERSION && FINAL)
    {
        DialogChangeLog * dialog = new DialogChangeLog(this);
        QTimer::singleShot(500, dialog, SLOT(show()));
        ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "last_version_installed", VERSION);
    }
}

MainWindow::~MainWindow()
{
    RepositoryManager::kill();
    delete ui;
    delete _recorder;
    delete _keyboard;
    delete _windowManager;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->setFocus();

    // Save state
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "windowGeometry", saveGeometry());
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "windowState", saveState());

    // Number of files not saved
    int nbFile = 0;
    EltID id(elementSf2);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    QList<int> nbSf2 = sf2->getSiblings(id);
    QString qStr = "";
    foreach (int i, nbSf2)
    {
        id.indexSf2 = i;
        if (sf2->isEdited(i))
        {
            if (nbFile)
                qStr.append("<br /> - " + sf2->getQstr(id, champ_name));
            else
                qStr = sf2->getQstr(id, champ_name);
            nbFile++;
        }
    }

    if (nbFile)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(trUtf8("Attention"));
        msgBox.setText(trUtf8("<b>Sauvegarder avant de quitter ?</b>"));
        if (nbFile > 1)
        {
            qStr.prepend(trUtf8("Les fichiers suivants n'ont pas été sauvegardés :<br/> - "));
            msgBox.setInformativeText(qStr);
        }
        else
        {
            qStr = trUtf8("Le fichier « ") + qStr + trUtf8(" » n'a pas été sauvegardé.");
            msgBox.setInformativeText(qStr);
        }
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Enregistrer"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Annuler"));
        msgBox.button(QMessageBox::Discard)->setText(trUtf8("&Quitter sans enregistrer"));
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
                if (sf2->isEdited(i))// && sauvegarder(i, false))
                {
                    event->ignore();
                    return;
                }
            }
            break;
        }
    }

    // Close magnéto
}

void MainWindow::slotAddTab()
{
    QWidget* newTab = new QWidget(ui->tabWidget);
    ui->tabWidget->addTab(newTab, tr("Tab %1").arg(QString::number(ui->tabWidget->count())));
    ui->tabWidget->setCurrentWidget(newTab);
}

void MainWindow::slotCloseTab(int index)
{
    delete ui->tabWidget->widget(index);
}

void MainWindow::dragAndDrop(QString path, EltID idDest, int *arg)
{

}

void MainWindow::recentSf2Changed()
{
    ui->widgetShowHistory->clear();
    for (int i = 0; i < RecentFileManager::MAX_SF2_FILES; i++)
    {
        QString filePath = ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_SF2, i);
        if (!filePath.isEmpty())
        {
            QDateTime dateTime = ContextManager::recentFile()->getLastFileDateTime(RecentFileManager::FILE_TYPE_SF2, i);
            ui->widgetShowHistory->addFile(filePath, dateTime);
        }
    }
}

void MainWindow::on_pushButtonDocumentation_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/" + trUtf8("fr/documentation", "path for the documentation online")));
}

void MainWindow::on_pushButtonForum_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/" + trUtf8("fr/forum", "path for the forum")));
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
    _windowManager->openRepository(NULL);
}

void MainWindow::on_pushButtonOpen_clicked()
{
    // Ouverture de fichiers
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Ouverture de fichiers"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT),
                                                        trUtf8("Soundfonts") + " (*.sf2 *.sf3 *.sfz *.sfArk);;" +
                                                        trUtf8("Fichiers .sf2") + " (*.sf2);;" +
                                                        trUtf8("Fichiers .sf3") + " (*.sf3);;" +
                                                        trUtf8("Fichiers .sfz") + " (*.sfz);;" +
                                                        trUtf8("Archives .sfArk") + " (*.sfArk)");

    foreach (QString file, strList)
        openFile(file);
}

void MainWindow::on_pushButtonNew_clicked()
{
    _windowManager->openNewSoundfont();
}

void MainWindow::openFile(QString fileName)
{
    if (fileName.left(7).compare("file://") == 0)
        fileName = fileName.right(fileName.length() - 7);
#ifdef Q_OS_WIN
    if (fileName.left(1).compare("/") == 0)
        fileName = fileName.remove(0, 1);
#endif

    if (fileName.endsWith("2"))
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SF2, fileName);
    else
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SOUNDFONT, fileName);
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
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_K)
    {
        if (!_keyboard->isVisible())
        {
            _keyboard->show();
            _keyboard->activateWindow();
        }
        else
        {
            _keyboard->activateWindow();
            _keyboard->glow();
        }
    }
    QMainWindow::keyPressEvent(event);
}
