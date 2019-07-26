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

#ifndef FILTERFLOW_H
#define FILTERFLOW_H

#include <QWidget>
class FlowLayout;

class FilterFlow : public QWidget
{
    Q_OBJECT

public:
    explicit FilterFlow(QWidget *parent = nullptr);

    // Clear the list
    void initialize(bool singleSelection = false, QString firstElementText = "");

    // Add an element in the list, with the possibility to link it with an id (for the categories)
    void addElement(QString text, int id = -1, bool selected = false);

    // Change the selection
    void select(QStringList elements);
    void select(QList<int> elements);
    void select(int element);

    // Get all selected elements (as text)
    QStringList getSelectedElements();

    // Get all selected elements (as int)
    QList<int> getSelectedIds();

    // Get the unique selected id (if singleselection is true)
    int getSelectedId();

signals:
    // Emitted when the selection changed
    void selectionChanged();

private slots:
    void buttonClicked();

private:
    QWidget * createItem(QString text, int id, bool selected);

    bool _singleSelection;
    FlowLayout * _layout;
};

#endif // FILTERFLOW_H
