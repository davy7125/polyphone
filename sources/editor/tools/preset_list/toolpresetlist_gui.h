#ifndef TOOLPRESETLIST_GUI_H
#define TOOLPRESETLIST_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolPresetList_gui;
}

class ToolPresetList_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolPresetList_gui(QWidget *parent = 0);
    ~ToolPresetList_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolPresetList_gui *ui;
};

#endif // TOOLPRESETLIST_GUI_H
