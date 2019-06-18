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

#ifndef WIDGETSHOWHISTORY_H
#define WIDGETSHOWHISTORY_H

#include <QWidget>
class QListWidgetItem;

namespace Ui {
class WidgetShowHistory;
}

class WidgetShowHistory : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetShowHistory(QWidget *parent = nullptr);
    ~WidgetShowHistory();

    /// Clear all files
    void clear();

    /// Add a recent file
    void addFile(QString path, QDateTime datetime);

signals:
    void openFile(QString file);

protected:
    void paintEvent(QPaintEvent *event) override;

    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_itemSelectionChanged();

private:
    Ui::WidgetShowHistory *ui;
    bool _withDecoration;
    QPixmap _decoration;
    static const int SIZE;
    static const int OFFSET_X;
    static const int OFFSET_Y;
    static const double COLOR_RATIO;
};

#endif // WIDGETSHOWHISTORY_H
