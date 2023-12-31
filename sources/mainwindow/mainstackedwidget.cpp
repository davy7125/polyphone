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
#include "qpushbutton.h"
#include "contextmanager.h"

MainStackedWidget::MainStackedWidget(QWidget *parent) : QStackedWidget(parent),
    _tabBar(nullptr)
{

}

void MainStackedWidget::setControls(QPushButton * pushHome, MainTabBar * tabBar)
{
    // This stacked widget is driven by a tabBar and a push button
    _tabBar = tabBar;
    connect(_tabBar, SIGNAL(widgetClicked(QWidget*)), SLOT(onWidgetClicked(QWidget*)));
    connect(_tabBar, SIGNAL(closeClicked(QWidget*)), this, SIGNAL(tabCloseRequested(QWidget*)));
    _pushHome = pushHome;
    connect(_pushHome, SIGNAL(clicked(bool)), SLOT(onHomeCliked(bool)));

    // Home button style
    _homeIcon = ContextManager::theme()->getColoredSvg(
        ":/icons/home.svg", QSize(28, 28),
        ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
            ThemeManager::LIST_TEXT : ThemeManager::LIST_BACKGROUND);
    _homeIconEnabled = ContextManager::theme()->getColoredSvg(":/icons/home.svg", QSize(28, 28), ThemeManager::HIGHLIGHTED_BACKGROUND);
    _pushHome->setIcon(_homeIconEnabled);
    pushHome->setStyleSheet(QString("QPushButton{margin: 0 0 6px 0;padding: 3px;background-color:%1;border-radius: 3px;}")
                                .arg(ContextManager::theme()->getColor(ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
                                                                           ThemeManager::LIST_BACKGROUND : ThemeManager::LIST_TEXT).name()));

    // Notify the tabbar that the current index changes
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

int MainStackedWidget::addWidgetWithTab(QWidget *widget, QString iconName, const QString &label, bool isColored)
{
    int index = this->addWidget(widget);
    _tabBar->addWidget(widget, iconName, label, isColored);
    return index;
}

void MainStackedWidget::removeWidgetWithTab(QWidget * widget)
{
    _tabBar->removeWidget(widget);
    this->removeWidget(widget);
}

void MainStackedWidget::onHomeCliked(bool clicked)
{
    Q_UNUSED(clicked)
    this->setCurrentIndex(0);
}

void MainStackedWidget::onWidgetClicked(QWidget * widget)
{
    // Change the current widget
    this->setCurrentWidget(widget);
}

void MainStackedWidget::onCurrentChanged(int index)
{
    _tabBar->currentWidgetChanged(this->widget(index));
    _pushHome->setIcon(index == 0 ? _homeIconEnabled : _homeIcon);
    _pushHome->setCursor(index == 0 ? Qt::ArrowCursor : Qt::PointingHandCursor);
}

void MainStackedWidget::setWidgetLabel(QWidget * widget, const QString &label)
{
    _tabBar->setWidgetLabel(widget, label);
}

void MainStackedWidget::setWidgetToolTip(QWidget * widget, const QString &tip)
{
    _tabBar->setWidgetToolTip(widget, tip);
}
