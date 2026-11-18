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

#ifndef TEMPERAMENTKEYBOARD_H
#define TEMPERAMENTKEYBOARD_H

#include <QWidget>

// Keys from C to B placed as on the virtual keyboard, each one showing its fine tune in cents: flat black keys with
// their value at the top, the values of the white keys at the bottom, the height being given by the layout (above a
// minimum keeping the texts readable).
// A value is changed with the mouse wheel or a vertical drag, a double click sets it back to 0.
// The keys not used are gray, a dot marks the tonality. The value of the note of the reference pitch is orange if it
// is not 0 (all the notes being shifted). The context menu of a key changes its use and the tonality.
class TemperamentKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit TemperamentKeyboard(QWidget * parent = nullptr);

    // Fine tunes in cents and use of the keys from C to B, tonality (0 = C, -1 if not used)
    void setTemperament(const double values[12], const bool used[12], int tonality);
    const double * getValues() const { return _values; }
    const bool * getUsed() const { return _used; }

    // Note of the reference pitch (0 = C), sounding at this pitch whatever its fine tune
    void setReferencePitchKey(int key);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    static constexpr double MIN_VALUE = -50.0;
    static constexpr double MAX_VALUE = 50.0;

signals:
    // A value or the use of a key has been changed by the user
    void edited();

    // Another tonality is asked with the context menu (transposition)
    void tonalityRequested(int key);

protected:
    void paintEvent(QPaintEvent * event) override;
    void wheelEvent(QWheelEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void contextMenuEvent(QContextMenuEvent * event) override;
    void leaveEvent(QEvent * event) override;
    void changeEvent(QEvent * event) override;
    bool event(QEvent * event) override;

private:
    QRectF getKeyRect(int key) const;
    int getKeyAt(const QPointF &pos) const;
    int getEditableKeyAt(const QPointF &pos) const;
    bool hasWarning(int key) const;
    void getTopGap(int key, double &left, double &right) const;
    QRect getTextInk() const;
    QFont getValueFont() const;
    double dotSize() const;
    void setHoveredKey(int key);
    void editValue(int key, double value);
    QString formatValue(double value) const;
    int whiteKeyWidth() const;
    int keyboardHeight() const;
    int blackKeyHeight() const;

    double _values[12];
    bool _used[12];
    int _tonality;
    int _referencePitchKey;
    int _hoveredKey;
    int _draggedKey;
    double _dragValue; // Not rounded, so that slow movements are not lost
    qreal _lastDragY;
    int _wheelKey;
    int _wheelDelta; // Remainder of the angle of the wheel (high resolution devices)
};

#endif // TEMPERAMENTKEYBOARD_H
