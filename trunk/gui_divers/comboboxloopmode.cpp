#include "comboboxloopmode.h"
#include "thememanager.h"

ComboBoxLoopMode::ComboBoxLoopMode(QWidget *parent) : QComboBox(parent),
    _ignoreFirstHide(true)
{
    this->setView(new ComboView());

    // Different loop possibilities
    if (ThemeManager::getInstance()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        this->addItem(QIcon(":/icones/loop_off_w.png"), "", 0);
        this->addItem(QIcon(":/icones/loop_on_w.png"), "", 1);
        this->addItem(QIcon(":/icones/loop_on_end_w.png"), "", 3);
    }
    else
    {
        this->addItem(QIcon(":/icones/loop_off.png"), "", 0);
        this->addItem(QIcon(":/icones/loop_on.png"), "", 1);
        this->addItem(QIcon(":/icones/loop_on_end.png"), "", 3);
    }

    QObject::connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
}

void ComboBoxLoopMode::showEvent(QShowEvent * event)
{
    QComboBox::showEvent(event);
    this->showPopup();
}

void ComboBoxLoopMode::hidePopup()
{
    if (_ignoreFirstHide)
        _ignoreFirstHide = false;
    else
    {
        this->blockSignals(true);
        // Remove all elements
        this->clear();

        // End the editor
        this->setDisabled(true);
    }
}

void ComboBoxLoopMode::onActivated(int index)
{
    // Select the right index
    if (this->view()->currentIndex().isValid())
        this->setCurrentIndex(index);

    // End the editor
    this->setDisabled(true);
}
