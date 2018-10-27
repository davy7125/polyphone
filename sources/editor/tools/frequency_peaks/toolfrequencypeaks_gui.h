#ifndef TOOLFREQUENCYPEAKS_GUI_H
#define TOOLFREQUENCYPEAKS_GUI_H

#include "abstracttoolgui.h"
#include "toolfrequencypeaks_parameters.h"

namespace Ui {
class ToolFrequencyPeaks_gui;
}

class ToolFrequencyPeaks_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolFrequencyPeaks_gui(QWidget *parent = 0);
    ~ToolFrequencyPeaks_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

signals:
    /// A sample has been processed
    void peakComputed(EltID id, const SampleFrequencyInfo sfi);

private slots:
    void on_pushExport_clicked();
    void on_pushClose_clicked();
    void onPeakComputed(EltID id, const SampleFrequencyInfo sfi);

private:
    Ui::ToolFrequencyPeaks_gui *ui;
    QString _path;
    QString _sf2Name;
    QList<SampleFrequencyInfo> _data;
    IdList _idsToCompute;
};

#endif // TOOLFREQUENCYPEAKS_GUI_H
