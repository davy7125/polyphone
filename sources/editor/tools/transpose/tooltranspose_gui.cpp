#include "tooltranspose_gui.h"
#include "ui_tooltranspose_gui.h"
#include "tooltranspose_parameters.h"
#include "contextmanager.h"

ToolTranspose_gui::ToolTranspose_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolTranspose_gui)
{
    ui->setupUi(this);
}

ToolTranspose_gui::~ToolTranspose_gui()
{
    delete ui;
}

void ToolTranspose_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolTranspose_parameters * params = (ToolTranspose_parameters *) parameters;

    // Load parameters
    
}

void ToolTranspose_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolTranspose_parameters * params = (ToolTranspose_parameters *) parameters;

    // Save current parameters
    
}

void ToolTranspose_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolTranspose_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
