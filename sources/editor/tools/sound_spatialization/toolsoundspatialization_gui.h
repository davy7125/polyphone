#ifndef TOOLSOUNDSPATIALIZATION_GUI_H
#define TOOLSOUNDSPATIALIZATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolSoundSpatialization_gui;
}

class ToolSoundSpatialization_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolSoundSpatialization_gui(QWidget *parent = 0);
    ~ToolSoundSpatialization_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    bool _isInst;
    Ui::ToolSoundSpatialization_gui *ui;
};

#endif // TOOLSOUNDSPATIALIZATION_GUI_H
