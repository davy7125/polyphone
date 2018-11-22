#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QMouseEvent>

class TabBar : public QTabBar
{
public:
    TabBar(QWidget *parent = nullptr) : QTabBar(parent)
    {
        this->setDrawBase(false);
    }

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        // Current tab
        int tabIndex = this->tabAt(event->pos());

        // Position in the current tab
        _leftOffset = event->x() - this->tabRect(tabIndex).left() + 2;
        _rightOffset = this->tabRect(tabIndex).right() - event->x() + 1;

        // All tabs movable except the first one
        setMovable(tabIndex != 0);
        QTabBar::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        if (event->x() - _leftOffset < this->tabRect(0).right())
        {
            // Too far on the left
            event->ignore();
            QMouseEvent newEvent(event->type(),
                                 QPointF(this->tabRect(0).right() + _leftOffset, event->y()),
                                 event->button(), event->buttons(), event->modifiers());
            QTabBar::mouseMoveEvent(&newEvent);
        }
        else if (event->x() + _rightOffset > this->tabRect(this->count() - 1).right())
        {
            // Too far on the right
            event->ignore();
            QMouseEvent newEvent(event->type(),
                                 QPointF(this->tabRect(this->count() - 1).right() - _rightOffset, event->y()),
                                 event->button(), event->buttons(), event->modifiers());
            QTabBar::mouseMoveEvent(&newEvent);
        }
        else
            QTabBar::mouseMoveEvent(event);
    }

private:
    int _leftOffset;
    int _rightOffset;
};

#endif // TABBAR_H
