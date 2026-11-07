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

#ifndef LEVELMETER_H
#define LEVELMETER_H

#include <QWidget>
#include <QElapsedTimer>

// Level meter: a bar per channel with the last peak held, and the highest level reached. This value stays on a red
// background once the maximum level has been reached, until the next reset or a click on the widget.
// It can also show the velocity of the MIDI notes played, with a single bar. Drawn on the background of the lists.
class LevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMeter(QWidget * parent = nullptr);

    // Type of values displayed, audio levels being displayed by default with 2 channels
    void showAudioLevels(int channelCount);
    void showVelocities();

    // Peaks since the last call (linear values, the maximum being 1), to give regularly
    void setPeaks(float left, float right);

    // Highest velocity since the last call (0 if no notes have been played), to give regularly
    void setVelocity(int velocity);

    // Forget the highest level
    void reset();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override { return sizeHint(); }

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;

private:
    struct Channel
    {
        float level; // dB
        float heldLevel; // dB
        float holdTime; // Seconds still to wait before the held level falls
    };

    void updateChannels(float left, float right, float highest);
    void updateChannel(Channel &channel, float level, float seconds);
    void paintChannel(QPainter &painter, const QRect &rect, const Channel &channel);
    QString getHighestLevelText();

    Channel _channels[2];
    int _channelCount;
    bool _isShowingVelocities;
    float _highest; // Linear peak or velocity
    QElapsedTimer _clock;
};

#endif // LEVELMETER_H
