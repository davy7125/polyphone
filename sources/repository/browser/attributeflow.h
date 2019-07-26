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

#ifndef ATTRIBUTEFLOW_H
#define ATTRIBUTEFLOW_H

#include <QWidget>
#include "soundfontinformation.h"
class FlowLayout;
class SoundfontFilter;
class ElidedPushButton;

class AttributeFlow : public QWidget
{
    Q_OBJECT

public:
    explicit AttributeFlow(QWidget *parent = nullptr);
    ~AttributeFlow();
    void addCategory(int id);
    void addProperty(SoundfontInformation::Property property, QString propertyValue);
    void addTag(QString tagName);

signals:
    // Signal emitted when an item is clicked
    // The filter must be deleted then
    void itemClicked(SoundfontFilter * filter);

protected:
    void resizeEvent(QResizeEvent * event);

private slots:
    void onClick(bool checked);

private:
    enum FilterType
    {
        CATEGORY,
        PROPERTY,
        TAG
    };

    class FilterDefinition
    {
    public:
        FilterType _type;
        int _id;
        SoundfontInformation::Property _property;
        QString _propertyValue;
        QString _tag;
    };

    ElidedPushButton * createItem(QString text);

    FlowLayout * _layout;
    QMap<ElidedPushButton *, FilterDefinition> _filterDefinitions;
};

#endif // ATTRIBUTEFLOW_H
