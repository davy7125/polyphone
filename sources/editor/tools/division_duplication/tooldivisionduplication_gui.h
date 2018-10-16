#ifndef TOOLDIVISIONDUPLICATION_GUI_H
#define TOOLDIVISIONDUPLICATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolDivisionDuplication_gui;
}

class ToolDivisionDuplication_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolDivisionDuplication_gui(QWidget *parent = 0);
    ~ToolDivisionDuplication_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    bool _isInst;
    Ui::ToolDivisionDuplication_gui *ui;
};

#endif // TOOLDIVISIONDUPLICATION_GUI_H
