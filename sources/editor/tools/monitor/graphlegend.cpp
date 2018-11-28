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
