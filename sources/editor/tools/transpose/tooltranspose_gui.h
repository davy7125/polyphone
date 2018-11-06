#ifndef TOOLTRANSPOSE_GUI_H
#define TOOLTRANSPOSE_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolTranspose_gui;
}

class ToolTranspose_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolTranspose_gui(QWidget *parent = 0);
    ~ToolTranspose_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolTranspose_gui *ui;
};

#endif // TOOLTRANSPOSE_GUI_H
