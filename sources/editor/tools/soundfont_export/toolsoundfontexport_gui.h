#ifndef TOOLSOUNDFONTEXPORT_GUI_H
#define TOOLSOUNDFONTEXPORT_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolSoundfontExport_gui;
}

class ToolSoundfontExport_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolSoundfontExport_gui(QWidget *parent = nullptr);
    ~ToolSoundfontExport_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_pushTick_clicked();
    void on_pushUntick_clicked();
    void on_pushFolder_clicked();
    void on_pushAnnuler_clicked();
    void on_pushExport_clicked();
    void on_comboFormat_currentIndexChanged(int index);

private:
    Ui::ToolSoundfontExport_gui *ui;
    QMap<int, QList<int> > _presetList;
};

#endif // TOOLSOUNDFONTEXPORT_GUI_H
