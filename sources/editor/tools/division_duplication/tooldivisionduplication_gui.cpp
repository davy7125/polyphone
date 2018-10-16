#include "tooldivisionduplication_gui.h"
#include "ui_tooldivisionduplication_gui.h"
#include "tooldivisionduplication_parameters.h"
#include "soundfontmanager.h"

ToolDivisionDuplication_gui::ToolDivisionDuplication_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolDivisionDuplication_gui)
{
    ui->setupUi(this);
}

ToolDivisionDuplication_gui::~ToolDivisionDuplication_gui()
{
    delete ui;
}

void ToolDivisionDuplication_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    _isInst = (ids.isEmpty() || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
    ToolDivisionDuplication_parameters * params = (ToolDivisionDuplication_parameters *) parameters;
    SoundfontManager * sm = SoundfontManager::getInstance();

}

void ToolDivisionDuplication_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolDivisionDuplication_parameters * params = (ToolDivisionDuplication_parameters *) parameters;

}

void ToolDivisionDuplication_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolDivisionDuplication_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
