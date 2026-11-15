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

#include "resettabledial.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QLabel>
#include <QTimer>
#include <QToolTip>
#include <QScreen>
#include <QStylePainter>
#include <QStyleOptionFrame>

// Number of pixels for covering the whole range, multiplied by 10 with Shift for a fine adjustment
static const double s_dragLength = 200.0;

// Angle of the minimum value and angle covered by the whole range, in degrees (counterclockwise from 3 o'clock, as QDial)
static const double s_angleMin = 240.0;
static const double s_angleRange = 300.0;

// Time during which the value remains displayed after the last wheel step, key or mouse release (ms)
static const int s_bubbleDuration = 1000;

// Space between the dial and its bubble (px)
static const int s_bubbleSpacing = 2;

// Label looking like a tooltip (same settings as QTipLabel in Qt), but not closed by the clicks and the wheel
// It is transparent for the window system: otherwise, when the mouse goes over it while dragging the dial, the
// compositor gives it the pointer (Wayland), the dial then losing the drag and the focus
class ValueBubble : public QLabel
{
public:
    ValueBubble(QWidget * parent) : QLabel(parent, Qt::ToolTip | Qt::BypassGraphicsProxyWidget |
                                                   Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus)
    {
        this->setForegroundRole(QPalette::ToolTipText);
        this->setBackgroundRole(QPalette::ToolTipBase);
        this->setPalette(QToolTip::palette());
        this->setFont(QToolTip::font());
        this->ensurePolished();
        this->setMargin(1 + this->style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, nullptr, this));
        this->setFrameStyle(QFrame::NoFrame);
        this->setAlignment(Qt::AlignCenter);
        this->setTextFormat(Qt::PlainText);
        this->setWindowOpacity(this->style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, nullptr, this) / 255.0);
        this->setAttribute(Qt::WA_ShowWithoutActivating);
    }

protected:
    void paintEvent(QPaintEvent * event) override
    {
        QStylePainter p(this);
        QStyleOptionFrame opt;
        opt.initFrom(this);
        p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
        p.end();

        QLabel::paintEvent(event);
    }

    void resizeEvent(QResizeEvent * event) override
    {
        QStyleHintReturnMask frameMask;
        QStyleOption option;
        option.initFrom(this);
        if (this->style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
            this->setMask(frameMask.region);

        QLabel::resizeEvent(event);
    }
};

ResettableDial * ResettableDial::s_dialWithBubble = nullptr;

ResettableDial::ResettableDial(QWidget * parent) : QDial(parent),
    _defaultValue(0),
    _dragValue(0),
    _valueBubble(nullptr),
    _bubbleTimer(new QTimer(this)),
    _valueBubbleEnabled(false),
    _bubbleFromDrag(false)
{
    _bubbleTimer->setSingleShot(true);
    _bubbleTimer->setInterval(s_bubbleDuration);
    connect(_bubbleTimer, &QTimer::timeout, this, &ResettableDial::hideValueBubble);
}

ResettableDial::~ResettableDial()
{
    if (s_dialWithBubble == this)
        s_dialWithBubble = nullptr;
}

void ResettableDial::setValueBubbleEnabled(bool enabled)
{
    _valueBubbleEnabled = enabled;
    if (!enabled)
        hideValueBubble();
}

bool ResettableDial::event(QEvent * event)
{
    // No tooltip over the bubble, which already contains the name
    if (event->type() == QEvent::ToolTip && _valueBubble != nullptr && _valueBubble->isVisible())
    {
        event->accept();
        return true;
    }

    return QDial::event(event);
}

void ResettableDial::mousePressEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mousePressEvent(event);
        return;
    }

    // Unlike QDial, the value doesn't jump to the position of the mouse
    event->accept();
    _dragValue = this->value();
#if QT_VERSION < 0x060000
    _lastPosition = event->localPos();
#else
    _lastPosition = event->position();
#endif
    this->setSliderDown(true);
    showValueBubble(true);
}

void ResettableDial::mouseMoveEvent(QMouseEvent * event)
{
    // Not after a double click, the button being held without having been pressed
    if (!this->isSliderDown())
    {
        event->ignore();
        return;
    }

    // The value increases upward and to the right
#if QT_VERSION < 0x060000
    QPointF position = event->localPos();
#else
    QPointF position = event->position();
#endif
    QPointF delta = position - _lastPosition;
    _lastPosition = position;
    double length = (event->modifiers() & Qt::ShiftModifier) ? 10.0 * s_dragLength : s_dragLength;
    _dragValue += (delta.x() - delta.y()) * (this->maximum() - this->minimum()) / length;
    _dragValue = qBound(static_cast<double>(this->minimum()), _dragValue, static_cast<double>(this->maximum()));
    this->setSliderPosition(qRound(_dragValue));
    showValueBubble(true);
}

void ResettableDial::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mouseReleaseEvent(event);
        return;
    }

    // QDial would take the value under the mouse
    this->setSliderDown(false);

    // The final value remains readable a moment
    if (_valueBubble != nullptr && _valueBubble->isVisible())
        _bubbleTimer->start();
}

void ResettableDial::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mouseDoubleClickEvent(event);
        return;
    }

    // The base implementation is not called since it would process the second click as a press
    event->accept();
    this->setValue(_defaultValue);
    showValueBubble(false);
}

void ResettableDial::wheelEvent(QWheelEvent * event)
{
    QDial::wheelEvent(event);
    showValueBubble(false);
}

void ResettableDial::keyPressEvent(QKeyEvent * event)
{
    QDial::keyPressEvent(event);
    if (event->isAccepted())
        showValueBubble(false);
}

void ResettableDial::leaveEvent(QEvent * event)
{
    // Not after a drag: the mouse can be released out of the dial
    if (!_bubbleFromDrag)
        hideValueBubble();
    QDial::leaveEvent(event);
}

void ResettableDial::hideEvent(QHideEvent * event)
{
    hideValueBubble();
    QDial::hideEvent(event);
}

void ResettableDial::showValueBubble(bool dragging)
{
    if (!_valueBubbleEnabled)
        return;

    if (s_dialWithBubble != nullptr && s_dialWithBubble != this)
        s_dialWithBubble->hideValueBubble();
    s_dialWithBubble = this;
    _bubbleFromDrag = dragging;

    if (_valueBubble == nullptr)
        _valueBubble = new ValueBubble(this);

    // Raw value, below the name of the parameter if it is not already written next to the dial
    QString text = QString::number(this->sliderPosition());
    if (!this->toolTip().isEmpty())
        text = this->toolTip() + "\n" + text;
    _valueBubble->setText(text);
    _valueBubble->adjustSize();

    // Centered above the dial, or below if the screen is too small above
    QRect dialRect(this->mapToGlobal(QPoint(0, 0)), this->size());
    QPoint position(dialRect.center().x() - _valueBubble->width() / 2,
                    dialRect.top() - s_bubbleSpacing - _valueBubble->height());
    if (this->screen() != nullptr)
    {
        QRect screenRect = this->screen()->geometry();
        if (position.y() < screenRect.top())
            position.setY(dialRect.bottom() + 1 + s_bubbleSpacing);
        position.setX(qMax(screenRect.left(), qMin(position.x(), screenRect.right() + 1 - _valueBubble->width())));
    }
    _valueBubble->move(position);
    _valueBubble->show();

    // Possible tooltip replaced by the bubble
    QToolTip::hideText();

    // Hidden after a delay, the mouse button being released if dragging
    if (dragging)
        _bubbleTimer->stop();
    else
        _bubbleTimer->start();
}

void ResettableDial::hideValueBubble()
{
    _bubbleTimer->stop();
    if (_valueBubble != nullptr)
        _valueBubble->hide();
    if (s_dialWithBubble == this)
        s_dialWithBubble = nullptr;
}

void ResettableDial::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    // Colors mixed with the text, so that they are contrasted with both light and dark backgrounds
    ThemeManager::ColorContext context = this->isEnabled() ? ThemeManager::NORMAL : ThemeManager::DISABLED;
    QColor windowColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND, context);
    QColor windowTextColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT, context);
    QColor buttonColor = ContextManager::theme()->getColor(ThemeManager::BUTTON_BACKGROUND, context);
    QColor buttonTextColor = ContextManager::theme()->getColor(ThemeManager::BUTTON_TEXT, context);
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, context);

    // From the outside to the center: notches, arc filled up to the value, knob
    QPointF center = QRectF(this->rect()).center();
    double radius = qMin(this->width(), this->height()) / 2.0 - 1.0;
    double notchLength = qMax(3.0, radius / 6);
    double arcRadius = this->notchesVisible() ? radius - notchLength - 2.0 : radius;
    double arcWidth = qMax(2.0, arcRadius / 8);
    arcRadius -= arcWidth / 2;
    double knobRadius = arcRadius - arcWidth / 2 - qMax(1.5, arcWidth / 2);
    if (knobRadius <= 0)
        return;

    double ratio = this->maximum() > this->minimum() ?
                static_cast<double>(this->sliderPosition() - this->minimum()) / (this->maximum() - this->minimum()) : 0.0;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Notches
    int notchSize = this->notchSize();
    if (this->notchesVisible() && notchSize > 0)
    {
        int notchNumber = (this->maximum() - this->minimum() + notchSize - 1) / notchSize;
        p.setPen(QPen(ThemeManager::mix(windowColor, windowTextColor, 0.5), 1.0));
        for (int i = 0; i <= notchNumber; i++)
        {
            double angle = qDegreesToRadians(s_angleMin - i * s_angleRange / notchNumber);
            QPointF direction(qCos(angle), -qSin(angle));
            p.drawLine(center + direction * (radius - notchLength), center + direction * radius);
        }
    }

    // Arc
    QRectF arcRect(center.x() - arcRadius, center.y() - arcRadius, 2 * arcRadius, 2 * arcRadius);
    QPen arcPen(ThemeManager::mix(windowColor, windowTextColor, 0.2), arcWidth, Qt::SolidLine, Qt::FlatCap);
    p.setPen(arcPen);
    p.drawArc(arcRect, qRound(16 * s_angleMin), qRound(-16 * s_angleRange));
    arcPen.setColor(highlightColor);
    p.setPen(arcPen);
    p.drawArc(arcRect, qRound(16 * s_angleMin), qRound(-16 * s_angleRange * ratio));

    // Knob, its border being highlighted when it has the focus (as with the Fusion style)
    p.setPen(this->hasFocus() ? QPen(highlightColor, 2.0) : QPen(ThemeManager::mix(buttonColor, buttonTextColor, 0.35), 1.0));
    p.setBrush(buttonColor);
    p.drawEllipse(center, knobRadius, knobRadius);

    // Pointer
    double angle = qDegreesToRadians(s_angleMin - s_angleRange * ratio);
    QPointF direction(qCos(angle), -qSin(angle));
    p.setPen(QPen(buttonTextColor, qMax(1.5, knobRadius / 8), Qt::SolidLine, Qt::RoundCap));
    p.drawLine(center + direction * 0.3 * knobRadius, center + direction * 0.8 * knobRadius);
}
