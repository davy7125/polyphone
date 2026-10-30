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

#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <QWidget>
#include <QVector>
class MainTabBarElement;
class Tab;
class QMimeData;

class MainTabBar : public QWidget
{
    Q_OBJECT

public:
    MainTabBar(QWidget *parent = nullptr);
    ~MainTabBar();

    void addTab(Tab * tab, QString iconName, QString label);
    void removeTab(Tab * tab);
    void setTabLabel(Tab * tab, const QString &label);
    void setTabToolTip(Tab * tab, const QString &tip);
    void currentTabChanged(Tab * tab);
    Tab * getCurrentTab();
    Tab * getFirstTab();
    Tab * getPreviousTab();
    Tab * getNextTab();
    Tab * getLastTab();

    QSize minimumSizeHint() const override;

signals:
    void widgetClicked(QWidget * widget);
    void closeClicked(QWidget * widget);
    void openFile(QString file);

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    int itemAt(const QPoint &pos);
    static QStringList getDroppedPaths(const QMimeData * mimeData);
    int adaptWidths(QVector<int> &widths, int maxWidth);
    void goLeft();
    void goRight();
    static int sum(QVector<int> &v);
    static void getTheTwoBiggestWidths(QVector<int> &v, int &firstBiggest, int &secondBiggest, int &firstBiggestCount);

    QPixmap _leftArrowIcon;
    QPixmap _rightArrowIcon;

    QVector<MainTabBarElement *> _tabs;
    int _clickedItemIndex;
    bool _clickedInCloseButton;
    int _clickedPosX;
    int _xShift;
    int _firstTabDisplayed; // 0 is first tab, -1 is align the right tab with the right border
    bool _withArrows;
    int _wheelAngleDelta; // Accumulated while Shift is pressed, for changing the current tab

    static const int TAB_MIN_WIDTH;
    static const int ARROW_WIDTH;
    static const int ARROW_INNER_MARGIN;
};

#endif // MAINTABBAR_H
