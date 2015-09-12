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

#ifndef PAGE_INST_H
#define PAGE_INST_H

#include <QWidget>
#include "pagetable.h"

namespace Ui {
class Page_Inst;
}

class Page_Inst : public PageTable
{
    Q_OBJECT
public:
    explicit Page_Inst(QWidget *parent = 0);
    ~Page_Inst();
    void setModVisible(bool visible);
    void afficher();
    void desaccorder();
    void repartitionAuto();
    void mixture();
    void release();
    void transposer();

private slots:
    void desaccorder(double doHerz, double division);
    void desaccorder(EltID id, double doHerz, double division);
    void repartitionAuto(EltID id);
    void mixture(QList<QList<int> > listeParam, QString nomInst, bool bouclage, int freq, bool stereo);
    void release(double duree36, double division, double deTune);
    void release(EltID id, double duree36, double division, double deTune);
    void transposer(double ton, bool adaptKeyRange);
    void transposer(EltID idInstSmpl, double ton, bool adaptKeyRange);
    void on_pushRangeMode_clicked();

private:
    Ui::Page_Inst *ui;

    // Outils
    static double getOffset(int type1, int type2);
    static EltID closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl);
    static QByteArray getSampleData(EltID idSmpl, qint32 nbRead);
    static QByteArray addSampleData(QByteArray baData1, QByteArray baData2, double mult);
};

// Classe TableWidget pour instruments
class TableWidgetInst : public TableWidget
{
    Q_OBJECT
public:
    // Constructeur
    TableWidgetInst(QWidget *parent = 0);
    ~TableWidgetInst();

    // Association champ - ligne
    Champ getChamp(int row);
    int getRow(quint16 champ);
};

#endif // PAGE_INST_H
