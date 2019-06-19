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

#include "graphvisualizer.h"
#include "contextmanager.h"

const double GraphVisualizer::MIN_LOG = 0.0001;

GraphVisualizer::GraphVisualizer(QWidget *parent) :
    QCustomPlot(parent),
    labelCoord(nullptr)
{
    this->setBackground(this->palette().color(QPalette::Base));

    // Layer pour la position des octaves
    this->addGraph();
    QPen graphPen;
    QColor color = this->palette().color(QPalette::Text);
    color.setAlpha(40);
    graphPen.setColor(color);
    graphPen.setWidth(1);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsLine);
    color.setAlpha(180);
    for (int i = 0; i < 10; i++)
    {
        int note = 12 * (i + 1);
        QCPItemText *textLabel = new QCPItemText(this);
        listTextOctave << textLabel;
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(note, 0);
        textLabel->setText(ContextManager::keyName()->getKeyName(note));
        textLabel->setFont(QFont(font().family(), 8));
        textLabel->setColor(color);
    }

    // Layer des moyennes
    this->addGraph();
    graphPen.setColor(ContextManager::theme()->getFixedColor(ThemeManager::GREEN, ThemeManager::LIST_BACKGROUND));
    graphPen.setWidth(2);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setLineStyle(QCPGraph::lsNone);
    this->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    this->graph(1)->setAntialiasedScatters(true);

    // Layer des valeurs
    this->addGraph();
    graphPen.setColor(this->palette().color(QPalette::Highlight));
    graphPen.setWidth(2);
    this->graph(2)->setPen(graphPen);
    this->graph(2)->setLineStyle(QCPGraph::lsNone);
    this->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->graph(2)->setAntialiasedScatters(false);

    // Layer des valeurs par défaut
    this->addGraph();
    graphPen.setColor(this->palette().color(QPalette::Highlight));
    graphPen.setWidth(1);
    this->graph(3)->setPen(graphPen);
    this->graph(3)->setLineStyle(QCPGraph::lsNone);
    this->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->graph(3)->setAntialiasedScatters(false);

    // Layer aperçu valeurs
    this->addGraph();
    color.setAlpha(255);
    graphPen.setColor(color);
    graphPen.setWidth(1);
    this->graph(4)->setPen(graphPen);
    this->graph(4)->setScatterStyle(QCPScatterStyle::ssPlus);
    labelCoord = new QCPItemText(this);
    labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    labelCoord->setText("");
    QFont fontLabel = QFont(font().family(), 9);
    fontLabel.setBold(true);
    labelCoord->setFont(fontLabel);
    labelCoord->setColor(color);

    // Message warning
    textWarning = new QCPItemText(this);
    textWarning->setPositionAlignment(Qt::AlignTop | Qt::AlignLeft);
    textWarning->position->setType(QCPItemPosition::ptPlotCoords);
    textWarning->position->setCoords(0, 0);
    textWarning->setFont(QFont(font().family(), 10, 100));
    textWarning->setColor(ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND));

    // Axes
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Filtre sur les événements
    this->installEventFilter(this);
}

GraphVisualizer::~GraphVisualizer()
{
}

void GraphVisualizer::setData(QVector<QList<double> > listPoints, QVector<QList<double> > listPointsDef)
{
    listX.clear();
    listY.clear();

    // Mise en forme des données, calcul de la moyenne
    QVector<double> vectValX, vectValY, vectDefValX, vectDefValY, vectMoyX, vectMoyY;
    int count;
    double somme;
    for (int i = 0; i < listPoints.size(); i++)
    {
        count = 0;
        somme = 0;
        for (int j = 0; j < listPoints.at(i).size(); j++)
        {
            count++;
            somme += listPoints.at(i).at(j);
            vectValX << i;
            vectValY << listPoints.at(i).at(j);
            listX << i;
            listY << listPoints.at(i).at(j);
        }
        for (int j = 0; j < listPointsDef.at(i).size(); j++)
        {
            count++;
            somme += listPointsDef.at(i).at(j);
            vectDefValX << i;
            vectDefValY << listPointsDef.at(i).at(j);
            listX << i;
            listY << listPointsDef.at(i).at(j);
        }
        if (count)
        {
            vectMoyX << i;
            vectMoyY << somme / count;
            listX << i;
            listY << somme / count;
        }
    }

    // Calcul des bornes
    if (listY.size())
    {
        xMinDonnees = listX.first();
        xMaxDonnees = listX.first();
        yMinDonnees = listY.first();
        yMaxDonnees = listY.first();
        yMin2Donnees = listY.first();
        for (int i = 1; i < listY.size(); i++)
        {
            xMinDonnees = qMin(xMinDonnees, listX.at(i));
            xMaxDonnees = qMax(xMaxDonnees, listX.at(i));
            yMinDonnees = qMin(yMinDonnees, listY.at(i));
            yMaxDonnees = qMax(yMaxDonnees, listY.at(i));
            if (yMin2Donnees < MIN_LOG)
                yMin2Donnees = listY.at(i);
            else
                if (listY.at(i) >= MIN_LOG && listY.at(i) < yMin2Donnees)
                    yMin2Donnees = listY.at(i);
        }
    }
    else
    {
        xMinDonnees = 0;
        xMaxDonnees = 127;
        yMinDonnees = 0;
        yMaxDonnees = 1;
    }
    if (yMin2Donnees < MIN_LOG)
        yMin2Donnees = yMaxDonnees;

    // Bornes sur x
    this->xAxis->setRange(xMinDonnees - 1, xMaxDonnees + 1);

    // Affichage
    this->graph(1)->setData(vectMoyX, vectMoyY);
    this->graph(2)->setData(vectValX, vectValY);
    this->graph(3)->setData(vectDefValX, vectDefValY);
    this->setScale();
}

void GraphVisualizer::setScale()
{
    double minY;
    double maxY = this->yMaxDonnees;

    if (isLog)
    {
        // Axe y
        this->yAxis->setScaleType(QCPAxis::stLogarithmic);

        // Bornes sur y
        minY = qMax(this->yMin2Donnees, MIN_LOG);
        maxY = qMax(maxY, MIN_LOG);
        if (minY == maxY)
        {
            minY /= 2;
            maxY *= 2;
        }
        else
        {
            double delta = maxY / minY;
            minY /= qPow(delta, 0.1);
            maxY *= qPow(delta, 0.05);
        }

        if (this->yMinDonnees < minY)
        {
            // Affichage avertissement
            textWarning->setText(trUtf8("Cannot display all the values."));
            textWarning->position->setCoords(this->xAxis->range().lower, maxY);
        }
        else
            textWarning->setText("");
    }
    else
    {
        // Axe y
        this->yAxis->setScaleType(QCPAxis::stLinear);

        // Bornes sur y
        minY = this->yMinDonnees;
        if (minY == maxY)
        {
            minY -= 1;
            maxY += 1;
        }
        else
        {
            double delta = maxY - minY;
            minY -= delta * 0.1;
            maxY += delta * 0.05;
        }
        textWarning->setText("");
    }
    this->yAxis->setRange(minY, maxY);

    // Octaves
    QPointF pointTmp;
    for (int i = 0; i < listTextOctave.size(); i++)
    {
        pointTmp = listTextOctave.at(i)->position->coords();
        pointTmp.setY(minY);
        listTextOctave[i]->position->setCoords(pointTmp);
    }
    QVector<double> x, y;
    x.resize(20);
    y.resize(20);
    y[0] = y[3] = y[4] = y[7] = y[8] = y[11] = y[12] = y[15] = y[16] = y[19] = maxY;
    y[1] = y[2] = y[5] = y[6] = y[9] = y[10] = y[13] = y[14] = y[17] = y[18] = minY;
    for (int i = 0; i < 10; i++)
        x[2*i] = x[2*i+1] = 12 * (i + 1);
    this->graph(0)->setData(x, y);

    this->replot();
}

void GraphVisualizer::setIsLog(bool isLog)
{
    this->isLog = isLog;
}

void GraphVisualizer::mouseMoved(QPoint pos)
{
    int posX = pos.x();
    int posY = pos.y();
    // Point le plus proche
    double distanceMin = -1;
    int index = -1;
    double distanceTmp;
    double minY = yAxis->range().lower;
    for (int i = 0; i < this->listX.size(); i++)
    {
        if (listY.at(i) >= minY)
        {
            distanceTmp = 10 * qAbs(posX - xAxis->coordToPixel(listX.at(i))) + qAbs(posY - yAxis->coordToPixel(listY.at(i)));
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                index = i;
            }
        }
    }
    if (index != -1)
        this->afficheCoord(this->listX.at(index), this->listY.at(index));
    else
        this->afficheCoord(-1, -1);
}

void GraphVisualizer::mouseLeft()
{
    this->afficheCoord(-1, -1);
}

void GraphVisualizer::afficheCoord(double x, double y)
{
    QVector<double> xVector, yVector;
    if (x >= - 0.5)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = x;
        yVector[0] = y;

        // Limites du graphique
        double xMin = this->xAxis->range().lower;
        double xMax = this->xAxis->range().upper;
        double yMin = this->yAxis->range().lower;
        double yMax = this->yAxis->range().upper;

        // Affichage texte
        if (y > (yMin + yMax) / 2)
            labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        QString label = ContextManager::keyName()->getKeyName(qRound(x)) + ":";
        if (y == 0)
            label += QString::number(y, 'f', 0);
        else if (y < 1)
            label += QString::number(y, 'f', 3);
        else if (y < 10)
            label += QString::number(y, 'f', 2);
        else if (y < 100)
            label += QString::number(y, 'f', 1);
        else
            label += QString::number(y, 'f', 0);
        labelCoord->setText(label);

        // Adjust the position on x
        QFontMetrics fm(labelCoord->font());
        double distX = this->xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 4) - xMin;
        if (x < xMin + distX)
            x = xMin + distX;
        else if (x > xMax - distX)
            x = xMax - distX;

        // Ajustement position sur y
        int pixelY = this->yAxis->coordToPixel(y);
        if (y > (yMin + yMax) / 2)
            pixelY += 3;
        else
            pixelY -= 3;
        y = yAxis->pixelToCoord(pixelY);

        // Positionnement du texte
        labelCoord->position->setCoords(x, y);
    }
    else
    {
        xVector.resize(0);
        yVector.resize(0);
        labelCoord->setText("");
    }
    this->graph(4)->setData(xVector, yVector);
    this->replot();
}
