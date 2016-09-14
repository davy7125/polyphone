#include "comboboxloopmode.h"
#include "thememanager.h"
#include <QMouseEvent>

ComboBoxLoopMode::ComboBoxLoopMode(QWidget *parent) : QComboBox(parent)
{
    this->setView(new ComboView());

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
}

void ComboBoxLoopMode::showEvent(QShowEvent * event)
{
    QComboBox::showEvent(event);
    this->showPopup();
}

void ComboBoxLoopMode::hidePopup()
{
    if (this->view()->currentIndex().isValid())
        this->setCurrentIndex(this->view()->currentIndex().row());
    this->setDisabled(true);
}
