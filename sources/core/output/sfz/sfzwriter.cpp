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

const QString SfzWriter::Block::SUFFIX_V2 = " // sfz v2";

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

void SfzWriter::addLine(QString left, QString right)
{
    _blocks.last()->addElement(left, right);
}

void SfzWriter::addLine(QString left, double value)
{
    addLine(left, QString::number(value));
}

void SfzWriter::addLine(QString left, int value)
{
    addLine(left, QString::number(value));
}

bool SfzWriter::write()
{
    QFile sfzFile(_filePath);
    if (!sfzFile.open(QIODevice::WriteOnly))
        return false;

    // Process opCodes
    Block * currentGroup = nullptr;
    QList<Block *> currentRegions;
    for (int i = 0; i < _blocks.count(); i++)
    {
        if (_blocks[i]->getBlockName() == "group")
        {
            if (currentGroup != nullptr && !currentRegions.empty())
            {
                // Factorize the regions inside the group
                factorizeOpCodes(currentGroup, currentRegions);

                // Delete parameters if they are needed in no regions
                cleanOpCodes(currentGroup, currentRegions);
            }

            // A new group is open
            currentGroup = _blocks[i];
            currentRegions.clear();
        }
        else
            currentRegions << _blocks[i];
    }

    QTextStream out(&sfzFile);
    foreach (Block * block, _blocks)
        block->write(out);

    return true;
}

void SfzWriter::factorizeOpCodes(Block * group, QList<Block *> regions)
{
    if (regions.count() < 2)
        return;

    QMap<QString, QString> base = regions[0]->getOpCodes();
    for (int i = 1; i < regions.count(); i++)
    {
        QMap<QString, QString> other = regions[i]->getOpCodes();
        QList<QString> baseKeys = base.keys();
        foreach (QString key, baseKeys)
        {
            if (!other.contains(key) || other[key] != base[key])
                base.remove(key);
        }
    }

    foreach (QString key, base.keys())
    {
        group->addElement(key, base[key]);
        foreach (Block * block, regions)
            block->removeElement(key);
    }
}

void SfzWriter::cleanOpCodes(Block * group, QList<Block *> regions)
{

}

void SfzWriter::Block::addElement(QString left, QString right)
{
    if (right.isEmpty())
        _lines.append(left);
    else
        _opCodes[left] = right;
}

void SfzWriter::Block::removeElement(QString left)
{
    _opCodes.remove(left);
}

void SfzWriter::Block::write(QTextStream &out)
{
    if (!_blockName.isEmpty())
    {
        out << Qt::endl << "<" << _blockName << ">";
        if (_opCodes.contains("sample"))
        {
            out << " sample=" << _opCodes["sample"];
            _opCodes.remove("sample");
        }
        out << Qt::endl;
    }

    // Some opcodes can be written together
    writeTogether(out, "lochan", "hichan");
    writeTogether(out, "lokey", "hikey");
    writeTogether(out, "lovel", "hivel");
    foreach (QString line, _lines)
        out << line << Qt::endl;
    foreach (QString key, _opCodes.keys())
    {
        out << key << "=" << _opCodes[key];
        if (key.endsWith("cc133"))
            out << SUFFIX_V2;
        out << Qt::endl;
    }
}

void SfzWriter::Block::writeTogether(QTextStream &out, QString opCode1, QString opCode2)
{
    if (!_opCodes.contains(opCode1) || !_opCodes.contains(opCode2))
        return;

    out << opCode1 << "=" << _opCodes[opCode1] << " "
        << opCode2 << "=" << _opCodes[opCode2] << Qt::endl;
    _opCodes.remove(opCode1);
    _opCodes.remove(opCode2);
}
