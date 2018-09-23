/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PAGE_SMPL_H
#define PAGE_SMPL_H

#include <QWidget>
#include "page.h"

namespace Ui
{
    class PageSmpl;
}

class PageSmpl : public Page
{
    Q_OBJECT
public:
    explicit PageSmpl(QWidget * parent = 0);
    ~PageSmpl();

    bool isPlaying();
    void pushPlayPause();
    void getPeakFrequencies(EltID id, QList<double> &frequencies, QList<double> &factors, QList<int> &keys, QList<int> &corrections);

protected:
    // Update the interface
    bool updateInterface(QString editingSource, IdList selectedIds, int displayOption);

private slots:
    void lecture();
    void lecteurFinished();
    void applyEQ();
    void initEQ();
    void setStartLoop();
    void setStartLoop(int val);
    void setEndLoop();
    void setEndLoop(int val);
    void on_pushFullLength_clicked();
    void setRootKey();
    void setRootKey(int val);
    void setTune();
    void setTune(int val);
    void setType(int index);
    void setLinkedSmpl(int index);
    void setRate(int index);
    void on_checkLectureBoucle_stateChanged(int arg1);
    void setSinusEnabled(bool val);
    void setGainSample(int val);
    void setStereo(bool val);
    void on_pushAutoTune_clicked();
    void onLinkClicked(EltID id);

private:
    Ui::PageSmpl *ui;
    bool lectureEnCours;
    int preventStop;
    IdList _currentIds;

    // Méthodes privées
    void setRateElt(EltID id, quint32 echFinal);
    EltID getRepercussionID(EltID id);
    void autoTune(EltID id, int &pitch, int &correction);
    void saveEQ();
    void loadEQ();
};


#endif // PAGE_SMPL_H
