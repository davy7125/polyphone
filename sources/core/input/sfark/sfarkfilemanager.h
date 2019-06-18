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

#ifndef SFARKFILEMANAGER_H
#define SFARKFILEMANAGER_H

#include <QString>
#include <QMap>
class QFile;

class SfArkFileManager
{
public:
    SfArkFileManager();
    ~SfArkFileManager();

    // Open a file simulated by the class, or a real file if not present yet
    // Return a file handler if success, otherwise -1
    int openReadOnly(const char *name);

    // Simulate the creation of a file
    // Return a file handler
    int create(const char * name);

    // Return true if success, otherwise false
    void close(int fileHandler);

    // Return true if success, otherwise false
    int setPos(int fileHandler, long offset);

    // Return the number of bytes read, -1 otherwise
    int read(int fileHandler, char *ptr, unsigned int count);

    // Return the number of bytes written, -1 otherwise
    int write(int fileHandler, const char * ptr, unsigned int count);

    // Clear data (not the files)
    void clearData();

private:
    QMap<QString, int> _mapName;
    QMap<int, QDataStream *> _mapDataStream;
    QMap<int, QFile *> _mapFile;

    static int _maxFileHandler;
};

#endif // SFARKFILEMANAGER_H
