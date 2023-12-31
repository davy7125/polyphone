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

#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <QWidget>
#include <QVector>
class MainTabBarElement;

class MainTabBar : public QWidget
{
    Q_OBJECT

public:
    MainTabBar(QWidget *parent = nullptr);

    void addWidget(QWidget * widget, QString iconName, QString label, bool isColored);
    void removeWidget(QWidget * widget);
    void setWidgetLabel(QWidget * widget, const QString &label);
    void setWidgetToolTip(QWidget * widget, const QString &tip);
    void currentWidgetChanged(QWidget * widget);

    QSize minimumSizeHint() const override;

signals:
    void widgetClicked(QWidget * widget);
    void closeClicked(QWidget * widget);

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int itemAt(const QPoint &pos);
    static void adaptWidths(QVector<int> &widths, int maxWidth);
    static int sum(QVector<int> &v);
    static void getTheTwoBiggestWidths(QVector<int> &v, int &firstBiggest, int &secondBiggest, int &firstBiggestCount);

    QVector<MainTabBarElement *> _tabs;
    int _clickedItemIndex;
    int _clickedInCloseButton;
    int _clickedPosX;
    int _xShift;
};

#endif // MAINTABBAR_H
