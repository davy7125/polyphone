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

#include "graphicsviewenvelop.h"
#include "contextmanager.h"
#include <QResizeEvent>
#include <QScrollBar>

GraphicsViewEnvelop::GraphicsViewEnvelop(QWidget *parent) : QCustomPlot(parent),
    _dontRememberScroll(false),
    _zoomFlag(false),
    _dragFlag(false),
    _zoomX(1),
    _posX(.5),
    _zoomXinit(1),
    _posXinit(.5),
    _bFromExt(false),
    _qScrollX(nullptr),
    _textPositionL(nullptr),
    _textPositionR(nullptr),
    _fixedTicker(new QCPAxisTickerFixed())
{
    // Images
    _imageNoteOn = QImage(":/icons/note_on.png").scaled(36, 36, Qt::KeepAspectRatio);
    _imageNoteOff = QImage(":/icons/note_off.png").scaled(36, 36, Qt::KeepAspectRatio);

    // Graphe 0 pour la ligne de zoom
    this->addGraph();

    // Graphe 1 pour le sample
    this->addGraph();

    // Axes
    this->xAxis2->setRange(0, 1);
    this->yAxis2->setRange(0, 1);
    this->yAxis->setRange(0, 1.2);
    this->yAxis->setVisible(false);
    _fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    this->xAxis->setTicker(_fixedTicker);
    this->xAxis->setTickLength(0, 0);
    this->xAxis->setSubTickLength(0, 0);
    this->xAxis->setTickPen(QPen(QColor(255, 0, 255), 0));
    this->xAxis->setTickLabelSide(QCPAxis::lsInside);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Positions
    _textPositionL = new QCPItemText(this);
    _textPositionL->position->setType(QCPItemPosition::ptAxisRectRatio);
    _textPositionL->setPositionAlignment(Qt::AlignLeft|Qt::AlignTop);
    _textPositionL->position->setCoords(0, 0);
    _textPositionL->setTextAlignment(Qt::AlignBottom);
    _textPositionL->setFont(QFont(font().family(), 8, QFont::Bold));
    _textPositionL->setText("");

    _textPositionR = new QCPItemText(this);
    _textPositionR->position->setType(QCPItemPosition::ptAxisRectRatio);
    _textPositionR->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    _textPositionR->position->setCoords(1, 0);
    _textPositionR->setTextAlignment(Qt::AlignBottom);
    _textPositionR->setFont(QFont(font().family(), 8, QFont::Bold));
    _textPositionR->setText("");

    this->updateStyle();
}

GraphicsViewEnvelop::~GraphicsViewEnvelop()
{
    clearEnvelops();
    _fixedTicker.clear();
}

void GraphicsViewEnvelop::updateStyle()
{
    // Colors
    QColor backgroundColor;
    QColor textColor;
    QColor redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, true);
    QColor highlightColor = this->palette().color(QPalette::Highlight);
    if (ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        backgroundColor = this->palette().color(QPalette::Base);
        textColor = this->palette().color(QPalette::Text);
    }
    else
    {
        backgroundColor = this->palette().color(QPalette::Text);
        textColor = this->palette().color(QPalette::Base);
    }
    this->setBackground(backgroundColor);

    // Zoom line
    QPen graphPen;
    graphPen.setColor(redColor);
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DashLine);
    this->graph(0)->setPen(graphPen);

    // Sample
    highlightColor.setAlpha(40);
    graphPen.setColor(highlightColor);
    graphPen.setStyle(Qt::SolidLine);
    this->graph(1)->setPen(graphPen);

    // Text
    textColor.setAlpha(180);
    _textPositionL->setColor(textColor);
    _textPositionR->setColor(textColor);

    // Style of the release line
    _releaseLinePen = QPen(textColor, 2);
    _releaseLinePen.setStyle(Qt::DashLine);

    // Grid
    textColor.setAlpha(0);
    graphPen.setColor(textColor);
    this->xAxis->grid()->setZeroLinePen(graphPen);
    textColor.setAlpha(70);
    graphPen.setColor(textColor);
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DotLine);
    this->xAxis->grid()->setPen(graphPen);
    textColor.setAlpha(255);
    this->xAxis->setTickLabelColor(textColor);

    this->replot();
}

void GraphicsViewEnvelop::linkSliderX(QScrollBar * qScrollX)
{
    _qScrollX = qScrollX;
}

void GraphicsViewEnvelop::setPosX(int posX)
{
    if (this->_qScrollX)
    {
        _bFromExt = true;
        if (this->_qScrollX->maximum() > 0)
            this->_posX = (double)posX / this->_qScrollX->maximum();
        else
            this->_posX = 0.5;
        this->zoomDrag();
        _bFromExt = false;
    }
}

void GraphicsViewEnvelop::zoomDrag()
{
    // Bornes des paramètres d'affichage
    if (this->_posX < 0)
        this->_posX = 0;
    else if (this->_posX > 1)
        this->_posX = 1;

    // Application du drag et zoom
    double etendueX = _sizeX / _zoomX;
    double offsetX = (_sizeX - etendueX) * _posX;
    this->xAxis->setRange(offsetX, offsetX + etendueX);

    // Mise à jour
    displayCurrentRange();
    this->replot(QCustomPlot::rpQueuedReplot);
    if (!_bFromExt && _qScrollX)
    {
        // Mise à jour du scrollbar
        _qScrollX->blockSignals(true);
        _qScrollX->setPageStep(10000. / _zoomX);
        _qScrollX->setRange(0, 10000. - _qScrollX->pageStep());
        _qScrollX->setValue(_qScrollX->maximum() * _posX);
        _qScrollX->blockSignals(false);
    }

    // Position of the release line in pixel
    _posReleaseLine = this->xAxis->coordToPixel(_triggeredKeyDuration);
}

double GraphicsViewEnvelop::getTickStep()
{
    double widthPerUnit = (double)this->width() * _zoomX / _sizeX;
    double tickStep = 0.0001;
    if (widthPerUnit < 1)
        tickStep = 200;
    else if (widthPerUnit < 2)
        tickStep = 100;
    else if (widthPerUnit < 4)
        tickStep = 50;
    else if (widthPerUnit < 10)
        tickStep = 20;
    else if (widthPerUnit < 20)
        tickStep = 10;
    else if (widthPerUnit < 40)
        tickStep = 5;
    else if (widthPerUnit < 100)
        tickStep = 2;
    else if (widthPerUnit < 200)
        tickStep = 1;
    else if (widthPerUnit < 400)
        tickStep = 0.5;
    else if (widthPerUnit < 1000)
        tickStep = 0.2;
    else if (widthPerUnit < 2000)
        tickStep = 0.1;
    else if (widthPerUnit < 4000)
        tickStep = 0.05;
    else if (widthPerUnit < 10000)
        tickStep = 0.02;
    else if (widthPerUnit < 20000)
        tickStep = 0.01;
    else if (widthPerUnit < 40000)
        tickStep = 0.005;
    else if (widthPerUnit < 100000)
        tickStep = 0.002;
    else if (widthPerUnit < 200000)
        tickStep = 0.001;
    else if (widthPerUnit < 400000)
        tickStep = 0.0005;
    else if (widthPerUnit < 1000000)
        tickStep = 0.0002;
    return tickStep;
}

void GraphicsViewEnvelop::clearEnvelops()
{
    // Delete all envelops
    while (!_envelops.isEmpty()) {
        Envelop * envelop = _envelops.take(_envelops.keys().first());
        this->removeGraph(envelop->graph1());
        this->removeGraph(envelop->graph2());
        delete envelop;
    }
    this->graph(0)->data()->clear();
    this->graph(1)->data()->clear();
    _index = 0;
}

int GraphicsViewEnvelop::addEnvelop()
{
    _index++;
    _envelops[_index] = new Envelop(this->addGraph(), this->addGraph());
    return _index;
}

void GraphicsViewEnvelop::setValue(int index, Envelop::ValueType type, double value, bool isDefined)
{
    _envelops[index]->set(type, value, isDefined);
}

void GraphicsViewEnvelop::setKeyRange(int index, int keyMin, int keyMax)
{
    _envelops[index]->set(Envelop::KEY_MIN, keyMin, true);
    _envelops[index]->set(Envelop::KEY_MAX, keyMax, true);
}

void GraphicsViewEnvelop::setSample(QVector<double> data, int sampleRate, int loopMode, int startLoop, int endLoop)
{
    // Number of points to display
    int size = _sizeX * sampleRate;
    QVector<double> dataX(size), dataY(size);

    // Compute the multiplier
    double max = 0;
    for (int i = 0; i < data.size(); i++)
        max = qMax(max, data[i]);
    double multiplier = (max == 0) ? 1 : (0.7 / max);

    // Key on part
    bool release = false;
    int currentSmplPos = 0;
    int nbRead = _triggeredKeyDuration * sampleRate;

    if ((loopMode == 1 || loopMode == 2 || (loopMode == 3 && !release)) && startLoop != endLoop)
    {
        // Boucle
        if (currentSmplPos >= endLoop)
            currentSmplPos = startLoop;
        int total = 0;
        while (nbRead - total > 0)
        {
            const int chunk = qMin(endLoop - currentSmplPos, nbRead - total);
            for (int i = 0; i < chunk; i++)
            {
                dataX[total + i] = (double)(total + i) / sampleRate;
                dataY[total + i] = data[currentSmplPos + i] * multiplier;
            }
            currentSmplPos += chunk;
            if (currentSmplPos >= endLoop)
                currentSmplPos = startLoop;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (data.size() - currentSmplPos < nbRead)
        {
            for (int i = 0; i < data.size() - currentSmplPos; i++)
            {
                dataX[i] = (double)i / sampleRate;
                dataY[i] = data[currentSmplPos + i] * multiplier;
            }
            for (int i = data.size() - currentSmplPos; i < nbRead; i++)
            {
                dataX[i] = (double)i / sampleRate;
                dataY[i] = 0;
            }
            currentSmplPos = data.size();
        }
        else
        {
            for (int i = 0; i < nbRead; i++)
            {
                dataX[i] = (double)i / sampleRate;
                dataY[i] = data[currentSmplPos + i] * multiplier;
            }
            currentSmplPos += nbRead;
        }
    }

    // Key off part
    release = true;
    int offset = nbRead;
    nbRead = _releasedKeyDuration * sampleRate;

    if ((loopMode == 1 || loopMode == 2 || (loopMode == 3 && !release)) && startLoop != endLoop)
    {
        // Boucle
        if (currentSmplPos >= endLoop)
            currentSmplPos = startLoop;
        int total = 0;
        while (nbRead - total > 0)
        {
            const int chunk = qMin(endLoop - currentSmplPos, nbRead - total);
            for (int i = 0; i < chunk; i++)
            {
                dataX[offset + total + i] = (double)(total + offset + i) / sampleRate;
                dataY[offset + total + i] = data[currentSmplPos + i] * multiplier;
            }
            currentSmplPos += chunk;
            if (currentSmplPos >= endLoop)
                currentSmplPos = startLoop;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (data.size() - currentSmplPos < nbRead)
        {
            for (int i = 0; i < data.size() - currentSmplPos; i++)
            {
                dataX[offset + i] = (double)(offset + i) / sampleRate;
                dataY[offset + i] = data[currentSmplPos + i] * multiplier;
            }
            for (int i = data.size() - currentSmplPos; i < nbRead; i++)
            {
                dataX[offset + i] = (double)(offset + i) / sampleRate;
                dataY[offset + i] = 0;
            }
            currentSmplPos = data.size();
        }
        else
        {
            for (int i = 0; i < nbRead; i++)
            {
                dataX[offset + i] = (double)(offset + i) / sampleRate;
                dataY[offset + i] = data[currentSmplPos + i] * multiplier;
            }
            currentSmplPos += nbRead;
        }
    }

    this->graph(1)->setData(dataX, dataY);
}

void GraphicsViewEnvelop::setEnvelopStyle(int index, bool isGlobal, bool isVolume, bool isMain)
{
    Q_UNUSED(isGlobal)

    // Colors
    QColor redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, true);
    QColor greenColor = ContextManager::theme()->getFixedColor(ThemeManager::GREEN, true);

    _envelops[index]->setColor(isVolume ? greenColor : redColor);
    _envelops[index]->setThick(isMain);
}

void GraphicsViewEnvelop::drawEnvelops()
{
    // Compute the maximum attack duration and release
    double attackDuration = 0;
    double releaseDuration = 0;
    foreach (Envelop * env, _envelops)
    {
        attackDuration = qMax(attackDuration, env->getAttackDuration());
        releaseDuration = qMax(releaseDuration, env->getReleaseDuration());
    }

    // Add an area for the sustain (minimum 1s)
    double sustainDuration = qMax(1., attackDuration * 0.25);

    // Round the values
    attackDuration = qCeil(attackDuration);
    sustainDuration = qCeil(sustainDuration);

    // Adapt the envelops and compute the size of the graph
    _triggeredKeyDuration = qMax(2., attackDuration + sustainDuration);
    _releasedKeyDuration = qMax(2., releaseDuration);
    foreach (Envelop * env, _envelops)
        env->draw(_triggeredKeyDuration, _releasedKeyDuration);
    _sizeX = _triggeredKeyDuration + _releasedKeyDuration;
}

void GraphicsViewEnvelop::paintEvent(QPaintEvent *event)
{
    // Tick step
    _fixedTicker->setTickStep(getTickStep());

    QCustomPlot::paintEvent(event);

    // Release line and images
    QPainter painter(this);
    painter.setPen(_releaseLinePen);
    painter.drawLine(QPointF(_posReleaseLine, -10000), QPointF(_posReleaseLine, 10000));

    const int width = this->width();
    const int minMargin = 50;
    const int posY = 20;
    if (_posReleaseLine > minMargin)
    {
        int x = qMin(width, _posReleaseLine) / 2;
        painter.drawImage(x - _imageNoteOn.width() / 2, posY - _imageNoteOn.height() / 2, _imageNoteOn);
    }
    if (_posReleaseLine < width - minMargin)
    {
        int x = qMin(width, width - _posReleaseLine) / 2;
        painter.drawImage(width - x - _imageNoteOff.width() / 2, posY - _imageNoteOff.height() / 2, _imageNoteOff);
    }
}

void GraphicsViewEnvelop::resizeEvent(QResizeEvent *event)
{
    QCustomPlot::resizeEvent(event);
    _posReleaseLine = this->xAxis->coordToPixel(_triggeredKeyDuration);
}

void GraphicsViewEnvelop::mousePressEvent(QMouseEvent *event)
{
    // Enregistrement situation
    _xInit = this->xAxis2->pixelToCoord(event->x());
    _yInit = this->yAxis2->pixelToCoord(event->y());
    _zoomXinit = _zoomX;
    _posXinit = _posX;
    if (event->button() == Qt::LeftButton && !_zoomFlag)
        _dragFlag = true;
    else if (event->button() == Qt::RightButton && !_dragFlag)
        _zoomFlag = true;
}

void GraphicsViewEnvelop::mouseReleaseEvent(QMouseEvent *event)
{
    int val = qRound(_sizeX / _zoomX * ((_zoomX - 1) * _posX + _xInit) - 1);
    if (val < 0)
        val = 0;
    if (event->button() == Qt::LeftButton)
    {
        _dragFlag = false;
        this->setCursor(Qt::ArrowCursor);
    }
    else if (event->button() == Qt::RightButton)
    {
        _zoomFlag = false;
        this->setZoomLine(-1, 0, 0, 0);
        this->setCursor(Qt::ArrowCursor);
        this->replot();
    }
}

void GraphicsViewEnvelop::mouseMoveEvent(QMouseEvent *event)
{
    if (_zoomFlag)
    {
        this->setCursor(Qt::SizeAllCursor);

        // Ligne de zoom
        this->setZoomLine(_xInit, _yInit,
                          this->xAxis2->pixelToCoord(event->x()),
                          this->yAxis2->pixelToCoord(event->y()));

        this->zoom(event->pos());
    }
    else if (_dragFlag)
    {
        this->setCursor(Qt::ClosedHandCursor);
        this->drag(event->pos());
    }
}

void GraphicsViewEnvelop::wheelEvent(QWheelEvent *event)
{
    if (!_dragFlag && !_zoomFlag)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        _qScrollX->setValue(_qScrollX->value() - 0.2 * event->angleDelta().x());
#else
        _qScrollX->setValue(_qScrollX->value() - 0.2 * event->delta());
#endif
}

void GraphicsViewEnvelop::zoom(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->_xInit;

    // Modification zoom & drag
    double newZoomX = _zoomXinit * pow(2, 25.0 * decX);

    if (newZoomX < 1)
        newZoomX = 1;
    else if (newZoomX > _sizeX * 200)
        newZoomX = _sizeX * 200;

    if (newZoomX != _zoomX)
    {
        _zoomX = newZoomX;

        // Ajustement posX
        if (_zoomX > 1)
            _posX = (_zoomX * _posXinit * (_zoomXinit - 1) + _xInit*(_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));

        // Mise à jour
        this->zoomDrag();
    }
}

void GraphicsViewEnvelop::drag(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->_xInit;

    // Modification posX et posY
    if (_zoomXinit > 1)
        _posX = _posXinit - decX / (_zoomXinit - 1);

    // Mise à jour
    this->zoomDrag();
}

void GraphicsViewEnvelop::setZoomLine(double x1, double y1, double x2, double y2)
{
    if (x1 >= 0)
    {
        // Conversion
        QVector<double> x(2), y(2);
        x[0] = _sizeX * (x1 + _posX * (_zoomX - 1)) / _zoomX;
        x[1] = _sizeX * (x2 + _posX * (_zoomX - 1)) / _zoomX;
        y[0] = 1.2 * y1;
        y[1] = 1.2 * y2;
        this->graph(0)->setData(x, y);
    }
    else
        this->graph(0)->data()->clear();
}

void GraphicsViewEnvelop::displayCurrentRange()
{
    double etendueX = _sizeX / _zoomX;
    double offsetX = (_sizeX - etendueX) * _posX;

    double coordX1 = qMax(0., offsetX);
    double coordX2 = qMax(0., offsetX + etendueX);

    _textPositionL->setText(" " + QString::number(coordX1, 'f', 3) + "s");
    _textPositionR->setText(QString::number(coordX2, 'f', 3) + "s ");
}
