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

#include "graphparamglobal.h"
#include "contextmanager.h"
#include <QPainter>
#include <QPaintEvent>

GraphParamGlobal::GraphParamGlobal(QWidget * parent) : QWidget(parent),
    forme(FORME_MANUELLE),
    flagEdit(false),
    limitEdit(0),
    _patternStiffnessExp(50.0),
    _patternYmin(0.), _patternYmax(1.),
    _patternXmin(0), _patternXmax(140),
    previousX(-1),
    _highlightedRangeMin(-1),
    _highlightedRangeMax(-1)
{
    // Prepare data
    this->_curve.resize(128);
    this->_curve.fill(0.5);

    // Colors
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    _octaveColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _octaveColor.setAlpha(40);
    _octaveNameColor = _octaveColor;
    _octaveNameColor.setAlpha(180);
    _curveColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _keyRangeColor = _curveColor;
    _keyRangeColor.setAlpha(20);

    // Enable the mouse tracking for the mouse move event
    this->setMouseTracking(true);
}

GraphParamGlobal::~GraphParamGlobal()
{
}

void GraphParamGlobal::setHighlightedRange(int minKey, int maxKey)
{
    _highlightedRangeMin = minKey;
    _highlightedRangeMax = maxKey;
    this->repaint();
}

void GraphParamGlobal::indexMotifChanged(int motif)
{
    this->forme = static_cast<TypeForme>(motif);
    this->flagEdit = false;
    this->limitEdit = 0;
    this->previousX = -1;
    this->_patternXmin = 0;
    this->_patternXmax = 127;
    this->computePattern();
}

void GraphParamGlobal::raideurChanged(double value)
{
    this->_patternStiffnessExp = value;
    this->computePattern();
}

QVector<float> GraphParamGlobal::getValues()
{
    return this->_curve;
}

void GraphParamGlobal::setValues(QVector<float> val)
{
    for (int i = 0; i <= qMin(127, val.size()); i++)
        _curve[i] = val.at(i);
}

void GraphParamGlobal::computePattern()
{
    // Ecriture début et fin
    if (this->forme == FORME_LINEAIRE_ASC || this->forme == FORME_EXP_ASC)
    {
        // Remplissage de 0 au début
        for (int i = 0; i < this->_patternXmin; i++)
            this->_curve[i] = 0;
        // Remplissage de 1 à la fin
        for (int i = this->_patternXmax; i <= 127; i++)
            this->_curve[i] = 1;
    }
    else if (this->forme == FORME_LINEAIRE_DESC || this->forme == FORME_EXP_DESC)
    {
        // Remplissage de 1 au début
        for (int i = 0; i < this->_patternXmin; i++)
            this->_curve[i] = 1;
        // Remplissage de 0 à la fin
        for (int i = this->_patternXmax; i <= 127; i++)
            this->_curve[i] = 0;
    }

    switch (this->forme)
    {
    case FORME_MANUELLE:
        break;
    case FORME_LINEAIRE_ASC:
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = (double)(i - this->_patternXmin) / (this->_patternXmax - this->_patternXmin);
        break;
    case FORME_LINEAIRE_DESC:
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = 1.0 - (double)(i - this->_patternXmin) / (this->_patternXmax - this->_patternXmin);
        break;
    case FORME_EXP_ASC:{
        double baseExp = 1. + 1.0 / (101.0 - this->_patternStiffnessExp);
        double alpha = 1.0 / (pow(baseExp, this->_patternXmax - this->_patternXmin) - 1.0);
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = alpha * (pow(baseExp, i - this->_patternXmin) - 1.0);
    }break;
    case FORME_EXP_DESC:{
        double baseExp = 1. + 1.0 / (101.0 - this->_patternStiffnessExp);
        double alpha = 1.0 / (pow(baseExp, this->_patternXmin - this->_patternXmax) - 1.0);
        for (int i = this->_patternXmin; i < this->_patternXmax; i++)
            this->_curve[i] = 1.0 - alpha * (pow(baseExp, this->_patternXmin - i) - 1.0);
    }break;
    case FORME_ALEATOIRE:
        for (int i = 0; i <= 127; i++)
        {
            double valTmp = (double)(QRandomGenerator::global()->generate() % 2000) / 1000. - 1.;
            double exp = qExp((50. - this->_patternStiffnessExp) / 15.);
            if (valTmp < 0)
                this->_curve[i] = -qPow(qAbs(valTmp), exp);
            else
                this->_curve[i] = qPow(qAbs(valTmp), exp);
            this->_curve[i] = (this->_curve[i] + 1.) / 2;
        }
        break;
    }

    // Display
    this->repaint();
}

void GraphParamGlobal::write(QPoint pos)
{
    // Convert coordinate
    int x = posToKey(pos.x());
    double y = posToValue(pos.y());
    if (x < 0)
        x = 0;
    else if (x > 127)
        x = 127;
    if (y < 0)
        y = 0;
    else if (y > 1)
        y = 1;

    // Change value
    if (this->previousX >= 0 && this->previousX != x)
    {
        if (this->previousX < x)
        {
            for (int i = this->previousX + 1; i < x; i++)
                this->_curve[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
        else
        {
            for (int i = this->previousX - 1; i > x; i--)
                this->_curve[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
    }
    this->_curve[x] = y;

    // Store the point
    this->previousX = x;
    this->previousY = y;

    // Display
    this->repaint();
}

void GraphParamGlobal::afficheCoord(float x, float y)
{
    _currentValuePosX = x;
    _currentValuePosY = y;
}

int GraphParamGlobal::posToKey(int x)
{
    float tmp = static_cast<float>(x) / this->size().width();
    int key = static_cast<int>(tmp * 129 + 0.5) - 1;
    if (key <= 0)
        return 0;
    if (key > 127)
        return 127;
    return key;
}

float GraphParamGlobal::keyToPos(int key, float &w)
{
    w = static_cast<float>(this->size().width()) / 129.;
    return static_cast<float>(key + 1) * w + 0.5f;
}

float GraphParamGlobal::posToValue(int y)
{
    return (1.0f - static_cast<float>(y) / this->size().height()) * 127.f;
}

float GraphParamGlobal::valueToPos(float value)
{
    return (1.0f - value / 127.f) * this->size().height();
}

void GraphParamGlobal::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (this->flagEdit)
    {
        this->afficheCoord(-1, -1);
        this->write(pos);
    }
    else if (this->limitEdit == -1 && forme != FORME_ALEATOIRE)
    {
        // Modification min X
        int x = posToKey(pos.x());
        if (x < 0)
            x = 0;
        else if (x > 127)
            x = 127 ;
        if (x < _patternXmax)
        {
            _patternXmin = x;
            this->computePattern();
        }
    }
    else if (this->limitEdit == 1 && forme != FORME_ALEATOIRE)
    {
        // Modification max X
        int x = posToKey(pos.x());
        if (x < 0)
            x = 0;
        else if (x > 127)
            x = 127;
        if (x > _patternXmin)
        {
            _patternXmax = x;
            this->computePattern();
        }
    }
    else
    {
        // Convert coordinates
        double x = posToKey(pos.x());
        double y = posToValue(pos.y());

        // Closest point
        double distanceMin = -1;
        int posX = -1;
        for (int i = 0; i < this->_curve.size(); i++)
        {
            double distanceTmp = 0.05 * qAbs(x - i) + qAbs(y - this->_curve[i]);
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                posX = i;
            }
        }
        if (posX != -1)
            this->afficheCoord(posX, this->_curve[posX]);
        else
            this->afficheCoord(-1, -1);
    }
}

void GraphParamGlobal::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if (event->button() == Qt::LeftButton)
    {
        this->afficheCoord(-1, -1);
        if (this->forme == FORME_MANUELLE)
        {
            this->flagEdit = true;
            // Inscription du premier point
            this->write(pos);
        }
        else
        {
            // Modification min X
            this->limitEdit = -1;
            int x = posToKey(pos.x());
            if (x < _patternXmax)
            {
                _patternXmin = x;
                this->computePattern();
            }
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (this->forme != FORME_MANUELLE)
        {
            this->afficheCoord(-1, -1);
            // Modification max X
            this->limitEdit = 1;
            int x = posToKey(pos.x());
            if (x > _patternXmin)
            {
                _patternXmax = x;
                this->computePattern();
            }
        }
    }
}

void GraphParamGlobal::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (this->limitEdit == -1 || this->flagEdit)
        {
            this->limitEdit = 0;
            this->flagEdit = false;
            this->previousX = -1;
            this->repaint();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (this->limitEdit == 1)
        {
            this->limitEdit = 0;
            this->repaint();
        }
    }
}

void GraphParamGlobal::leaveEvent(QEvent *event)
{
    this->afficheCoord(-1, -1);
}

void GraphParamGlobal::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QSize size = this->size();

    // Background color
    painter.fillRect(this->rect(), _backgroundColor);

    // Draw octaves
    painter.setPen(_octaveNameColor);
    painter.setFont(QFont(font().family(), 8, QFont::Bold));
    float w;
    for (int i = 12; i < 127; i += 12)
    {
        float x = this->keyToPos(i, w);
        painter.fillRect(QRectF(x - 0.5f * w, 0, w, size.height()), _octaveColor);
        painter.drawText(x - 200, size.height() - 200, 400, 200, Qt::AlignBottom | Qt::AlignHCenter,
            ContextManager::keyName()->getKeyName(i));
    }

    // Highlighted range
    if ((_highlightedRangeMin != 0 || _highlightedRangeMax != 127) && _highlightedRangeMin >= 0 &&
        _highlightedRangeMin <= _highlightedRangeMax && _highlightedRangeMax <= 127)
    {
        float xMin = this->keyToPos(_highlightedRangeMin, w);
        float xMax = this->keyToPos(_highlightedRangeMax, w);
        painter.fillRect(QRectF(xMin - 0.5f * w, 0, xMax + 0.5f * w, size.height()), _keyRangeColor);
    }

    // Curve
    painter.setPen(QPen(_curveColor, 4));
    for (int i = 0; i <= 127; i++)
        painter.drawPoint(keyToPos(i, w), valueToPos(_curve[i]));

    // // Layer aperçu valeurs
    // this->addGraph();
    // graphPen.setColor(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT));
    // graphPen.setWidth(1);
    // this->graph(4)->setPen(graphPen);
    // this->graph(4)->setScatterStyle(QCPScatterStyle::ssPlus);
    // labelCoord = new QCPItemText(this);
    // labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    // labelCoord->setText("");
    // QFont fontLabel = QFont(font().family(), 9);
    // fontLabel.setBold(true);
    // labelCoord->setFont(fontLabel);
    // labelCoord->setColor(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT));


    // Current coordinates
    // QVector<double> xVector, yVector;
    // if (x >= - 0.5)
    // {
    //     // Coordonnées du point
    //     xVector.resize(1);
    //     yVector.resize(1);
    //     xVector[0] = x;
    //     yVector[0] = y;
    //     // Affichage texte
    //     if (y >= 0.5)
    //         labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
    //     else
    //         labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    //     labelCoord->setText(ContextManager::keyName()->getKeyName(qRound((double)x * 128. / this->nbPoints)) + ":" +
    //                         QLocale::system().toString(yMin + (yMax - yMin) * y, 'f', 2));
    //     // Ajustement position
    //     QFontMetrics fm(labelCoord->font());
    //     double distX = this->xAxis->pixelToCoord(fm.horizontalAdvance(labelCoord->text()) / 2 + 2);
    //     if (x < distX)
    //         x = distX;
    //     else if (x > this->nbPoints - distX)
    //         x = this->nbPoints - distX;
    //     double distY = 1. - this->yAxis->pixelToCoord(2);
    //     if (y >= 0.5)
    //         y -= distY;
    //     else
    //         y += distY;
    //     labelCoord->position->setCoords(x, y);
    // }
    // else
    // {
    //     xVector.resize(0);
    //     yVector.resize(0);
    //     labelCoord->setText("");
    // }
    // this->graph(4)->setData(xVector, yVector, true);
    // this->replot();
}
