#include "toolpresetlist_gui.h"
#include "ui_toolpresetlist_gui.h"
#include "toolpresetlist_parameters.h"
#include "contextmanager.h"

ToolPresetList_gui::ToolPresetList_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolPresetList_gui)
{
    ui->setupUi(this);
}

ToolPresetList_gui::~ToolPresetList_gui()
{
    delete ui;
}

void ToolPresetList_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolPresetList_parameters * params = (ToolPresetList_parameters *) parameters;

    // Load parameters
}

void ToolPresetList_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolPresetList_parameters * params = (ToolPresetList_parameters *) parameters;

    // Save current parameters
}

void ToolPresetList_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolPresetList_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
