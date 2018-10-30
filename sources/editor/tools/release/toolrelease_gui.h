#ifndef TOOLRELEASE_GUI_H
#define TOOLRELEASE_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolRelease_gui;
}

class ToolRelease_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolRelease_gui(QWidget *parent = nullptr);
    ~ToolRelease_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolRelease_gui *ui;
};

#endif // TOOLRELEASE_GUI_H
