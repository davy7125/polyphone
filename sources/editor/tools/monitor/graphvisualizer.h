/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef GRAPHVISUALIZER_H
#define GRAPHVISUALIZER_H

#include "qcustomplot.h"

class GraphVisualizer : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphVisualizer(QWidget *parent = nullptr);
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
