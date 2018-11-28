#include "mainmenu.h"
#include "contextmanager.h"
#include "soundfont_export/toolsoundfontexport.h"

MainMenu::MainMenu(QWidget * parent) : QMenu(parent),
    _toolExport(new ToolSoundfontExport())
{
    // Style
    this->setStyleSheet(QString("QMenu::separator {background: ") +
                        ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                          ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.5).name() +
                        ";margin: 10px 45px; height: 1px}");

    // Elements
    _newAction = new QAction(trUtf8("&New"), this);
    _newAction->setShortcut(QString("Ctrl+N"));
    connect(_newAction, SIGNAL(triggered()), this, SIGNAL(newClicked()));
    this->addAction(_newAction);

    _openAction = new QAction(trUtf8("&Open..."), this);
    _openAction->setShortcut(QString("Ctrl+O"));
    connect(_openAction, SIGNAL(triggered()), this, SIGNAL(openClicked()));
    this->addAction(_openAction);

    this->addSeparator();

    _saveAction = new QAction(trUtf8("&Save"), this);
    _saveAction->setShortcut(QString("Ctrl+S"));
    connect(_saveAction, SIGNAL(triggered()), this, SIGNAL(save()));
    this->addAction(_saveAction);

    _saveAsAction = new QAction(trUtf8("Save &as..."), this);
    _saveAsAction->setShortcut(QString("Ctrl+Shift+S"));
    connect(_saveAsAction, SIGNAL(triggered()), this, SIGNAL(saveAs()));
    this->addAction(_saveAsAction);

    _exportAction = new QAction(trUtf8("&Export soundfonts"), this);
    _exportAction->setShortcut(QString("Ctrl+E"));
    connect(_exportAction, SIGNAL(triggered()), this, SLOT(onExport()));
    this->addAction(_exportAction);

    this->addSeparator();

    _fullScreenAction = new QAction(trUtf8("&Full screen"), this);
    _fullScreenAction->setShortcut(Qt::Key_F11);
    _fullScreenAction->setCheckable(true);
    connect(_fullScreenAction, SIGNAL(triggered()), this, SIGNAL(fullScreenTriggered()));
    this->addAction(_fullScreenAction);

    this->addSeparator();

    _settingsAction = new QAction(trUtf8("Se&ttings"), this);
    connect(_settingsAction, SIGNAL(triggered()), this, SIGNAL(openSettingsClicked()));
    this->addAction(_settingsAction);

    _helpAction = new QAction(trUtf8("Online &manual"), this);
    _helpAction->setShortcut(Qt::Key_F1);
    connect(_helpAction, SIGNAL(triggered()), this, SIGNAL(onlineHelpClicked()));
    this->addAction(_helpAction);

    _helpAction = new QAction(trUtf8("About &Polyphone..."), this);
    connect(_helpAction, SIGNAL(triggered()), this, SIGNAL(aboutClicked()));
    this->addAction(_helpAction);

    this->addSeparator();

    _closeFileAction = new QAction(trUtf8("&Close file"), this);
    _closeFileAction->setShortcut(QString("Ctrl+W"));
    connect(_closeFileAction, SIGNAL(triggered()), this, SIGNAL(closeFileClicked()));
    this->addAction(_closeFileAction);

    _closeAction = new QAction(trUtf8("&Quit"), this);
    _closeAction->setShortcut(QString("Ctrl+Q"));
    connect(_closeAction, SIGNAL(triggered()), this, SIGNAL(closeClicked()));
    this->addAction(_closeAction);

    // Initialize the current sf2
    onEditorOpen(false);
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

void MainMenu::onEditorOpen(bool isOpen)
{
    if (isOpen)
    {
        _saveAction->setEnabled(true);
        _saveAsAction->setEnabled(true);
        _closeFileAction->setEnabled(true);
        _exportAction->setEnabled(true);
    }
    else
    {
        _saveAction->setEnabled(false);
        _saveAsAction->setEnabled(false);
        _closeFileAction->setEnabled(false);
        _exportAction->setEnabled(false);
    }
}

void MainMenu::onExport()
{
    _toolExport->run();
}
