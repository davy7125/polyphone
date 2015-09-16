#include "comboboxloopmode.h"

ComboBoxLoopMode::ComboBoxLoopMode(QWidget *parent) : QComboBox(parent)
{
    this->setView(new ComboView());

    this->addItem(QIcon(":/icones/loop_off.png"), "", 0);
    this->addItem(QIcon(":/icones/loop_on.png"), "", 1);
    this->addItem(QIcon(":/icones/loop_on_end.png"), "", 3);
}
