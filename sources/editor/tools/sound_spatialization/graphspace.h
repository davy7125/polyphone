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

#ifndef GRAPHSPACE_H
#define GRAPHSPACE_H

#include <QWidget>
#include <QMouseEvent>

class GraphSpace : public QWidget
{
    Q_OBJECT

public:
    explicit GraphSpace(QWidget *parent = nullptr);
    void setData(QVector<double> x, QVector<int> y);

    bool eventFilter(QObject* o, QEvent* e) override
    {
        if ((e->type() == QEvent::MouseMove ||
             e->type() == QEvent::MouseButtonPress ||
             e->type() == QEvent::MouseButtonRelease ||
             e->type() == QEvent::Leave)
                && o == this)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
            if (mouseEvent->type() == QEvent::MouseMove)
                this->mouseMoved(mouseEvent->pos());
            else if (mouseEvent->type() == QEvent::Leave)
                this->mouseLeft();
            return true;
        }
        return false;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void mouseMoved(QPoint pos);
    void mouseLeft();

    // Configuration
    QColor _backgroundColor;

    // Data
    QVector<double> _xPan, _yLength;
    QVector<int> _yKey;

    // Coordinates
    QString _currentLabel;
    double _currentPan, _currentLength;
};

#endif // GRAPHSPACE_H
