#include "toolmonitor_gui.h"
#include "ui_toolmonitor_gui.h"
#include "toolmonitor_parameters.h"
#include "soundfontmanager.h"

ToolMonitor_gui::ToolMonitor_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMonitor_gui)
{
    ui->setupUi(this);
}

ToolMonitor_gui::~ToolMonitor_gui()
{
    delete ui;
}

void ToolMonitor_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolMonitor_parameters * params = (ToolMonitor_parameters *) parameters;
    SoundfontManager * sm = SoundfontManager::getInstance();

}

void ToolMonitor_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMonitor_parameters * params = (ToolMonitor_parameters *) parameters;

}

void ToolMonitor_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolMonitor_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
