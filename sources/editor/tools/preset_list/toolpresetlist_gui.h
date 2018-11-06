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
    explicit ToolPresetList_gui(QWidget *parent = nullptr);
    ~ToolPresetList_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_radioCsv_clicked();
    void on_radioHtml_clicked();
    void on_pushCopy_clicked();
    void on_pushClose_clicked();

private:
    Ui::ToolPresetList_gui *ui;
    QMap<int, QMap<int, QString> > _mapName;
};

#endif // TOOLPRESETLIST_GUI_H
