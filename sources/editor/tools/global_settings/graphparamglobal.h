#ifndef GRAPHPARAMGLOBAL_H
#define GRAPHPARAMGLOBAL_H

#include "qcustomplot.h"

class GraphParamGlobal : public QCustomPlot
{
    Q_OBJECT

public:
    enum TypeForme
    {
        FORME_MANUELLE,
        FORME_LINEAIRE_ASC,
        FORME_LINEAIRE_DESC,
        FORME_EXP_ASC,
        FORME_EXP_DESC,
        FORME_ALEATOIRE
    };

    explicit GraphParamGlobal(QWidget *parent = 0);
    ~GraphParamGlobal();

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
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mousePressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseReleased(pos);
            }
            else if (mouseEvent->button() == Qt::RightButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mouseRightPressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseRightReleased(pos);
            }
            return true;
        }
        return false;
    }

    void indexMotifChanged(int index);
    void raideurChanged(double value);
    void setKeyboardRange(int keyboardType);
    void setMinMax(double min, double max)  { yMin = qMin(min, max); yMax = qMax(min, max); }
    void setMinMaxX(int min, int max)       { xMin = qMin(min, max); xMax = qMax(min, max); }
    QVector<double> getValues();
    void setValues(QVector<double> val);
    int getXmin()                           { return xMin; }
    int getXmax()                           { return xMax; }

private:
    TypeForme forme;
    QVector<double> dValues;
    bool flagEdit;
    int limitEdit;
    void replot();
    int nbPoints;
    double raideurExp;
    double yMin, yMax;
    int xMin, xMax;
    QCPItemText * labelCoord;
    int previousX;
    double previousY;

    // Méthodes privées
    void mousePressed(QPoint pos);
    void mouseRightPressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseRightReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void writeMotif();
    void write(QPoint pos);
    void afficheCoord(double x, double y);
};

#endif // GRAPHPARAMGLOBAL_H
