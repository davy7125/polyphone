#ifndef DIALOG_VISUALIZER_H
#define DIALOG_VISUALIZER_H

#include <QDialog>
#include "pile_sf2.h"
#include "qcustomplot.h"

namespace Ui {
class DialogVisualizer;
}

class DialogVisualizer : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogVisualizer(Pile_sf2 *sf2, EltID id, QWidget *parent = 0);
    ~DialogVisualizer();
    
private slots:
    void on_comboParameter_currentIndexChanged(int index);

    void on_checkLog_stateChanged(int arg1);

private:
    Pile_sf2 * _sf2;
    Ui::DialogVisualizer *ui;
    EltID _initialID;
};


class GraphVisualizer : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphVisualizer(QWidget *parent = 0);

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
};

#endif // DIALOG_VISUALIZER_H
