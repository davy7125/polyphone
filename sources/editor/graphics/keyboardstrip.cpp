/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "keyboardstrip.h"
#include "contextmanager.h"
#include <QGraphicsView>
#include <QScrollBar>
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QApplication>
#include <cmath>

const int KeyboardStrip::TOP_MARGIN = 2;

KeyboardStrip::KeyboardStrip(QWidget * parent) : QWidget(parent),
    _view(nullptr),
    _playingKey(-1)
{
    ThemeManager * theme = ContextManager::theme();
    QColor background = theme->getColor(ThemeManager::LIST_BACKGROUND);
    QColor text = theme->getColor(ThemeManager::LIST_TEXT);
    _isDark = theme->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT);
    _border = ThemeManager::mix(background, text, 0.25);
    _highlight = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    this->setFixedHeight(TOP_MARGIN + 20);
}

bool KeyboardStrip::isBlackKey(int key)
{
    int note = key % 12;
    return note == 1 || note == 3 || note == 6 || note == 8 || note == 10;
}

QColor KeyboardStrip::keyColor(int key, bool isDark)
{
    if (isBlackKey(key))
        return isDark ? QColor(30, 30, 30) : QColor(40, 40, 40);
    return isDark ? QColor(210, 210, 210) : QColor(250, 250, 250);
}

void KeyboardStrip::paintKeys(QPainter &painter, const QList<Key> &keys, double top, double height, const QColor &border)
{
    if (keys.isEmpty())
        return;
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Fills on integer abscissas
    int y = qRound(top);
    int h = qRound(top + height) - y;
    QVector<int> lefts(keys.count()), rights(keys.count());
    for (int i = 0; i < keys.count(); i++)
    {
        lefts[i] = qRound(keys[i].left);
        rights[i] = qRound(keys[i].right);
        painter.fillRect(QRect(lefts[i], y, rights[i] - lefts[i], h), keys[i].color);
    }

    // Then the separators between adjacent keys and the border
    painter.setPen(border);
    painter.setBrush(Qt::NoBrush);
    for (int i = 1; i < keys.count(); i++)
        if (lefts[i] == rights[i - 1])
            painter.drawLine(lefts[i], y, lefts[i], y + h);
    painter.drawRect(QRect(lefts.first(), y, rights.last() - lefts.first(), h));

    painter.restore();
}

void KeyboardStrip::setView(QGraphicsView * view)
{
    _view = view;
    update();
}

void KeyboardStrip::setPlayedKey(int key, bool isOn)
{
    if (isOn)
    {
        if (!_playedKeys.contains(key))
            _playedKeys << key;
    }
    else
        _playedKeys.removeAll(key);
    update();
}

bool KeyboardStrip::getScale(double &left, double &keyWidth, int &visibleLeft, int &visibleRight) const
{
    if (_view == nullptr || !_view->isVisible())
        return false;

    // Abscissas of the keys in the viewport of the view
    QTransform transform = _view->viewportTransform();
    double x0 = transform.map(QPointF(-0.5, 0)).x();
    double x1 = transform.map(QPointF(127.5, 0)).x();
    if (x1 - x0 < 1)
        return false;

    // Then in this widget, limited to the viewport (not under the vertical scroll bar)
    QWidget * viewport = _view->viewport();
    int offset = this->mapFromGlobal(viewport->mapToGlobal(QPoint(0, 0))).x();
    left = offset + x0;
    keyWidth = (x1 - x0) / 128;
    visibleLeft = qMax(0, offset);
    visibleRight = qMin(this->width(), offset + viewport->width());
    return visibleRight > visibleLeft;
}

int KeyboardStrip::keyAt(QPoint position) const
{
    double left, keyWidth;
    int visibleLeft, visibleRight;
    if (!getScale(left, keyWidth, visibleLeft, visibleRight) ||
        position.x() < visibleLeft || position.x() >= visibleRight)
        return -1;

    // Key whose fill contains the pixel (centered on x + 0.5)
    int key = static_cast<int>(std::floor((position.x() + 0.5 - left) / keyWidth));
    return (key >= 0 && key <= 127) ? key : -1;
}

int KeyboardStrip::velocityAt(QPoint position) const
{
    double ratio = static_cast<double>(position.y() - TOP_MARGIN + 1) / (this->height() - TOP_MARGIN);
    return qBound(1, qRound(127 * ratio), 127);
}

void KeyboardStrip::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    double left, keyWidth;
    int visibleLeft, visibleRight;
    if (!getScale(left, keyWidth, visibleLeft, visibleRight))
        return;

    // Keys in the visible part of the view
    int firstKey = qMax(0, static_cast<int>(std::floor((visibleLeft - left) / keyWidth)));
    int lastKey = qMin(127, static_cast<int>(std::floor((visibleRight - left) / keyWidth)));
    QList<Key> keys;
    for (int key = firstKey; key <= lastKey; key++)
        keys << Key { key, left + key * keyWidth, left + (key + 1) * keyWidth,
                     _playedKeys.contains(key) ? _highlight : keyColor(key, _isDark) };

    QPainter painter(this);
    painter.setClipRect(QRect(visibleLeft, 0, visibleRight - visibleLeft, this->height()));
    paintKeys(painter, keys, TOP_MARGIN, this->height() - TOP_MARGIN - 1, _border);
}

void KeyboardStrip::mousePressEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton || _playingKey >= 0)
        return;
    int key = keyAt(event->pos());
    if (key >= 0)
    {
        _playingKey = key;
        ContextManager::midi()->processKeyOn(-1, key, velocityAt(event->pos()));
    }
}

void KeyboardStrip::mouseMoveEvent(QMouseEvent * event)
{
    if (_playingKey < 0)
        return;
    int key = keyAt(event->pos());
    if (key >= 0 && key != _playingKey)
    {
        // Glissando
        ContextManager::midi()->processKeyOff(-1, _playingKey);
        _playingKey = key;
        ContextManager::midi()->processKeyOn(-1, key, velocityAt(event->pos()));
    }
}

void KeyboardStrip::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
        stopKey();
}

void KeyboardStrip::leaveEvent(QEvent * event)
{
    stopKey();
    QWidget::leaveEvent(event);
}

void KeyboardStrip::hideEvent(QHideEvent * event)
{
    // The key releases are not received while the page is hidden
    stopKey();
    _playedKeys.clear();
    QWidget::hideEvent(event);
}

void KeyboardStrip::stopKey()
{
    if (_playingKey >= 0)
    {
        ContextManager::midi()->processKeyOff(-1, _playingKey);
        _playingKey = -1;
    }
}

void KeyboardStrip::wheelEvent(QWheelEvent * event)
{
    // Horizontal scroll, as over the scroll bar just above
    if (_view != nullptr)
        QApplication::sendEvent(_view->horizontalScrollBar(), event);
}

bool KeyboardStrip::event(QEvent * event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
        int key = keyAt(helpEvent->pos());
        double left, keyWidth;
        int visibleLeft, visibleRight;
        if (key >= 0 && getScale(left, keyWidth, visibleLeft, visibleRight))
        {
            // Hidden as soon as the mouse leaves the key
            int x1 = qRound(left + key * keyWidth);
            int x2 = qRound(left + (key + 1) * keyWidth);
            QToolTip::showText(helpEvent->globalPos(), ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key)),
                               this, QRect(x1, 0, x2 - x1, this->height()));
        }
        else
            QToolTip::hideText();
        return true;
    }
    return QWidget::event(event);
}
