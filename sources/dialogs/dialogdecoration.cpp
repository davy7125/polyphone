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

#include "dialogdecoration.h"
#include <QAbstractButton>
#include <QAbstractSlider>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QToolButton>
#include <QWindow>
#include "contextmanager.h"
#include "topbackground.h"
#include "windowresizer.h"

const int DialogDecoration::CLOSE_MARGIN = 6;

void DialogDecoration::apply(QDialog * dialog, QWidget * banner)
{
    if (!ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "window_borders", false).toBool())
        new DialogDecoration(dialog, banner);
}

DialogDecoration::DialogDecoration(QDialog * dialog, QWidget * banner) : QObject(dialog),
    _dialog(dialog),
    _titleBar(nullptr),
    _labelTitle(nullptr),
    _aboutToMove(false)
{
    // Only the type of the window is kept: Qt ignores FramelessWindowHint when a hint about the title bar is present.
    // The background is painted here, the corners outside the rounded top being transparent.
    dialog->setWindowFlags((dialog->windowFlags() & Qt::WindowType_Mask) | Qt::FramelessWindowHint);
    dialog->setAttribute(Qt::WA_TranslucentBackground, true);

    // Colors of the top of the main window without borders (see TopBackground and TopRightWidget), and of the banner
    ThemeManager * theme = ContextManager::theme();
    _borderColor = theme->getColor(theme->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
                                       ThemeManager::LIST_BACKGROUND : ThemeManager::LIST_TEXT);
    _titleColor = _borderColor.lighter(170);
    _bannerColor = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    ThemeManager::ColorType textColor = theme->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
                                            ThemeManager::WINDOW_TEXT : ThemeManager::WINDOW_BACKGROUND;

    // Bar made of the title, the banner and the close button, its background being painted with the window so that it
    // follows the rounded corners. It moves the window.
    _titleBar = new QWidget(dialog);
    _titleBar->installEventFilter(this);
    _titleBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    delete dialog->layout()->replaceWidget(banner, _titleBar);
    QHBoxLayout * layout = new QHBoxLayout(_titleBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _labelTitle = new QLabel(dialog->windowTitle());
    _labelTitle->setObjectName("labelTitle");
    _labelTitle->setStyleSheet("QLabel{padding:0 12px;color:" + theme->getColor(textColor).name() + "}");
    QFont font = _labelTitle->font();
    font.setBold(true);
    _labelTitle->setFont(font);
    connect(dialog, SIGNAL(windowTitleChanged(QString)), _labelTitle, SLOT(setText(QString)));
    layout->addWidget(_labelTitle);

    layout->addWidget(banner, 1);

    // Close button with the background of the one of the main window, surrounded by the color of the banner. Its icon
    // changes when hovered, as the other buttons of the banner.
    QToolButton * buttonClose = new QToolButton();
    buttonClose->setAutoRaise(true);
    buttonClose->setFocusPolicy(Qt::NoFocus); // The space key must not close the window
    buttonClose->setCursor(Qt::PointingHandCursor);
    buttonClose->setToolTip(tr("Close"));
    int size = qMax(20, banner->minimumHeight() - 2 * CLOSE_MARGIN);
    buttonClose->setFixedSize(size, size);
    buttonClose->setIconSize(QSize(16, 16));
    QIcon icon;
    icon.addPixmap(theme->getColoredSvg(":/icons/close.svg", QSize(16, 16), textColor), QIcon::Normal);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = theme->getColor(textColor, ThemeManager::HOVERED).name();
    icon.addPixmap(theme->getColoredSvg(":/icons/close.svg", QSize(16, 16), replacement), QIcon::Active);
    buttonClose->setIcon(icon);
    buttonClose->setStyleSheet("QToolButton{border:0;border-radius:3px;background-color:" + _titleColor.name() + "}");
    connect(buttonClose, SIGNAL(clicked()), dialog, SLOT(close()));
    int bannerMargin = banner->layout() != nullptr ? banner->layout()->contentsMargins().right() : 0;
    layout->addSpacing(qMax(0, CLOSE_MARGIN - bannerMargin));
    layout->addWidget(buttonClose, 0, Qt::AlignVCenter);
    layout->addSpacing(CLOSE_MARGIN);

    // Room for the line around the window
    QLayout * dialogLayout = dialog->layout();
    dialogLayout->setContentsMargins(dialogLayout->contentsMargins() + QMargins(1, 1, 1, 1));
    dialog->installEventFilter(this);

    // The edges resize the window
    new WindowResizer(dialog);
}

QPainterPath DialogDecoration::getOutline(bool closed)
{
    // Rounded top corners as the main window, unless the size is imposed. The path follows the middle of the border
    // pixels; when it is open, its ends go beyond the bottom so that the sides fill the bottom corners.
    QRectF rect = QRectF(_dialog->rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    double bottom = closed ? rect.bottom() : _dialog->height();
    QPainterPath path;
    path.moveTo(rect.left(), bottom);
    if (_dialog->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen))
    {
        path.lineTo(rect.topLeft());
        path.lineTo(rect.topRight());
    }
    else
    {
        double diameter = 2 * TopBackground::CORNER_RADIUS;
        path.arcTo(rect.left(), rect.top(), diameter, diameter, 180, -90);
        path.arcTo(rect.right() - diameter, rect.top(), diameter, diameter, 90, -90);
    }
    path.lineTo(rect.right(), bottom);
    if (closed)
        path.closeSubpath();
    return path;
}

void DialogDecoration::paintBackground()
{
    QPainterPath outline = getOutline(true);
    int width = _dialog->width();
    int height = _dialog->height();
    int barBottom = _titleBar->geometry().bottom() + 1;
    int titleRight = _titleBar->x() + _labelTitle->geometry().right() + 1;

    QPainter p(_dialog);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);

    // Title on the left side of the bar, color of the banner for the rest (behind the close button)
    p.setClipRect(0, 0, titleRight, barBottom);
    p.setBrush(_titleColor);
    p.drawPath(outline);
    p.setClipRect(titleRight, 0, width - titleRight, barBottom);
    p.setBrush(_bannerColor);
    p.drawPath(outline);

    // Content of the window
    p.setClipRect(0, barBottom, width, height - barBottom);
    p.setBrush(_dialog->palette().window());
    p.drawPath(outline);

    // Line around the window, the bottom being drawn without antialiasing (it would make the corners transparent)
    p.setClipping(false);
    p.setPen(QPen(_borderColor, 1, Qt::SolidLine, Qt::FlatCap));
    p.setBrush(Qt::NoBrush);
    p.drawPath(getOutline(false));
    p.fillRect(0, height - 1, width, 1, _borderColor);
}

bool DialogDecoration::eventFilter(QObject * watched, QEvent * event)
{
    if (watched == _dialog)
    {
        // Background of the window, below its content (no other painting for a dialog)
        if (event->type() == QEvent::Paint)
            paintBackground();
    }
    else if (watched == _titleBar)
    {
        // The bar moves the window, except where a widget reacts to the mouse: the presses on the title, on a label or on
        // a free space of the banner come here, these widgets ignoring them. A control can also ignore a press (slider
        // without range, outside the shape of a button...), which doesn't move the window either.
        switch (event->type())
        {
        case QEvent::MouseButtonPress: {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
#if QT_VERSION < 0x060000
            QPoint pos = mouseEvent->pos();
#else
            QPoint pos = mouseEvent->position().toPoint();
#endif
            _aboutToMove = mouseEvent->button() == Qt::LeftButton && !isControl(_titleBar->childAt(pos));
            return _aboutToMove;
        }
        case QEvent::MouseMove:
            if (_aboutToMove)
            {
                _aboutToMove = false;
                if (_dialog->windowHandle() != nullptr)
                    _dialog->windowHandle()->startSystemMove();
                return true;
            }
            break;
        case QEvent::MouseButtonRelease:
            _aboutToMove = false;
            break;
        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}

bool DialogDecoration::isControl(QWidget * widget)
{
    // Button, menu, list, slider or text field, or a part of it
    for (; widget != nullptr && widget != _titleBar; widget = widget->parentWidget())
        if (qobject_cast<QAbstractButton *>(widget) != nullptr || qobject_cast<QComboBox *>(widget) != nullptr ||
            qobject_cast<QAbstractSlider *>(widget) != nullptr || qobject_cast<QLineEdit *>(widget) != nullptr)
            return true;
    return false;
}
