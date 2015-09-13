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

#ifndef PAGE_H
#define PAGE_H

#include "pile_sf2.h"
#include "synth.h"
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
    static QString getGenName(quint16 iVal, int type = 0);

protected:
    bool _preparation;
    static MainWindow *_mainWindow;
    static Tree *_tree;
    static QStackedWidget *_qStackedWidget;
    static Pile_sf2 *_sf2;
    static Synth * _synth;
    TypePage _typePage;

    char * getTextValue(char * T, quint16 champ, genAmountType genVal);
    static char * getTextValue(char * T, quint16 champ, int iVal);
    static char * getTextValue(char * T, quint16 champ, SFModulator sfModVal);
    static QString getIndexName(quint16 iVal, int CC);
    genAmountType getValue(QString texte, quint16 champ, bool &ok);

protected slots:
    void updateMainwindow();
    void playKey(int key, int velocity);
    void selectInTree(QList<EltID> ids);

private:
    // Méthodes privées
    int limit(int iTmp, int minInst, int maxInst, int minPrst = 0, int maxPrst = 0);
};

#endif // PAGE_H
