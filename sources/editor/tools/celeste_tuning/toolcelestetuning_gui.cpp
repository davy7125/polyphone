#include "toolcelestetuning_gui.h"
#include "ui_toolcelestetuning_gui.h"
#include "toolcelestetuning_parameters.h"
#include "contextmanager.h"

ToolCelesteTuning_gui::ToolCelesteTuning_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolCelesteTuning_gui)
{
    ui->setupUi(this);
}

ToolCelesteTuning_gui::~ToolCelesteTuning_gui()
{
    delete ui;
}

void ToolCelesteTuning_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolCelesteTuning_parameters * params = (ToolCelesteTuning_parameters *) parameters;

    // Load parameters
    ui->doubleSpinHerz->setValue(params->getBaseFrequency());
    ui->doubleSpinDiv->setValue(params->getCoefficient());
    ui->label->setText(trUtf8("Nombre de battements par secondes") + " (" + trUtf8("note") + " " + ContextManager::keyName()->getKeyName(60) + ")");
}

void ToolCelesteTuning_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolCelesteTuning_parameters * params = (ToolCelesteTuning_parameters *) parameters;

    // Save current parameters
    params->setBaseFrequency(ui->doubleSpinHerz->value());
    params->setCoefficient(ui->doubleSpinDiv->value());
}

void ToolCelesteTuning_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolCelesteTuning_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
