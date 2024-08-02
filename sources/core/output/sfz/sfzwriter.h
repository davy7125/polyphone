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

#ifndef SFZWRITER_H
#define SFZWRITER_H

#include <QString>
#include <QList>
#include <QMap>
#include <QTextStream>

class SfzWriter
{
public:
    SfzWriter(QString filePath);
    ~SfzWriter();

    void newGroup();
    void newRegion();
    void addLine(QString left, QString right = "");
    void addLine(QString left, double value);
    void addLine(QString left, int value);

    bool write();

private:
    class Block
    {
    public:
        Block(QString blockName) : _blockName(blockName) {}
        void addElement(QString left, QString right);
        void removeElement(QString left);
        void write(QTextStream &out);
        QString getBlockName() { return _blockName; }
        QMap<QString, QString> getOpCodes()  { return _opCodes; }

    private:
        void writeTogether(QTextStream &out, QString opCode1, QString opCode2);
        QString _blockName;
        QStringList _lines;
        QMap<QString, QString> _opCodes;
        static const QString SUFFIX_V2;
    };

    void factorizeOpCodes(Block * group, QList<Block *> regions);
    void cleanOpCodes(Block *group, QList<Block *> regions);

    QString _filePath;
    QList<Block *> _blocks;
};

#endif // SFZWRITER_H
