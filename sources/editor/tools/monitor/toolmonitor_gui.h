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
    explicit ToolMonitor_gui(QWidget *parent = nullptr);
    ~ToolMonitor_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_comboParameter_currentIndexChanged(int index);
    void on_checkLog_stateChanged(int arg1);
    void on_buttonBox_accepted();

private:
    Ui::ToolMonitor_gui *ui;
    bool _isInst;
    EltID _initialID;
    QList<AttributeType> _attributeList;
};

#endif // TOOLMONITOR_GUI_H
