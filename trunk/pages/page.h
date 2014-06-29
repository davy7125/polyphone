/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#ifndef PAGE_H
#define PAGE_H

#include "pile_sf2.h"
#include "synth.h"
#include "tablewidget.h"
#include "tablewidgetmod.h"
#include <QStackedWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QApplication>
#include <QPushButton>

class Config;

namespace Ui
{
    class Page;
    class PageTable;
    class TableComboBox;
    class ComboBox;
}

class Page : public QWidget
{
    Q_OBJECT

public:
    enum TypePage
    {
        PAGE_SF2,
        PAGE_SMPL,
        PAGE_INST,
        PAGE_PRST
    };
    Page(TypePage typePage, QWidget *parent = 0);

    virtual void afficher() = 0;
    static QString getGenName(WORD iVal, int type = 0);

protected:
    bool preparation;
    static MainWindow *mainWindow;
    static Tree *tree;
    static QStackedWidget *qStackedWidget;
    static Pile_sf2 *sf2;
    static Synth * synth;
    TypePage m_typePage;

    char * getTextValue(char * T, WORD champ, genAmountType genVal);
    static char * getTextValue(char * T, WORD champ, int iVal);
    static char * getTextValue(char * T, WORD champ, SFModulator sfModVal);
    static QString getIndexName(WORD iVal, int CC);
    genAmountType getValue(QString texte, WORD champ, bool &ok);

private:
    // Méthodes privées
    int limit(int iTmp, int minInst, int maxInst, int minPrst = 0, int maxPrst = 0);
};

#endif // PAGE_H
