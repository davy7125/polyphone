#ifndef TOOLFREQUENCYFILTER_GUI_H
#define TOOLFREQUENCYFILTER_GUI_H

#include "abstracttoolgui.h"
#include "qcustomplot.h"

namespace Ui {
class ToolFrequencyFilter_gui;
}

class ToolFrequencyFilter_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolFrequencyFilter_gui(QWidget *parent = 0);
    ~ToolFrequencyFilter_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolFrequencyFilter_gui *ui;
};

#endif // TOOLFREQUENCYFILTER_GUI_H
