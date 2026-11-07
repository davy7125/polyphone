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

#include "windowresizer.h"
#include <QApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

const int WindowResizer::BORDER_WIDTH = 5;

WindowResizer::WindowResizer(QWidget * window) : QObject(window),
    _window(window),
    _cursorChanged(false)
{
    // The events of all widgets are watched: the edges of the window are covered by its children
    qApp->installEventFilter(this);
}

WindowResizer::~WindowResizer()
{
    if (qApp != nullptr)
        qApp->removeEventFilter(this);
}

bool WindowResizer::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::MouseMove)
    {
        // No resize when the pointer is over another window (it could start when coming back without any move here),
        // or if the size is imposed
        QWidget * widget = qobject_cast<QWidget *>(watched);
        if (widget == nullptr || widget->window() != _window ||
            (_window->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen)))
        {
            reset();
            return false;
        }

        // Not during a drag
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() != Qt::NoButton)
            return false;

#if QT_VERSION < 0x060000
        _edges = getEdges(mouseEvent->globalPos());
#else
        _edges = getEdges(mouseEvent->globalPosition().toPoint());
#endif
        updateCursor();
        return _edges != Qt::Edges(); // The widget under the edge doesn't react
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        // Clicks in the other windows are not for this one
        QWidget * widget = qobject_cast<QWidget *>(watched);
        if (widget == nullptr || widget->window() != _window)
            return false;

        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() == Qt::LeftButton && _edges != Qt::Edges())
        {
            QWindow * window = _window->windowHandle();
            if (window != nullptr)
                window->startSystemResize(_edges);
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
        reset();

    return false;
}

Qt::Edges WindowResizer::getEdges(QPoint globalPosition)
{
    // Only the edges that can be moved: the width or the height can be fixed
    QPoint pos = _window->mapFromGlobal(globalPosition);
    Qt::Edges edges;
    if (_window->minimumWidth() != _window->maximumWidth())
    {
        if (pos.x() >= 0 && pos.x() <= BORDER_WIDTH)
            edges |= Qt::LeftEdge;
        else if (pos.x() >= _window->width() - BORDER_WIDTH && pos.x() <= _window->width())
            edges |= Qt::RightEdge;
    }
    if (_window->minimumHeight() != _window->maximumHeight())
    {
        if (pos.y() >= 0 && pos.y() <= BORDER_WIDTH)
            edges |= Qt::TopEdge;
        else if (pos.y() >= _window->height() - BORDER_WIDTH && pos.y() <= _window->height())
            edges |= Qt::BottomEdge;
    }
    return edges;
}

void WindowResizer::updateCursor()
{
    Qt::CursorShape shape;
    if (_edges == Qt::LeftEdge || _edges == Qt::RightEdge)
        shape = Qt::SizeHorCursor;
    else if (_edges == Qt::TopEdge || _edges == Qt::BottomEdge)
        shape = Qt::SizeVerCursor;
    else if (_edges == (Qt::LeftEdge | Qt::TopEdge) || _edges == (Qt::RightEdge | Qt::BottomEdge))
        shape = Qt::SizeFDiagCursor;
    else if (_edges == (Qt::LeftEdge | Qt::BottomEdge) || _edges == (Qt::RightEdge | Qt::TopEdge))
        shape = Qt::SizeBDiagCursor;
    else
    {
        if (_cursorChanged)
        {
            _window->unsetCursor();
            _cursorChanged = false;
        }
        return;
    }

    if (!_cursorChanged || _window->cursor().shape() != shape)
    {
        _window->setCursor(shape);
        _cursorChanged = true;
    }
}

void WindowResizer::reset()
{
    _edges = Qt::Edges();
    updateCursor();
}
