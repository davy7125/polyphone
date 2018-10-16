#ifndef TOOLMONITOR_GUI_H
#define TOOLMONITOR_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolMonitor_gui;
}

class ToolMonitor_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMonitor_gui(QWidget *parent = 0);
    ~ToolMonitor_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    bool _isInst;
    Ui::ToolMonitor_gui *ui;
};

#endif // TOOLMONITOR_GUI_H
