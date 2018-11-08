/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "graphique.h"
#include "basetypes.h"
#include <QScrollBar>
#include <QSpinBox>
#include "contextmanager.h"

Graphique::Graphique(QWidget * parent) : QCustomPlot(parent),
    _zoomFlag(false),
    _dragFlag(false),
    _zoomX(1),
    _zoomY(1),
    _posX(.5),
    _posY(.5),
    _zoomXinit(1),
    _zoomYinit(1),
    _posXinit(.5),
    _posYinit(.5),
    _bFromExt(false),
    _qScrollX(NULL),
    _spinStart(NULL),
    _spinEnd(NULL),
    _filterEventEnabled(true),
    _textMultipleSelection(NULL),
    _textPositionL(NULL),
    _textPositionR(NULL),
    _sampleRate(0)
{
    // Graphe des données
    this->addGraph();
    this->graph(0)->setAntialiased(true);

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
    _textMultipleSelection = new QCPItemText(this);
    _textMultipleSelection->position->setType(QCPItemPosition::ptAxisRectRatio);
    _textMultipleSelection->setPositionAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    _textMultipleSelection->position->setCoords(.5, .5);
    _textMultipleSelection->setTextAlignment(Qt::AlignHCenter);
    _textMultipleSelection->setFont(QFont(font().family(), 16, QFont::Bold));
    _textMultipleSelection->setText(trUtf8("Sélection multiple"));
    addItem(_textMultipleSelection);

    // Positions
    _textPositionL = new QCPItemText(this);
    _textPositionL->position->setType(QCPItemPosition::ptAxisRectRatio);
    _textPositionL->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    _textPositionL->position->setCoords(0, 1);
    _textPositionL->setTextAlignment(Qt::AlignBottom);
    _textPositionL->setFont(QFont(font().family(), 8, QFont::Bold));
    _textPositionL->setText("");
    addItem(_textPositionL);

    _textPositionR = new QCPItemText(this);
    _textPositionR->position->setType(QCPItemPosition::ptAxisRectRatio);
    _textPositionR->setPositionAlignment(Qt::AlignRight|Qt::AlignBottom);
    _textPositionR->position->setCoords(1, 1);
    _textPositionR->setTextAlignment(Qt::AlignBottom);
    _textPositionR->setFont(QFont(font().family(), 8, QFont::Bold));
    _textPositionR->setText("");
    addItem(_textPositionR);

    // Paramétrage des couleurs
    this->updateStyle();

    // Start update timer
    _lastUpdate.start();
}

// Méthodes publiques
void Graphique::updateStyle()
{
    // Couleurs
    QColor backgroundColor;
    QColor textColor;
    QColor redColor;
    QColor greenColor;
    if (ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        backgroundColor = this->palette().color(QPalette::Base);
        textColor = this->palette().color(QPalette::Text);
        redColor = this->palette().color(QPalette::BrightText);
        greenColor = this->palette().color(QPalette::NoRole);
    }
    else
    {
        backgroundColor = this->palette().color(QPalette::Text);
        textColor = this->palette().color(QPalette::Base);
        redColor = this->palette().color(QPalette::BrightText).lighter();
        greenColor = this->palette().color(QPalette::NoRole).lighter();
    }

    this->setBackground(backgroundColor);
    QPen graphPen;
    graphPen.setColor(backgroundColor);
    this->yAxis->setBasePen(graphPen);

    // Couleur de l'onde
    graphPen.setColor(this->palette().color(QPalette::Highlight));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);

    // Couleur début de boucle
    graphPen.setColor(greenColor);
    graphPen.setWidthF(2);
    this->graph(1)->setPen(graphPen);

    // Couleur fin de boucle
    graphPen.setColor(redColor);
    graphPen.setWidthF(2);
    this->graph(2)->setPen(graphPen);

    // Ligne de zoom
    graphPen.setColor(redColor);
    graphPen.setWidthF(1);
    graphPen.setStyle(Qt::DashLine);
    this->graph(3)->setPen(graphPen);

    // Overlay
    QColor colorTmp = greenColor;
    colorTmp.setAlpha(180);
    graphPen.setColor(colorTmp);
    graphPen.setWidth(2);
    graphPen.setStyle(Qt::DotLine);
    this->graph(4)->setPen(graphPen);
    colorTmp = redColor;
    colorTmp.setAlpha(180);
    graphPen.setColor(colorTmp);
    this->graph(5)->setPen(graphPen);

    // Couleur de la grille
    textColor.setAlpha(40);
    graphPen.setColor(textColor);
    graphPen.setWidthF(1);
    this->yAxis->grid()->setZeroLinePen(graphPen);
    graphPen.setStyle(Qt::DotLine);
    this->yAxis->grid()->setPen(graphPen);

    // Curseur de lecture et texte
    textColor.setAlpha(255);
    _penLecture.setColor(textColor);
    _penLecture.setWidthF(1);
    textColor.setAlpha(180);
    _textMultipleSelection->setColor(textColor);
    _textPositionL->setColor(textColor);
    _textPositionR->setColor(textColor);

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

void Graphique::setData(QByteArray baData, int sampleRate)
{
    // Ajout des données
    quint32 size_x = baData.size() / 2;
    this->_sizeX = ((double)size_x + 1);
    qint16 * data = (qint16 *)baData.data();
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i = 0; i < size_x; i++)
    {
        x[i] = i;
        y[i] = (double)data[i] / 32768.; // normalisation entre -1 et 1
    }
    this->graph(0)->setData(x, y);
    _sampleRate = sampleRate;
}

void Graphique::linkSliderX(QScrollBar * qScrollX)
{
    this->_qScrollX = qScrollX;
}

void Graphique::linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd)
{
    this->_spinStart = spinStart;
    this->_spinEnd = spinEnd;
}

void Graphique::setPosX(int posX)
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

void Graphique::setStartLoop(int pos, bool replot)
{
    if (_filterEventEnabled)
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
    if (_filterEventEnabled)
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
    if (_lastUpdate.elapsed() < 16 && pos != 0)
        return;
    _lastUpdate.restart();

    _currentPos = this->xAxis->coordToPixel(pos);
    this->repaint();
    this->update();
}

void Graphique::paintEvent(QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);

    // Ajout du trait de lecture
    if (_currentPos > 0)
    {
        QPainter painter(this);
        painter.setPen(_penLecture);
        painter.drawLine(QPointF(_currentPos, -10000), QPointF(_currentPos, 10000));
    }
}

void Graphique::zoomDrag()
{
    // Bornes des paramètres d'affichage
    if (this->_posX < 0)
        this->_posX = 0;
    else if (this->_posX > 1)
        this->_posX = 1;
    if (this->_posY < 0)
        this->_posY = 0;
    else if (this->_posY > 1)
        this->_posY = 1;
    _posY = 0.5; // blocage sur Y

    // Application du drag et zoom
    double etendueX = _sizeX / _zoomX;
    double offsetX = (_sizeX - etendueX) * _posX - 1;
    this->xAxis->setRange(offsetX, offsetX + etendueX);
    double etendueY = 2. / _zoomY;
    double offsetY = (2. - etendueY) * _posY - 1;
    this->yAxis->setRange(offsetY, offsetY + etendueY);

    // Mise à jour
    displayCurrentRange();
    this->replot(QCustomPlot::rpQueued);
    if (!_bFromExt && _qScrollX)
    {
        // Mise à jour du scrollbar
        _qScrollX->blockSignals(true);
        _qScrollX->setPageStep(10000. / _zoomX);
        _qScrollX->setRange(0, 10000. - _qScrollX->pageStep());
        _qScrollX->setValue(_qScrollX->maximum() * _posX);
        _qScrollX->blockSignals(false);
    }
}

void Graphique::displayMultipleSelection(bool isOn)
{
    _qScrollX->setEnabled(!isOn);
    _filterEventEnabled = !isOn;
    _textMultipleSelection->setVisible(isOn);
    if (isOn)
        zoomDrag();
    else
        _qScrollX->setRange(0, 0);
}

// Méthodes privées
void Graphique::zoom(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->_xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->_yInit;

    // Modification zoom & drag
    double newZoomX = _zoomXinit * pow(2, 25.0 * decX);
    double newZoomY = _zoomYinit * pow(2,  5.0 * decY);

    if (newZoomX < 1)
        newZoomX = 1;
    else if (newZoomX > _sizeX * 200)
        newZoomX = _sizeX * 200;
    if (newZoomY < 1)
        newZoomY = 1;
    else if (newZoomY > 50)
        newZoomY = 50;

    if (newZoomX != _zoomX || newZoomY != _zoomY)
    {
        _zoomX = newZoomX;
        _zoomY = newZoomY;

        // Ajustement posX et posY
        if (_zoomX > 1)
            _posX = (_zoomX * _posXinit * (_zoomXinit - 1) + _xInit*(_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
        if (_zoomY > 1)
            _posY = (_zoomY * _posYinit * (_zoomYinit - 1) + _yInit*(_zoomY - _zoomYinit)) / (_zoomYinit * (_zoomY - 1));

        // Mise à jour
        this->zoomDrag();
    }
}

void Graphique::drag(QPoint point)
{
    // Décalage
    double decX = this->xAxis2->pixelToCoord(point.x()) - this->_xInit;
    double decY = this->yAxis2->pixelToCoord(point.y()) - this->_yInit;

    // Modification posX et posY
    if (_zoomXinit > 1)
        _posX = _posXinit - decX / (_zoomXinit - 1);
    if (_zoomYinit > 1)
        _posY = _posYinit - decY / (_zoomYinit - 1);

    // Mise à jour
    this->zoomDrag();
}

void Graphique::setZoomLine(double x1, double y1, double x2, double y2)
{
    if (x1 >= 0)
    {
        // Conversion
        QVector<double> x(2), y(2);
        x[0] = _sizeX * (x1 + _posX * (_zoomX - 1)) / _zoomX - 1;
        x[1] = _sizeX * (x2 + _posX * (_zoomX - 1)) / _zoomX - 1;
        y[0] = 2 * (y1 + _posY * (_zoomY - 1)) / _zoomY - 1;
        y[1] = 2 * (y2 + _posY * (_zoomY - 1)) / _zoomY - 1;
        this->graph(3)->setData(x, y);
    }
    else
        this->graph(3)->clearData();
}

void Graphique::plotOverlay()
{
    int sizeOverlay = 20;
    qint32 posDebut = this->_spinStart->value();
    qint32 posFin = this->_spinEnd->value();
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
    if (!_filterEventEnabled)
        return;

    // Enregistrement situation
    _xInit = this->xAxis2->pixelToCoord(event->x());
    _yInit = this->yAxis2->pixelToCoord(event->y());
    _zoomXinit = _zoomX;
    _zoomYinit = _zoomY;
    _posXinit = _posX;
    _posYinit = _posY;
    _modifiedFlag = false;
    if (event->button() == Qt::LeftButton && !_zoomFlag)
        _dragFlag = true;
    else if (event->button() == Qt::RightButton && !_dragFlag)
        _zoomFlag = true;
}

void Graphique::mouseReleaseEvent(QMouseEvent *event)
{
    if (!_filterEventEnabled)
        return;

    int val = qRound(_sizeX / _zoomX * ((_zoomX - 1) * _posX + _xInit) - 1);
    if (val < 0)
        val = 0;
    if (event->button() == Qt::LeftButton)
    {
        _dragFlag = false;
        if (!_modifiedFlag)
        {
            // Modification start loop
            if (this->_spinStart && this->_spinEnd)
            {
                if (this->_spinEnd->value() > val)
                {
                    this->_spinEnd->setMinimum(val);
                    this->_spinStart->setValue(val);
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
        _zoomFlag = false;
        if (!_modifiedFlag)
        {
            // Modification end loop
            if (this->_spinStart && this->_spinEnd)
            {
                if (this->_spinStart->value() < val)
                {
                    this->_spinStart->setMaximum(val);
                    this->_spinEnd->setValue(val);
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
            this->replot();
        }
    }
}

void Graphique::mouseMoveEvent(QMouseEvent *event)
{
    if (!_filterEventEnabled)
        return;

    if (_zoomFlag)
    {
        if (!_modifiedFlag)
        {
            _modifiedFlag = true;
            this->setCursor(Qt::SizeAllCursor);
        }

        // Ligne de zoom
        this->setZoomLine(_xInit, _yInit,
                          this->xAxis2->pixelToCoord(event->x()),
                          this->yAxis2->pixelToCoord(event->y()));

        this->zoom(event->pos());
    }
    else if (_dragFlag)
    {
        if (!_modifiedFlag)
        {
            _modifiedFlag = true;
            this->setCursor(Qt::ClosedHandCursor);
        }
        this->drag(event->pos());
    }
}

void Graphique::wheelEvent(QWheelEvent *event)
{
    if (!_filterEventEnabled)
        return;

    if (!_dragFlag && !_zoomFlag)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        _qScrollX->setValue(_qScrollX->value() - 0.2 * event->angleDelta().x());
#else
        _qScrollX->setValue(_qScrollX->value() - 0.2 * event->delta());
#endif
}

void Graphique::displayCurrentRange()
{
    if (_sampleRate > 0)
    {
        double etendueX = _sizeX / _zoomX;
        double offsetX = (_sizeX - etendueX) * _posX - 1;

        double coordX1 = qMax(0., offsetX / _sampleRate);
        double coordX2 = qMax(0., (offsetX + etendueX) / _sampleRate);

        _textPositionL->setText(" " + QString::number(coordX1, 'f', 3) + "s");
        _textPositionR->setText(QString::number(coordX2, 'f', 3) + "s ");
    }
    else
    {
        _textPositionL->setText("");
        _textPositionR->setText("");
    }
}
