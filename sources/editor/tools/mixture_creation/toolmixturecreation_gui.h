#ifndef TOOLMIXTURECREATION_GUI_H
#define TOOLMIXTURECREATION_GUI_H

#include "abstracttoolgui.h"
#include "toolmixturecreation_parameters.h"

namespace Ui {
class ToolMixtureCreation_gui;
}

class ToolMixtureCreation_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMixtureCreation_gui(QWidget *parent = nullptr);
    ~ToolMixtureCreation_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushAddDiv_pressed();
    void on_pushRemoveDiv_pressed();
    void on_spinDivStart_valueChanged(int arg1);
    void on_spinDivEnd_valueChanged(int arg1);
    void on_pushAddRank_clicked();
    void on_pushRemoveRank_clicked();
    void on_comboType1_currentIndexChanged(int index);
    void on_comboType2_currentIndexChanged(int index);
    void on_listDivisions_itemSelectionChanged();
    void on_listRangs_itemSelectionChanged();

private:
    QList<QList<int> > getStoredRanks();
    void storeRanks(QList<QList<int> > val);
    void dispDiv();
    void dispRang(int numDiv = -1);
    static QString getText(int overtoneType, int octave);
    int getNumDiv();
    int getNumRang();
    void dispType2(int index);

    Ui::ToolMixtureCreation_gui *ui;
    QList<DivisionInfo> _divisions;
};

#endif // TOOLMIXTURECREATION_GUI_H
