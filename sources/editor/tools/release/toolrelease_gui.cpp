#include "toolrelease_gui.h"
#include "ui_toolrelease_gui.h"
#include "toolrelease_parameters.h"
#include "contextmanager.h"

ToolRelease_gui::ToolRelease_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolRelease_gui)
{
    ui->setupUi(this);
}

ToolRelease_gui::~ToolRelease_gui()
{
    delete ui;
}

void ToolRelease_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolRelease_parameters * params = (ToolRelease_parameters *) parameters;

    // Load parameters
    ui->doubleDuree36->setValue(params->getBaseDuration());
    ui->doubleDivision->setValue(params->getEvolution());
    ui->doubleDeTune->setValue(params->getDetune());
    ui->label->setText(trUtf8("Durée release (note %1)").arg(ContextManager::keyName()->getKeyName(36)));
}

void ToolRelease_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolRelease_parameters * params = (ToolRelease_parameters *) parameters;

    // Save current parameters
    params->setBaseDuration(ui->doubleDuree36->value());
    params->setEvolution(ui->doubleDivision->value());
    params->setDetune(ui->doubleDeTune->value());
}

void ToolRelease_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolRelease_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
