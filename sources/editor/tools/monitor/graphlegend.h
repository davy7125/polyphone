#ifndef GRAPHLEGEND_H
#define GRAPHLEGEND_H

#include "qcustomplot.h"

class GraphLegend : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphLegend(QWidget *parent = 0);
    void plot(QCPScatterStyle::ScatterShape style, QColor couleur, int size, int epaisseur, bool antiAliased);
};
#endif // GRAPHLEGEND_H
