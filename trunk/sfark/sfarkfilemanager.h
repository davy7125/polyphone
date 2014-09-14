/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2014 Davy Triponney                                     **
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

#ifndef SFARKFILEMANAGER_H
#define SFARKFILEMANAGER_H

#include <QMap>
class QFile;

class SfArkFileManager
{
public:
    SfArkFileManager();
    ~SfArkFileManager();

    // Return file handler if success, otherwise -1
    int openReadOnly(const char *name);

    // Return file handler
    int create(const char * name);

    // Return true if success, otherwise false
    void close(int fileHandler);

    // Return true if success, otherwise false
    int setPos(int fileHandler, long offset);

    // Return the number of bytes read, -1 otherwise
    int read(int fileHandler, char *ptr, unsigned int count);

    // Return the number of bytes written, -1 otherwise
    int write(int fileHandler, const char * ptr, unsigned int count);

    char * retrieveData(const char * name, int &size);

private:
    QMap<QString, int> _mapName;
    QMap<int, QDataStream *> _mapDataStream;
    QMap<int, QByteArray *> _mapByteArray;
    QMap<int, QFile *> _mapFile;

    static int _maxFileHandler;
};

#endif // SFARKFILEMANAGER_H
