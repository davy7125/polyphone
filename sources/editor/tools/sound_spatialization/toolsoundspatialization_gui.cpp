#include "toolsoundspatialization_gui.h"
#include "ui_toolsoundspatialization_gui.h"
#include "toolsoundspatialization_parameters.h"
#include "soundfontmanager.h"

ToolSoundSpatialization_gui::ToolSoundSpatialization_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolSoundSpatialization_gui)
{
    ui->setupUi(this);
}

ToolSoundSpatialization_gui::~ToolSoundSpatialization_gui()
{
    delete ui;
}

void ToolSoundSpatialization_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *) parameters;
    SoundfontManager * sm = SoundfontManager::getInstance();

}

void ToolSoundSpatialization_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *) parameters;

}

void ToolSoundSpatialization_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolSoundSpatialization_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
