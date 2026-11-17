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

#ifndef KEYBOARDSTRIP_H
#define KEYBOARDSTRIP_H

#include <QWidget>
class QGraphicsView;

// Row of white and black squares, one per key, following the horizontal zoom and scroll of a view in which the key k
// lies between the abscissas k - 0.5 and k + 0.5 (range editor, which places it between its viewport and its
// horizontal scroll bar). The part of the scene of the view on the left of the first key (margin of the labels) is left
// empty, with the background of the graph. Clicking plays the keys, with a velocity increasing downwards as on the
// virtual keyboard. The painting of the keys is shared with the timbre view.
class KeyboardStrip : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardStrip(QWidget * parent = nullptr);
    void setView(QGraphicsView * view);
    void setPlayedKey(int key, bool isOn);

    // Painting shared with the timbre view
    struct Key
    {
        int key;
        double left, right;
        QColor color;
    };
    static const int KEYS_HEIGHT; // Height of the painted keys (no bottom border)
    static bool isBlackKey(int key);
    static QColor keyColor(int key, bool isDark);

    // Keys side by side, the separators being drawn after all the fills (otherwise, depending on the rounding of the
    // abscissas, the next key covers the separator and two white keys merge). Border on the top, left and right only.
    static void paintKeys(QPainter &painter, const QList<Key> &keys, double top, double height, const QColor &border);

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void leaveEvent(QEvent * event) override;
    void hideEvent(QHideEvent * event) override;
    void wheelEvent(QWheelEvent * event) override;
    bool event(QEvent * event) override;

private:
    bool getScale(double &left, double &keyWidth, int &visibleLeft, int &visibleRight) const;
    int keyAt(QPoint position) const;
    int velocityAt(QPoint position) const;
    void stopKey();

    static const int TOP_MARGIN;

    QGraphicsView * _view;
    QList<int> _playedKeys;
    int _playingKey;
    bool _isDark;
    QColor _border, _highlight;
};

#endif // KEYBOARDSTRIP_H
