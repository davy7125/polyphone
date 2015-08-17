/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PAGE_SMPL_H
#define PAGE_SMPL_H

#include <QWidget>
#include "page.h"

namespace Ui
{
    class Page_Smpl;
}

class Page_Smpl : public Page
{
    Q_OBJECT
public:
    explicit Page_Smpl(QWidget * parent = 0);
    ~Page_Smpl();

    // Méthodes publiques
    void afficher();
    void normalisation();
    void enleveBlanc();
    void enleveBlanc(EltID id);
    void enleveFin();
    void enleveFin(EltID id);
    void bouclage();
    void filtreMur();
    void reglerBalance();
    void transposer();
    void sifflements();
    bool isPlaying();
    void pushPlayPause();
    void getPeakFrequencies(EltID id, QList<double> &frequencies, QList<double> &factors, QList<int> &keys, QList<int> &corrections);

signals:
    void noteChanged(int unused, int vel);

private slots:
    void lecture();
    void lecteurFinished();
    void applyEQ();
    void initEQ();
    void sifflements(int freq1, int freq2, double raideur);
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
    void selectionChanged();
    void updateColors();
    void on_pushAutoTune_clicked();

private:
    Ui::Page_Smpl *ui;
    bool lectureEnCours;
    int preventStop;

    // Méthodes privées
    void setRateElt(EltID id, quint32 echFinal);
    EltID getRepercussionID(EltID id);
    void autoTune(EltID id, int &pitch, int &correction);
};


#endif // PAGE_SMPL_H
