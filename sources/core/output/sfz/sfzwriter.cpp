/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "sfzwriter.h"
#include <QFile>

const QString SfzWriter::SUFFIX_V2 = " // sfz v2";

SfzWriter::SfzWriter(QString filePath) :
    _filePath(filePath)
{
    _blocks << new Block("");
}

SfzWriter::~SfzWriter()
{
    while (!_blocks.isEmpty())
        delete _blocks.takeFirst();
}

void SfzWriter::newGroup()
{
    _blocks << new Block("group");
}

void SfzWriter::newRegion()
{
    _blocks << new Block("region");
}

void SfzWriter::addLine(QString left, QString right, bool v2suffix)
{
    _blocks.last()->addElement(left, right);
}

void SfzWriter::addLine(QString left, double value, bool v2suffix)
{
    _blocks.last()->addElement(left, QString::number(value));
}

void SfzWriter::addLine(QString left, int value, bool v2suffix)
{
    _blocks.last()->addElement(left, QString::number(value));
}

bool SfzWriter::write()
{
    QFile sfzFile(_filePath);
    if (!sfzFile.open(QIODevice::WriteOnly))
        return false;

    QTextStream out(&sfzFile);
    foreach (Block * block, _blocks)
        block->write(out);

    return true;
}

void SfzWriter::Block::addElement(QString left, QString right)
{
    if (right.isEmpty())
        _lines.append(left);
    else
        _opCodes[left] = right;
}

void SfzWriter::Block::write(QTextStream &out)
{
    if (!_blockName.isEmpty())
        out << Qt::endl << "[" << _blockName << "]" << Qt::endl;

    foreach (QString line, _lines)
        out << line << Qt::endl;
    foreach (QString key, _opCodes.keys())
        out << key << "=" << _opCodes[key] << Qt::endl;
}
