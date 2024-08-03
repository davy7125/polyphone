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
    for (int i = 0; i <= _blocks.count(); i++)
    {
        if (i == _blocks.count() || _blocks[i]->getBlockName() == "group")
        {
            if (currentGroup != nullptr && !currentRegions.empty())
            {
                // Factorize the regions inside the group
                factorizeOpCodes(currentGroup, currentRegions);

                // Delete parameters if they are needed in no regions
                cleanOpCodes(currentGroup, currentRegions);
            }

            // A new group is open
            if (i < _blocks.count())
            {
                currentGroup = _blocks[i];
                currentRegions.clear();
            }
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

    // All parameter defined in the first region (taking into account the default parameters)
    QMap<QString, QString> sameParameters = regions[0]->getOpCodes();
    QMap<QString, QString> defaultParameters = group->getOpCodes();
    foreach (QString key, defaultParameters.keys())
        if (!sameParameters.contains(key))
            sameParameters[key] = defaultParameters[key];

    // Then compare with all other regions (still taking into account the default parameters)
    for (int i = 1; i < regions.count(); i++)
    {
        QMap<QString, QString> otherParameters = regions[i]->getOpCodes();
        foreach (QString key, defaultParameters.keys())
            if (!otherParameters.contains(key))
                otherParameters[key] = defaultParameters[key];

        QList<QString> keys = sameParameters.keys();
        foreach (QString key, keys)
        {
            if (!otherParameters.contains(key) || otherParameters[key] != sameParameters[key])
                sameParameters.remove(key);
        }
    }

    // For all parameters that are the same accross all regions, define the value in the group only
    foreach (QString key, sameParameters.keys())
    {
        foreach (Block * block, regions)
            block->removeElement(key);

        // Detect default values (they don't need to be written)
        bool isDefault = false;
        if (key == "pan" || key == "tune" || key == "volume")
            isDefault = sameParameters[key] == "0";

        if (!isDefault)
            group->addElement(key, sameParameters[key]);
    }
}

void SfzWriter::cleanOpCodes(Block * group, QList<Block *> regions)
{
    cleanOpCodes(group, regions, "amplfo_freq", "amplfo_depth");
    cleanOpCodes(group, regions, "fillfo_freq", "fillfo_depth");
    cleanOpCodes(group, regions, "pitchlfo_freq", "pitchlfo_depth");
}

void SfzWriter::cleanOpCodes(Block * group, QList<Block *> regions, QString opCodeInGroup, QString opCodeCondition)
{
    if (group->getOpCodes().contains(opCodeInGroup) && !group->getOpCodes().contains(opCodeCondition))
    {
        bool withDepth = false;
        foreach (Block * block, regions)
        {
            if (block->getOpCodes().contains(opCodeCondition))
            {
                withDepth = true;
                break;
            }
        }
        if (!withDepth)
            group->removeElement(opCodeInGroup);
    }
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
