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

#include "levelmeter.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include "contextmanager.h"

// Scale of the bars and limits of their colors (dB)
static const float MIN_LEVEL = -60;
static const float WARNING_LEVEL = -12;
static const float DANGER_LEVEL = -3;
static const float GRADUATION_STEP = 10;

// Fall of the bars (dB per second) and time a peak stays (seconds)
static const float FALL_SPEED = 25;
static const float HOLD_TIME = 1.5f;

// Dimensions (pixels)
static const int BAR_HEIGHT = 7;
static const int BAR_SPACING = 2;
static const int TEXT_SPACING = 6;

static float toDecibels(float peak)
{
    return peak > 0 ? 20 * std::log10(peak) : MIN_LEVEL;
}

LevelMeter::LevelMeter(QWidget * parent) : QWidget(parent),
    _channelCount(2),
    _isShowingVelocities(false),
    _highest(0)
{
    showAudioLevels(2);
    _clock.start();
}

void LevelMeter::showAudioLevels(int channelCount)
{
    _channelCount = channelCount;
    _isShowingVelocities = false;
    this->setToolTip(tr("Highest level reached (a click resets it)"));
    for (int i = 0; i < 2; i++)
        _channels[i] = {MIN_LEVEL, MIN_LEVEL, 0};
    this->reset();
}

void LevelMeter::showVelocities()
{
    _channelCount = 1;
    _isShowingVelocities = true;
    this->setToolTip(tr("Highest velocity reached (a click resets it)"));
    for (int i = 0; i < 2; i++)
        _channels[i] = {MIN_LEVEL, MIN_LEVEL, 0};
    this->reset();
}

void LevelMeter::setPeaks(float left, float right)
{
    updateChannels(qBound(MIN_LEVEL, toDecibels(left), 0.0f), qBound(MIN_LEVEL, toDecibels(right), 0.0f), qMax(left, right));
}

void LevelMeter::setVelocity(int velocity)
{
    // The bars being graduated in decibels, the velocities take the whole scale
    float level = MIN_LEVEL * (1.0f - velocity / 127.0f);
    updateChannels(level, level, velocity);
}

void LevelMeter::updateChannels(float left, float right, float highest)
{
    float seconds = _clock.restart() / 1000.0f;
    float previousLevels[4] = {_channels[0].level, _channels[0].heldLevel, _channels[1].level, _channels[1].heldLevel};
    float previousHighest = _highest;

    updateChannel(_channels[0], left, seconds);
    updateChannel(_channels[1], right, seconds);
    _highest = qMax(_highest, highest);

    // Nothing to paint during the silences
    if (previousLevels[0] != _channels[0].level || previousLevels[1] != _channels[0].heldLevel ||
        previousLevels[2] != _channels[1].level || previousLevels[3] != _channels[1].heldLevel ||
        previousHighest != _highest)
        this->update();
}

void LevelMeter::updateChannel(Channel &channel, float level, float seconds)
{
    // The bar rises immediately and falls slowly
    channel.level = qMax(level, channel.level - FALL_SPEED * seconds);

    // The last peak stays, then falls
    if (level >= channel.heldLevel)
    {
        channel.heldLevel = level;
        channel.holdTime = HOLD_TIME;
    }
    else if (channel.holdTime > 0)
        channel.holdTime -= seconds;
    else
        channel.heldLevel = qMax(channel.level, channel.heldLevel - FALL_SPEED * seconds);
}

void LevelMeter::reset()
{
    _highest = 0;
    this->update();
}

QSize LevelMeter::sizeHint() const
{
    return QSize(150, 2 * BAR_HEIGHT + BAR_SPACING);
}

QString LevelMeter::getHighestLevelText()
{
    if (_isShowingVelocities)
        return _highest > 0 ? QString::number(qRound(_highest)) : "-";

    QString unit = tr("dB", "unit for decibels");
    if (_highest <= 0)
        return QString::fromUtf8("-\xE2\x88\x9E ") + unit;
    return QLocale::system().toString(toDecibels(_highest), 'f', 1) + " " + unit;
}

void LevelMeter::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QFont font = this->font();
    font.setPointSizeF(font.pointSizeF() * 0.85);
    QFontMetrics metrics(font);

    // Highest level, on the right side
    ThemeManager * theme = ContextManager::theme();
    int textWidth = metrics.horizontalAdvance(QLocale::system().toString(-88.8, 'f', 1) + " " + tr("dB", "unit for decibels")) + 8;
    QRect textRect(this->width() - textWidth, 0, textWidth, this->height());
    QPainter painter(this);
    painter.setFont(font);
    if (!_isShowingVelocities && _highest >= 1)
    {
        painter.fillRect(textRect, theme->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND));
        painter.setPen(theme->getColor(ThemeManager::LIST_BACKGROUND));
    }
    else
        painter.setPen(theme->getColor(ThemeManager::LIST_TEXT));
    painter.drawText(textRect, Qt::AlignCenter, getHighestLevelText());

    // A bar for each channel on the left side, a single one taking the whole height
    int barWidth = this->width() - textWidth - TEXT_SPACING;
    int top = (this->height() - 2 * BAR_HEIGHT - BAR_SPACING) / 2;
    if (_channelCount == 1)
        paintChannel(painter, QRect(0, top, barWidth, 2 * BAR_HEIGHT + BAR_SPACING), _channels[0]);
    else
    {
        paintChannel(painter, QRect(0, top, barWidth, BAR_HEIGHT), _channels[0]);
        paintChannel(painter, QRect(0, top + BAR_HEIGHT + BAR_SPACING, barWidth, BAR_HEIGHT), _channels[1]);
    }
}

void LevelMeter::paintChannel(QPainter &painter, const QRect &rect, const Channel &channel)
{
    ThemeManager * theme = ContextManager::theme();
    QColor background = theme->getColor(ThemeManager::LIST_BACKGROUND);

    // Position of a level, the scale being linear in decibels
    auto toPosition = [&rect](float level) {
        return rect.left() + qRound(rect.width() * (level - MIN_LEVEL) / -MIN_LEVEL);
    };

    // Empty bar
    painter.fillRect(rect, ThemeManager::mix(background, theme->getColor(ThemeManager::LIST_TEXT), 0.15));

    // Colors of the level, a velocity being never too high
    struct Zone
    {
        float start;
        float end;
        ThemeManager::FixedColorType color;
    };
    const Zone zones[3] = {
        {MIN_LEVEL, _isShowingVelocities ? 0 : WARNING_LEVEL, ThemeManager::GREEN},
        {WARNING_LEVEL, DANGER_LEVEL, ThemeManager::YELLOW},
        {DANGER_LEVEL, 0, ThemeManager::RED}
    };
    for (const Zone &zone : zones)
    {
        if (_isShowingVelocities && zone.color != ThemeManager::GREEN)
            break;
        if (channel.level <= zone.start)
            break;
        int start = toPosition(zone.start);
        int end = toPosition(qMin(channel.level, zone.end));
        painter.fillRect(start, rect.top(), end - start, rect.height(),
                         theme->getFixedColor(zone.color, ThemeManager::LIST_BACKGROUND));
    }

    // Last peak
    if (channel.heldLevel > MIN_LEVEL)
    {
        ThemeManager::FixedColorType color = ThemeManager::GREEN;
        if (!_isShowingVelocities && channel.heldLevel > DANGER_LEVEL)
            color = ThemeManager::RED;
        else if (!_isShowingVelocities && channel.heldLevel > WARNING_LEVEL)
            color = ThemeManager::YELLOW;
        painter.fillRect(qMin(toPosition(channel.heldLevel), rect.right() - 1), rect.top(), 2, rect.height(),
                         theme->getFixedColor(color, ThemeManager::LIST_BACKGROUND));
    }

    // Graduations
    for (float level = MIN_LEVEL + GRADUATION_STEP; level < 0; level += GRADUATION_STEP)
        painter.fillRect(toPosition(level), rect.top(), 1, rect.height(), background);
}

void LevelMeter::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
        this->reset();
    QWidget::mousePressEvent(event);
}
