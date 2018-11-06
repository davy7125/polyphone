#ifndef TOOLCELESTETUNING_GUI_H
#define TOOLCELESTETUNING_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolCelesteTuning_gui;
}

class ToolCelesteTuning_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolCelesteTuning_gui(QWidget *parent = 0);
    ~ToolCelesteTuning_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolCelesteTuning_gui *ui;
};

#endif // TOOLCELESTETUNING_GUI_H
