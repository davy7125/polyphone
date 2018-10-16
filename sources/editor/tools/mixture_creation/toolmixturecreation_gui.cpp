#include "toolmixturecreation_gui.h"
#include "ui_toolmixturecreation_gui.h"
#include "toolmixturecreation_parameters.h"
#include "contextmanager.h"

ToolMixtureCreation_gui::ToolMixtureCreation_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMixtureCreation_gui)
{
    ui->setupUi(this);
}

ToolMixtureCreation_gui::~ToolMixtureCreation_gui()
{
    delete ui;
}

void ToolMixtureCreation_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *) parameters;
}

void ToolMixtureCreation_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *) parameters;

}

void ToolMixtureCreation_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolMixtureCreation_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
