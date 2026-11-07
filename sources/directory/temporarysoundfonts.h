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

#ifndef TEMPORARYSOUNDFONTS_H
#define TEMPORARYSOUNDFONTS_H

#include <QObject>
#include <QMap>
#include <QElapsedTimer>
class QTimer;

// Soundfonts loaded without tab by the soundfont explorer, held by its preview (virtual keyboard) and by the MIDI player.
// A temporary soundfont that is not held anymore is deleted once no voice reads its data, the voices still sounding after
// a while being released. It is kept if a tab has been opened with it meanwhile (it is not temporary anymore).
class TemporarySoundfonts : public QObject
{
    Q_OBJECT

public:
    static void hold(int sf2Index);
    static void release(int sf2Index);

private slots:
    void check();

private:
    TemporarySoundfonts();
    static TemporarySoundfonts * getInstance();

    QMap<int, int> _holdCounts;
    QMap<int, qint64> _releaseTimes; // Soundfonts to delete, with the time of their release (ms)
    QElapsedTimer _clock;
    QTimer * _timer;

    static TemporarySoundfonts * s_instance;
    static const int CHECK_INTERVAL_MS;
    static const int VOICE_RELEASE_DELAY_MS;
};

#endif // TEMPORARYSOUNDFONTS_H
