/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#ifndef PAGE_SF2_H
#define PAGE_SF2_H

#include <QWidget>
#include "page.h"

namespace Ui {
class Page_Sf2;
}

// Création d'un signal editingFinished() pour qTextEdit
class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent=0) : QTextEdit(parent) {}
private:
    void focusOutEvent(QFocusEvent *e) {if (e->lostFocus()) emit(editingFinished());}
signals:
    void editingFinished();
};

class Page_Sf2 : public Page
{
    Q_OBJECT
public:
    explicit Page_Sf2(MainWindow *mainWindow, Tree *tree, QStackedWidget *qStackedWidget, \
                      Pile_sf2 * sf2, Synth * synth, QWidget *parent = 0);
    ~Page_Sf2();
    // Méthodes publiques
    void afficher();
    void compte(int &unusedSmpl, int &unusedInst, int &usedSmpl, int &usedInst, int &usedPrst, int &instGen, int &prstGen);

public slots:
        void set24bits(int checked);
        void setName();
        void setCopyright();
        void setAuthor();
        void setDate();
        void setProduct();
        void setCommentaire();
        void setNow();
private:
    Ui::Page_Sf2 *ui;
    // Méthodes privées
    void compte();
};

#endif // PAGE_SF2_H
