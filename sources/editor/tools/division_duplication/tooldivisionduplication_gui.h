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
    void on_pushAdd_clicked();
    void on_pushRemove_clicked();
    void on_spinMinVel_valueChanged(int arg1);
    void on_spinMaxVel_valueChanged(int arg1);
    void on_checkForEachVelocityRange_clicked();
    void on_listVelocites_currentRowChanged(int currentRow);

private:
    void dispVel();
    void storeVelocities(QVector<QPair<int, int> > val);

    Ui::ToolDivisionDuplication_gui *ui;
    bool _isInst;
    QList<QPair<int, int> > _listeVelocites;
};

#endif // TOOLDIVISIONDUPLICATION_GUI_H
