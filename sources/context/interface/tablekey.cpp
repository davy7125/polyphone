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

#include "tablekey.h"
#include "editkey.h"
#include "contextmanager.h"
#include <QHeaderView>


TableKey::TableKey(QWidget * parent) : QTableWidget(parent),
    _signalMapper(new QSignalMapper(this))
{
    QFont font = this->font();
    font.setBold(true);
    this->horizontalHeader()->setFont(font);
    this->verticalHeader()->setFont(font);

    // Signal mapper
    connect(_signalMapper, SIGNAL(mapped(QString)), this, SLOT(rowChanged(QString)));

    // Populate the table
    this->populate();
}

void TableKey::populate()
{
    for (int j = 0; j < columnCount(); j++)
    {
        for (int i = 0; i < rowCount(); i++)
        {
            EditKey * editKey = new EditKey();
            editKey->setStyleSheet("EditKey{border: 0px; padding-left: 2px;}");
            editKey->setCombinaison(ContextManager::configuration()->getMapping(i, (ConfManager::Key)j));
            this->setCellWidget(i, j, editKey);

            _signalMapper->setMapping(editKey, QString::number(i) + ":" + QString::number(j));
            connect(editKey, SIGNAL(combinaisonChanged(QString)), _signalMapper, SLOT(map()));
        }
    }
}

void TableKey::rowChanged(QString id)
{
    int posSeparator = id.indexOf(":");
    int ligne = id.left(posSeparator).toInt();
    int colonne = id.right(id.length() - posSeparator - 1).toInt();

    QString sequence = ((EditKey*)this->cellWidget(ligne, colonne))->text();
    //KeyboardManager::setMapping((PianoKeybd::Key)colonne, ligne, QKeySequence(sequence));

    emit(combinaisonChanged(colonne, ligne, sequence));
}
