#include "windowmanager.h"
#include "contextmanager.h"
#include "configpanel.h"
#include "soundfontbrowser.h"
#include "coloredtabwidget.h"
#include "soundfontmanager.h"
#include "soundfontfilter.h"
#include "editor.h"
#include "inputfactory.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QAbstractButton>

WindowManager::WindowManager(ColoredTabWidget *tabWidget) : QObject(NULL),
    _tabWidget(tabWidget),
    _configTab(new ConfigPanel()),
    _browserTab(new SoundfontBrowser()),
    _inputFactory(new InputFactory())
{
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    connect(sf2, SIGNAL(editingDone(QString,QList<int>)), this, SLOT(editingDone(QString,QList<int>)));
    connect(_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested(int)));
}

WindowManager::~WindowManager()
{
    delete _inputFactory;
    delete _configTab;
    delete _browserTab;
    while (!_editors.isEmpty())
        delete _editors.takeFirst();
}

void WindowManager::openConfiguration()
{
    _configTab->initializeInterface();
    int index = _tabWidget->indexOf(_configTab);
    if (index == -1)
        index = _tabWidget->addTab(_configTab,
                                   ContextManager::theme()->getColoredSvg(":/icons/settings.svg", QSize(32, 32), ThemeManager::WINDOW_TEXT),
                                   trUtf8("Préférences"));
    _tabWidget->setCurrentIndex(index);
}

void WindowManager::openNewSoundfont()
{
    // Create a new editor
    Editor * editor = new Editor();
    int index = _tabWidget->addColoredTab(editor, ":/icons/file-audio.svg", "",
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT));
    connect(editor, SIGNAL(tabTitleChanged(QString)), this, SLOT(onTabTitleChanged(QString)));
    _editors << editor;

    // Initialize and display it
    editor->initialize(_inputFactory->getInput(""));
    _tabWidget->setCurrentIndex(index);
}

void WindowManager::openSoundfont(QString fileName)
{
    // Check if the file is not already open?
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    int indexSf2 = -1;
    foreach (int i, sf2->getSiblings(EltID(elementSf2)))
    {
        if (sf2->getQstr(EltID(elementSf2, i), champ_filenameInitial) == fileName)
        {
            indexSf2 = i;
            break;
        }
    }

    // Find the corresponding editor if the file is already open
    if (indexSf2 != -1)
    {
        foreach (Editor * editor, _editors)
        {
            if (editor->getSf2Index() == indexSf2)
            {
                int index = _tabWidget->indexOf(editor);
                _tabWidget->setCurrentIndex(index);
                return;
            }
        }
    }

    // Otherwise, create a new editor
    Editor * editor = new Editor();
    int index = _tabWidget->addColoredTab(editor, ":/icons/file-audio.svg", QFileInfo(fileName).fileName(),
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT));
    connect(editor, SIGNAL(tabTitleChanged(QString)), this, SLOT(onTabTitleChanged(QString)));
    _editors << editor;

    // Initialize and display it
    editor->initialize(_inputFactory->getInput(fileName));
    _tabWidget->setCurrentIndex(index);
}

void WindowManager::openRepository(SoundfontFilter *filter)
{
    if (filter)
    {
        _browserTab->applyFilter(filter);
        delete filter;
    }
    int index = _tabWidget->indexOf(_browserTab);
    if (index == -1)
        index = _tabWidget->addColoredTab(_browserTab, ":/icons/globe.svg", trUtf8("Soundfonts en ligne"),
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                          ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT));
    _tabWidget->setCurrentIndex(index);
}

void WindowManager::openUser()
{

}

void WindowManager::editingDone(QString source, QList<int> sf2Indexes)
{
    // Update all editing pages related to one of the edited sf2
    foreach (Editor* editor, _editors)
        if (sf2Indexes.contains(editor->getSf2Index()))
            editor->update(source);
}

void WindowManager::onTabTitleChanged(QString title)
{
    int index = _tabWidget->indexOf((Editor*)QObject::sender());
    if (index != -1)
        _tabWidget->setTabText(index, title);
}

void WindowManager::onTabCloseRequested(int tabIndex)
{
    QWidget * widget = _tabWidget->widget(tabIndex);
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    if (_editors.contains((Editor*)widget))
    {
        // Close a soundfont
        Editor * editor = (Editor*)widget;

        int ret;
        EltID id(elementSf2, editor->getSf2Index());
        if (sf2->isEdited(id.indexSf2))
        {
            QMessageBox msgBox(_tabWidget);
            msgBox.setIcon(QMessageBox::Warning);
            id.typeElement = elementSf2;
            QString qStr = trUtf8("<b>Sauvegarder avant de quitter ?</b>");
            msgBox.setText(qStr);
            qStr = trUtf8("Le fichier « ") + sf2->getQstr(id,champ_name) + trUtf8(" » a été modifié.");
            msgBox.setInformativeText(qStr);
            msgBox.setWindowTitle(trUtf8("Attention"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.button(QMessageBox::Save)->setText(trUtf8("&Enregistrer"));
            msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Annuler"));
            msgBox.button(QMessageBox::Discard)->setText(trUtf8("&Fermer sans enregistrer"));
            msgBox.button(QMessageBox::Save)->setIcon(QIcon::fromTheme("filesave"));
            msgBox.setDefaultButton(QMessageBox::Save);
            ret = msgBox.exec();
        }
        else
            ret = QMessageBox::Discard;

        switch (ret)
        {
        case QMessageBox::Cancel:
            return;
        case QMessageBox::Save:
            //if (sauvegarder(id.indexSf2, false))
                return;
        case QMessageBox::Discard:
            sf2->remove(id);
            break;
        }

        _editors.removeAll(editor);
        _tabWidget->removeTab(tabIndex);
        delete editor;
    }
    else if (widget == (QWidget *)_configTab)
    {
        // Close the configurations
        _tabWidget->removeTab(tabIndex);
    }
    else if (widget == (QWidget *)_browserTab)
    {
        // Close the soundfont browser
        _tabWidget->removeTab(tabIndex);
    }
}
