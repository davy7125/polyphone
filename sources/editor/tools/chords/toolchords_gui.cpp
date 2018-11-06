#include "toolchords_gui.h"
#include "ui_toolchords_gui.h"
#include "contextmanager.h"

ToolChords_gui::ToolChords_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolChords_gui)
{
    ui->setupUi(this);

    // Root key selection
    ui->comboRootKey->addItem(trUtf8("mobile"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("C"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("C♯"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("D"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("D♯"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("E"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("F"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("F♯"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("G"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("G♯"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("A"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("A♯"));
    ui->comboRootKey->addItem(trUtf8("fixe : %0").arg("B"));
}

ToolChords_gui::~ToolChords_gui()
{
    delete ui;
}

void ToolChords_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolChords_parameters * params = (ToolChords_parameters *) parameters;
    
    // Instrument & samples parameters
    ui->lineName->setText(params->getInstrumentName());
    ui->comboFreq->setCurrentIndex(params->getDensityType());
    ui->checkLoop->setChecked(params->getLoopSample());
    ui->checkStereo->setChecked(params->getStereoSample());

    // Chord configuration
    ChordInfo ci = params->getChordConfiguration();
    ui->comboType3->setCurrentIndex(ci.chordType3);
    ui->comboType5->setCurrentIndex(ci.chordType5);
    ui->comboType7->setCurrentIndex(ci.chordType7);
    ui->comboType9->setCurrentIndex(ci.chordType9);
    ui->spinHarmonicNumber->setValue(ci.harmonicNumber);
    ui->comboRootKey->setCurrentIndex(ci.rootNote + 1);
    ui->doubleSpinAttenuation->setValue(ci.attenuation);
    ui->horizontalPosition->setValue(ci.position);
}

void ToolChords_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolChords_parameters * params = (ToolChords_parameters *) parameters;
    
    // Instrument & samples parameters
    params->setInstrumentName(ui->lineName->text());
    params->setDensityType(ui->comboFreq->currentIndex());
    params->setLoopSample(ui->checkLoop->isChecked());
    params->setStereoSample(ui->checkStereo->isChecked());

    // Chord configuration
    ChordInfo ci;
    ci.chordType3 = ui->comboType3->currentIndex();
    ci.chordType5 = ui->comboType5->currentIndex();
    ci.chordType7 = ui->comboType7->currentIndex();
    ci.chordType9 = ui->comboType9->currentIndex();
    ci.harmonicNumber = ui->spinHarmonicNumber->value();
    ci.rootNote = ui->comboRootKey->currentIndex() - 1;
    ci.attenuation = ui->doubleSpinAttenuation->value();
    ci.position = ui->horizontalPosition->value();
    params->setChordConfiguration(ci);
}

void ToolChords_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolChords_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
