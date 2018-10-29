#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include "qcustomplot.h"

class GraphVisualizer : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphVisualizer(QWidget *parent = 0);
    ~GraphVisualizer();
    void setScale();
    void setData(QVector<QList<double> > listPoints, QVector<QList<double> > listPointsDef);
    void setIsLog(bool isLog);

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
    void afficheCoord(double x, double y);

    static const double MIN_LOG;
    double xMinDonnees, xMaxDonnees, yMinDonnees, yMaxDonnees, yMin2Donnees;
    bool isLog;
    QList<QCPItemText *> listTextOctave;
    QCPItemText * textWarning;
    QList<double> listX, listY;
    QCPItemText * labelCoord;
};

#endif // GRAPHVISUALIZER_H
