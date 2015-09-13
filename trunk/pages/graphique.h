/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include "qcustomplot.h"
class QScrollBar;
class QSpinBox;


class Graphique : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Graphique(QWidget *parent = 0);

    // Méthodes publiques
    void clearAll();
    void setData(QByteArray baData);
    void linkSliderX(QScrollBar * qScrollX);
    void linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd);
    void zoomDrag();
    void updateStyle();
    void displayMultipleSelection(bool isOn);

public slots:
    void setPosX(int posX);
    void setStartLoop(int pos, bool replot = true);
    void setEndLoop(int pos, bool replot = true);
    void setCurrentSample(int pos);

signals:
    void startLoopChanged();
    void endLoopChanged();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QPen penLecture;
    double sizeX;
    bool zoomFlag;
    bool dragFlag;
    bool modifiedFlag;
    double xInit, yInit;
    double zoomX, zoomY, posX, posY;
    double zoomXinit, zoomYinit, posXinit, posYinit;
    QPoint pointInit;
    bool bFromExt;
    QScrollBar * qScrollX;
    QSpinBox * spinStart;
    QSpinBox * spinEnd;
    int m_currentPos;
    bool filterEventEnabled;
    QCPItemText * textMultipleSelection;
    QTime _lastUpdate;

    // Méthodes privées
    void zoom(QPoint point);
    void drag(QPoint point);
    void setZoomLine(double x1, double y1, double x2, double y2);
    void plotOverlay();
};

#endif // GRAPHIQUE_H
