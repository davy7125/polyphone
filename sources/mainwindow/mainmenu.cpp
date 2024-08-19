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

#include "mainmenu.h"
#include "contextmanager.h"
#include "soundfont_export/toolsoundfontexport.h"

MainMenu::MainMenu(QWidget * parent) : QMenu(parent),
    _newAction(nullptr),
    _openAction(nullptr),
    _saveAction(nullptr),
    _saveAsAction(nullptr),
    _exportAction(nullptr),
    _fullScreenAction(nullptr),
    _settingsAction(nullptr),
    _helpAction(nullptr),
    _aboutAction(nullptr),
    _closeFileAction(nullptr),
    _closeAction(nullptr),
    _toolExport(new ToolSoundfontExport())
{
    // Style
    this->setStyleSheet(ContextManager::theme()->getMenuTheme());

    // Elements
#ifndef NO_SF2_CREATION
    if (!ContextManager::s_playerMode)
    {
        _newAction = new QAction(tr("&New"), this);
        _newAction->setShortcut(QString("Ctrl+N"));
        connect(_newAction, SIGNAL(triggered()), this, SIGNAL(newClicked()));
        this->addAction(_newAction);
    }
#endif

    _openAction = new QAction(tr("&Open..."), this);
    _openAction->setShortcut(QString("Ctrl+O"));
    connect(_openAction, SIGNAL(triggered()), this, SIGNAL(openClicked()));
    this->addAction(_openAction);

    this->addSeparator();

    if (!ContextManager::s_playerMode)
    {
        _saveAction = new QAction(tr("&Save"), this);
        _saveAction->setShortcut(QString("Ctrl+S"));
        connect(_saveAction, SIGNAL(triggered()), this, SIGNAL(save()));
        this->addAction(_saveAction);

        _saveAsAction = new QAction(tr("Save &as..."), this);
        _saveAsAction->setShortcut(QString("Ctrl+Shift+S"));
        connect(_saveAsAction, SIGNAL(triggered()), this, SIGNAL(saveAs()));
        this->addAction(_saveAsAction);

        _exportAction = new QAction(tr("&Export soundfonts..."), this);
        _exportAction->setShortcut(QString("Ctrl+E"));
        connect(_exportAction, SIGNAL(triggered()), this, SLOT(onExport()));
        this->addAction(_exportAction);

        this->addSeparator();
    }

    _fullScreenAction = new QAction(tr("&Full screen"), this);
    _fullScreenAction->setShortcut(Qt::Key_F11);
    connect(_fullScreenAction, SIGNAL(triggered()), this, SIGNAL(fullScreenTriggered()));
    this->addAction(_fullScreenAction);

    this->addSeparator();

    _settingsAction = new QAction(tr("Se&ttings"), this);
    connect(_settingsAction, SIGNAL(triggered()), this, SIGNAL(openSettingsClicked()));
    this->addAction(_settingsAction);

    _helpAction = new QAction(tr("Online &manual"), this);
    _helpAction->setShortcut(Qt::Key_F1);
    //_helpAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(_helpAction, SIGNAL(triggered()), this, SIGNAL(onlineHelpClicked()));
    this->addAction(_helpAction);

    _helpAction = new QAction(tr("About &Polyphone..."), this);
    connect(_helpAction, SIGNAL(triggered()), this, SIGNAL(aboutClicked()));
    this->addAction(_helpAction);

    this->addSeparator();

    _closeFileAction = new QAction(tr("&Close file"), this);
    _closeFileAction->setShortcut(QString("Ctrl+W"));
    connect(_closeFileAction, SIGNAL(triggered()), this, SIGNAL(closeFileClicked()));
    this->addAction(_closeFileAction);

    _closeAction = new QAction(tr("&Quit"), this);
    _closeAction->setShortcut(QString("Ctrl+Q"));
    connect(_closeAction, SIGNAL(triggered()), this, SIGNAL(closeClicked()));
    this->addAction(_closeAction);

    // Initialize the current sf2
    onTabOpen(false);
}

MainMenu::~MainMenu()
{
    delete _toolExport;
}

void MainMenu::setFullScreen(bool isOn)
{
    _fullScreenAction->blockSignals(true);
    _fullScreenAction->setChecked(isOn);
    _fullScreenAction->blockSignals(false);
}

void MainMenu::onTabOpen(bool isOpen)
{
    if (_saveAction)
        _saveAction->setEnabled(isOpen);
    if (_saveAsAction)
        _saveAsAction->setEnabled(isOpen);
    if (_closeFileAction)
        _closeFileAction->setEnabled(isOpen);
    if (_exportAction)
        _exportAction->setEnabled(isOpen);
}

void MainMenu::onExport()
{
    _toolExport->run();
}
