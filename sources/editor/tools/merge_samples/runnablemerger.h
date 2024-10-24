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

#ifndef RUNNABLEMERGER_H
#define RUNNABLEMERGER_H

#include <QRunnable>
#include "basetypes.h"
class ToolMergeSamples;
class Soundfont;
class InstPrst;
class Division;
class Smpl;

class RunnableMerger: public QRunnable
{
public:
    RunnableMerger(ToolMergeSamples * tool, EltID idPrst, quint32 key, quint32 minKey, bool loop, bool stereo, quint32 sustainDuration, quint32 releaseDuration);
    ~RunnableMerger() override;
    void run() override;

private:
    void playPrst(float * dataR, float * dataL, Soundfont * soundfont, InstPrst * prst);
    void playInst(float * dataR, float * dataL, Soundfont * soundfont, InstPrst * prst, Division * prstDiv, InstPrst * inst);
    void playSmpl(float * dataR, float * dataL, InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv, Smpl * smpl);
    static quint32 SAMPLE_RATE; // In samples per second
    static quint32 BUFFER_LENGTH;

    ToolMergeSamples * _tool;
    EltID _idPrst;
    quint32 _key, _minKey;
    bool _loop;
    bool _stereo;
    quint32 _sustainDuration;
    quint32 _releaseDuration;
};

#endif // RUNNABLEMERGER_H
