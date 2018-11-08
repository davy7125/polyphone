#include "configsectionsound.h"
#include "ui_configsectionsound.h"
#include "contextmanager.h"

ConfigSectionSound::ConfigSectionSound(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionSound)
{
    ui->setupUi(this);
}

ConfigSectionSound::~ConfigSectionSound()
{
    delete ui;
}

void ConfigSectionSound::initialize()
{
    // Volume
    int gain = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    ui->sliderGain->blockSignals(true);
    ui->sliderGain->setValue(gain);
    ui->sliderGain->blockSignals(false);
    ui->labelGain->setNum(gain);

    // Reverb
    ui->dialRevNiveau->blockSignals(true);
    ui->dialRevNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt());
    ui->dialRevNiveau->blockSignals(false);

    ui->dialRevProfondeur->blockSignals(true);
    ui->dialRevProfondeur->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt());
    ui->dialRevProfondeur->blockSignals(false);

    ui->dialRevDensite->blockSignals(true);
    ui->dialRevDensite->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt());
    ui->dialRevDensite->blockSignals(false);

    ui->dialRevAttenuation->blockSignals(true);
    ui->dialRevAttenuation->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt());
    ui->dialRevAttenuation->blockSignals(false);

    // Chorus
    ui->dialChoNiveau->blockSignals(true);
    ui->dialChoNiveau->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt());
    ui->dialChoNiveau->blockSignals(false);

    ui->dialChoAmplitude->blockSignals(true);
    ui->dialChoAmplitude->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt());
    ui->dialChoAmplitude->blockSignals(false);

    ui->dialChoFrequence->blockSignals(true);
    ui->dialChoFrequence->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt());
    ui->dialChoFrequence->blockSignals(false);
}

void ConfigSectionSound::on_dialRevNiveau_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", value);
}

void ConfigSectionSound::on_dialRevProfondeur_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", value);
}

void ConfigSectionSound::on_dialRevDensite_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", value);
}

void ConfigSectionSound::on_dialRevAttenuation_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", value);
}

void ConfigSectionSound::on_dialChoNiveau_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", value);
}

void ConfigSectionSound::on_dialChoAmplitude_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", value);
}

void ConfigSectionSound::on_dialChoFrequence_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", value);
}

void ConfigSectionSound::on_sliderGain_valueChanged(int value)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", value);
    ui->labelGain->setText(QString::number(value));
}
