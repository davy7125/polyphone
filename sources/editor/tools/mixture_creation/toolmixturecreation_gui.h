#ifndef TOOLMIXTURECREATION_GUI_H
#define TOOLMIXTURECREATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolMixtureCreation_gui;
}

class ToolMixtureCreation_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMixtureCreation_gui(QWidget *parent = 0);
    ~ToolMixtureCreation_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolMixtureCreation_gui *ui;
};

#endif // TOOLMIXTURECREATION_GUI_H
