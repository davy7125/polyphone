#ifndef GRAPHSPACE_H
#define GRAPHSPACE_H

#include "qcustomplot.h"

class GraphSpace : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphSpace(QWidget *parent = nullptr);
    void setData(QVector<double> x, QVector<int> y);

    bool eventFilter(QObject* o, QEvent* e)
    {
        if ((e->type() == QEvent::MouseMove ||
             e->type() == QEvent::MouseButtonPress ||
             e->type() == QEvent::MouseButtonRelease ||
             e->type() == QEvent::Leave)
                && o == this)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
            QPoint pos = mouseEvent->pos();
            if (mouseEvent->type() == QEvent::MouseMove)
                this->mouseMoved(pos);
            else if (mouseEvent->type() == QEvent::Leave)
                this->mouseLeft();
            return true;
        }
        return false;
    }

private:
    void mouseMoved(QPoint pos);
    void mouseLeft();

    void afficheCoord(double xPan, double yLength, int key);
    QVector<double> _xPan, _yLength;
    QVector<int> _yKey;
    QCPItemText * labelCoord;
};

#endif // GRAPHSPACE_H
