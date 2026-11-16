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


#include "timbremapwidget.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "keyboardstrip.h"
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QApplication>
#include <cmath>

const double TimbreMapWidget::LABEL_PADDING = 4;
const double TimbreMapWidget::MAX_ZOOM = 8;

TimbreMapWidget::TimbreMapWidget(QWidget * parent) : QWidget(parent),
    _model(nullptr),
    _selectedKey(-1),
    _ghostKey(-1),
    _velocity(100),
    _graphLeft(2),
    _graphWidth(100),
    _labelWidth(50),
    _marginWidth(58),
    _zoom(1),
    _scroll(0),
    _left(60),
    _columnWidth(1),
    _namesTop(0),
    _summaryTop(0),
    _summaryHeight(14),
    _mapTop(0),
    _mapHeight(0),
    _rowHeight(1),
    _decisionTop(0),
    _decisionHeight(20),
    _keyboardTop(0),
    _keyboardHeight(KeyboardStrip::KEYS_HEIGHT),
    _maxLevel(0),
    _maxTransposition(1),
    _pressedButton(Qt::NoButton),
    _pressScroll(0),
    _pressZoom(1),
    _pressColumn(0),
    _moved(false),
    _pressedKey(-1),
    _playingKey(-1),
    _rangeCache(12)
{
    this->setMouseTracking(true);
    this->setMinimumHeight(250);
    this->setAttribute(Qt::WA_OpaquePaintEvent, true);
}

QString TimbreMapWidget::getSummaryName(int summary)
{
    switch (summary)
    {
    case TimbreDisplay::SUMMARY_LEVEL: return tr("Level");
    case TimbreDisplay::SUMMARY_TIMBRE: return tr("Timbre");
    case TimbreDisplay::SUMMARY_TRANSPOSITION: return tr("Transposition");
    case TimbreDisplay::SUMMARY_BRIGHTNESS: return tr("Brightness");
    case TimbreDisplay::SUMMARY_ODD_EVEN: return tr("Odd / even");
    case TimbreDisplay::SUMMARY_NOISE: return tr("Noise");
    case TimbreDisplay::SUMMARY_TUNING: return tr("Tuning");
    case TimbreDisplay::SUMMARY_LOOP: return tr("Loop");
    default: return "";
    }
}

QString TimbreMapWidget::getModeName(int mode)
{
    switch (mode)
    {
    case TimbreDisplay::MODE_ABSOLUTE: return tr("Absolute levels");
    case TimbreDisplay::MODE_RELATIVE: return tr("Relative levels");
    case TimbreDisplay::MODE_DEVIATION: return tr("Deviation from the reference");
    case TimbreDisplay::MODE_JUMP: return tr("Jumps");
    case TimbreDisplay::MODE_TRANSPOSITION: return tr("Transposition");
    case TimbreDisplay::MODE_CONFIDENCE: return tr("Confidence");
    default: return "";
    }
}

void TimbreMapWidget::setDisplay(const TimbreDisplay &display)
{
    _display = display;
    updateColumns();
    update();
}

void TimbreMapWidget::setSelection(int key, const QList<int> &positions)
{
    _selectedKey = key;
    _selectedPositions = positions;
    update();
}

void TimbreMapWidget::setGhost(int key)
{
    _ghostKey = key;
    update();
}

void TimbreMapWidget::setPlayedKey(int key, bool isOn)
{
    if (isOn && !_playedKeys.contains(key))
        _playedKeys << key;
    else if (!isOn)
        _playedKeys.removeAll(key);
    update();
}

void TimbreMapWidget::updateColumns()
{
    _columns.clear();
    if (_model == nullptr || _model->getMinKey() < 0)
        return;
    if (_display.positionsOnly)
    {
        // Keys where the root keys are displayed
        foreach (TimbrePosition position, _model->getPositions().values())
            if (!_columns.contains(position.anchor))
                _columns << position.anchor;
        std::sort(_columns.begin(), _columns.end());
    }
    else
    {
        for (int key = _model->getMinKey(); key <= _model->getMaxKey(); key++)
            _columns << key;
    }

    // Values of the rows and bounds of the summaries
    _maxLevel = -1000;
    _maxTransposition = 1;
    _summaryBounds.clear();
    for (int key = 0; key < 128; key++)
    {
        const TimbreKey &tk = _model->getKey(key);
        _levels[key] = _model->getRowLevels(tk.spectrum);
        _states[key] = _model->getRowStates(tk.spectrum);
        _confidence[key] = _model->getRowConfidence(tk.spectrum);
        if (tk.sources.isEmpty() || !_columns.contains(key))
            continue;
        foreach (double level, _levels[key])
            if (!std::isnan(level))
                _maxLevel = qMax(_maxLevel, level);
        _maxTransposition = qMax(_maxTransposition, std::fabs(tk.transposition));
        QList<QPair<int, double> > values;
        values << qMakePair(static_cast<int>(TimbreDisplay::SUMMARY_LEVEL), tk.totalLevel)
               << qMakePair(static_cast<int>(TimbreDisplay::SUMMARY_BRIGHTNESS), tk.centroid);
        for (int i = 0; i < values.count(); i++)
        {
            if (!std::isfinite(values[i].second))
                continue;
            if (!_summaryBounds.contains(values[i].first))
                _summaryBounds[values[i].first] = qMakePair(values[i].second, values[i].second);
            QPair<double, double> &bounds = _summaryBounds[values[i].first];
            bounds.first = qMin(bounds.first, values[i].second);
            bounds.second = qMax(bounds.second, values[i].second);
        }
    }
    _maxTransposition = std::ceil(_maxTransposition);
    _rangeCache = getAutoRange();
}

double TimbreMapWidget::getAutoRange()
{
    if (_model == nullptr)
        return 12;
    if (!_display.autoRange)
        return _display.fixedRange;
    const TimbreSettings &settings = _model->getSettings();
    switch (_display.mode)
    {
    case TimbreDisplay::MODE_ABSOLUTE: case TimbreDisplay::MODE_RELATIVE: case TimbreDisplay::MODE_CONFIDENCE:
        return settings.dynamicRange;
    case TimbreDisplay::MODE_TRANSPOSITION:
        return _maxTransposition;
    default: {
        // Deviations: twice the maximum threshold, more if needed (95% of the values within the range)
        QVector<double> values;
        foreach (TimbrePosition position, _model->getPositions().values())
            foreach (double value, position.deviation.rowDifferences)
                if (!std::isnan(value))
                    values << std::fabs(value);
        std::sort(values.begin(), values.end());
        double range = 2 * settings.maximum;
        if (!values.isEmpty())
            range = qMax(range, values[static_cast<int>(0.95 * (values.size() - 1))]);
        return std::ceil(qMin(range, 40.0));
    }
    }
}

void TimbreMapWidget::computeGeometry()
{
    QFontMetrics metrics(this->font());

    // Labels written inside the graph, in a margin keeping the first key away from them
    _labelWidth = qMax(metrics.horizontalAdvance(tr("Decisions")), metrics.horizontalAdvance("129-161"));
    foreach (int summary, _display.summaries)
        _labelWidth = qMax(_labelWidth, static_cast<double>(metrics.horizontalAdvance(getSummaryName(summary))));
    if (_model != nullptr)
    {
        foreach (TimbreRow row, _model->getRows())
            _labelWidth = qMax(_labelWidth, static_cast<double>(metrics.horizontalAdvance(row.label)));
    }
    _marginWidth = _labelWidth + 2 * LABEL_PADDING;

    // Horizontal axis: zoom and scroll
    _graphLeft = 2;
    _graphWidth = qMax(1.0, this->width() - _graphLeft - 4);
    _columnWidth = getColumnWidth(_zoom);
    _scroll = qBound(0.0, _scroll, _zoom * _graphWidth - _graphWidth);
    _left = _graphLeft + _marginWidth - _scroll;

    // Vertical axis: summaries, map, decisions, names of the keys and keyboard at the bottom (as in the range editor)
    _summaryHeight = qMax(12, metrics.height());
    _summaryTop = 4;
    _mapTop = _summaryTop + _display.summaries.count() * _summaryHeight + 6;
    _keyboardHeight = KeyboardStrip::KEYS_HEIGHT;
    _keyboardTop = this->height() - 1 - _keyboardHeight;
    _namesTop = _keyboardTop - metrics.height();
    _decisionHeight = qMax(18, metrics.height() + 4);
    _decisionTop = _namesTop - 1 - _decisionHeight;
    _mapHeight = qMax(10.0, _decisionTop - 4 - _mapTop);
    int rowCount = _model != nullptr ? static_cast<int>(_model->getRows().count()) : 0;
    _rowHeight = rowCount > 0 ? _mapHeight / rowCount : _mapHeight;
}

double TimbreMapWidget::getColumnWidth(double zoom) const
{
    // The whole content, margin included, is "zoom" times as wide as the graph
    if (_columns.isEmpty())
        return 1;
    return qMax(1.0, (zoom * _graphWidth - _marginWidth) / _columns.count());
}

void TimbreMapWidget::setScroll(double scroll)
{
    _scroll = qBound(0.0, scroll, _zoom * _graphWidth - _graphWidth);
    _left = _graphLeft + _marginWidth - _scroll;
    update();
}

QRectF TimbreMapWidget::columnRect(int column, double top, double height) const
{
    return QRectF(_left + column * _columnWidth, top, _columnWidth, height);
}

void TimbreMapWidget::drawLabel(QPainter &painter, double top, double height, const QString &text)
{
    // The labels stay on the left of the graph: once the margin is scrolled, they are written over the columns
    QRectF rect(_graphLeft + LABEL_PADDING, top, _labelWidth, height);
    if (_scroll > 0)
    {
        QFontMetrics metrics(this->font());
        QColor background = _palette.background();
        background.setAlpha(220);
        double textHeight = qMin(height, static_cast<double>(metrics.height()));
        painter.fillRect(QRectF(rect.left() - 2, rect.center().y() - textHeight / 2,
                                metrics.horizontalAdvance(text) + 4, textHeight), background);
    }
    painter.setPen(_palette.axisText());
    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip, text);
}

int TimbreMapWidget::columnAt(int x) const
{
    if (_columns.isEmpty() || x < _left || x < _graphLeft || x >= _graphLeft + _graphWidth)
        return -1;
    int column = static_cast<int>((x - _left) / _columnWidth);
    return column < _columns.count() ? column : -1;
}

int TimbreMapWidget::keyAt(int x) const
{
    int column = columnAt(x);
    return column >= 0 ? _columns[column] : -1;
}

QColor TimbreMapWidget::divergingColor(double value, double range, bool withThresholds)
{
    if (std::isnan(value))
        return _palette.neutral();
    if (withThresholds && _model != nullptr)
    {
        const TimbreSettings &settings = _model->getSettings();
        double absolute = std::fabs(value);
        double t = absolute <= settings.tolerance ? 0 : (absolute <= settings.maximum ? 0.5 : 1);
        return _palette.diverging(std::copysign(t, value));
    }
    return _palette.diverging(range > 0 ? value / range : 0);
}

QColor TimbreMapWidget::cellColor(int key, int row, bool &hatched, bool &absent)
{
    hatched = false;
    absent = false;
    const TimbreKey &tk = _model->getKey(key);
    if (tk.sources.isEmpty())
    {
        absent = true;
        return _palette.background();
    }
    if (!tk.analyzed)
    {
        hatched = true;
        return _palette.neutral();
    }
    const QVector<int> &states = _states[key];
    if (row >= states.size() || states[row] == 0)
    {
        absent = true;
        hatched = true;
        return _palette.background();
    }
    hatched = (states[row] == 1 || states[row] == 3);
    double range = _rangeCache;

    switch (_display.mode)
    {
    case TimbreDisplay::MODE_ABSOLUTE: case TimbreDisplay::MODE_RELATIVE: {
        const QVector<double> &levels = _levels[key];
        double top = _maxLevel; // Strongest level of the instrument
        if (_display.mode == TimbreDisplay::MODE_RELATIVE)
        {
            top = -1000;
            foreach (double level, levels)
                if (!std::isnan(level))
                    top = qMax(top, level);
        }
        return _palette.sequential(1 + (levels[row] - top) / range);
    }
    case TimbreDisplay::MODE_DEVIATION: {
        if (!_model->getPositions().contains(tk.position))
            return _palette.neutral();
        const TimbrePosition &position = _model->getPositions()[tk.position];
        if (!position.deviation.valid)
            return _palette.neutral();
        double value = row < position.deviation.rowDifferences.size() ? position.deviation.rowDifferences[row] : std::nan("");
        if (std::isnan(value))
            hatched = true;
        return divergingColor(value, range, _display.steps);
    }
    case TimbreDisplay::MODE_JUMP: {
        if (!tk.jump.valid)
            return _palette.neutral();
        double value = row < tk.jump.rowDifferences.size() ? tk.jump.rowDifferences[row] : std::nan("");
        return divergingColor(value, range, _display.steps);
    }
    case TimbreDisplay::MODE_TRANSPOSITION:
        return _palette.diverging(range > 0 ? tk.transposition / range : 0);
    case TimbreDisplay::MODE_CONFIDENCE: {
        const QVector<double> &confidence = _confidence[key];
        return std::isnan(confidence[row]) ? _palette.neutral() : _palette.sequential(confidence[row]);
    }
    }
    return _palette.neutral();
}

QColor TimbreMapWidget::summaryColor(int summary, int key, bool &hatched)
{
    hatched = false;
    const TimbreKey &tk = _model->getKey(key);
    if (tk.sources.isEmpty())
        return _palette.background();
    if (!tk.analyzed)
    {
        hatched = true;
        return _palette.neutral();
    }
    const TimbreSettings &settings = _model->getSettings();
    auto normalized = [&](double value, int summaryType) -> double {
        if (!_summaryBounds.contains(summaryType))
            return 0.5;
        QPair<double, double> bounds = _summaryBounds[summaryType];
        return bounds.second > bounds.first ? (value - bounds.first) / (bounds.second - bounds.first) : 0.5;
    };

    switch (summary)
    {
    case TimbreDisplay::SUMMARY_LEVEL:
        if (!std::isfinite(tk.totalLevel))
            return _palette.neutral();
        return _palette.sequential(normalized(tk.totalLevel, TimbreDisplay::SUMMARY_LEVEL));
    case TimbreDisplay::SUMMARY_TIMBRE: {
        if (!_model->getPositions().contains(tk.position))
            return _palette.neutral();
        const TimbrePosition &position = _model->getPositions()[tk.position];
        if (!position.deviation.valid)
            return _palette.neutral();
        hatched = position.deviation.confidence < 0.5 || position.referenceAtEdge;
        double value = position.deviation.timbreDifference;
        if (_display.steps)
            return _palette.diverging(value <= settings.tolerance ? 0 : (value <= settings.maximum ? 0.5 : 1));
        return _palette.diverging(qMin(1.0, value / (2 * settings.maximum)));
    }
    case TimbreDisplay::SUMMARY_TRANSPOSITION:
        return _palette.diverging(tk.transposition / _maxTransposition);
    case TimbreDisplay::SUMMARY_BRIGHTNESS:
        if (!std::isfinite(tk.centroid))
            return _palette.neutral();
        return _palette.sequential(normalized(tk.centroid, TimbreDisplay::SUMMARY_BRIGHTNESS));
    case TimbreDisplay::SUMMARY_ODD_EVEN:
        if (!std::isfinite(tk.oddEven))
            return _palette.neutral();
        return _palette.diverging(tk.oddEven / 12);
    case TimbreDisplay::SUMMARY_NOISE:
        if (!std::isfinite(tk.hnr))
            return _palette.neutral();
        return _palette.sequential(tk.hnr / 60);
    case TimbreDisplay::SUMMARY_TUNING: {
        if (!std::isfinite(tk.tuning))
            return _palette.neutral();
        double cents = tk.tuning - 100 * std::round(tk.tuning / 100); // Octave or mutation stops
        return _palette.diverging(cents / 20);
    }
    case TimbreDisplay::SUMMARY_LOOP:
        if (!std::isfinite(tk.loopCorrelation))
        {
            hatched = true;
            return _palette.neutral();
        }
        return _palette.sequential(qBound(0.0, (tk.loopCorrelation - 0.9) / 0.1, 1.0));
    default:
        break;
    }
    return _palette.neutral();
}

void TimbreMapWidget::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(this->rect(), _palette.background());
    if (_model == nullptr || _columns.isEmpty() || _model->getRows().isEmpty())
        return;
    computeGeometry();
    QFontMetrics metrics(this->font());
    painter.setFont(this->font());
    QBrush hatchBrush(_palette.hatch(), Qt::BDiagPattern);
    QBrush pendingBrush(_palette.hatch(), Qt::Dense6Pattern);
    const QList<TimbreRow> &rows = _model->getRows();
    painter.setClipRect(QRectF(_graphLeft, 0, _graphWidth, this->height()));

    // Summaries, their names in the margin
    for (int s = 0; s < _display.summaries.count(); s++)
    {
        double top = _summaryTop + s * _summaryHeight;
        for (int c = 0; c < _columns.count(); c++)
        {
            bool hatched;
            QRectF rect = columnRect(c, top + 1, _summaryHeight - 2);
            painter.fillRect(rect, summaryColor(_display.summaries[s], _columns[c], hatched));
            if (hatched)
                painter.fillRect(rect, _model->getKey(_columns[c]).analyzed ? hatchBrush : pendingBrush);
        }
    }

    // Map, rank 1 at the bottom
    for (int c = 0; c < _columns.count(); c++)
    {
        int key = _columns[c];
        bool pending = !_model->getKey(key).sources.isEmpty() && !_model->getKey(key).analyzed;
        for (int row = 0; row < rows.count(); row++)
        {
            bool hatched, absent;
            QRectF rect = columnRect(c, _mapTop + _mapHeight - (row + 1) * _rowHeight, _rowHeight);
            painter.fillRect(rect, cellColor(key, row, hatched, absent));
            if (hatched)
                painter.fillRect(rect, pending ? pendingBrush : hatchBrush);
        }
    }

    // Separation between the detailed ranks and the bands
    int detailed = 0;
    while (detailed < rows.count() && rows[detailed].firstRank == rows[detailed].lastRank)
        detailed++;
    if (detailed > 0 && detailed < rows.count())
    {
        double y = _mapTop + _mapHeight - detailed * _rowHeight;
        painter.setPen(QPen(_palette.background(), 2));
        painter.drawLine(QPointF(_left - _marginWidth, y), QPointF(_left + _columns.count() * _columnWidth, y));
    }


    // Ghost column: sound predicted at a free key
    int ghostColumn = _columns.indexOf(_ghostKey);
    if (ghostColumn >= 0 && !_model->getPositions().contains(_ghostKey) &&
        (_display.mode == TimbreDisplay::MODE_ABSOLUTE || _display.mode == TimbreDisplay::MODE_RELATIVE))
    {
        TimbreSpectrum predicted = _model->predict(_ghostKey);
        QVector<double> levels = _model->getRowLevels(predicted);
        double top = _maxLevel;
        if (_display.mode == TimbreDisplay::MODE_RELATIVE)
        {
            top = -1000;
            foreach (double level, levels)
                if (!std::isnan(level))
                    top = qMax(top, level);
        }
        for (int row = 0; row < rows.count() && row < levels.size(); row++)
        {
            QRectF rect = columnRect(ghostColumn, _mapTop + _mapHeight - (row + 1) * _rowHeight, _rowHeight);
            if (std::isnan(levels[row]))
                painter.fillRect(rect, hatchBrush);
            else
                painter.fillRect(rect, _palette.sequential(1 + (levels[row] - top) / _rangeCache));
        }
    }

    // Junctions: thicker when the sample changes
    for (int c = 1; c < _columns.count(); c++)
    {
        const TimbreKey &tk = _model->getKey(_columns[c]);
        if (_columns[c] != _columns[c - 1] + 1 || !tk.junction)
            continue;
        double x = _left + c * _columnWidth;
        QColor color = _palette.text();
        color.setAlpha(tk.sampleChange ? 140 : 50);
        painter.setPen(QPen(color, tk.sampleChange ? 1.5 : 1));
        painter.drawLine(QPointF(x, _mapTop), QPointF(x, _mapTop + _mapHeight));
    }

    // Root keys: a dot, or a triangle pointing to the root key when it is outside the keys covered by its divisions
    // (displayed at the closest key they cover)
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(_palette.background(), 1));
    painter.setBrush(_palette.text());
    double radius = qBound(2.0, _columnWidth / 4, 4.0);
    foreach (TimbrePosition position, _model->getPositions().values())
    {
        int c = _columns.indexOf(position.anchor);
        if (c < 0)
            continue;
        QPointF center(_left + (c + 0.5) * _columnWidth, _mapTop + _mapHeight - radius - 1);
        if (position.anchor == position.key)
            painter.drawEllipse(center, radius, radius);
        else
        {
            double direction = position.key > position.anchor ? 1 : -1;
            double half = radius * 1.25;
            painter.drawPolygon(QPolygonF() << QPointF(center.x() - direction * half, center.y() - half)
                                << QPointF(center.x() + direction * half, center.y())
                                << QPointF(center.x() - direction * half, center.y() + half));
        }
    }
    painter.setRenderHint(QPainter::Antialiasing, false);

    paintDecisions(painter);
    paintKeyboard(painter);

    // Selected positions and key
    painter.setBrush(Qt::NoBrush);
    QColor highlight = _palette.highlight();
    foreach (int key, _selectedPositions)
    {
        if (!_model->getPositions().contains(key))
            continue;
        const TimbrePosition &position = _model->getPositions()[key];
        int c1 = -1, c2 = -1;
        for (int c = 0; c < _columns.count(); c++)
        {
            if (_columns[c] >= position.keyMin && _columns[c] <= position.keyMax)
            {
                if (c1 == -1)
                    c1 = c;
                c2 = c;
            }
        }
        if (c1 == -1)
            continue;
        QRectF rect(_left + c1 * _columnWidth, _decisionTop, (c2 - c1 + 1) * _columnWidth, _decisionHeight);
        QColor fill = highlight;
        fill.setAlpha(60);
        painter.fillRect(rect, fill);
        painter.setPen(QPen(highlight, 1));
        painter.drawRect(QRectF(_left + c1 * _columnWidth, _mapTop, (c2 - c1 + 1) * _columnWidth, _mapHeight).adjusted(0.5, 0.5, -0.5, -0.5));
    }
    int selectedColumn = _columns.indexOf(_selectedKey);
    if (selectedColumn >= 0)
    {
        painter.setPen(QPen(highlight, 2));
        painter.drawRect(columnRect(selectedColumn, _summaryTop, _decisionTop + _decisionHeight - _summaryTop).adjusted(1, 1, -1, -1));
    }
    if (ghostColumn >= 0 && !_model->getPositions().contains(_ghostKey))
    {
        painter.setPen(QPen(highlight, 2, Qt::DashLine));
        painter.drawRect(columnRect(ghostColumn, _mapTop, _mapHeight).adjusted(1, 1, -1, -1));
    }

    // Names of the rows, above everything as in the range editor (every n rows if they don't fit)
    for (int s = 0; s < _display.summaries.count(); s++)
        drawLabel(painter, _summaryTop + s * _summaryHeight, _summaryHeight, getSummaryName(_display.summaries[s]));
    int step = qMax(1, static_cast<int>(std::ceil(metrics.height() / qMax(1.0, _rowHeight))));
    for (int row = 0; row < rows.count(); row += step)
        drawLabel(painter, _mapTop + _mapHeight - (row + 1) * _rowHeight, _rowHeight, rows[row].label);
    drawLabel(painter, _decisionTop, _decisionHeight, tr("Decisions"));

    // Dilation in progress: line from the point where the right button has been pressed, as in the range editor
    if (_pressedButton == Qt::RightButton && _moved)
    {
        painter.setPen(QPen(ContextManager::theme()->getFixedColor(ThemeManager::RED, true), 2, Qt::DotLine));
        painter.drawLine(_pressPosition, QPoint(_currentPosition.x(), _pressPosition.y()));
    }
}

void TimbreMapWidget::paintDecisions(QPainter &painter)
{
    double size = qMin(_decisionHeight * 0.6, qMax(_columnWidth, 8.0) * 1.2);

    // Labels of the positions (upper part) and of the level (lower part)
    foreach (TimbrePosition position, _model->getPositions().values())
    {
        int c = _columns.indexOf(position.anchor);
        if (c < 0)
            continue;
        double x = _left + (c + 0.5) * _columnWidth;
        QRectF rect(x - size / 2, _decisionTop + 1, size, size);
        switch (position.label)
        {
        case TimbrePosition::LABEL_CONFORM:
            TimbrePalette::drawShape(painter, TimbrePalette::SHAPE_CHECK, rect, _palette.conform());
            break;
        case TimbrePosition::LABEL_REDUNDANT:
            TimbrePalette::drawShape(painter, TimbrePalette::SHAPE_MINUS, rect, _palette.redundant());
            break;
        case TimbrePosition::LABEL_TO_CORRECT:
            TimbrePalette::drawShape(painter, TimbrePalette::SHAPE_TILDE, rect, _palette.toCorrect());
            break;
        case TimbrePosition::LABEL_TOO_DIFFERENT:
            TimbrePalette::drawShape(painter, TimbrePalette::SHAPE_CROSS, rect, _palette.tooDifferent());
            break;
        default:
            break;
        }
        if (position.levelLabel)
        {
            double triangle = _decisionHeight - size - 1;
            QRectF levelRect(x - triangle / 2, _decisionTop + size, triangle, triangle);
            TimbrePalette::drawShape(painter, position.deviation.levelDifference > 0 ? TimbrePalette::SHAPE_UP :
                                                                                       TimbrePalette::SHAPE_DOWN,
                                     levelRect, _palette.levelColor());
        }
    }

    // Breaks at the junctions
    for (int c = 1; c < _columns.count(); c++)
    {
        if (_columns[c] != _columns[c - 1] + 1 || !_model->getKey(_columns[c]).breakLabel)
            continue;
        double x = _left + c * _columnWidth;
        TimbrePalette::drawShape(painter, TimbrePalette::SHAPE_PLUS, QRectF(x - size / 2, _decisionTop + 1, size, size),
                                 _palette.breakColor());
    }
}

void TimbreMapWidget::paintKeyboard(QPainter &painter)
{
    // Gray area below the labels, that cannot be played
    QList<KeyboardStrip::Key> keys;
    keys << KeyboardStrip::Key { -1, _left - _marginWidth, _left,
                                 KeyboardStrip::marginColor(_palette.background(), _palette.text()) };
    for (int c = 0; c < _columns.count(); c++)
    {
        int key = _columns[c];
        QColor color = KeyboardStrip::keyColor(key, _palette.isDark());
        if (_playedKeys.contains(key))
            color = _palette.highlight();
        else if (_model->getKey(key).sources.isEmpty())
            color = ThemeManager::mix(color, _palette.background(), 0.6);
        keys << KeyboardStrip::Key { key, _left + c * _columnWidth, _left + (c + 1) * _columnWidth, color };
    }
    KeyboardStrip::paintKeys(painter, keys, _keyboardTop, _keyboardHeight, _palette.grid());

    // Names of the C keys, above the keyboard
    QFontMetrics metrics(this->font());
    painter.setPen(_palette.axisText());
    for (int c = 0; c < _columns.count(); c++)
    {
        int key = _columns[c];
        if (key % 12 != 0 && !(_display.positionsOnly && _columnWidth > metrics.horizontalAdvance("C#00")))
            continue;
        QString name = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key));
        double width = metrics.horizontalAdvance(name) + 4;
        QRectF rect(_left + (c + 0.5) * _columnWidth - width / 2, _namesTop, width, metrics.height());
        painter.drawText(rect, Qt::AlignCenter, name);
    }
}

void TimbreMapWidget::mousePressEvent(QMouseEvent * event)
{
    if (_model == nullptr || _columns.isEmpty() || _pressedButton != Qt::NoButton || _playingKey >= 0)
        return;
    int key = keyAt(event->pos().x());
    if (event->button() == Qt::LeftButton && event->pos().y() >= _keyboardTop && event->pos().y() <= _keyboardTop + _keyboardHeight)
    {
        // Keyboard
        if (key >= 0)
        {
            _playingKey = key;
            ContextManager::midi()->processKeyOn(-1, key, _velocity);
        }
        return;
    }
    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton)
        return;

    // Selection on release if the mouse hasn't moved, otherwise translation (left) or dilation (right)
    _pressedButton = event->button();
    _pressPosition = _currentPosition = event->pos();
    _pressScroll = _scroll;
    _pressZoom = _zoom;
    _pressColumn = (event->pos().x() - _left) / _columnWidth;
    _moved = false;
    _pressedKey = key;
}

void TimbreMapWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (_playingKey >= 0)
    {
        // Glissando
        int key = keyAt(event->pos().x());
        if (key >= 0 && key != _playingKey)
        {
            ContextManager::midi()->processKeyOff(-1, _playingKey);
            _playingKey = key;
            ContextManager::midi()->processKeyOn(-1, key, _velocity);
        }
        return;
    }
    if (_pressedButton == Qt::NoButton)
        return;
    _currentPosition = event->pos();
    int dx = event->pos().x() - _pressPosition.x();
    if (!_moved && (event->pos() - _pressPosition).manhattanLength() < QApplication::startDragDistance())
        return;
    _moved = true;

    if (_pressedButton == Qt::LeftButton)
    {
        // Translation
        this->setCursor(Qt::ClosedHandCursor);
        setScroll(_pressScroll - dx);
    }
    else
    {
        // Dilation around the point where the button has been pressed (the same formula as in the range editor)
        this->setCursor(Qt::ClosedHandCursor);
        _zoom = qBound(1.0, _pressZoom * std::pow(2.0, 10.0 * dx / this->width()), MAX_ZOOM);
        _columnWidth = getColumnWidth(_zoom);
        setScroll(_graphLeft + _marginWidth + _pressColumn * _columnWidth - _pressPosition.x());
    }
}

void TimbreMapWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (_playingKey >= 0)
    {
        if (event->button() == Qt::LeftButton)
        {
            ContextManager::midi()->processKeyOff(-1, _playingKey);
            _playingKey = -1;
        }
        return;
    }
    if (event->button() != _pressedButton)
        return;
    if (_pressedButton == Qt::LeftButton && !_moved && _pressedKey >= 0)
        emit keySelected(_pressedKey, event->modifiers());
    _pressedButton = Qt::NoButton;
    _pressedKey = -1;
    _moved = false;
    this->setCursor(Qt::ArrowCursor);
    update();
}

void TimbreMapWidget::leaveEvent(QEvent * event)
{
    if (_playingKey >= 0)
    {
        ContextManager::midi()->processKeyOff(-1, _playingKey);
        _playingKey = -1;
    }
    QWidget::leaveEvent(event);
}

bool TimbreMapWidget::event(QEvent * event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
        QString text = getTooltip(helpEvent->pos());
        if (text.isEmpty())
            QToolTip::hideText();
        else
            QToolTip::showText(helpEvent->globalPos(), text, this);
        return true;
    }
    return QWidget::event(event);
}

QString TimbreMapWidget::getTooltip(QPoint position)
{
    int key = keyAt(position.x());
    if (key < 0 || _model == nullptr)
        return "";
    const TimbreKey &tk = _model->getKey(key);
    QLocale locale;
    auto number = [&](double value, int decimals) -> QString {
        return std::isfinite(value) ? locale.toString(value, 'f', decimals) : "-";
    };
    QStringList lines;
    lines << "<b>" + ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key)) + "</b>";
    if (tk.sources.isEmpty())
    {
        lines << tr("No sample");
        return lines.join("<br/>");
    }

    // Row under the mouse
    double y = position.y();
    const QList<TimbreRow> &rows = _model->getRows();
    if (y >= _mapTop && y < _mapTop + _mapHeight && _rowHeight > 0)
    {
        int row = static_cast<int>((_mapTop + _mapHeight - y) / _rowHeight);
        if (row >= 0 && row < rows.count())
        {
            const QVector<double> &levels = _levels[key];
            lines << tr("Row: %1").arg(rows[row].label) + " — " + tr("level: %1 dB").arg(number(levels[row], 1));
            if (_model->getPositions().contains(tk.position))
            {
                const TimbrePosition &p = _model->getPositions()[tk.position];
                if (p.deviation.valid && row < p.deviation.rowDifferences.size() && !std::isnan(p.deviation.rowDifferences[row]))
                    lines << tr("Deviation from the reference: %1 dB").arg(number(p.deviation.rowDifferences[row], 1));
            }
            if (tk.jump.valid && row < tk.jump.rowDifferences.size() && !std::isnan(tk.jump.rowDifferences[row]))
                lines << tr("Jump from the previous key: %1 dB").arg(number(tk.jump.rowDifferences[row], 1));
        }
    }

    // Sources
    SoundfontManager * sm = SoundfontManager::getInstance();
    QStringList samples;
    foreach (TimbreVoice voice, tk.voices)
    {
        QString name = sm->getQstr(EltID(elementSmpl, _model->getInstrument().indexSf2, voice.sampleIndex), champ_name);
        if (!samples.contains(name))
            samples << name;
    }
    lines << tr("Samples: %1").arg(samples.join(", "));
    if (_model->getPositions().contains(tk.position))
    {
        QString name = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(tk.position));
        if (_model->getPositions()[tk.position].anchor == tk.position)
            lines << tr("Root key: %1").arg(name);
        else
            lines << tr("Root key: %1, outside the key range of its divisions").arg(name);
    }
    lines << tr("Transposition: %1 semitones").arg(number(tk.transposition, 2));
    lines << tr("Duration factor: %1").arg(number(tk.durationFactor, 3));
    lines << tr("Level: %1 dB").arg(number(tk.totalLevel, 1));
    return lines.join("<br/>");
}
