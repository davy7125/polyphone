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

#include "temperamentkeyboard.h"
#include "contextmanager.h"
#include "pianokey.h"
#include "pianoscene.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QLocale>
#include <QMenu>
#include <QContextMenuEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QtMath>

// Change of a value: one cent per notch of the wheel and half a cent per pixel when dragging,
// ten times less with Ctrl. Values are rounded to a tenth of cent.
static const double WHEEL_STEP = 1.0;
static const double DRAG_STEP = 0.5;
static const double FINE_RATIO = 0.1;

// Width of the black keys as on the virtual keyboard, preferred height (if the texts fit)
static const double BLACK_KEY_WIDTH = 0.8;
static const double BLACK_KEY_HEIGHT = 0.6;

// Margin around the texts and the dots, values smaller than the font of the interface
static const double MARGIN = 3.0;
static const double FONT_RATIO = 0.85;

static QPointF eventPosition(QMouseEvent * event)
{
#if QT_VERSION < 0x060000
    return event->localPos();
#else
    return event->position();
#endif
}

static double roundValue(double value)
{
    return qRound(10.0 * qBound(TemperamentKeyboard::MIN_VALUE, value, TemperamentKeyboard::MAX_VALUE)) / 10.0;
}

TemperamentKeyboard::TemperamentKeyboard(QWidget * parent) : QWidget(parent),
    _tonality(0),
    _referencePitchKey(9),
    _hoveredKey(-1),
    _draggedKey(-1),
    _dragValue(0),
    _lastDragY(0),
    _wheelKey(-1),
    _wheelDelta(0)
{
    for (int i = 0; i < 12; i++)
    {
        _values[i] = 0;
        _used[i] = true;
    }
    this->setMouseTracking(true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->setToolTip(tr("Fine tune of each key in cents, compared to the equal temperament.") + "\n" +
                     tr("Mouse wheel or vertical drag: change a value (finer with Ctrl).") + "\n" +
                     tr("Double click: back to 0.") + "\n" +
                     tr("Right click: note used or not, tonality.") + "\n" +
                     tr("Dot: tonality."));
}

void TemperamentKeyboard::setTemperament(const double values[12], const bool used[12], int tonality)
{
    // A drag continues from the new value of its key if it changed (otherwise the remainder is kept)
    if (_draggedKey != -1 && values[_draggedKey] != _values[_draggedKey])
        _dragValue = values[_draggedKey];

    for (int i = 0; i < 12; i++)
    {
        _values[i] = values[i];
        _used[i] = used[i];
    }
    _tonality = tonality;
    if (_draggedKey != -1 && !_used[_draggedKey])
        _draggedKey = -1;
    if (_hoveredKey != -1 && !_used[_hoveredKey])
        setHoveredKey(-1);
    this->update();
}

int TemperamentKeyboard::whiteKeyWidth() const
{
    // The widest value must fit in a black key
    int textWidth = QFontMetrics(getValueFont()).horizontalAdvance(formatValue(-88.8));
    return qMax(30, qCeil((textWidth + 2 * MARGIN) / BLACK_KEY_WIDTH));
}

int TemperamentKeyboard::keyboardHeight() const
{
    return qMax(1, this->height() - 1);
}

int TemperamentKeyboard::blackKeyHeight() const
{
    // Value and dot inside, the values of the white keys below
    int height = keyboardHeight();
    int inkHeight = getTextInk().height();
    double minimum = inkHeight + dotSize() + 3 * MARGIN;
    double maximum = height - inkHeight - 2 * MARGIN;
    return qRound(qMax(minimum, qMin(BLACK_KEY_HEIGHT * height, maximum)));
}

QRect TemperamentKeyboard::getTextInk() const
{
    // Ink of the values around their baseline (signs, digits, decimal separator), smaller than the line height
    return QFontMetrics(getValueFont()).tightBoundingRect("+-0123456789" + QString(QLocale().decimalPoint()));
}

QFont TemperamentKeyboard::getValueFont() const
{
    QFont font = this->font();
    if (font.pointSizeF() > 0)
        font.setPointSizeF(FONT_RATIO * font.pointSizeF());
    else
        font.setPixelSize(qRound(FONT_RATIO * font.pixelSize()));
    return font;
}

double TemperamentKeyboard::dotSize() const
{
    return qMax(5.0, 0.12 * whiteKeyWidth());
}

QSize TemperamentKeyboard::sizeHint() const
{
    return minimumSizeHint();
}

QSize TemperamentKeyboard::minimumSizeHint() const
{
    // Value and dot in a black key, value of a white key below
    int inkHeight = getTextInk().height();
    return QSize(7 * whiteKeyWidth() + 1, qCeil(2 * inkHeight + dotSize() + 5 * MARGIN) + 2);
}

QRectF TemperamentKeyboard::getKeyRect(int key) const
{
    // Same layout as the virtual keyboard
    int whiteWidth = whiteKeyWidth();
    int height = keyboardHeight();
    int whiteIndex = key >= 5 ? (key + 1) / 2 : key / 2; // White key, or the one just before a black key
    double offsetX = PianoScene::getOffsetX(key);
    if (offsetX == 0)
        return QRectF(whiteIndex * whiteWidth + 0.5, 0.5, whiteWidth, height);

    int x = qRound((whiteIndex + (1.0 - BLACK_KEY_WIDTH) / 2 + offsetX) * whiteWidth);
    return QRectF(x + 0.5, 0.5, qRound(BLACK_KEY_WIDTH * whiteWidth), blackKeyHeight());
}

int TemperamentKeyboard::getKeyAt(const QPointF &pos) const
{
    // Black keys first since they are above the white keys
    for (int pass = 0; pass < 2; pass++)
        for (int key = 0; key < 12; key++)
            if ((PianoScene::getOffsetX(key) != 0) == (pass == 0) && getKeyRect(key).contains(pos))
                return key;
    return -1;
}

int TemperamentKeyboard::getEditableKeyAt(const QPointF &pos) const
{
    // The value of a key not used cannot be changed
    int key = getKeyAt(pos);
    return (key != -1 && _used[key]) ? key : -1;
}

QString TemperamentKeyboard::formatValue(double value) const
{
    value = qRound(10.0 * value) / 10.0;
    if (value == 0)
        return "0";
    return (value > 0 ? "+" : "-") + QLocale().toString(qAbs(value), 'f', 1);
}

void TemperamentKeyboard::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    int activeKey = _draggedKey != -1 ? _draggedKey : _hoveredKey;
    painter.setFont(getValueFont());
    QFontMetrics fm(painter.font());
    QRect ink = getTextInk();
    double dot = dotSize();
    double blackBottom = 0.5 + blackKeyHeight();

    // White keys then black keys
    for (int pass = 0; pass < 2; pass++)
    {
        bool black = (pass == 1);
        for (int key = 0; key < 12; key++)
        {
            if ((PianoScene::getOffsetX(key) != 0) != black)
                continue;

            // Key, highlighted if it is hovered or edited, gray if it is not used (as the disabled keys of the
            // virtual keyboard), the black keys being flat
            QRectF rect = getKeyRect(key);
            QColor keyColor = black ? Qt::black : Qt::white;
            if (!_used[key])
                keyColor = ThemeManager::mix(Qt::black, Qt::white, 0.5);
            else if (key == activeKey && this->isEnabled())
                keyColor = ThemeManager::mix(keyColor, highlightColor, black ? 0.5 : 0.3);
            if (black)
            {
                painter.setPen(QPen(Qt::black, 1));
                painter.setBrush(keyColor);
                painter.drawRoundedRect(rect, 2, 2);
            }
            else
                PianoKey::drawKey(&painter, rect, false, keyColor);

            // Value at the top of a black key, below the black keys on a white key, the dots of the tonality on a
            // line between them (in the gap between the black keys for a white key)
            QPointF dotCenter(rect.center().x(), blackBottom - MARGIN - dot / 2);
            double baseline;
            if (black)
                baseline = rect.top() + MARGIN - ink.top();
            else
            {
                baseline = blackBottom + (rect.bottom() - blackBottom - ink.height()) / 2 - ink.top();
                double left, right;
                getTopGap(key, left, right);
                dotCenter.setX((left + right) / 2);
            }

            if (_used[key])
            {
                // 0 is lighter, the note of the reference pitch with a fine tune is orange
                QColor textColor = black ? Qt::white : Qt::black;
                if (hasWarning(key))
                    textColor = QColor(black ? "#ffaf28" : "#e07800");
                else if (qRound(10.0 * _values[key]) == 0)
                    textColor.setAlphaF(0.45);
                painter.setPen(textColor);
                QString text = formatValue(_values[key]);
                painter.drawText(QPointF(rect.center().x() - fm.horizontalAdvance(text) / 2.0, baseline), text);
            }

            // Tonality
            if (key == _tonality)
            {
                painter.setPen(QPen(black ? Qt::white : Qt::black, 1));
                painter.setBrush(highlightColor);
                painter.drawEllipse(dotCenter, dot / 2, dot / 2);
            }
        }
    }
}

void TemperamentKeyboard::setHoveredKey(int key)
{
    if (key != _hoveredKey)
    {
        _hoveredKey = key;
        this->setCursor(key == -1 ? Qt::ArrowCursor : Qt::SizeVerCursor);
        this->update();
    }
}

void TemperamentKeyboard::editValue(int key, double value)
{
    value = roundValue(value);
    if (value == _values[key])
        return;
    _values[key] = value;
    this->update();
    emit edited();
}

void TemperamentKeyboard::wheelEvent(QWheelEvent * event)
{
    int key = getKeyAt(event->position());
    if (key == -1)
    {
        QWidget::wheelEvent(event);
        return;
    }
    event->accept();
    if (!_used[key])
    {
        setHoveredKey(-1);
        return;
    }
    setHoveredKey(key);

    // Notches (120 units each), possibly given in several events by high resolution devices
    if (key != _wheelKey)
    {
        _wheelKey = key;
        _wheelDelta = 0;
    }
    _wheelDelta += event->angleDelta().y();
    int notches = _wheelDelta / 120;
    _wheelDelta -= 120 * notches;
    if (notches != 0)
    {
        double step = (event->modifiers() & Qt::ControlModifier) ? WHEEL_STEP * FINE_RATIO : WHEEL_STEP;
        editValue(key, _values[key] + notches * step);
    }
}

void TemperamentKeyboard::mousePressEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;
    int key = getEditableKeyAt(eventPosition(event));
    if (key == -1)
        return;
    _draggedKey = key;
    _dragValue = _values[key];
    _lastDragY = eventPosition(event).y();
    this->update();
}

void TemperamentKeyboard::mouseMoveEvent(QMouseEvent * event)
{
    if (_draggedKey == -1)
    {
        setHoveredKey(getEditableKeyAt(eventPosition(event)));
        return;
    }

    // Upward: higher value
    qreal y = eventPosition(event).y();
    double step = (event->modifiers() & Qt::ControlModifier) ? DRAG_STEP * FINE_RATIO : DRAG_STEP;
    _dragValue = qBound(MIN_VALUE, _dragValue + (_lastDragY - y) * step, MAX_VALUE);
    _lastDragY = y;
    editValue(_draggedKey, _dragValue);
}

void TemperamentKeyboard::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton || _draggedKey == -1)
        return;
    _draggedKey = -1;
    setHoveredKey(getEditableKeyAt(eventPosition(event)));
    this->update();
}

void TemperamentKeyboard::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
        return;
    int key = getEditableKeyAt(eventPosition(event));
    if (key == -1)
        return;
    editValue(key, 0);

    // The second press of a double click must also start a drag
    _draggedKey = key;
    _dragValue = _values[key];
    _lastDragY = eventPosition(event).y();
}

void TemperamentKeyboard::contextMenuEvent(QContextMenuEvent * event)
{
    int key = getKeyAt(event->pos());
    if (key == -1 || _draggedKey != -1)
        return;

    QMenu menu(this);
    menu.setStyleSheet(ContextManager::theme()->getMenuTheme());
    QAction * usedAction = menu.addAction(tr("Note used"));
    usedAction->setCheckable(true);
    usedAction->setChecked(_used[key]);
    QAction * tonalityAction = menu.addAction(tr("Tonality on this note"));
    tonalityAction->setCheckable(true);
    tonalityAction->setChecked(key == _tonality);
    tonalityAction->setEnabled(_tonality != -1 && key != _tonality); // Not for the equal temperament

    QAction * action = menu.exec(event->globalPos());
    if (action == usedAction)
    {
        _used[key] = !_used[key];
        if (!_used[key] && _hoveredKey == key)
            setHoveredKey(-1);
        this->update();
        emit edited();
    }
    else if (action == tonalityAction)
        emit tonalityRequested(key);
}

void TemperamentKeyboard::setReferencePitchKey(int key)
{
    if (key != _referencePitchKey)
    {
        _referencePitchKey = key;
        this->update();
    }
}

bool TemperamentKeyboard::hasWarning(int key) const
{
    // The note of the reference pitch keeps its pitch: a fine tune shifts all the notes
    return key == _referencePitchKey && _used[key] && qRound(10.0 * _values[key]) != 0;
}

void TemperamentKeyboard::getTopGap(int key, double &left, double &right) const
{
    // Part of a key not covered by the black keys at the top
    QRectF rect = getKeyRect(key);
    left = rect.left();
    right = rect.right();
    if (PianoScene::getOffsetX(key) == 0)
    {
        if (key > 0 && PianoScene::getOffsetX(key - 1) != 0)
            left = getKeyRect(key - 1).right();
        if (key < 11 && PianoScene::getOffsetX(key + 1) != 0)
            right = getKeyRect(key + 1).left();
    }
}

bool TemperamentKeyboard::event(QEvent * event)
{
    // Explanation of the warning, otherwise the help of the keyboard
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
        int key = getKeyAt(helpEvent->pos());
        if (key != -1 && hasWarning(key))
        {
            QToolTip::showText(helpEvent->globalPos(),
                               tr("%1 is the note of the reference pitch: its fine tune of %2 cents is compensated by "
                                  "shifting all the notes by %3 cents.")
                               .arg(ContextManager::keyName()->getKeyName(static_cast<unsigned int>(key), true, false, false, true))
                               .arg(formatValue(_values[key])).arg(formatValue(-_values[key])), this);
            return true;
        }
    }
    return QWidget::event(event);
}

void TemperamentKeyboard::leaveEvent(QEvent * event)
{
    if (_draggedKey == -1)
        setHoveredKey(-1);
    QWidget::leaveEvent(event);
}

void TemperamentKeyboard::changeEvent(QEvent * event)
{
    // The size of the keys depends on the font
    if (event->type() == QEvent::FontChange)
        this->updateGeometry();
    QWidget::changeEvent(event);
}
