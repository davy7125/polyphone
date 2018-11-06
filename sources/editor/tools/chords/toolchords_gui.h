#ifndef TOOLCHORDS_GUI_H
#define TOOLCHORDS_GUI_H

#include "abstracttoolgui.h"
#include "toolchords_parameters.h"

namespace Ui {
class ToolChords_gui;
}

class ToolChords_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolChords_gui(QWidget *parent = nullptr);
    ~ToolChords_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolChords_gui *ui;
};

#endif // TOOLCHORDS_GUI_H
