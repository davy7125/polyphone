/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "graphicsviewenvelop.h"
#include "graphicswavepainter.h"
#include "contextmanager.h"
#include <QResizeEvent>
#include <QScrollBar>
#include <QPainter>

GraphicsViewEnvelop::GraphicsViewEnvelop(QWidget *parent) : QWidget(parent),
    _wavePainter(new GraphicsWavePainter()),
    _dontRememberScroll(false),
    _zoomFlag(false),
    _dragFlag(false),
    _zoomX(1),
    _posX(.5),
    _zoomXinit(1),
    _posXinit(.5),
    _bFromExt(false),
    _qScrollX(nullptr)
{
    // Images
    _imageNoteOn = QImage(":/icons/note_on.png").scaled(36, 36, Qt::KeepAspectRatio);
    _imageNoteOff = QImage(":/icons/note_off.png").scaled(36, 36, Qt::KeepAspectRatio);

    // Colors
    _redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, true);
    if (ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
        _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    }
    else
    {
        _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
        _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    }

    // Line styles
    _releaseLinePen = QPen(_textColor, 2, Qt::DashLine);
    _zoomLinePen = QPen(_redColor, 1, Qt::DashLine);
    _textColor.setAlpha(40);
    _gridPen = QPen(_textColor, 1, Qt::DotLine);
    _textColor.setAlpha(180);

    // Wave configuration
    _wavePainter->setWaveColor(ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND), _backgroundColor, 0.7));
    _wavePainter->setOnlyPositiveValues(true);

    this->installEventFilter(this);
}

GraphicsViewEnvelop::~GraphicsViewEnvelop()
{
    clearEnvelops();
    delete _wavePainter;
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
        if (_qScrollX->maximum() > 0)
            _posX = static_cast<double>(posX) / this->_qScrollX->maximum();
        else
            _posX = 0.5;
        this->repaint();
        _bFromExt = false;
    }
}

double GraphicsViewEnvelop::getTickStep()
{
    double widthPerUnit = static_cast<double>(this->width()) * _zoomX / _sizeX;
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
    while (!_envelops.isEmpty())
        delete _envelops.take(_envelops.keys().first());
    _index = 0;
    this->repaint();
}

int GraphicsViewEnvelop::addEnvelop()
{
    _index++;
    _envelops[_index] = new Envelop();
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

void GraphicsViewEnvelop::setSample(QVector<float> vData, int sampleRate, int loopMode, int startLoop, int endLoop)
{
    _sampleRate = sampleRate;

    // Number of points to display
    int dataSize = vData.size();
    int loopedDataSize = _sizeX * sampleRate;
    QVector<float> loopedData(loopedDataSize, 0);

    // Pointers to data
    const float * fData = vData.constData();
    float * fLoopedData = loopedData.data();

    // Compute the multiplier to normalize data
    float max = 0;
    for (int i = 0; i < dataSize; i++)
        max = qMax(max, qAbs(fData[i]));
    float multiplier = (max == 0) ? 1.0f : (0.7f / max);

    // Key on part
    int currentSmplPos = 0;
    int nbRead = _triggeredKeyDuration * sampleRate;

    if (loopMode != 0 && startLoop != endLoop)
    {
        // Loop
        if (currentSmplPos >= endLoop)
            currentSmplPos = startLoop;
        int total = 0;
        while (nbRead - total > 0)
        {
            const int chunk = qMin(endLoop - currentSmplPos, nbRead - total);
            for (int i = 0; i < chunk; i++)
                fLoopedData[total + i] = multiplier * fData[currentSmplPos + i];
            currentSmplPos += chunk;
            if (currentSmplPos >= endLoop)
                currentSmplPos = startLoop;
            total += chunk;
        }
    }
    else
    {
        // No loop
        if (dataSize - currentSmplPos < nbRead)
        {
            for (int i = 0; i < dataSize - currentSmplPos; i++)
                fLoopedData[i] = multiplier * fData[currentSmplPos + i];
            for (int i = dataSize - currentSmplPos; i < nbRead; i++)
                fLoopedData[i] = 0;
            currentSmplPos = dataSize;
        }
        else
        {
            for (int i = 0; i < nbRead; i++)
                fLoopedData[i] = multiplier * fData[currentSmplPos + i];
            currentSmplPos += nbRead;
        }
    }

    // Key off part
    int offset = nbRead;
    nbRead = loopedDataSize - nbRead;

    if ((loopMode == 1 || loopMode == 2) && startLoop != endLoop)
    {
        // Loop
        if (currentSmplPos >= endLoop)
            currentSmplPos = startLoop;
        int total = 0;
        while (nbRead - total > 0)
        {
            const int chunk = qMin(endLoop - currentSmplPos, nbRead - total);
            for (int i = 0; i < chunk; i++)
                fLoopedData[offset + total + i] = multiplier * fData[currentSmplPos + i];
            currentSmplPos += chunk;
            if (currentSmplPos >= endLoop)
                currentSmplPos = startLoop;
            total += chunk;
        }
    }
    else
    {
        // No loop
        if (dataSize - currentSmplPos < nbRead)
        {
            for (int i = 0; i < dataSize - currentSmplPos; i++)
                fLoopedData[offset + i] = multiplier * fData[currentSmplPos + i];
            for (int i = dataSize - currentSmplPos; i < nbRead; i++)
                fLoopedData[offset + i] = 0;
            currentSmplPos = dataSize;
        }
        else
        {
            for (int i = 0; i < nbRead; i++)
                fLoopedData[offset + i] = multiplier * fData[currentSmplPos + i];
            currentSmplPos += nbRead;
        }
    }

    _wavePainter->setData(loopedData);
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

void GraphicsViewEnvelop::computeEnvelops()
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
    _sizeX = _triggeredKeyDuration + _releasedKeyDuration;

    // Compute the points
    foreach (Envelop * env, _envelops)
        env->computePoints(_triggeredKeyDuration, _releasedKeyDuration);
}

void GraphicsViewEnvelop::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    const int width = this->width();
    const int height = this->height();
    const double etendueX = _sizeX / _zoomX; // In seconds
    const double offsetX = (_sizeX - etendueX) * _posX; // In seconds

    // Background
    painter.fillRect(this->rect(), _backgroundColor);

    // Grid
    double tickStep = getTickStep();
    painter.setPen(_gridPen);
    for (int i = offsetX / tickStep; i < (offsetX + etendueX) / tickStep + 1; i++)
    {
        double valueX = tickStep * i;
        int pos = (valueX - offsetX) / etendueX * width + 0.5;
        painter.drawLine(pos, 0, pos, height);
    }

    // Paint the waveform
    double start = offsetX < 0 ? 0 : offsetX;
    double end = (offsetX + etendueX) < 0 ? 0 : (offsetX + etendueX);
    _wavePainter->paint(&painter, event->rect(), start * _sampleRate + 0.5, end * _sampleRate + 0.5, 1.0);

    // Draw the envelops
    foreach (Envelop * env, _envelops)
        env->draw(&painter, width, height, etendueX, offsetX);

    // Grid values
    painter.setPen(_textColor);
    for (int i = offsetX / tickStep; i < (offsetX + etendueX) / tickStep + 1; i++)
    {
        double valueX = tickStep * i;
        int pos = (valueX - offsetX) / etendueX * width + 0.5;
        painter.drawText(QRectF(pos - 200, height - 105, 400, 100), Qt::AlignHCenter | Qt::AlignBottom,
                         doubleToString(valueX, tickStep < 0.001) + " " + tr("s", "unit for seconds"));
    }

    // Release line and images
    int posReleaseLine = (_triggeredKeyDuration - offsetX) / etendueX * width + 0.5;
    painter.setPen(_releaseLinePen);
    painter.drawLine(posReleaseLine, 0, posReleaseLine, height);
    if (posReleaseLine > _imageNoteOn.width() * 2)
        painter.drawImage(qMin(width, posReleaseLine) / 2 - _imageNoteOn.width() / 2,
                          15, _imageNoteOn);
    if (posReleaseLine < width - _imageNoteOff.width() * 2)
        painter.drawImage(width - qMin(width, width - posReleaseLine) / 2 - _imageNoteOff.width() / 2,
                          15, _imageNoteOff);

    // Zoom line
    if (_zoomFlag)
    {
        // Zoom line
        if (_x != _xInit || _y != _yInit)
        {
            painter.setPen(QPen(_redColor, 1.0, Qt::DashLine));
            painter.drawLine(QPointF(static_cast<double>(_xInit * this->width()), static_cast<double>(_yInit * this->height())),
                             QPointF(static_cast<double>(_x * this->width()), static_cast<double>(_y * this->height())));
        }
    }

    // Possibly update scrollbar
    if (!_bFromExt && _qScrollX)
    {
        _qScrollX->blockSignals(true);
        _qScrollX->setPageStep(static_cast<qint32>(10000 / _zoomX));
        _qScrollX->setRange(0, static_cast<qint32>(10000. - _qScrollX->pageStep()));
        _qScrollX->setValue(static_cast<qint32>(_qScrollX->maximum() * _posX));
        _qScrollX->blockSignals(false);
    }
}

QString GraphicsViewEnvelop::doubleToString(double value, bool accurate)
{
    QString txt = QLocale::system().toString(value, 'f', accurate ? 4 : 3);
    while (!txt.isEmpty() && (txt.endsWith("0")))
        txt = txt.left(txt.size() - 1);
    if (!txt.isEmpty() && (txt.endsWith(".") || txt.endsWith(",")))
        txt = txt.left(txt.size() - 1);
    return txt;
}

void GraphicsViewEnvelop::mousePressEvent(QMouseEvent *event)
{
    // Enregistrement situation
#if QT_VERSION < 0x060000
    _xInit = static_cast<double>(event->x()) / this->width();
    _yInit = static_cast<double>(event->y()) / this->height();
#else
    _xInit = static_cast<double>(event->position().x()) / this->width();
    _yInit = static_cast<double>(event->position().y()) / this->height();
#endif
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
        this->setCursor(Qt::ArrowCursor);
        this->repaint();
    }
}

void GraphicsViewEnvelop::mouseMoveEvent(QMouseEvent *event)
{    
#if QT_VERSION < 0x060000
    _x = static_cast<double>(event->x()) / this->width();
    _y = static_cast<double>(event->y()) / this->height();
#else
    _x = static_cast<double>(event->position().x()) / this->width();
    _y = static_cast<double>(event->position().y()) / this->height();
#endif

    if (_zoomFlag)
    {
        this->setCursor(Qt::SizeAllCursor);
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
    double decX = static_cast<double>(point.x()) / this->width() - this->_xInit;

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
        {
            _posX = (_zoomX * _posXinit * (_zoomXinit - 1) + _xInit * (_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
            if (_posX < 0)
                _posX = 0;
            else if (_posX > 1)
                _posX = 1;
        }
    }

    this->repaint();
}

void GraphicsViewEnvelop::drag(QPoint point)
{
    // Décalage
    double decX = static_cast<double>(point.x()) / this->width() - this->_xInit;

    // Modification posX
    if (_zoomXinit > 1)
        _posX = _posXinit - decX / (_zoomXinit - 1);
    if (_posX < 0)
        _posX = 0;
    else if (_posX > 1)
        _posX = 1;

    // Mise à jour
    this->repaint();
}

bool GraphicsViewEnvelop::event(QEvent * event)
{
    if (event->type() == QEvent::NativeGesture)
    {
        QNativeGestureEvent * nge = dynamic_cast<QNativeGestureEvent *>(event);
        if (nge->fingerCount() == 2)
        {
            if (nge->gestureType() == Qt::ZoomNativeGesture)
            {
                // WARNING: not using nge->position() since it seems to return the global position
                _xInit = (nge->globalPosition().x() - this->mapToGlobal(QPoint(0, 0)).x()) / this->width();
                _posXinit = _posX;
                _zoomXinit = _zoomX;
                _zoomX = _zoomX * (1 + nge->value());

                if (_zoomX < 1)
                    _zoomX = 1;
                else if (_zoomX > _sizeX * 200)
                    _zoomX = _sizeX * 200;

                // Update posX
                if (_zoomX > 1)
                {
                    _posX = (_zoomX * _posXinit * (_zoomXinit - 1) + _xInit * (_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
                    if (_posX < 0)
                        _posX = 0;
                    else if (_posX > 1)
                        _posX = 1;
                }
            }

            this->repaint();
        }
    }
    return QWidget::event(event);
}
