#ifndef TOOLSOUNDSPATIALIZATION_GUI_H
#define TOOLSOUNDSPATIALIZATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
  class ToolSoundSpatialization_gui;
}

class ToolSoundSpatialization_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolSoundSpatialization_gui(QWidget *parent = nullptr);
    ~ToolSoundSpatialization_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_comboPattern_currentIndexChanged(int index);
    void on_spinNbDivision_valueChanged(int arg1);
    void on_spinFilling_valueChanged(int arg1);
    void on_spinSpreading_valueChanged(int arg1);
    void on_spinOffset_valueChanged(int arg1);
    void on_checkFlip_stateChanged(int arg1);
    void on_checkFlop_stateChanged(int arg1);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    double spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending);
    double spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow);
    void updateRenversements();
    void updateGraph();
    void computeData(QVector<double> &x, QVector<int> &y);

    static double getAxe(int note, int nbDiv, bool sens);

    Ui::ToolSoundSpatialization_gui *ui;
    bool _isInst;
    int _minKey, _maxKey;
};

#endif // TOOLSOUNDSPATIALIZATION_GUI_H
