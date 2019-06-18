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

#ifndef DETAILSMANAGER_H
#define DETAILSMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
class SoundfontDetails;
class UrlReaderJson;

class DetailsManager: public QObject
{
    Q_OBJECT

public:
    // Singleton: get the instance of the object and finally kill it
    static DetailsManager * getInstance();
    static void kill();

    // Ask for details. The signal "detailsReady" will be then emitted
    void askForSoundfontDetails(int id);

    // Get the details of a soundfont
    SoundfontDetails * getDetails(int id);

signals:
    // Emitted when soundfont details are ready
    void detailsReady(int id);

    // Emitted when it was not possible to get the details
    void detailsFailed(int id, QString error);

private slots:
    void downloadCompleted(QString error);

private:
    DetailsManager();
    ~DetailsManager();

    void run();

    static DetailsManager * s_instance;
    QMap<int, SoundfontDetails *> _soundfontDetails; // Classified by ID
    UrlReaderJson * _urlReaderDetails;
    int _currentReaderDetailId;
    QList<int> _stack;
    QMutex _mutex;
};

#endif // DETAILSMANAGER_H
