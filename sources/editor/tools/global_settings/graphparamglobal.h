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

#ifndef GRAPHPARAMGLOBAL_H
#define GRAPHPARAMGLOBAL_H

#include <QWidget>
class QMouseEvent;

class GraphParamGlobal : public QWidget
{
    Q_OBJECT

public:
    enum TypeForme
    {
        FORME_MANUELLE,
        FORME_LINEAIRE_ASC,
        FORME_LINEAIRE_DESC,
        FORME_EXP_ASC,
        FORME_EXP_DESC,
        FORME_ALEATOIRE
    };

    explicit GraphParamGlobal(QWidget *parent = nullptr);
    ~GraphParamGlobal();

    void indexMotifChanged(int index);
    void raideurChanged(double value);
    void setHighlightedRange(int minKey, int maxKey);
    void setMinMax(double min, double max)  { _patternYmin = qMin(min, max); _patternYmax = qMax(min, max); }
    void setMinMaxX(int min, int max)       { _patternXmin = qMin(min, max); _patternXmax = qMax(min, max); }
    QVector<float> getValues();
    void setValues(QVector<float> val);
    int getXmin()                           { return _patternXmin; }
    int getXmax()                           { return _patternXmax; }

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    TypeForme forme;
    QVector<float> _curve;
    bool flagEdit;
    int limitEdit;
    double _patternStiffnessExp;
    double _patternYmin, _patternYmax;
    int _patternXmin, _patternXmax;
    int previousX;
    double previousY;
    int _highlightedRangeMin, _highlightedRangeMax;
    float _currentValuePosX, _currentValuePosY;

    QColor _backgroundColor, _octaveColor, _octaveNameColor, _keyRangeColor, _curveColor;

    int posToKey(int x);
    float keyToPos(int key, float &w);
    float posToValue(int y);
    float valueToPos(float value);

    void mouseLeft();
    void computePattern();
    void write(QPoint pos);
    void afficheCoord(float x, float y);
};

#endif // GRAPHPARAMGLOBAL_H
