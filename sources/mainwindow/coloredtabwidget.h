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

#ifndef COLOREDTABWIDGET_H
#define COLOREDTABWIDGET_H

#include <QTabWidget>
#include <QMap>
class QPushButton;

class ColoredTabInfo
{
public:
    QColor _backgroundColor;
    QColor _textColor;
    QString _iconName;
    QPushButton * _closeButton;
};


class ColoredTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    static const QSize TAB_ICON_SIZE;

    ColoredTabWidget(QWidget *parent = nullptr);
    int addColoredTab(QWidget *widget, QString iconName, const QString &label, QColor backgroundColor, QColor textColor);

private slots:
    void onCurrentChanged(int index);
    void onCloseButtonClicked();

private:
    void changeStyleSheet(QColor backgroundColor, QColor textColor);

    QWidget * _lastWidget;
    QColor _defaultWindowColor;
    QColor _defaultTextColor;
    QString _styleSheetFirstPart;
    static const QString s_styleSheetLastPart;
    QMap<QWidget *, ColoredTabInfo> _tabInfo;
};

#endif // COLOREDTABWIDGET_H
