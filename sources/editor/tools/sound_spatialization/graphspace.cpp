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

#include "graphspace.h"
#include "contextmanager.h"

GraphSpace::GraphSpace(QWidget * parent) : QCustomPlot(parent)
{
    this->setBackground(this->palette().color(QPalette::Base));

    // Couche déco
    this->addGraph();
    QPen graphPen;
    graphPen.setWidthF(1);
    QColor color = this->palette().color(QPalette::Text);
    color.setAlpha(40);
    graphPen.setColor(color);
    graphPen.setStyle(Qt::DashDotLine);
    graph(0)->setPen(graphPen);
    QVector<double> x, y;
    x << 0.5 << 0.5;
    y << -10 << 10;
    graph(0)->setData(x, y);

    // Couche données
    this->addGraph();
    graphPen.setColor(this->palette().color(QPalette::Highlight));
    graphPen.setWidthF(2);
    graphPen.setStyle(Qt::SolidLine);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    this->graph(1)->setAntialiased(true);

    // Axes
    this->xAxis->setRange(-0.01, 1.01);
    this->yAxis->setRange(0, 1.05);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Ajout G / D
    QCPItemText * text = new QCPItemText(this);
    text->position->setType(QCPItemPosition::ptAxisRectRatio);
    text->position->setCoords(0, 0);
    text->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    text->setTextAlignment(Qt::AlignLeft);
    text->setFont(QFont(font().family(), 10, 100));
    color.setAlpha(180);
    text->setColor(color);
    text->setText(trUtf8("L", "first letter of Left in your language"));

    text = new QCPItemText(this);
    text->position->setType(QCPItemPosition::ptAxisRectRatio);
    text->position->setCoords(1, 0);
    text->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
    text->setTextAlignment(Qt::AlignRight);
    text->setFont(QFont(font().family(), 10, 100));
    text->setColor(color);
    text->setText(trUtf8("R", "first letter of Right in your language"));

    // Layer aperçu valeurs
    this->addGraph();
    color.setAlpha(255);
    graphPen.setColor(color);
    graphPen.setWidth(1);
    this->graph(2)->setPen(graphPen);
    this->graph(2)->setScatterStyle(QCPScatterStyle::ssPlus);
    labelCoord = new QCPItemText(this);

    labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    labelCoord->setText("");
    QFont fontLabel = QFont(font().family(), 9);
    fontLabel.setBold(true);
    labelCoord->setFont(fontLabel);
    labelCoord->setColor(color);

    // Filtre sur les événements
    this->installEventFilter(this);
}

void GraphSpace::setData(QVector<double> x, QVector<int> y)
{
    _xPan = x;
    _yKey = y;

    // Longueur max tuyau
    _yLength.resize(_yKey.size());
    double freq = qPow(2., (_yKey.first() + 36.3763) / 12);
    double l = 172 / freq; // en mètres pour un tuyau ouvert
    for (int i = 0; i < _yKey.size(); i++)
    {
        // Longueur tuyau
        freq = qPow(2., (_yKey.at(i) + 36.3763) / 12);
        _yLength[i] = 172 / freq / l;
    }

    // Affichage de la courbe
    this->graph(1)->setData(_xPan, _yLength);
    this->replot();
}

void GraphSpace::mouseMoved(QPoint pos)
{
    // Conversion des coordonnées
    double x = xAxis->pixelToCoord(pos.x());
    double y = yAxis->pixelToCoord(pos.y());

    // Point le plus proche
    double distanceMin = -1;
    int posX = -1;
    for (int i = 0; i < _xPan.size(); i++)
    {
        double distanceTmp = 5 * qAbs(x - _xPan[i]) + qAbs(y - _yLength[i]);
        if (distanceMin < -0.5 || distanceTmp < distanceMin)
        {
            distanceMin = distanceTmp;
            posX = i;
        }
    }
    if (posX != -1)
        this->afficheCoord(_xPan[posX], _yLength[posX], _yKey[posX]);
    else
        this->afficheCoord(-1, -1, -1);
}

void GraphSpace::mouseLeft()
{
    this->afficheCoord(-1, -1, -1);
}

void GraphSpace::afficheCoord(double xPan, double yLength, int key)
{
    QVector<double> xVector, yVector;
    if (key != -1)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = xPan;
        yVector[0] = yLength;

        // Affichage texte
        if (yLength >= 0.5)
            labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        labelCoord->setText(ContextManager::keyName()->getKeyName(key) + ":" +
                            QString::number(xPan * 100 - 50, 'f', 1));

        // Ajustement position
        QFontMetrics fm(labelCoord->font());
        double distX = xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 2);
        if (xPan < distX)
            xPan = distX;
        else if (xPan > 1. - distX)
            xPan = 1. - distX;
        double distY = 1.05 - this->yAxis->pixelToCoord(2);
        if (yLength >= 0.5)
            yLength -= distY;
        else
            yLength += distY;
        labelCoord->position->setCoords(xPan, yLength);
    }
    else
    {
        xVector.resize(0);
        yVector.resize(0);
        labelCoord->setText("");
    }
    this->graph(2)->setData(xVector, yVector);
    this->replot();
}
