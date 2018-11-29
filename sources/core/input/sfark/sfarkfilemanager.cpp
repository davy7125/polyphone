/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2014-2018 Davy Triponney                                **
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

#include "sfarkfilemanager.h"
#include <QFile>
#include <QDataStream>

int SfArkFileManager::_maxFileHandler = 0;

SfArkFileManager::SfArkFileManager()
{}

SfArkFileManager::~SfArkFileManager()
{
    clearData();
}

// Return file handler if success, otherwise -1
int SfArkFileManager::openReadOnly(const char * name)
{
    int handler = _maxFileHandler;

    if (_mapName.contains(name))
        handler = _mapName.value(name);
    else
    {
        QFile * file = new QFile(name);
        if (file->open(QIODevice::ReadOnly))
        {
            _mapName[name] = _maxFileHandler;
            _mapFile[_maxFileHandler] = file;
            _maxFileHandler++;
        }
        else
        {
            delete file;
            return -1;
        }
    }

    _mapDataStream[handler] = new QDataStream(_mapFile[handler]);
    return handler;
}

// Return file handler
int SfArkFileManager::create(const char *name)
{
    int handler = _maxFileHandler;
    if (_mapName.contains(name))
        handler = _mapName.value(name);
    else
    {
        QFile * file = new QFile(name);
        if (file->open(QIODevice::ReadWrite))
        {
            _mapName[name] = _maxFileHandler;
            _mapFile[_maxFileHandler] = file;
            _maxFileHandler++;
        }
        else
        {
            delete file;
            return -1;
        }
    }

    _mapDataStream[handler] = new QDataStream(_mapFile[handler]);
    return handler;
}

// Return true if success, otherwise false
void SfArkFileManager::close(int fileHandler)
{
    // Suppression du DataStream
    if (_mapDataStream.contains(fileHandler))
         delete _mapDataStream.take(fileHandler);

    // Fermeture si fichier ouvert
    if (_mapFile.contains(fileHandler))
    {
        QFile * file = _mapFile.take(fileHandler);
        file->close();

        QString key = _mapName.key(fileHandler, "");
        if (!key.isEmpty())
            _mapName.remove(key);
    }
}

// Return true if success, otherwise false
int SfArkFileManager::setPos(int fileHandler, long offset)
{
    if (_mapDataStream.contains(fileHandler))
    {
        QDataStream * stream = _mapDataStream.value(fileHandler);
        QIODevice * device = stream->device();
        return device->seek(offset + device->pos());
    }
    return false;
}

// Return the number of bytes read, -1 otherwise
int SfArkFileManager::read(int fileHandler, char * ptr, unsigned int count)
{
    if (_mapDataStream.contains(fileHandler))
    {
        QDataStream * stream = _mapDataStream.value(fileHandler);
        return stream->readRawData(ptr, count);
    }
    return -1;
}

// Return the number of bytes written, -1 otherwise
int SfArkFileManager::write(int fileHandler, const char *ptr, unsigned int count)
{
    if (_mapDataStream.contains(fileHandler))
    {
        QDataStream * stream = _mapDataStream.value(fileHandler);
        return stream->writeRawData(ptr, count);
    }
    return -1;
}

void SfArkFileManager::clearData()
{
    _maxFileHandler = 0;
    _mapName.clear();

    // Close the data streams
    QList<int> keys = _mapDataStream.keys();
    foreach (int key, keys)
        delete _mapDataStream.take(key);

    // Close the files
    keys = _mapFile.keys();
    foreach (int key, keys)
    {
        QFile * file = _mapFile.take(key);
        file->close();
    }
}
