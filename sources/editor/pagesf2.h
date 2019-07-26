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

#ifndef PAGE_SF2_H
#define PAGE_SF2_H

#include <QWidget>
#include <QFocusEvent>
#include <QKeyEvent>
#include "page.h"

namespace Ui {
class PageSf2;
}

// Création d'un signal editingFinished() pour qTextEdit
class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

private:
    void focusOutEvent(QFocusEvent *e)
    {
        if (e->lostFocus())
            emit(editingFinished());
        QTextEdit::focusOutEvent(e);
    }

    void keyPressEvent(QKeyEvent *e)
    {
        int key = e->key();
        if (this->toPlainText().size() < 65536 || key == Qt::Key_Backspace || key == Qt::Key_Delete ||
                key == Qt::Key_Left || key == Qt::Key_Up || key == Qt::Key_Right ||
                key == Qt::Key_Down || key == Qt::Key_PageUp || key == Qt::Key_PageDown ||
                key == Qt::Key_Home || key == Qt::Key_End)
            QTextEdit::keyPressEvent(e);
    }

signals:
    void editingFinished();
};


class PageSf2 : public Page
{
    Q_OBJECT

public:
    explicit PageSf2(QWidget *parent = nullptr);
    ~PageSf2() override;

private slots:
    void setName();
    void setCopyright();
    void setAuthor();
    void setDate();
    void setProduct();
    void setCommentaire();
    void dateFocussed(bool hasFocus);
    void on_pushButton_setNow_pressed();
    void on_pushViewSamples_clicked();
    void on_pushViewInstruments_clicked();
    void on_pushViewPresets_clicked();
    void on_comboBox_currentIndexChanged(int index);

protected:
    // Update the interface
    bool updateInterface(QString editingSource, IdList selectedIds, int displayOption) override;

    // Refresh things after a page is shown
    void onShow() override;

private:
    EltID _currentID;
    Ui::PageSf2 *ui;

    void countElements();
    void countElements(int &unusedSmpl, int &unusedInst, int &usedSmpl, int &usedInst, int &usedPrst,
                       int &instGen, int &prstGen, int &instMod, int &prstMod);
};

#endif // PAGE_SF2_H
