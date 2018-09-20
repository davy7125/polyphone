#ifndef TOOLEXTERNALCOMMAND_GUI_H
#define TOOLEXTERNALCOMMAND_GUI_H

#include "abstracttoolgui.h"
class AbstractToolParameters;

namespace Ui {
class ToolExternalCommand_gui;
}

class ToolExternalCommand_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolExternalCommand_gui(QWidget *parent = 0);
    ~ToolExternalCommand_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_comboPrevious_currentIndexChanged(const QString &arg1);
    void on_pushOpen_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ToolExternalCommand_gui *ui;
    static const int HISTORY_SIZE;
    static bool isCommandValid(QString command);
};

#endif // TOOLEXTERNALCOMMAND_GUI_H
