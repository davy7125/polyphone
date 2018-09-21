#include "tooltransposesmpl_gui.h"
#include "ui_tooltransposesmpl_gui.h"
#include "tooltransposesmpl_parameters.h"

ToolTransposeSmpl_gui::ToolTransposeSmpl_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolTransposeSmpl_gui)
{
    ui->setupUi(this);
}

ToolTransposeSmpl_gui::~ToolTransposeSmpl_gui()
{
    delete ui;
}

void ToolTransposeSmpl_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolTransposeSmpl_parameters * params = (ToolTransposeSmpl_parameters *)parameters;
    ui->spinTon->setValue(params->getSemiTones());
}

void ToolTransposeSmpl_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolTransposeSmpl_parameters * params = (ToolTransposeSmpl_parameters *)parameters;
    params->setSemiTones(ui->spinTon->value());
}

void ToolTransposeSmpl_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolTransposeSmpl_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
