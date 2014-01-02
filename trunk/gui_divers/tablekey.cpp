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

#include "tablekey.h"
#include "editkey.h"
#include "keymapper.h"
#include <QHeaderView>
#include <QLabel>

TableKey::TableKey(QWidget * parent) : QTableWidget(parent),
    _mapper(NULL),
    _signalMapper(new QSignalMapper(this))
{
    // Apparence et comportement
    this->setColumnCount(2);
    this->verticalHeader()->setVisible(false);
    this->verticalHeader()->setDefaultSectionSize(20);
    QStringList labels;
    labels << trUtf8("Note") << trUtf8("Clavier");
    this->setHorizontalHeaderLabels(labels);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionMode(QAbstractItemView::NoSelection);

    QFont font = this->font();
    font.setBold(true);
    this->horizontalHeader()->setFont(font);

    // Redimensionnement initial
    this->resizeEvent(NULL);

    // Signal mapper
    connect(_signalMapper, SIGNAL(mapped(int)), this, SLOT(rowChanged(int)));
}

void TableKey::setMapper(KeyMapper * mapper)
{
    _mapper = mapper;
    this->setRowCount(0);
    this->setRowCount(mapper->getKeyCount() + 3);
    _rootKey = mapper->getRootKey();
    for (int i = 0; i < this->rowCount(); i++)
    {
        QLabel * label = new QLabel();
        label->setStyleSheet("QLabel{border: 0px; padding-left: 5px;}");
        this->setCellWidget(i, 0, label);

        EditKey * editKey = new EditKey();
        editKey->setStyleSheet("EditKey{border: 0px; padding-left: 2px;}");
        editKey->setCombinaison(mapper->getSequence(_rootKey + i - i / 13, (i % 13) == 0 && i != 0));
        this->setCellWidget(i, 1, editKey);

        _signalMapper->setMapping(editKey, i);
        connect(editKey, SIGNAL(combinaisonChanged(QString)), _signalMapper, SLOT(map()));
    }
}

void TableKey::resizeEvent(QResizeEvent *)
{
    int width = this->viewport()->width() / 2;
    if (this->columnCount() == 2)
    {
        this->setColumnWidth(0, width);
        this->setColumnWidth(1, this->viewport()->width() - width);
    }
}

void TableKey::setOctave(int octave)
{
    for (int i = 0; i < this->rowCount(); i++)
    {
        QLabel * label = (QLabel *)this->cellWidget(i, 0);
        label->setText(this->getName(i + _rootKey + 12 * octave - i / 13));
    }
}

QString TableKey::getName(int note)
{
    if (note > 127)
        note = 127;

    QString strRet;
    strRet.sprintf("%.3d - ", note);
    switch (note % 12)
    {
    case 0:  strRet += trUtf8("do");   break;
    case 1:  strRet += trUtf8("do#");  break;
    case 2:  strRet += trUtf8("ré");   break;
    case 3:  strRet += trUtf8("ré#");  break;
    case 4:  strRet += trUtf8("mi");   break;
    case 5:  strRet += trUtf8("fa");   break;
    case 6:  strRet += trUtf8("fa#");  break;
    case 7:  strRet += trUtf8("sol");  break;
    case 8:  strRet += trUtf8("sol#"); break;
    case 9:  strRet += trUtf8("la");   break;
    case 10: strRet += trUtf8("la#");  break;
    case 11: strRet += trUtf8("si");   break;
    }

    return strRet;
}

void TableKey::rowChanged(int row)
{
    QString sequence = ((EditKey*)this->cellWidget(row, 1))->text();

    int num = row - row / 13 + _rootKey;
    if (row % 13 == 0 && row != 0)
    {
        _mapper->setCombinaisonKey(num, 1, sequence);
        num = 10 * num + 2;
    }
    else
        _mapper->setCombinaisonKey(num, 0, sequence);

    emit(combinaisonChanged(num, sequence));
}
