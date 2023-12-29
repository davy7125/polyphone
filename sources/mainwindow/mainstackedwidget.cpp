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

#include "mainstackedwidget.h"
#include "maintabbar.h"

MainStackedWidget::MainStackedWidget(QWidget *parent) : QStackedWidget(parent),
    _tabBar(nullptr)
{

}

void MainStackedWidget::setTabBar(MainTabBar * tabBar)
{
    // This stacked widget is driven by a tabBar
    _tabBar = tabBar;
    connect(_tabBar, SIGNAL(widgetClicked(QWidget*)), SLOT(onWidgetClicked(QWidget*)));
    connect(_tabBar, SIGNAL(closeClicked(QWidget*)), this, SIGNAL(tabCloseRequested(QWidget*)));

    // Notify the tabbar that the current index changes
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));

    // Add the first tab: home
    _tabBar->addWidget(this->widget(0), ":/icons/home.svg", "", false, true);
}

int MainStackedWidget::addWidgetWithTab(QWidget *widget, QString iconName, const QString &label, bool isColored)
{
    int index = this->addWidget(widget);
    _tabBar->addWidget(widget, iconName, label, isColored, false);
    return index;
}

void MainStackedWidget::removeWidgetWithTab(QWidget * widget)
{
    _tabBar->removeWidget(widget);
    this->removeWidget(widget);
}

void MainStackedWidget::onWidgetClicked(QWidget * widget)
{
    // Change the current widget
    this->setCurrentWidget(widget);
}

void MainStackedWidget::onCurrentChanged(int index)
{
    _tabBar->currentWidgetChanged(this->widget(index));
}

void MainStackedWidget::setWidgetLabel(QWidget * widget, const QString &label)
{
    _tabBar->setWidgetLabel(widget, label);
}

void MainStackedWidget::setWidgetToolTip(QWidget * widget, const QString &tip)
{
    _tabBar->setWidgetToolTip(widget, tip);
}
