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

#include "graphlegend.h"

GraphLegend::GraphLegend(QWidget *parent) : QCustomPlot(parent)
{
    // Couche pour le symbole
    this->addGraph();

    // Ajout d'un point
    QVector<double> x, y;
    x << 0;
    y << 0;
    this->graph(0)->setData(x, y);

    // Axes
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->xAxis->setRange(-1, 1);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    this->yAxis->setRange(-1, 1);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Couleur de fond
    this->setBackground(parent->palette().background().color());
}

void GraphLegend::plot(QCPScatterStyle::ScatterShape style, QColor couleur, int size, int epaisseur, bool antiAliased)
{
    this->graph(0)->setScatterStyle(QCPScatterStyle(style, size));
    this->graph(0)->setAntialiasedScatters(antiAliased);
    QPen pen;
    pen.setColor(couleur);
    pen.setWidth(epaisseur);
    this->graph(0)->setPen(pen);

    this->replot();
}
