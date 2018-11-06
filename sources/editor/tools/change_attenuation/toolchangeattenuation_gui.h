#ifndef TOOLCHANGEATTENUATION_GUI_H
#define TOOLCHANGEATTENUATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolChangeAttenuation_gui;
}

class ToolChangeAttenuation_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolChangeAttenuation_gui(QWidget *parent = 0);
    ~ToolChangeAttenuation_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    static QString formatDouble(double value);

    bool _isInst;
    Ui::ToolChangeAttenuation_gui *ui;
};

#endif // TOOLCHANGEATTENUATION_GUI_H
