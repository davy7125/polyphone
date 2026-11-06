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
#include <QTimer>
#include <cmath>

static const QPointF MOUSE_OUTSIDE(-10000, -10000);
static const int EDGE_MARGIN = 10; // Pixels
static const int LEFT_MARGIN = 12; // Pixels, so that the first points can be grabbed
static const double MAX_DISPLAYED_SAMPLES = 1 << 24; // The waveform is not displayed after
static const int AXIS_THRESHOLD = 3; // Pixels

GraphicsViewEnvelop::GraphicsViewEnvelop(QWidget *parent) : QWidget(parent),
    _wavePainter(new GraphicsWavePainter()),
    _sampleRate(0),
    _sampleDataDuration(0),
    _loopMode(0),
    _startLoop(0),
    _endLoop(0),
    _index(0),
    _dontRememberScroll(false),
    _triggeredKeyDuration(2),
    _releasedKeyDuration(2),
    _sizeX(4),
    _zoomFlag(false),
    _dragFlag(false),
    _zoomX(1),
    _posX(.5),
    _zoomXinit(1),
    _posXinit(.5),
    _bFromExt(false),
    _qScrollX(nullptr),
    _mouseInside(false),
    _hoveredEnvelop(-1),
    _hoveredHandle(Envelop::HANDLE_NONE),
    _editedEnvelop(-1),
    _editedHandle(Envelop::HANDLE_NONE),
    _valuesEdited(false),
    _axisXActive(false),
    _axisYActive(false),
    _pivotOnCenter(false),
    _edgeTimer(new QTimer(this)),
    _edgeDirection(0)
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

    // Sustain level is stored between 0 and 1
    _valueRanges[Envelop::SUSTAIN] = QPair<double, double>(0, 1);

    // Scroll or zoom out while a point is dragged against an edge
    _edgeTimer->setInterval(30);
    connect(_edgeTimer, SIGNAL(timeout()), this, SLOT(onEdgeTimer()));

    this->setMouseTracking(true);
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

    // Very long envelopes (key modifiers): steps 500, 1000, 2000, 5000...
    if (widthPerUnit < 1)
        for (int i = 0; tickStep * widthPerUnit < 100 && tickStep < 1e15; i++)
            tickStep *= (i % 3 == 0) ? 2.5 : 2;
    return tickStep;
}

void GraphicsViewEnvelop::clearEnvelops()
{
    abortEdition();
    _hoveredEnvelop = -1;
    _hoveredHandle = Envelop::HANDLE_NONE;

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

double GraphicsViewEnvelop::getValue(int index, Envelop::ValueType type)
{
    return _envelops[index]->get(type);
}

void GraphicsViewEnvelop::setKeyRange(int index, int keyMin, int keyMax)
{
    _envelops[index]->set(Envelop::KEY_MIN, keyMin, true);
    _envelops[index]->set(Envelop::KEY_MAX, keyMax, true);
}

void GraphicsViewEnvelop::setSample(QVector<float> vData, int sampleRate, int loopMode, int startLoop, int endLoop)
{
    _sampleData = vData;
    _sampleRate = sampleRate;
    _loopMode = loopMode;
    _startLoop = startLoop;
    _endLoop = endLoop;
    computeSampleData();
}

void GraphicsViewEnvelop::clearSample()
{
    _sampleData.clear();
    _sampleDataDuration = 0;
    _wavePainter->clearData();
}

void GraphicsViewEnvelop::computeSampleData()
{
    const QVector<float> &vData = _sampleData;
    const int sampleRate = _sampleRate;
    const int loopMode = _loopMode;
    const int startLoop = _startLoop;
    const int endLoop = _endLoop;

    // Number of points to display, limited for very long envelopes
    int dataSize = vData.size();
    double samples = _sizeX * sampleRate;
    int loopedDataSize = (samples > 0) ? static_cast<int>(qMin(samples, MAX_DISPLAYED_SAMPLES)) : 0;
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
    samples = _triggeredKeyDuration * sampleRate;
    int nbRead = (samples > 0) ? static_cast<int>(qMin(samples, static_cast<double>(loopedDataSize))) : 0;

    if ((loopMode == 1 || loopMode == 3) && startLoop != endLoop)
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
    else if (loopMode == 0)
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

    if (loopMode == 1 && startLoop != endLoop)
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
    _sampleDataDuration = sampleRate > 0 ? static_cast<double>(loopedDataSize) / sampleRate : 0;
}

void GraphicsViewEnvelop::setEnvelopStyle(int index, bool isGlobal, bool isVolume, bool isMain)
{
    Q_UNUSED(isGlobal)

    // Colors
    QColor redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, true);
    QColor greenColor = ContextManager::theme()->getFixedColor(ThemeManager::GREEN, true);

    _envelops[index]->setColor(isVolume ? greenColor : redColor);
    _envelops[index]->setThick(isMain);
    _envelops[index]->setVolume(isVolume);
    _envelops[index]->setEditable(isMain);
}

void GraphicsViewEnvelop::setEnvelopName(int index, QString name)
{
    _envelops[index]->setName(name);
}

void GraphicsViewEnvelop::setValueRange(Envelop::ValueType type, double min, double max)
{
    _valueRanges[type] = QPair<double, double>(min, max);
}

void GraphicsViewEnvelop::setValueText(Envelop::ValueType type, QString label, QString suffix, int decimals)
{
    _valueLabels[type] = label;
    _valueSuffixes[type] = suffix;
    _valueDecimals[type] = decimals;
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
    attackDuration = std::ceil(attackDuration);
    sustainDuration = std::ceil(sustainDuration);

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
    double etendueX, offsetX; // In seconds
    getDisplayedRange(offsetX, etendueX);

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
    QRect waveRect = event->rect();
    if (offsetX < 0)
        waveRect.setLeft(qRound(-offsetX / etendueX * width));
    if (end > _sampleDataDuration)
    {
        // Possible while dragging a point
        end = _sampleDataDuration;
        waveRect.setRight(qRound((end - offsetX) / etendueX * width));
    }
    if (end > start)
        _wavePainter->paint(&painter, waveRect, start * _sampleRate + 0.5, end * _sampleRate + 0.5, 1.0);

    // Draw the envelops
    updateHandles();
    foreach (Envelop * env, _envelops)
        env->draw(&painter);

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

    // Values of the hovered point
    drawLegend(&painter);
    if (!_zoomFlag && !_dragFlag)
        updateCursor();

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
    if (_editedEnvelop != -1)
        return;

    if (event->button() == Qt::LeftButton && !_zoomFlag)
    {
        // Drag a point or move the view
        _mousePos = event->pos();
        _mouseInside = true;
        updateHandles();
        if (_hoveredHandle != Envelop::HANDLE_NONE)
            startEdition(event->pos());
        else
            _dragFlag = true;
    }
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
        if (_editedEnvelop != -1)
            stopEdition(false);
        else
        {
            _dragFlag = false;
            this->setCursor(Qt::ArrowCursor);
        }
    }
    else if (event->button() == Qt::RightButton && _editedEnvelop == -1)
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
    _mousePos = event->pos();
    _mouseInside = this->rect().contains(_mousePos);

    if (_editedEnvelop != -1)
    {
        // Move the point, finer with shift
        QPoint delta = _mousePos - _editLastPos;
        _editLastPos = _mousePos;
        double factor = (event->modifiers() & Qt::ShiftModifier) ? 0.1 : 1.0;
        _editCoord += QPointF(factor * delta.x() * _editRange / qMax(1, this->width() - LEFT_MARGIN),
                              -factor * delta.y() / (this->height() - 70)); // Same margins as in Envelop

        // An axis is used after a minimal displacement, so that a click doesn't change anything
        if (qAbs(_mousePos.x() - _editStartPos.x()) > AXIS_THRESHOLD)
            _axisXActive = true;
        if (qAbs(_mousePos.y() - _editStartPos.y()) > AXIS_THRESHOLD)
            _axisYActive = true;

        // Scroll or zoom out against the edges
        _edgeDirection = 0;
        if (_axisXActive && _editedHandle != Envelop::HANDLE_SUSTAIN)
        {
            if (_mousePos.x() >= this->width() - EDGE_MARGIN)
                _edgeDirection = 1;
            else if (_mousePos.x() < EDGE_MARGIN && _editOffset > 0)
                _edgeDirection = -1;
        }
        if (_edgeDirection == 0)
            _edgeTimer->stop();
        else if (!_edgeTimer->isActive())
            _edgeTimer->start();

        _pivotOnCenter = (event->modifiers() & Qt::ControlModifier);
        editValues();
    }
    else if (_zoomFlag)
    {
        this->setCursor(Qt::SizeAllCursor);
        this->zoom(event->pos());
    }
    else if (_dragFlag)
    {
        this->setCursor(Qt::ClosedHandCursor);
        this->drag(event->pos());
    }
    else
    {
        // Hovered point
        QString previousState = getHoverState();
        updateHandles();
        updateCursor();
        if (getHoverState() != previousState)
            this->update();
    }
}

void GraphicsViewEnvelop::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || _editedEnvelop != -1)
        return;

    // Restore the values changed by the point
    _mousePos = event->pos();
    _mouseInside = true;
    updateHandles();
    QList<int> valueTypes;
    switch (_hoveredHandle)
    {
    case Envelop::HANDLE_NONE:
        return;
    case Envelop::HANDLE_HOLD_KEY_MAX: case Envelop::HANDLE_HOLD_KEY_MIN:
        valueTypes << Envelop::KEYNUM_TO_HOLD;
        break;
    case Envelop::HANDLE_DECAY_KEY_MAX: case Envelop::HANDLE_DECAY_KEY_MIN:
        valueTypes << Envelop::KEYNUM_TO_DECAY;
        break;
    default:
        foreach (Envelop::ValueType valueType, getValueTypes(_hoveredHandle))
            valueTypes << valueType;
        break;
    }
    emit resetRequested(valueTypes);
}

void GraphicsViewEnvelop::keyPressEvent(QKeyEvent *event)
{
    if (_editedEnvelop == -1)
    {
        QWidget::keyPressEvent(event);
        return;
    }

    // The keyboard is grabbed during an edition
    if (event->key() == Qt::Key_Escape)
    {
        stopEdition(true);
        updateHandles();
        updateCursor();
    }
    else if (event->key() == Qt::Key_Control)
    {
        _pivotOnCenter = true;
        editValues();
    }
}

void GraphicsViewEnvelop::keyReleaseEvent(QKeyEvent *event)
{
    if (_editedEnvelop == -1)
    {
        QWidget::keyReleaseEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Control)
    {
        _pivotOnCenter = false;
        editValues();
    }
}

void GraphicsViewEnvelop::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _mouseInside = false;
    if (_editedEnvelop == -1)
    {
        QString previousState = getHoverState();
        updateHandles();
        if (getHoverState() != previousState)
            this->update();
    }
}

void GraphicsViewEnvelop::wheelEvent(QWheelEvent *event)
{
    if (!_dragFlag && !_zoomFlag && _editedEnvelop == -1)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        // The graph only scrolls horizontally: the wheel is vertical, or horizontal with Alt or a touchpad
        QPoint angleDelta = event->angleDelta();
        int delta = qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? angleDelta.x() : angleDelta.y();
#else
        int delta = event->delta();
#endif
        _qScrollX->setValue(_qScrollX->value() - 0.2 * delta);
    }
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
    if (event->type() == QEvent::NativeGesture && _editedEnvelop == -1)
    {
        QNativeGestureEvent * nge = dynamic_cast<QNativeGestureEvent *>(event);
        if (nge->gestureType() == Qt::ZoomNativeGesture)
        {
            // WARNING: not using nge->position() since it seems to return the global position
#if QT_VERSION < 0x060000
            _xInit = (nge->globalPos().x() - this->mapToGlobal(QPoint(0, 0)).x()) / this->width();
#else
            _xInit = (nge->globalPosition().x() - this->mapToGlobal(QPoint(0, 0)).x()) / this->width();
#endif

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
    return QWidget::event(event);
}

void GraphicsViewEnvelop::getDisplayedRange(double &xOffset, double &xRange)
{
    // Part of the graph defined by the zoom and the position, the time 0 being after a margin
    double range = _sizeX / _zoomX;
    double width = qMax(1, this->width() - LEFT_MARGIN);
    xRange = range * this->width() / width;
    xOffset = (_sizeX - range) * _posX - LEFT_MARGIN * range / width;
}

void GraphicsViewEnvelop::updateHandles()
{
    // Positions in pixels
    double xRange, xOffset;
    getDisplayedRange(xOffset, xRange);
    QList<int> editableIndexes;
    foreach (int index, _envelops.keys())
    {
        _envelops[index]->computePixels(this->width(), this->height(), xRange, xOffset);
        if (_envelops[index]->isEditable())
            editableIndexes << index;
    }

    // Rulers of the envelops with a key modifier are always visible if there is only one envelop
    bool isSingle = (editableIndexes.count() == 1);

    if (_editedEnvelop != -1)
    {
        // The edited point stays highlighted
        foreach (int index, editableIndexes)
        {
            Envelop * env = _envelops[index];
            env->setMousePosition(index == _editedEnvelop ? env->getHandlePixels(_editedHandle) : MOUSE_OUTSIDE, isSingle);
            env->setHoveredHandle(index == _editedEnvelop ? _editedHandle : Envelop::HANDLE_NONE);
        }
        _hoveredEnvelop = _editedEnvelop;
        _hoveredHandle = _editedHandle;
        return;
    }

    // The envelop displaying its rulers keeps them while the mouse is close
    QPointF pos = _mouseInside ? QPointF(_mousePos) : MOUSE_OUTSIDE;
    int rulerEnvelop = -1;
    if (editableIndexes.contains(_hoveredEnvelop) && _envelops[_hoveredEnvelop]->isInRulerZone(pos))
        rulerEnvelop = _hoveredEnvelop;
    foreach (int index, editableIndexes)
        _envelops[index]->setMousePosition((isSingle || index == rulerEnvelop) ? pos : MOUSE_OUTSIDE, isSingle);

    // Closest point, the last envelop being drawn on top
    int hoveredEnvelop = -1;
    Envelop::HandleType hoveredHandle = Envelop::HANDLE_NONE;
    double minDistance = 0;
    foreach (int index, editableIndexes)
    {
        double distance;
        Envelop::HandleType handle = _envelops[index]->getHandleAt(pos, distance);
        if (handle != Envelop::HANDLE_NONE && (hoveredEnvelop == -1 || distance <= minDistance))
        {
            hoveredEnvelop = index;
            hoveredHandle = handle;
            minDistance = distance;
        }
    }

    // Only the hovered envelop displays its rulers if there are several envelops
    if (!isSingle && hoveredEnvelop != -1 && hoveredEnvelop != rulerEnvelop)
    {
        if (rulerEnvelop != -1)
            _envelops[rulerEnvelop]->setMousePosition(MOUSE_OUTSIDE, false);
        _envelops[hoveredEnvelop]->setMousePosition(pos, false);
        hoveredHandle = _envelops[hoveredEnvelop]->getHandleAt(pos, minDistance);
        rulerEnvelop = hoveredEnvelop;
    }

    _hoveredEnvelop = (hoveredEnvelop != -1 ? hoveredEnvelop : rulerEnvelop);
    _hoveredHandle = hoveredHandle;
    foreach (int index, editableIndexes)
        _envelops[index]->setHoveredHandle(index == hoveredEnvelop ? hoveredHandle : Envelop::HANDLE_NONE);
}

QString GraphicsViewEnvelop::getHoverState()
{
    QString state = QString::number(_hoveredEnvelop) + ":" + QString::number(_hoveredHandle);
    foreach (Envelop * env, _envelops)
        state += ":" + QString::number(env->getRulerVisibility());
    return state;
}

void GraphicsViewEnvelop::updateCursor()
{
    switch (_hoveredHandle)
    {
    case Envelop::HANDLE_NONE:
        this->setCursor(Qt::ArrowCursor);
        break;
    case Envelop::HANDLE_SUSTAIN:
        this->setCursor(Qt::SizeVerCursor);
        break;
    case Envelop::HANDLE_DECAY:
        this->setCursor(Qt::SizeAllCursor);
        break;
    default:
        this->setCursor(Qt::SizeHorCursor);
        break;
    }
}

QList<Envelop::ValueType> GraphicsViewEnvelop::getValueTypes(Envelop::HandleType handle)
{
    QList<Envelop::ValueType> valueTypes;
    switch (handle)
    {
    case Envelop::HANDLE_DELAY:
        valueTypes << Envelop::DELAY;
        break;
    case Envelop::HANDLE_ATTACK:
        valueTypes << Envelop::ATTACK;
        break;
    case Envelop::HANDLE_HOLD:
        valueTypes << Envelop::HOLD;
        break;
    case Envelop::HANDLE_HOLD_KEY_MAX: case Envelop::HANDLE_HOLD_KEY_MIN:
        valueTypes << Envelop::HOLD << Envelop::KEYNUM_TO_HOLD;
        break;
    case Envelop::HANDLE_DECAY:
        valueTypes << Envelop::DECAY << Envelop::SUSTAIN;
        break;
    case Envelop::HANDLE_DECAY_KEY_MAX: case Envelop::HANDLE_DECAY_KEY_MIN:
        valueTypes << Envelop::DECAY << Envelop::KEYNUM_TO_DECAY;
        break;
    case Envelop::HANDLE_SUSTAIN:
        valueTypes << Envelop::SUSTAIN;
        break;
    case Envelop::HANDLE_RELEASE:
        valueTypes << Envelop::RELEASE;
        break;
    default:
        break;
    }
    return valueTypes;
}

void GraphicsViewEnvelop::startEdition(QPoint pos)
{
    _editedEnvelop = _hoveredEnvelop;
    _editedHandle = _hoveredHandle;
    _valuesEdited = false;
    _axisXActive = _axisYActive = false;
    _pivotOnCenter = false;
    _editStartPos = _editLastPos = pos;
    _editStartCoord = _editCoord = _envelops[_editedEnvelop]->getHandleCoordinates(_editedHandle);

    // Initial values of all envelops that can be edited
    QList<Envelop::ValueType> valueTypes;
    valueTypes << Envelop::DELAY << Envelop::ATTACK << Envelop::HOLD << Envelop::DECAY << Envelop::SUSTAIN
               << Envelop::RELEASE << Envelop::KEYNUM_TO_HOLD << Envelop::KEYNUM_TO_DECAY
               << Envelop::KEY_MIN << Envelop::KEY_MAX;
    _editStartValues.clear();
    foreach (int index, _envelops.keys())
    {
        if (!_envelops[index]->isEditable())
            continue;
        foreach (Envelop::ValueType valueType, valueTypes)
            _editStartValues[index][valueType] = _envelops[index]->get(valueType);
    }

    // The scale doesn't change during the edition
    _editRange = _sizeX / _zoomX;
    _editOffset = (_sizeX - _editRange) * _posX;
    _editTriggeredKeyDuration = _triggeredKeyDuration;
    _editReleasedKeyDuration = _releasedKeyDuration;
    _editSizeX = _sizeX;
    _editZoomX = _zoomX;
    _editPosX = _posX;
    _edgeDirection = 0;
    _sampleDataTimer.start();

    // For Escape and Ctrl
    this->grabKeyboard();
    this->update();
}

void GraphicsViewEnvelop::editValues()
{
    if (!_envelops.contains(_editedEnvelop))
        return;

    // New values of the edited envelop
    const QMap<Envelop::ValueType, double> startValues = _editStartValues[_editedEnvelop];
    QMap<Envelop::ValueType, double> values = startValues;
    const double position = _axisXActive ? _editCoord.x() : _editStartCoord.x();
    const double level = _axisYActive ? _editCoord.y() : _editStartCoord.y();
    const double keyMin = startValues[Envelop::KEY_MIN];
    const double keyMax = startValues[Envelop::KEY_MAX];
    const double keyCenter = 0.5 * (keyMin + keyMax);
    const double attackEnd = startValues[Envelop::DELAY] + startValues[Envelop::ATTACK];
    const bool isKeyMax = (_editedHandle == Envelop::HANDLE_HOLD_KEY_MAX || _editedHandle == Envelop::HANDLE_DECAY_KEY_MAX);
    const double key = isKeyMax ? keyMax : keyMin;
    const double pivotKey = _pivotOnCenter ? keyCenter : (isKeyMax ? keyMin : keyMax);

    switch (_editedHandle)
    {
    case Envelop::HANDLE_DELAY:
        values[Envelop::DELAY] = position;
        break;
    case Envelop::HANDLE_ATTACK:
        values[Envelop::ATTACK] = position - startValues[Envelop::DELAY];
        break;
    case Envelop::HANDLE_HOLD:
        scaleForKey(values, Envelop::HOLD, Envelop::KEYNUM_TO_HOLD, keyCenter, position - attackEnd);
        break;
    case Envelop::HANDLE_HOLD_KEY_MAX: case Envelop::HANDLE_HOLD_KEY_MIN:
        solveForKeys(values, Envelop::HOLD, Envelop::KEYNUM_TO_HOLD, key, position - attackEnd, pivotKey);
        break;
    case Envelop::HANDLE_DECAY:
        scaleForKey(values, Envelop::DECAY, Envelop::KEYNUM_TO_DECAY, keyCenter, position - attackEnd -
                    Envelop::getValueForKey(startValues[Envelop::HOLD], startValues[Envelop::KEYNUM_TO_HOLD], keyCenter));
        values[Envelop::SUSTAIN] = level;
        break;
    case Envelop::HANDLE_DECAY_KEY_MAX: case Envelop::HANDLE_DECAY_KEY_MIN:
        solveForKeys(values, Envelop::DECAY, Envelop::KEYNUM_TO_DECAY, key, position - attackEnd -
                     Envelop::getValueForKey(startValues[Envelop::HOLD], startValues[Envelop::KEYNUM_TO_HOLD], key), pivotKey);
        break;
    case Envelop::HANDLE_SUSTAIN:
        values[Envelop::SUSTAIN] = level;
        break;
    case Envelop::HANDLE_RELEASE:
        values[Envelop::RELEASE] = position - _triggeredKeyDuration;
        break;
    default:
        break;
    }
    limitValues(values);

    // Same variation for all envelops: ratio for the durations, offset for the other values
    QList<Envelop::ValueType> valueTypes = getValueTypes(_editedHandle);
    QList<int> editedTypes;
    foreach (Envelop::ValueType valueType, valueTypes)
        if (qAbs(values[valueType] - startValues[valueType]) > 1e-9 * qMax(1., qAbs(startValues[valueType])))
            editedTypes << valueType;
    foreach (int index, _editStartValues.keys())
    {
        QMap<Envelop::ValueType, double> envValues = _editStartValues[index];
        foreach (Envelop::ValueType valueType, valueTypes)
        {
            if (valueType == Envelop::SUSTAIN || valueType == Envelop::KEYNUM_TO_HOLD || valueType == Envelop::KEYNUM_TO_DECAY)
                envValues[valueType] += values[valueType] - startValues[valueType];
            else if (startValues[valueType] > 0)
                envValues[valueType] *= values[valueType] / startValues[valueType];
            else
                envValues[valueType] = values[valueType];
        }
        limitValues(envValues);
        foreach (Envelop::ValueType valueType, valueTypes)
            _envelops[index]->set(valueType, envValues[valueType], true);
    }

    updateEditionLayout();
    updateHandles();
    this->update();

    // Notify the changes, possibly a return to the initial values
    if (!editedTypes.isEmpty() || _valuesEdited)
    {
        _valuesEdited = true;
        emit valuesEdited(editedTypes);
    }
}

void GraphicsViewEnvelop::scaleForKey(QMap<Envelop::ValueType, double> &values, Envelop::ValueType type,
                                      Envelop::ValueType keyType, double key, double duration)
{
    // Duration for a key: all keys are scaled, the key modifier being unchanged
    duration = qMax(duration, _valueRanges.value(type, QPair<double, double>(0.001, 100)).first);
    values[type] = duration / Envelop::getValueForKey(1, values[keyType], key);
}

void GraphicsViewEnvelop::solveForKeys(QMap<Envelop::ValueType, double> &values, Envelop::ValueType type,
                                       Envelop::ValueType keyType, double key, double duration, double pivotKey)
{
    // Duration for a key, the duration of the pivot key being unchanged
    // Timecents are linear with the key: tc(key) = tc(60) + keyModifier * (60 - key)
    duration = qMax(duration, _valueRanges.value(type, QPair<double, double>(0.001, 100)).first);
    double pivotDuration = Envelop::getValueForKey(values[type], values[keyType], pivotKey);
    QPair<double, double> range = _valueRanges.value(keyType, QPair<double, double>(-1200, 1200));
    double keyModifier = qRound(qBound(range.first, 1200. * std::log2(duration / pivotDuration) / (pivotKey - key), range.second));
    values[keyType] = keyModifier;
    values[type] = pivotDuration / Envelop::getValueForKey(1, keyModifier, pivotKey);
}

void GraphicsViewEnvelop::limitValues(QMap<Envelop::ValueType, double> &values)
{
    foreach (Envelop::ValueType valueType, _valueRanges.keys())
        if (values.contains(valueType))
            values[valueType] = qBound(_valueRanges[valueType].first, values[valueType], _valueRanges[valueType].second);
}

void GraphicsViewEnvelop::updateEditionLayout()
{
    // The scale is kept, the release line being pushed if needed
    double attackDuration = 0;
    double releaseDuration = 0;
    foreach (Envelop * env, _envelops)
    {
        attackDuration = qMax(attackDuration, env->getAttackDuration());
        releaseDuration = qMax(releaseDuration, env->getReleaseDuration());
    }
    double margin = 0.05 * _editRange;
    double previousTriggeredKeyDuration = _triggeredKeyDuration;
    _triggeredKeyDuration = qMax(_editTriggeredKeyDuration, attackDuration + margin);
    _releasedKeyDuration = qMax(_editReleasedKeyDuration, releaseDuration + margin);
    _sizeX = qMax(_triggeredKeyDuration + _releasedKeyDuration, _editOffset + _editRange);
    _releasedKeyDuration = _sizeX - _triggeredKeyDuration;
    _zoomX = _sizeX / _editRange;
    _posX = (_sizeX - _editRange > 1e-9) ? _editOffset / (_sizeX - _editRange) : 0;

    foreach (Envelop * env, _envelops)
        env->computePoints(_triggeredKeyDuration, _releasedKeyDuration);

    // The waveform follows the release line (not too often)
    if (!_sampleData.isEmpty() && _triggeredKeyDuration != previousTriggeredKeyDuration && _sampleDataTimer.elapsed() > 100)
    {
        computeSampleData();
        _sampleDataTimer.start();
    }
}

void GraphicsViewEnvelop::onEdgeTimer()
{
    if (_editedEnvelop == -1 || _edgeDirection == 0)
    {
        _edgeTimer->stop();
        return;
    }

    // Nothing more if the point doesn't follow the mouse anymore (limit reached)
    double handleX = _envelops[_editedEnvelop]->getHandlePixels(_editedHandle).x();
    if ((_edgeDirection > 0 && handleX < this->width() - 3 * EDGE_MARGIN) || (_edgeDirection < 0 && handleX > 3 * EDGE_MARGIN))
        return;

    // Faster if the mouse is far from the edge
    double distance = (_edgeDirection > 0) ? _mousePos.x() - this->width() + EDGE_MARGIN : EDGE_MARGIN - _mousePos.x();
    double speed = 0.01 * qBound(1., 1. + distance / 20., 6.);

    if (_edgeDirection < 0)
    {
        // Scroll to the left
        double shift = qMin(speed * _editRange, _editOffset);
        _editOffset -= shift;
        _editCoord.rx() -= shift;
    }
    else if (_editOffset + _editRange < _editSizeX - 1e-9)
    {
        // Scroll to the right, until the end of the initial graph
        double shift = qMin(speed * _editRange, _editSizeX - _editOffset - _editRange);
        _editOffset += shift;
        _editCoord.rx() += shift;
    }
    else
    {
        // Zoom out, the left side being fixed
        double newRange = _editRange * (1. + speed);
        _editCoord.rx() += (newRange - _editRange) *
                qBound(0., static_cast<double>(_mousePos.x() - LEFT_MARGIN) / qMax(1, this->width() - LEFT_MARGIN), 1.);
        _editRange = newRange;
    }

    editValues();
}

void GraphicsViewEnvelop::stopEdition(bool isCancelled)
{
    if (_editedEnvelop == -1)
        return;

    bool valuesEdited = _valuesEdited;
    _edgeTimer->stop();
    this->releaseKeyboard();
    _editedEnvelop = -1;
    _editedHandle = Envelop::HANDLE_NONE;
    _valuesEdited = false;

    // Initial layout and values, until the envelops are possibly updated
    _triggeredKeyDuration = _editTriggeredKeyDuration;
    _releasedKeyDuration = _editReleasedKeyDuration;
    _sizeX = _editSizeX;
    _zoomX = _editZoomX;
    _posX = _editPosX;
    if (isCancelled)
    {
        foreach (int index, _editStartValues.keys())
            foreach (Envelop::ValueType valueType, _editStartValues[index].keys())
                _envelops[index]->set(valueType, _editStartValues[index][valueType], true);
    }
    foreach (Envelop * env, _envelops)
        env->computePoints(_triggeredKeyDuration, _releasedKeyDuration);
    this->update();

    if (valuesEdited)
    {
        if (isCancelled)
            emit editionCancelled();
        else
            emit editionFinished();
    }
}

bool GraphicsViewEnvelop::abortEdition()
{
    if (_editedEnvelop == -1)
        return false;

    // Stop without notification
    bool valuesEdited = _valuesEdited;
    _valuesEdited = false;
    stopEdition(true);
    return valuesEdited;
}

void GraphicsViewEnvelop::drawLegend(QPainter * painter)
{
    if (!_envelops.contains(_hoveredEnvelop) || _hoveredHandle == Envelop::HANDLE_NONE ||
        (_editedEnvelop == -1 && !_mouseInside))
        return;

    // Texts
    Envelop * env = _envelops[_hoveredEnvelop];
    QStringList values;
    QStringList labels = getLegendLines(values);
    int editableCount = 0;
    foreach (Envelop * envTmp, _envelops)
        if (envTmp->isEditable())
            editableCount++;
    QString name = (editableCount > 1) ? env->getName() : "";

    // Size
    const int border = 5;
    QFont font(this->font().family(), 8);
    QFont fontB = font;
    fontB.setBold(true);
    QFontMetrics fm(font);
    int labelWidth = 0, valueWidth = 0;
    for (int i = 0; i < labels.count(); i++)
    {
        labelWidth = qMax(labelWidth, fm.horizontalAdvance(labels[i]));
        valueWidth = qMax(valueWidth, fm.horizontalAdvance(values[i]));
    }
    int width = qMax(labelWidth + 2 * border + valueWidth, QFontMetrics(fontB).horizontalAdvance(name)) + 2 * border;
    int lineCount = labels.count() + (name.isEmpty() ? 0 : 1);
    int height = lineCount * fm.height() + 2 * border;

    // In the top corner opposite to the mouse
    int x = (_mousePos.x() < this->width() / 2) ? this->width() - width - 5 : 5;
    QRect rect(x, 5, width, height);

    // Paint foreground and border
    QColor color = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    painter->setPen(QPen(color, 1, Qt::SolidLine));
    color.setAlpha(200);
    painter->setBrush(color);
    painter->drawRect(rect);

    // Paint text
    painter->setPen(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT));
    int line = 0;
    if (!name.isEmpty())
    {
        painter->setFont(fontB);
        painter->drawText(QRect(x + border, 5 + border, width - 2 * border, fm.height()), Qt::AlignLeft | Qt::AlignVCenter, name);
        line++;
    }
    painter->setFont(font);
    for (int i = 0; i < labels.count(); i++)
    {
        QRect lineRect(x + border, 5 + border + (line + i) * fm.height(), width - 2 * border, fm.height());
        painter->drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, labels[i]);
        painter->drawText(lineRect, Qt::AlignRight | Qt::AlignVCenter, values[i]);
    }
}

QStringList GraphicsViewEnvelop::getLegendLines(QStringList &values)
{
    Envelop * env = _envelops[_hoveredEnvelop];
    QStringList labels;

    // Duration for the key of a ruler
    if (_hoveredHandle == Envelop::HANDLE_HOLD_KEY_MAX || _hoveredHandle == Envelop::HANDLE_HOLD_KEY_MIN ||
        _hoveredHandle == Envelop::HANDLE_DECAY_KEY_MAX || _hoveredHandle == Envelop::HANDLE_DECAY_KEY_MIN)
    {
        bool isHold = (_hoveredHandle == Envelop::HANDLE_HOLD_KEY_MAX || _hoveredHandle == Envelop::HANDLE_HOLD_KEY_MIN);
        bool isKeyMax = (_hoveredHandle == Envelop::HANDLE_HOLD_KEY_MAX || _hoveredHandle == Envelop::HANDLE_DECAY_KEY_MAX);
        Envelop::ValueType valueType = isHold ? Envelop::HOLD : Envelop::DECAY;
        int key = qRound(env->get(isKeyMax ? Envelop::KEY_MAX : Envelop::KEY_MIN));
        labels << _valueLabels.value(valueType) + " (" + ContextManager::keyName()->getKeyName(key) + ")";
        values << valueToString(env, valueType, Envelop::getValueForKey(
                                    env->get(valueType), env->get(isHold ? Envelop::KEYNUM_TO_HOLD : Envelop::KEYNUM_TO_DECAY), key));
    }

    // Values changed by the point
    foreach (Envelop::ValueType valueType, getValueTypes(_hoveredHandle))
    {
        labels << _valueLabels.value(valueType);
        values << valueToString(env, valueType, env->get(valueType));
    }

    return labels;
}

QString GraphicsViewEnvelop::valueToString(Envelop * env, Envelop::ValueType type, double value)
{
    // Sustain is stored as a level between 0 and 1
    if (type == Envelop::SUSTAIN)
        value = (1. - value) * (env->isVolume() ? 144. : 100.);
    return QLocale().toString(value, 'f', _valueDecimals.value(type, 3)) + _valueSuffixes.value(type);
}
