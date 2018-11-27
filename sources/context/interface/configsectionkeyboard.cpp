#include "configsectionkeyboard.h"
#include "ui_configsectionkeyboard.h"
#include "contextmanager.h"

ConfigSectionKeyboard::ConfigSectionKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionKeyboard)
{
    ui->setupUi(this);
    connect(ui->tableKeyboardMap, SIGNAL(combinaisonChanged(int,int,QString)), this, SLOT(combinaisonChanged(int,int,QString)));
    this->renameComboDo();
}

ConfigSectionKeyboard::~ConfigSectionKeyboard()
{
    delete ui;
}

void ConfigSectionKeyboard::initialize()
{
    // Name of the keys in the table
    for (int i = 0; i <= 12; i++)
    {
        ui->tableKeyboardMap->horizontalHeaderItem(i)->setText(ContextManager::keyName()->getKeyName(i, true, false, false, true));
    }

    // Populate the table
    ui->tableKeyboardMap->blockSignals(true);
    ui->tableKeyboardMap->populate();
    ui->tableKeyboardMap->blockSignals(false);

    // Octave configuration
    ui->comboDo->blockSignals(true);
    int octaveMapping = ContextManager::configuration()->getValue(ConfManager::SECTION_MAP, "octave_offset", 3).toInt();
    if (octaveMapping >= ui->comboDo->count())
        octaveMapping = 3;
    else if (octaveMapping < 0)
        octaveMapping = 0;
    ui->comboDo->setCurrentIndex(octaveMapping);
    ui->comboDo->blockSignals(false);

    // Default velocity
    ui->spinDefaultVelocity->blockSignals(true);
    ui->spinDefaultVelocity->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "velocity", 127).toInt());
    ui->spinDefaultVelocity->blockSignals(false);
}

void ConfigSectionKeyboard::renameComboDo()
{
    int nbElement = ui->comboDo->count();
    for (int i = 0; i < nbElement; i++)
        ui->comboDo->setItemText(i, ContextManager::keyName()->getKeyName(12 * i));
}

void ConfigSectionKeyboard::combinaisonChanged(int key, int numOctave, QString combinaison)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP,
                                              "key_" + QString::number(numOctave) + "_" + QString::number(key),
                                              combinaison);
}

void ConfigSectionKeyboard::on_comboDo_currentIndexChanged(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_MAP, "octave_offset", index);
}

void ConfigSectionKeyboard::on_spinDefaultVelocity_editingFinished()
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "velocity", ui->spinDefaultVelocity->value());
}
