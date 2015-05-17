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

#ifndef DIALOG_MIXTURE_H
#define DIALOG_MIXTURE_H

#include <QDialog>

namespace Ui
{
    class DialogMixture;
}

class DialogMixture : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogMixture(QWidget *parent = 0);
    ~DialogMixture();

signals:
    void accepted(QList<QList<int> > listeParam, QString nom,
                  bool bouclage, int freq, bool stereo);

private slots:
    void accept();
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
    Ui::DialogMixture *ui;
    QList<QList<int> > listeParam;

    // Méthodes privées
    void dispDiv();
    void dispRang(int numDiv = -1);
    static QString getText(int type1, int type2);
    int getNumDiv();
    int getNumRang();
    void dispType2(int index);
};

#endif // DIALOG_MIXTURE_H
