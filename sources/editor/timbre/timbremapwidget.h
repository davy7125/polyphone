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


#ifndef TIMBREMAPWIDGET_H
#define TIMBREMAPWIDGET_H

#include <QWidget>
#include "timbremodel.h"
#include "timbrepalette.h"

// Options of the display of the map
struct TimbreDisplay
{
    enum Mode
    {
        MODE_ABSOLUTE = 0, // Levels in dB
        MODE_RELATIVE = 1, // Levels relative to the strongest row of the key
        MODE_DEVIATION = 2, // Deviation from the reference, over the keys of each position
        MODE_JUMP = 3, // Difference with the previous key
        MODE_TRANSPOSITION = 4, // Semitones from the root key
        MODE_CONFIDENCE = 5 // Part of the power well measured
    };
    Mode mode = MODE_DEVIATION;
    bool autoRange = true;
    double fixedRange = 12; // dB of the diverging palettes (deviation, jump), the levels using the dynamic range
    bool steps = false; // Colors by steps aligned on the thresholds, the tolerance in the neutral color
    bool positionsOnly = false; // One column per root key

    // Optional summary rows
    enum Summary
    {
        SUMMARY_LEVEL = 0,
        SUMMARY_TIMBRE = 1,
        SUMMARY_TRANSPOSITION = 2,
        SUMMARY_BRIGHTNESS = 3,
        SUMMARY_ODD_EVEN = 4,
        SUMMARY_NOISE = 5,
        SUMMARY_TUNING = 6,
        SUMMARY_LOOP = 7
    };
    QList<int> summaries = { SUMMARY_LEVEL, SUMMARY_TIMBRE, SUMMARY_TRANSPOSITION };
};

// Map of the timbre: one column per key, one row per rank or band (rank 1 at the bottom, as a spectrogram), summaries
// above, the names of the keys and a playable keyboard below. The names of the rows are written inside the graph, in
// a margin on the left of the first key. The horizontal axis is dilated by holding the right button (as the zoom of the
// range editor) and moved by holding the left button, a click selecting a key.
class TimbreMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimbreMapWidget(QWidget * parent = nullptr);
    void setModel(TimbreModel * model) { _model = model; updateColumns(); update(); }
    void setDisplay(const TimbreDisplay &display);
    const TimbreDisplay &getDisplay() const { return _display; }
    void setSelection(int key, const QList<int> &positions);
    void setGhost(int key); // Predicted column at a free key, -1 for none
    void setPlayedKey(int key, bool isOn);
    void setVelocity(int velocity) { _velocity = velocity; }
    void modelUpdated() { updateColumns(); update(); }

    static QString getSummaryName(int summary);
    static QString getModeName(int mode);

signals:
    void keySelected(int key, Qt::KeyboardModifiers modifiers);

protected:
    void paintEvent(QPaintEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void leaveEvent(QEvent * event) override;
    bool event(QEvent * event) override;

private:
    void updateColumns();
    void computeGeometry();
    double getColumnWidth(double zoom) const;
    void setScroll(double scroll);
    int columnAt(int x) const;
    int keyAt(int x) const;
    QRectF columnRect(int column, double top, double height) const;
    void drawLabel(QPainter &painter, double top, double height, const QString &text);
    QColor cellColor(int key, int row, bool &hatched, bool &absent);
    QColor summaryColor(int summary, int key, bool &hatched);
    QColor divergingColor(double value, double range, bool withThresholds);
    QString getTooltip(QPoint position);
    double getAutoRange();
    void paintKeyboard(QPainter &painter);
    void paintDecisions(QPainter &painter);

    TimbreModel * _model;
    TimbreDisplay _display;
    TimbrePalette _palette;
    QList<int> _columns; // Keys displayed
    int _selectedKey;
    QList<int> _selectedPositions;
    int _ghostKey;
    QList<int> _playedKeys;
    int _velocity;

    // Geometry: the graph is visible between _graphLeft and _graphLeft + _graphWidth, its content (margin of the labels
    // then the columns) being _zoom times as wide and scrolled by _scroll pixels
    double _graphLeft, _graphWidth, _labelWidth, _marginWidth;
    double _zoom, _scroll;
    double _left, _columnWidth; // Abscissa of the first column (after the margin), possibly outside the graph
    double _namesTop;
    double _summaryTop, _summaryHeight; // Per summary row
    double _mapTop, _mapHeight, _rowHeight;
    double _decisionTop, _decisionHeight;
    double _keyboardTop, _keyboardHeight;

    // Values per key computed once for each model
    QVector<double> _levels[128], _confidence[128];
    QVector<int> _states[128];
    double _maxLevel;
    QMap<int, QPair<double, double> > _summaryBounds; // Summary => minimum, maximum
    double _maxTransposition;

    // Mouse: click (selection), translation (left button held), dilation (right button held), keyboard
    Qt::MouseButton _pressedButton;
    QPoint _pressPosition, _currentPosition;
    double _pressScroll, _pressZoom, _pressColumn; // Column under the mouse, in column widths from the first one
    bool _moved;
    int _pressedKey, _playingKey;
    double _rangeCache;

    static const double LABEL_PADDING;
    static const double MAX_ZOOM;
};

#endif // TIMBREMAPWIDGET_H
