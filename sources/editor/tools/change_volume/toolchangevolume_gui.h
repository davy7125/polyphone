#ifndef TOOLCHANGEVOLUME_GUI_H
#define TOOLCHANGEVOLUME_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolChangeVolume_gui;
}

class ToolChangeVolume_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolChangeVolume_gui(QWidget *parent = 0);
    ~ToolChangeVolume_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_radioAdd_toggled(bool checked);
    void on_radioMultiply_toggled(bool checked);
    void on_radioNormalize_toggled(bool checked);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolChangeVolume_gui *ui;
};

#endif // TOOLCHANGEVOLUME_GUI_H
