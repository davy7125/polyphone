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

#include "graphique.h"
#include "config.h"
#include "sf2_types.h"
#include <QScrollBar>
#include <QSpinBox>

Graphique::Graphique(QWidget * parent) : QCustomPlot(parent),
    zoomFlag(false),
    dragFlag(false),
    zoomX(1),
    zoomY(1),
    posX(.5),
    posY(.5),
    zoomXinit(1),
    zoomYinit(1),
    posXinit(.5),
    posYinit(.5),
    bFromExt(false),
    qScrollX(NULL),
    spinStart(NULL),
    spinEnd(NULL),
    filterEventEnabled(true),
    textMultipleSelection(NULL)
{
    // Graphe des données
    this->addGraph();
    this->graph(0)->setAntialiased(false);

    // Graphes contenant startLoop, endLoop
    this->addGraph();
    this->addGraph();

    // Graphe pour la ligne de zoom
    this->addGraph();

    // Graphes contenant l'overlay
    this->addGraph();
    this->addGraph();

    // Axes
    this->xAxis2->setRange(0, 1);
    this->yAxis2->setRange(0, 1);
    this->yAxis->setRange(-1, 1);
    this->xAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Message "sélection multiple"
    textMultipleSelection = new QCPItemText(this);
    textMultipleSelection->position->setType(QCPItemPosition::ptAxisRectRatio);
    textMultipleSelection->setPositionAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    textMultipleSelection->position->setCoords(.5, .5);
    textMultipleSelection->setTextAlignment(Qt::AlignHCenter);
    textMultipleSelection->setFont(QFont(font().family(), 16, QFont::Bold));
    textMultipleSelection->setText(trUtf8("Sélection multiple"));
    addItem(textMultipleSelection);

    // Paramétrage des couleurs
    this->updateStyle();

    // Start update timer
    _lastUpdate.start();
}

// Méthodes publiques
void Graphique::updateStyle()
{
    QList<QColor> colors = Config::getInstance()->getColors();
    if (colors.size() != 6)
        return;
    // Couleur de fond
    this->setBackground(colors.at(0));
    QPen graphPen;
    graphPen.setColor(colors.at(0));
    this->yAxis->setBasePen(graphPen);
    // Couleur de l'onde
    graphPen.setColor(colors.at(1));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);
    // Couleur début de boucle
    graphPen.setColor(colors.at(3));
    graphPen.setWidthF(2);
    this->graph(1)->setPen(graphPen);
    // Couleur fin de boucle
    graphPen.setColor(colors.at(4));
    graphPen.setWidthF(2);
    this->graph(2)->setPen(graphPen);
    // Ligne de zoom
    graphPen.setColor(colors.at(5));
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DashLine);
    this->graph(3)->setPen(graphPen);
    // Overlay
    QColor colorTmp = colors.at(3);
    colorTmp.setAlpha(175);
    graphPen.setColor(colorTmp);
    graphPen.setWidth(2);
    graphPen.setStyle(Qt::DotLine);
    this->graph(4)->setPen(graphPen);
    colorTmp = colors.at(4);
    colorTmp.setAlpha(175);
    graphPen.setColor(colorTmp);
    this->graph(5)->setPen(graphPen);
    // Couleur de la grille
    graphPen.setColor(colors.at(2));
    graphPen.setWidthF(1);
    this->yAxis->grid()->setZeroLinePen(graphPen);
    graphPen.setStyle(Qt::DotLine);
    this->yAxis->grid()->setPen(graphPen);
    // Curseur de lecture et message (sélection multiple)
    penLecture.setColor(colors.at(5));
    penLecture.setWidthF(1);
    colorTmp = colors.at(5);
    colorTmp.setAlpha(180);
    textMultipleSelection->setColor(colorTmp);

    this->replot();
}

void Graphique::clearAll()
{
    this->graph(0)->clearData();
    this->graph(1)->clearData();
    this->graph(2)->clearData();
    this->graph(3)->clearData();
    this->graph(4)->clearData();
    this->graph(5)->clearData();
}
void Graphique::setData(QByteArray baData)
{
    // Ajout des données
    quint32 size_x = baData.size() / 2;
    this->sizeX = ((double)size_x + 1);
    qint16 * data = (qint16 *)baData.data();
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i = 0; i < size_x; i++)
    {
        x[i] = i;
        y[i] = (double)data[i] / 32768.; // normalisation entre -1 et 1
    }
    this->graph(0)->setData(x, y);
}
void Graphique::linkSliderX(QScrollBar * qScrollX)
{
    this->qScrollX = qScrollX;
}
void Graphique::linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd)
{
    this->spinStart = spinStart;
    this->spinEnd = spinEnd;
}
void Graphique::setPosX(int posX)
{
    if (this->qScrollX)
    {
        bFromExt = true;
        if (this->qScrollX->maximum() > 0)
            this->posX = (double)posX / this->qScrollX->maximum();
        else
            this->posX = 0.5;
        this->zoomDrag();
        bFromExt = false;
    }
}
void Graphique::setStartLoop(int pos, bool replot)
{
    if (filterEventEnabled)
    {
        if (pos >= 0)
        {
            QVector<double> x(2), y(2);
            x[0] = pos;
            x[1] = pos;
            y[0] = -1;
            y[1] = 1;
            this->graph(1)->setData(x, y);
        }
        this->plotOverlay();
        if (replot)
            this->replot();
    }
}
void Graphique::setEndLoop(int pos, bool replot)
{
    if (filterEventEnabled)
    {
        if (pos >= 0)
        {
            QVector<double> x(2), y(2);
            x[0] = pos;
            x[1] = pos;
            y[0] = -1;
            y[1] = 1;
            this->graph(2)->setData(x, y);
        }
        this->plotOverlay();
        if (replot)
            this->replot();
    }
}
void Graphique::setCurrentSample(int pos)
{
    if (_lastUpdate.elapsed() < 16)
        return;
    _lastUpdate.restart();

    m_currentPos = this->xAxis->coordToPixel(pos);
    this->repaint();
    this->update();
}
void Graphique::paintEvent(QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);

    // Ajout du trait de lecture
    if (m_currentPos > 0)
    {
        QPainter painter(this);
        painter.setPen(penLecture);
        painter.drawLine(QPointF(m_currentPos, -10000), QPointF(m_currentPos, 10000));
    }
}
void Graphique::zoomDrag()
{
    // Bornes des paramètres d'affichage
    if (this->zoomX < 1)
        this->zoomX = 1;
    else if (this->zoomX > sizeX * 200)
        this->zoomX = sizeX * 200;
    if (this->zoomY < 1)
        this->zoomY = 1;
    else if (this->zoomY > 50)
        this->zoomY = 50;
    if (this->posX < 0)
        this->posX = 0;
    else if (this->posX > 1)
        this->posX = 1;
    if (this->posY < 0)
        this->posY = 0;
    else if (this->posY > 1)
        this->posY = 1;
    posY = 0.5; // blocage sur Y

    // Application du drag et zoom
    double etendueX = sizeX / zoomX;
    double offsetX = (sizeX - etendueX) * posX - 1;
    this->xAxis->setRange(offsetX, offsetX + etendueX);
    double etendueY = 2. / zoomY;
    double offsetY = (2. - etendueY) * posY - 1;
    this->yAxis->setRange(offsetY, offsetY + etendueY);
    // Mise à jour
    this->replot();
    if (!bFromExt && qScrollX)
    {
        // Mise à jour du scrollbar
        double valMax = ((zoomX - 1.) * sizeX) / 10000;
        qScrollX->blockSignals(true);
        qScrollX->setRange(0, valMax);
        qScrollX->setValue(valMax*posX);
        qScrollX->blockSignals(false);
    }
}
void Graphique::displayMultipleSelection(bool isOn)
{
    qScrollX->setEnabled(!isOn);
    filterEventEnabled = !isOn;
    textMultipleSelection->setVisible(isOn);
    if (isOn)
        zoomDrag();
    else
        qScrollX->setRange(0, 0);
}

// Méthodes privées
void Graphique::zoom(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->yInit;

    // Modification zoom & drag
    this->zoomX = zoomXinit * pow(2, 25.0 * decX);
    this->zoomY = zoomYinit * pow(2,  5.0 * decY);

    // Ajustement posX et posY
    if (zoomX > 1)
        posX = (zoomX * posXinit * (zoomXinit - 1) + xInit*(zoomX - zoomXinit)) / (zoomXinit*(zoomX - 1));
    if (zoomY > 1)
        posY = (zoomY * posYinit * (zoomYinit - 1) + yInit*(zoomY - zoomYinit)) / (zoomYinit*(zoomY - 1));

    // Mise à jour
    this->zoomDrag();
}
void Graphique::drag(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->yInit;

    // Modification posX et posY
    if (zoomXinit > 1)
        posX = posXinit - decX / (zoomXinit - 1);
    if (zoomYinit > 1)
        posY = posYinit - decY / (zoomYinit - 1);

    // Mise à jour
    this->zoomDrag();
}
void Graphique::setZoomLine(double x1, double y1, double x2, double y2)
{
    if (x1 >= 0)
    {
        // Conversion
        QVector<double> x(2), y(2);
        x[0] = sizeX * (x1 + posX * (zoomX - 1)) / zoomX - 1;
        x[1] = sizeX * (x2 + posX * (zoomX - 1)) / zoomX - 1;
        y[0] = 2 * (y1 + posY * (zoomY - 1)) / zoomY - 1;
        y[1] = 2 * (y2 + posY * (zoomY - 1)) / zoomY - 1;
        this->graph(3)->setData(x, y);
    }
    else
        this->graph(3)->clearData();
    this->replot();
}
void Graphique::plotOverlay()
{
    int sizeOverlay = 20;
    qint32 posDebut = this->spinStart->value();
    qint32 posFin = this->spinEnd->value();
    if (posDebut >= 0 && posFin >= 0 && posDebut != posFin)
    {
        QVector<double> x1(2 * sizeOverlay), y1(2 * sizeOverlay), x2(2 * sizeOverlay), y2(2 * sizeOverlay);
        for (int i = 0; i < 2 * sizeOverlay; i++)
        {
            // Partie droite recopiée à gauche
            x1[i]                       = posDebut - sizeOverlay + i;
            x1[2 * sizeOverlay - i - 1] = posDebut + sizeOverlay - i;
            y1[i]                       = this->graph(0)->data()->value(posFin - sizeOverlay + i).value;
            y1[2 * sizeOverlay - i - 1] = this->graph(0)->data()->value(posFin + sizeOverlay - i).value;
            // Partie gauche recopiée à droite
            x2[i]                       = posFin - sizeOverlay + i;
            x2[2 * sizeOverlay - i - 1] = posFin + sizeOverlay - i;
            y2[i]                       = this->graph(0)->data()->value(posDebut - sizeOverlay + i).value;
            y2[2 * sizeOverlay - i - 1] = this->graph(0)->data()->value(posDebut + sizeOverlay - i).value;
        }
        this->graph(4)->setData(x1, y1);
        this->graph(5)->setData(x2, y2);
    }
    else
    {
        this->graph(4)->clearData();
        this->graph(5)->clearData();
    }
}

void Graphique::mousePressEvent(QMouseEvent *event)
{
    if (!filterEventEnabled)
        return;

    // Enregistrement situation
    xInit = this->xAxis2->pixelToCoord(event->x());
    yInit = this->yAxis2->pixelToCoord(event->y());
    zoomXinit = zoomX;
    zoomYinit = zoomY;
    posXinit = posX;
    posYinit = posY;
    modifiedFlag = false;
    if (event->button() == Qt::LeftButton && !zoomFlag)
        dragFlag = true;
    else if (event->button() == Qt::RightButton && !dragFlag)
        zoomFlag = true;
}

void Graphique::mouseReleaseEvent(QMouseEvent *event)
{
    if (!filterEventEnabled)
        return;

    int val = qRound(sizeX / zoomX * ((zoomX - 1) * posX + xInit) - 1);
    if (val < 0)
        val = 0;
    if (event->button() == Qt::LeftButton)
    {
        dragFlag = false;
        if (!modifiedFlag)
        {
            // Modification start loop
            if (this->spinStart && this->spinEnd)
            {
                if (this->spinEnd->value() > val)
                {
                    this->spinEnd->setMinimum(val);
                    this->spinStart->setValue(val);
                    emit(startLoopChanged());
                }
            }
            else
                this->setStartLoop(val);
        }
        else
            this->setCursor(Qt::ArrowCursor);
    }
    else if (event->button() == Qt::RightButton)
    {
        zoomFlag = false;
        if (!modifiedFlag)
        {
            // Modification end loop
            if (this->spinStart && this->spinEnd)
            {
                if (this->spinStart->value() < val)
                {
                    this->spinStart->setMaximum(val);
                    this->spinEnd->setValue(val);
                    emit(endLoopChanged());
                }
            }
            else
                this->setEndLoop(val);
        }
        else
        {
            this->setZoomLine(-1, 0, 0, 0);
            this->setCursor(Qt::ArrowCursor);
        }
    }
}

void Graphique::mouseMoveEvent(QMouseEvent *event)
{
    if (!filterEventEnabled)
        return;

    if (zoomFlag)
    {
        if (!modifiedFlag)
        {
            modifiedFlag = true;
            this->setCursor(Qt::SizeAllCursor);
        }
        this->zoom(event->pos());

        // Ligne de zoom
        this->setZoomLine(xInit, yInit,
                          this->xAxis2->pixelToCoord(event->x()),
                          this->yAxis2->pixelToCoord(event->y()));
    }
    else if (dragFlag)
    {
        if (!modifiedFlag)
        {
            modifiedFlag = true;
            this->setCursor(Qt::ClosedHandCursor);
        }
        this->drag(event->pos());
    }
}

void Graphique::wheelEvent(QWheelEvent *event)
{
    if (!dragFlag && !zoomFlag)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qScrollX->setValue(qScrollX->value() - 0.05 * event->angleDelta().x());
#else
        qScrollX->setValue(qScrollX->value() - 0.05 * event->delta());
#endif
}
