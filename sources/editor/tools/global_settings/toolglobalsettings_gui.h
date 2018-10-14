#ifndef TOOLGLOBALSETTINGS_GUI_H
#define TOOLGLOBALSETTINGS_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolGlobalSettings_gui;
}

class ToolGlobalSettings_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolGlobalSettings_gui(QWidget *parent = 0);
    ~ToolGlobalSettings_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_comboPattern_currentIndexChanged(int index);
    void on_doubleSpinParam_valueChanged(double arg1);
    void on_doubleSpinMin_valueChanged(double arg1);
    void on_doubleSpinMax_valueChanged(double arg1);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    bool _isInst;
    Ui::ToolGlobalSettings_gui *ui;
};

#endif // TOOLGLOBALSETTINGS_GUI_H
