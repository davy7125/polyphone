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

#include "graphfilterfrequencies.h"
#include "contextmanager.h"

const int GraphFilterFrequencies::POINT_NUMBER = 201;

GraphFilterFrequencies::GraphFilterFrequencies(QWidget * parent) : QCustomPlot(parent),
    flagEdit(false),
    raideurExp(50.0),
    labelCoord(nullptr),
    previousX(-1)
{
    this->setBackground(this->palette().color(QPalette::Base));

    // Layer for the position of frequencies
    this->addGraph();
    QPen graphPen;
    QColor color = this->palette().color(QPalette::Text);
    color.setAlpha(40);
    graphPen.setColor(color);
    graphPen.setWidth(1);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsLine);
    QVector<double> x, y;
    x.resize(18);
    y.resize(18);
    y[0] = y[3] = y[4] = y[7] = y[8] = y[11] = y[12] = y[15] = y[16] =  2;
    y[1] = y[2] = y[5] = y[6] = y[9] = y[10] = y[13] = y[14] = y[17] = -1;
    color.setAlpha(180);
    for (int i = 0; i < 9; i++)
    {
        int freq = 2000 * (i + 1);
        double pos = (double)(freq * POINT_NUMBER) / 20000.;
        x[2*i] = x[2*i+1] = pos;
        QCPItemText *textLabel = new QCPItemText(this);
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(pos, 0.0001);
        textLabel->setText(QString::number(freq / 1000) + " kHz");
        textLabel->setFont(QFont(font().family(), 8));
        textLabel->setColor(color);
    }
    this->graph(0)->setData(x, y);

    // Layer des valeurs
    this->addGraph();
    graphPen.setColor(ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND));
    graphPen.setWidth(2);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setLineStyle(QCPGraph::lsLine);
    this->graph(1)->setAntialiased(true);

    // Layer aperçu valeurs
    this->addGraph();
    graphPen.setColor(this->palette().color(QPalette::Text));
    graphPen.setWidth(1);
    this->graph(2)->setPen(graphPen);
    this->graph(2)->setScatterStyle(QCPScatterStyle::ssPlus);
    labelCoord = new QCPItemText(this);
    labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    labelCoord->setText("");
    QFont fontLabel = QFont(font().family(), 9);
    fontLabel.setBold(true);
    labelCoord->setFont(fontLabel);
    labelCoord->setColor(this->palette().color(QPalette::Text));

    // Red color for what will be removed
    this->addGraph();
    x.resize(2);
    y.resize(2);
    x[0] = -1;
    x[1] = POINT_NUMBER + 1;
    y[0] = y[1] = 10;
    color = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    color.setAlpha(30);
    this->graph(3)->setData(x, y);
    this->graph(3)->setBrush(QBrush(color));
    this->graph(3)->setChannelFillGraph(this->graph(1));

    // Axes
    this->xAxis->setRange(0, POINT_NUMBER - 1);
    this->yAxis->setRange(0.0001, 1);
    this->yAxis->setScaleType(QCPAxis::stLogarithmic);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Préparation des données
    this->dValues.resize(POINT_NUMBER);
    this->dValues.fill(0.5);

    // Filtre sur les événements
    this->installEventFilter(this);

    // Affichage
    this->replot();
}

GraphFilterFrequencies::~GraphFilterFrequencies() {}

void GraphFilterFrequencies::setNbFourier(int nbFourier)
{
    while (this->graphCount() > 4)
        this->removeGraph(4);
    _nbFourier = nbFourier;
}

void GraphFilterFrequencies::addFourierTransform(QVector<float> fData, quint32 sampleRate)
{
    // Layer Fourier transform
    QVector<double> x, y;
    x.resize(2);
    y.resize(2);
    x[0] = -1;
    x[1] = POINT_NUMBER + 1;
    y[0] = y[1] = -2;
    QColor color = this->palette().color(QPalette::Text);
    color.setAlpha(40 / _nbFourier);
    QPen graphPen;
    graphPen.setColor(color);
    graphPen.setWidth(0);
    color = this->palette().color(QPalette::Highlight);
    color.setAlpha(80 / _nbFourier);

    int nbGraphs = this->graphCount();

    this->addGraph();
    this->graph(nbGraphs)->setPen(graphPen);
    this->graph(nbGraphs)->setData(x, y);
    this->graph(nbGraphs)->setBrush(QBrush(color));
    this->addGraph();
    this->graph(nbGraphs + 1)->setPen(graphPen);
    this->graph(nbGraphs)->setChannelFillGraph(this->graph(nbGraphs + 1));
    this->graph(nbGraphs)->setAntialiased(false);
    this->graph(nbGraphs + 1)->setAntialiased(false);

    // Max fourier
    float max = 0;
    for (int i = 0; i < fData.size(); i++)
        if (fData[i] > max)
            max = fData[i];
    if (max == 0)
        max = 1;

    this->graph(nbGraphs + 1)->data()->clear();
    quint32 sizeToProcess = ((long int)fData.size() * 40000) / sampleRate;
    x.resize(POINT_NUMBER + 1);
    y.resize(POINT_NUMBER + 1);
    for (int i = 0; i < POINT_NUMBER + 1; i++)
    {
        x[i] = i;
        y[i] = 0;
    }
    for (unsigned long i = 0; i < sizeToProcess; i++)
    {
        int index = (int)qRound((double)i/(sizeToProcess - 1) * POINT_NUMBER);
        double value = 0;
        if (i < (unsigned)fData.size())
            value = fData[i] / max; // normalisation entre 0 et 1
        if (value > y[index])
            y[index] = value;
    }
    this->graph(nbGraphs + 1)->setData(x, y);
}

QVector<double> GraphFilterFrequencies::getValues()
{
    return this->dValues;
}

void GraphFilterFrequencies::setValues(QVector<double> val)
{
    for (int i = 0; i < qMin(POINT_NUMBER, val.size()); i++)
        dValues[i] = val.at(i);
    this->replot();
}

// Méthodes privées
void GraphFilterFrequencies::mousePressed(QPoint pos)
{
    this->afficheCoord(-1, -1);
    this->flagEdit = true;

    // Inscription du premier point
    this->write(pos);
}

void GraphFilterFrequencies::mouseReleased(QPoint pos)
{
    if (this->flagEdit)
    {
        this->flagEdit = false;
        this->previousX = -1;

        // Affichage coordonnées
        this->mouseMoved(pos);
    }
}

void GraphFilterFrequencies::mouseMoved(QPoint pos)
{
    if (this->flagEdit)
    {
        this->afficheCoord(-1, -1);
        this->write(pos);
    }
    else
    {
        // Conversion des coordonnées
        double x = this->xAxis->pixelToCoord(pos.x());
        double y = this->yAxis->pixelToCoord(pos.y());

        // Point le plus proche
        double distanceMin = -1;
        int posX = -1;
        for (int i = 0; i < this->dValues.size(); i++)
        {
            double distanceTmp = 0.05 * qAbs(x - i) + qAbs(y - this->dValues[i]);
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                posX = i;
            }
        }
        if (posX != -1)
            this->afficheCoord(posX, this->dValues[posX]);
        else
            this->afficheCoord(-1, -1);
    }
}

void GraphFilterFrequencies::mouseLeft()
{
    this->afficheCoord(-1, -1);
}

void GraphFilterFrequencies::write(QPoint pos)
{
    // Conversion coordonnées
    int x = (int)this->xAxis->pixelToCoord(pos.x());
    double y = this->yAxis->pixelToCoord(pos.y());
    if (x < 0)
        x = 0;
    else if (x > POINT_NUMBER - 1)
        x = POINT_NUMBER - 1;
    if (y < 0)
        y = 0;
    else if (y > 1)
        y = 1;

    // Modification valeur
    if (this->previousX >= 0 && this->previousX != x)
    {
        if (this->previousX < x)
        {
            for (int i = this->previousX + 1; i < x; i++)
                this->dValues[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
        else
        {
            for (int i = this->previousX - 1; i > x; i--)
                this->dValues[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
    }
    this->dValues[x] = y;

    // Mémorisation du point
    this->previousX = x;
    this->previousY = y;

    // Affichage
    this->replot();
}

void GraphFilterFrequencies::replot()
{
    QVector<double> x(POINT_NUMBER);
    for (int i = 0; i < POINT_NUMBER; i++)
        x[i] = i;
    this->graph(1)->setData(x, this->dValues);

    // Affichage
    QCustomPlot::replot();
}

void GraphFilterFrequencies::afficheCoord(double x, double y)
{
    QVector<double> xVector, yVector;
    if (x >= - 0.5)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = x;
        yVector[0] = y;

        // Affichage texte
        if (y >= 0.01)
            labelCoord->setPositionAlignment(Qt::AlignTop| Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        labelCoord->setText(QString::number((int)(qRound(x)) * 20100 / POINT_NUMBER) + "Hz : " +
                            QString::number(y, 'g', 2));

        // Ajustement position
        QFontMetrics fm(labelCoord->font());
        double distX = this->xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 2);
        if (x < distX)
            x = distX;
        else if (x > POINT_NUMBER - distX)
            x = POINT_NUMBER - distX;
        labelCoord->position->setCoords(x, y);
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
