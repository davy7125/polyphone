#ifndef GRAPHFILTERFREQUENCIES_H
#define GRAPHFILTERFREQUENCIES_H

#include "qcustomplot.h"

class GraphFilterFrequencies : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphFilterFrequencies(QWidget *parent = 0);
    ~GraphFilterFrequencies();

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
            return true;
        }
        return false;
    }

    void setNbFourier(int nbFourier);
    void addFourierTransform(QVector<float> fData, quint32 sampleRate);
    QVector<double> getValues();
    void setValues(QVector<double> val);

private:
    QVector<double> dValues;
    bool flagEdit;
    void replot();
    double raideurExp;
    QCPItemText * labelCoord;
    int previousX;
    double previousY;
    int _nbFourier;

    void mousePressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void write(QPoint pos);
    void afficheCoord(double x, double y);

    static const int POINT_NUMBER;
};

#endif // GRAPHFILTERFREQUENCIES_H
