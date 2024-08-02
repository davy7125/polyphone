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
    void addLine(QString left, QString right = "", bool v2suffix = false);
    void addLine(QString left, double value, bool v2suffix = false);
    void addLine(QString left, int value, bool v2suffix = false);

    bool write();

private:
    class Block
    {
    public:
        Block(QString blockName) : _blockName(blockName) {}
        void addElement(QString left, QString right);
        void write(QTextStream &out);

    private:
        QString _blockName;
        QStringList _lines;
        QMap<QString, QString> _opCodes;
    };

    QString _filePath;
    QList<Block *> _blocks;
    static const QString SUFFIX_V2;
};

#endif // SFZWRITER_H
