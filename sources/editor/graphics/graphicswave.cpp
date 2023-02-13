/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "graphicswave.h"
#include "graphicswavepainter.h"
#include "contextmanager.h"
#include <QPainter>
#include "qmath.h"
#include <QScrollBar>
#include <QSpinBox>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>

const int GraphicsWave::TEXT_MARGIN = 5;
const int GraphicsWave::OVERLAY_SIZE = 20;

GraphicsWave::GraphicsWave(QWidget *parent) : QWidget(parent),
    _wavePainter(new GraphicsWavePainter(this)),
    _zoomX(1),
    _zoomY(1),
    _posX(.5),
    _zoomXinit(1),
    _zoomYinit(1),
    _posXinit(.5),
    _zoomFlag(false),
    _dragFlag(false),
    _cutFlag(false),
    _modifiedFlag(false),
    _sampleSize(0),
    _sampleRate(0),
    _startLoop(0),
    _endLoop(0),
    _currentPosition(0),
    _multipleSelection(false),
    _qScrollX(nullptr),
    _spinStart(nullptr),
    _spinEnd(nullptr),
    _bFromExt(false)
{
    // Colors
    _redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, true);
    _greenColor = ContextManager::theme()->getFixedColor(ThemeManager::GREEN, true);
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
    _textColor.setAlpha(180);

    // Font
    _textFont = this->font();
    _textFont.setBold(true);
}

GraphicsWave::~GraphicsWave()
{
    delete _wavePainter;
}

void GraphicsWave::setData(const QVector<float> &vData, quint32 sampleRate)
{
    // Reset zoom & drag
    _zoomX = 1;
    _zoomY = 1;
    _posX = 0.5;

    // Set data
    _wavePainter->setData(vData);
    _sizeX = static_cast<double>(vData.size()) - 1.0;

    // Save the sample rate
    _sampleRate = sampleRate;
}

void GraphicsWave::linkSliderX(QScrollBar * qScrollX)
{
    _qScrollX = qScrollX;
}

void GraphicsWave::linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd)
{
    _spinStart = spinStart;
    _spinEnd = spinEnd;
}

void GraphicsWave::displayMultipleSelection(bool isOn)
{
    _qScrollX->setEnabled(!isOn);
    _multipleSelection = isOn;
    if (isOn)
        _qScrollX->setRange(0, 0);
    repaint();
}

void GraphicsWave::setPosX(int posX)
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

void GraphicsWave::setStartLoop(int pos, bool repaint)
{
    if (_multipleSelection)
        return;

    _startLoop = static_cast<quint32>(pos);
    if (repaint)
        this->repaint();
}

void GraphicsWave::setEndLoop(int pos, bool repaint)
{
    if (_multipleSelection)
        return;

    _endLoop = static_cast<quint32>(pos);
    if (repaint)
        this->repaint();
}

void GraphicsWave::setCurrentSample(quint32 pos)
{
    _currentPosition = pos;
    repaint();
}

void GraphicsWave::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (_multipleSelection)
    {
        painter.fillRect(this->rect(), _backgroundColor);
        painter.setPen(_textColor);
        QFont font = _textFont;
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(0, 0, this->width(), this->height(),
                         Qt::AlignCenter | Qt::AlignHCenter, tr("Multiple selection"));
        return;
    }

    // Paint the waveform
    double etendueX = _sizeX / _zoomX;
    double offsetX = (_sizeX - etendueX) * _posX - 1;
    quint32 start = offsetX < 0 ? 0 : static_cast<quint32>(offsetX);
    quint32 end = offsetX + etendueX < 0 ? 0 : static_cast<quint32>(offsetX + etendueX);
    if (start >= end)
        return;
    _wavePainter->paint(&painter, start, end, static_cast<float>(_zoomY));

    // Left and right limits
    painter.setPen(_textColor);
    painter.setFont(_textFont);
    int fontHeight = 50;
    painter.drawText(TEXT_MARGIN, this->height() - fontHeight - TEXT_MARGIN,
                     this->width() - TEXT_MARGIN * 2, fontHeight, Qt::AlignLeft | Qt::AlignBottom,
                     QLocale::system().toString(static_cast<double>(start) / _sampleRate, 'f', 3) + " " + tr("s", "unit for seconds"));
    painter.drawText(TEXT_MARGIN, this->height() - fontHeight - TEXT_MARGIN,
                     this->width() - TEXT_MARGIN * 2, fontHeight, Qt::AlignRight | Qt::AlignBottom,
                     QLocale::system().toString(static_cast<double>(end) / _sampleRate, 'f', 3) + " " + tr("s", "unit for seconds"));

    // Ajout du trait de lecture
    double coeff = static_cast<double>(this->width()) / (end - start);
    if (_currentPosition > 0)
    {
        painter.setPen(_textColor);
        int pos = static_cast<int>(coeff * (_currentPosition - start));
        painter.drawLine(pos, -1, pos, this->height() + 1);
    }

    // Display loop
    if (_startLoop != _endLoop)
    {
        // Left vertical bar
        painter.setPen(QPen(_greenColor, 2.0, Qt::SolidLine));
        int pos = static_cast<int>(coeff * (_startLoop - start));
        painter.drawLine(pos, -1, pos, this->height() + 1);

        // Right vertical bar
        painter.setPen(QPen(_redColor, 2.0, Qt::SolidLine));
        pos = static_cast<int>(coeff * (_endLoop - start));
        painter.drawLine(pos, -1, pos, this->height() + 1);

        // Left overlay
        painter.setRenderHint(QPainter::Antialiasing);
        QColor color = _greenColor;
        color.setAlpha(180);
        painter.setPen(QPen(color, 2.0, Qt::DotLine));
        quint32 pointNumber = 0;
        QPointF * points =_wavePainter->getDataAround(_endLoop, OVERLAY_SIZE, pointNumber);
        for (quint32 i = 0; i < pointNumber; i++)
            points[i].setX(coeff * (points[i].x() - start - _endLoop + _startLoop));
        painter.drawPolyline(points, static_cast<int>(pointNumber));
        delete [] points;

        // Right overlay
        color = _redColor;
        color.setAlpha(180);
        painter.setPen(QPen(color, 2.0, Qt::DotLine));
        points =_wavePainter->getDataAround(_startLoop, OVERLAY_SIZE, pointNumber);
        for (quint32 i = 0; i < pointNumber; i++)
            points[i].setX(coeff * (points[i].x() - start - _startLoop + _endLoop));
        painter.drawPolyline(points, static_cast<int>(pointNumber));
        delete [] points;
    }

    if (_cutFlag)
    {
        // Cut area
        if (_x != _xInit || _y != _yInit)
        {
            painter.setRenderHint(QPainter::Antialiasing);
            QColor color = _textColor;
            color.setAlpha(100);
            painter.setPen(color);
            painter.drawRect(QRectF(qMin(_x, _xInit) * this->width(), -1,
                                    qAbs(_x - _xInit) * this->width(), this->height() + 2));
            painter.fillRect(QRectF(qMin(_x, _xInit) * this->width(), -1,
                                    qAbs(_x - _xInit) * this->width(), this->height() + 2),
                             QBrush(color, Qt::BDiagPattern));
        }
    }
    else if (_zoomFlag)
    {
        // Zoom line
        if (_x != _xInit || _y != _yInit)
        {
            painter.setRenderHint(QPainter::Antialiasing);
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

void GraphicsWave::mousePressEvent(QMouseEvent *event)
{
    if (_multipleSelection)
        return;

    if (event->button() == Qt::LeftButton && !_zoomFlag)
    {
        // Save the initial state
#if QT_VERSION < 0x060000
        _xInit = static_cast<double>(event->x()) / this->width();
        _yInit = static_cast<double>(event->y()) / this->height();
#else
        _xInit = static_cast<double>(event->position().x()) / this->width();
        _yInit = static_cast<double>(event->position().y()) / this->height();
#endif
        _zoomXinit = _zoomX;
        _zoomYinit = _zoomY;
        _posXinit = _posX;
        _modifiedFlag = false;

        if (QApplication::keyboardModifiers() == Qt::AltModifier)
            _cutFlag = true;
        else
            _dragFlag = true;
    }
    else if (event->button() == Qt::RightButton && !_dragFlag && !_cutFlag)
    {
        // Save the initial state
#if QT_VERSION < 0x060000
        _xInit = static_cast<double>(event->x()) / this->width();
        _yInit = static_cast<double>(event->y()) / this->height();
#else
        _xInit = static_cast<double>(event->position().x()) / this->width();
        _yInit = static_cast<double>(event->position().y()) / this->height();
#endif
        _zoomXinit = _zoomX;
        _zoomYinit = _zoomY;
        _posXinit = _posX;
        _modifiedFlag = false;

        _zoomFlag = true;
    }
}

void GraphicsWave::mouseReleaseEvent(QMouseEvent *event)
{
    if (_multipleSelection)
        return;

    int startSamplePosition = getSamplePosX(_zoomXinit, _posXinit, _xInit);

    if (event->button() == Qt::LeftButton)
    {
        if (!_modifiedFlag)
        {
            // Modification start loop
            if (this->_spinStart && this->_spinEnd)
            {
                if (this->_spinEnd->value() > startSamplePosition)
                {
                    this->_spinEnd->setMinimum(startSamplePosition);
                    this->_spinStart->setValue(startSamplePosition);
                    emit(startLoopChanged());
                }
            }
            else
                this->setStartLoop(startSamplePosition);
        }
        else
        {
            if (_cutFlag)
            {
                int endSamplePosition = getSamplePosX(
                            _zoomX, _posX,
#if QT_VERSION < 0x060000
                            static_cast<double>(event->x()) / this->width()
#else
                            static_cast<double>(event->position().x()) / this->width()
#endif
                            );
                if (startSamplePosition < endSamplePosition)
                    emit(cutOrdered(startSamplePosition, endSamplePosition));
                else
                    emit(cutOrdered(endSamplePosition, startSamplePosition));
            }
            this->setCursor(Qt::ArrowCursor);
        }

        _dragFlag = false;
        _cutFlag = false;
        this->repaint();
    }
    else if (event->button() == Qt::RightButton)
    {
        if (!_modifiedFlag)
        {
            // Modification end loop
            if (this->_spinStart && this->_spinEnd)
            {
                if (this->_spinStart->value() < startSamplePosition)
                {
                    this->_spinStart->setMaximum(startSamplePosition);
                    this->_spinEnd->setValue(startSamplePosition);
                    emit(endLoopChanged());
                }
            }
            else
                this->setEndLoop(startSamplePosition);
        }
        else
        {
            // Stop the cut process
            _cutFlag = false;

            // Remove the zoom line
            this->setCursor(Qt::ArrowCursor);
        }

        _zoomFlag = false;
        this->repaint();
    }
}

void GraphicsWave::mouseMoveEvent(QMouseEvent *event)
{
    if (_multipleSelection)
        return;
#if QT_VERSION < 0x060000
    _x = static_cast<double>(event->x()) / this->width();
    _y = static_cast<double>(event->y()) / this->height();
#else
    _x = static_cast<double>(event->position().x()) / this->width();
    _y = static_cast<double>(event->position().y()) / this->height();
#endif

    if (_zoomFlag)
    {
        if (!_modifiedFlag)
        {
            _modifiedFlag = true;
            this->setCursor(Qt::SizeAllCursor);
        }

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
    else if (_cutFlag)
    {
        if (!_modifiedFlag)
        {
            _modifiedFlag = true;
            this->setCursor(Qt::UpArrowCursor);
        }
        this->repaint();
    }
}

void GraphicsWave::wheelEvent(QWheelEvent *event)
{
    if (_multipleSelection)
        return;

    if (!_dragFlag && !_zoomFlag && !_cutFlag)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        _qScrollX->setValue(static_cast<qint32>(_qScrollX->value() - 0.2 * event->angleDelta().x()));
#else
        _qScrollX->setValue(static_cast<qint32>(_qScrollX->value() - 0.2 * event->delta()));
#endif
}

int GraphicsWave::getSamplePosX(double zoomX, double shiftX, double x)
{
    int pos = static_cast<int>(_sizeX / zoomX * ((zoomX - 1) * shiftX + x) - 1.0);
    if (pos < 0)
        pos = 0;
    else if (pos > _sizeX)
        pos = qRound(_sizeX - 1.0);
    return pos;
}

void GraphicsWave::zoom(QPoint point)
{
    // Shift
    double decX = static_cast<double>(point.x()) / this->width() - this->_xInit;
    double decY = this->_yInit - static_cast<double>(point.y()) / this->height();

    // Update zoom & drag
    double newZoomX = _zoomXinit * pow(2, 25.0 * decX);
    double newZoomY = _zoomYinit * pow(2,  5.0 * decY);

    if (newZoomX > 20 * _sizeX / this->width())
        newZoomX = 20 * _sizeX / this->width();
    if (newZoomX < 1)
        newZoomX = 1;
    if (newZoomY < 1)
        newZoomY = 1;
    else if (newZoomY > 50)
        newZoomY = 50;

    if (newZoomX != _zoomX || newZoomY != _zoomY)
    {
        _zoomX = newZoomX;
        _zoomY = newZoomY;

        // Update posX
        if (_zoomX > 1)
        {
            _posX = (_zoomX * _posXinit * (_zoomXinit - 1) + _xInit*(_zoomX - _zoomXinit)) / (_zoomXinit * (_zoomX - 1));
            if (_posX < 0)
                _posX = 0;
            else if (_posX > 1)
                _posX = 1;
        }
    }

    this->repaint();
}

void GraphicsWave::drag(QPoint point)
{
    // Shift
    double decX = static_cast<double>(point.x()) / this->width() - this->_xInit;

    // Update posX et posY
    if (_zoomXinit > 1)
        _posX = _posXinit - decX / (_zoomXinit - 1);

    if (_posX < 0)
        _posX = 0;
    else if (_posX > 1)
        _posX = 1;

    this->repaint();
}
