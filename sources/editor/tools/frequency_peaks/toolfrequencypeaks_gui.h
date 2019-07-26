/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

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
    explicit ToolFrequencyPeaks_gui(QWidget *parent = nullptr);
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
