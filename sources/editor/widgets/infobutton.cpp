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

#include "infobutton.h"
#include "contextmanager.h"
#include <QToolTip>
#include <QLabel>
#include <QTimer>
#include <QScreen>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QApplication>
#include <QTextDocument>

// Window with the look of a tooltip
class InfoPopup : public QLabel
{
public:
    InfoPopup(QWidget * parent) : QLabel(parent, Qt::ToolTip | Qt::BypassGraphicsProxyWidget)
    {
        this->setAttribute(Qt::WA_ShowWithoutActivating);
        this->setForegroundRole(QPalette::ToolTipText);
        this->setBackgroundRole(QPalette::ToolTipBase);
        this->setPalette(QToolTip::palette());
        this->setFont(QToolTip::font());
        this->ensurePolished();
        this->setMargin(1 + this->style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, nullptr, this));
        this->setFrameStyle(QFrame::NoFrame);
        this->setAlignment(Qt::AlignLeft);
        this->setIndent(1);
        this->setWindowOpacity(this->style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, nullptr, this) / 255.0);
    }

protected:
    void paintEvent(QPaintEvent * event) override
    {
        QStylePainter painter(this);
        QStyleOptionFrame option;
        option.initFrom(this);
        painter.drawPrimitive(QStyle::PE_PanelTipLabel, option);
        painter.end();
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

InfoButton::InfoButton(QWidget * parent) : QToolButton(parent),
    _popup(nullptr)
{
    ThemeManager * theme = ContextManager::theme();
    this->setIconSize(QSize(18, 18));
    this->setIcon(theme->getColoredSvg(":/icons/info.svg", QSize(18, 18), ThemeManager::HIGHLIGHTED_BACKGROUND));
    QColor hover = ThemeManager::mix(theme->getColor(ThemeManager::WINDOW_BACKGROUND),
                                     theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND), 0.2);
    this->setStyleSheet("QToolButton{border:0;border-radius:2px;padding:3px}"
                        "QToolButton:hover,QToolButton:pressed{background-color:" + hover.name() + "}");
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));

    _hideTimer = new QTimer(this);
    _hideTimer->setSingleShot(true);
    _hideTimer->setInterval(300);
    connect(_hideTimer, SIGNAL(timeout()), this, SLOT(onHideTimeout()));
}

QString InfoButton::tableRow(QString name, QString value, bool alignRight)
{
    // Names lighter than the values, colors of the tooltips
    QPalette palette = QToolTip::palette();
    QString nameColor = ThemeManager::mix(palette.color(QPalette::ToolTipText), palette.color(QPalette::ToolTipBase), 0.4).name();
    // (tooltips wrap rich text: the cells don't)
    return "<tr><td style=\"white-space:nowrap;padding-right:15px;color:" + nameColor + "\">" + name +
            "</td><td style=\"white-space:nowrap\"" + (alignRight ? " align=\"right\"" : "") + ">" + value + "</td></tr>";
}

QString InfoButton::tableHeader(QString text, bool first)
{
    return "<tr><td colspan=\"2\" style=\"white-space:nowrap;font-weight:bold;padding-bottom:4px" + QString(first ? "" : ";padding-top:10px") +
            "\">" + text + "</td></tr>";
}

bool InfoButton::event(QEvent * event)
{
    switch (event->type())
    {
    case QEvent::ToolTip:
        // Mouse staying over the icon: the window replaces the tooltip
        showPopup();
        return true;
    case QEvent::Enter:
        _hideTimer->stop();
        break;
    case QEvent::Leave:
        if (_popup != nullptr && _popup->isVisible())
            _hideTimer->start();
        break;
    case QEvent::ToolTipChange:
        if (_popup != nullptr && _popup->isVisible())
            updatePopup();
        break;
    case QEvent::Hide:
        hidePopup();
        break;
    default:
        break;
    }
    return QToolButton::event(event);
}

bool InfoButton::eventFilter(QObject * watched, QEvent * event)
{
    // Filter installed on the application while the window is visible
    switch (event->type())
    {
    case QEvent::Enter:
        if (watched == _popup)
            _hideTimer->stop();
        break;
    case QEvent::Leave:
        if (watched == _popup)
            _hideTimer->start();
        break;
    case QEvent::MouseButtonPress: case QEvent::MouseButtonDblClick: case QEvent::Wheel:
        // Somewhere else than the icon or the window (the events first reach the windows, which are not widgets)
        if (watched->isWidgetType() && watched != _popup && watched != this)
            hidePopup();
        break;
    case QEvent::KeyPress:
        hidePopup();
        break;
    case QEvent::WindowDeactivate:
        if (watched == this->window())
            hidePopup();
        break;
    default:
        break;
    }
    return QToolButton::eventFilter(watched, event);
}

void InfoButton::onClicked()
{
    showPopup();
}

void InfoButton::onHideTimeout()
{
    // Neither over the icon nor over the window
    if (!this->underMouse() && (_popup == nullptr || !_popup->underMouse()))
        hidePopup();
}

void InfoButton::showPopup()
{
    _hideTimer->stop();
    if (this->toolTip().isEmpty())
    {
        hidePopup();
        return;
    }

    if (_popup == nullptr)
        _popup = new InfoPopup(this);
    bool alreadyVisible = _popup->isVisible();
    updatePopup();
    if (!alreadyVisible)
    {
        _popup->show();
        qApp->installEventFilter(this);
    }
}

void InfoButton::hidePopup()
{
    _hideTimer->stop();
    if (_popup != nullptr && _popup->isVisible())
    {
        qApp->removeEventFilter(this);
        _popup->hide();
    }
}

void InfoButton::updatePopup()
{
    // Size of the text, wrapped if it's rich text (as for a tooltip)
    QString text = this->toolTip();
    _popup->setWordWrap(Qt::mightBeRichText(text));
    _popup->setText(text);
    _popup->resize(_popup->sizeHint() + QSize(1, 0));

    // Below the icon, otherwise above, without overlapping it so that the mouse can go from one to the other
    QPoint topLeft = this->mapToGlobal(QPoint(0, 0));
    QRect area = this->screen()->availableGeometry();
    int x = topLeft.x();
    if (x + _popup->width() > area.right() + 1)
        x = topLeft.x() + this->width() - _popup->width();
    x = qMax(x, area.left());
    int y = topLeft.y() + this->height();
    if (y + _popup->height() > area.bottom() + 1 && topLeft.y() - _popup->height() >= area.top())
        y = topLeft.y() - _popup->height();
    _popup->move(x, y);
}
