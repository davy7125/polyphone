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

#include "attributeflow.h"
#include "flowlayout.h"
#include "repositorymanager.h"
#include "windowmanager.h"
#include "elidedpushbutton.h"

AttributeFlow::AttributeFlow(QWidget *parent) : QWidget(parent)
{
    _layout = new FlowLayout(this, 6, 6);
    _layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(_layout);
}

AttributeFlow::~AttributeFlow()
{
    this->clear();
    delete _layout;
}

void AttributeFlow::clear()
{
    while (!_layout->isEmpty())
        delete _layout->takeAt(0);
}

void AttributeFlow::addCategory(int id)
{
    ElidedPushButton * widget = createItem(RepositoryManager::getInstance()->getCategoryName(id));
    _filterDefinitions[widget]._type = FilterType::CATEGORY;
    _filterDefinitions[widget]._id = id;
    _filters << widget;
    _layout->addWidget(widget);
}

void AttributeFlow::addProperty(SoundfontInformation::Property property, QString propertyValue)
{
    ElidedPushButton * widget = createItem(propertyValue);
    _filterDefinitions[widget]._type = FilterType::PROPERTY;
    _filterDefinitions[widget]._property = property;
    _filterDefinitions[widget]._propertyValue = propertyValue;
    _filters << widget;
    _layout->addWidget(widget);
}

void AttributeFlow::addTag(QString tagName)
{
    ElidedPushButton * widget = createItem(tagName);
    _filterDefinitions[widget]._type = FilterType::TAG;
    _filterDefinitions[widget]._tag = tagName;
    _filters << widget;
    _layout->addWidget(widget);
}

ElidedPushButton * AttributeFlow::createItem(QString text)
{
    ElidedPushButton * button = new ElidedPushButton(this);
    button->setTextToElide(text, this->width() - 12);
    button->setCursor(Qt::PointingHandCursor);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    return button;
}

void AttributeFlow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    foreach (ElidedPushButton * button, _filters)
        button->setAvailableWidth(this->width());
    _layout->setGeometry(QRect(0, 0, this->width(), 0));

    QWidget::resizeEvent(event);
}

void AttributeFlow::onClick(bool checked)
{
    Q_UNUSED(checked)

    // Create a soundfont filter
    ElidedPushButton * sender = dynamic_cast<ElidedPushButton*>(QObject::sender());
    if (_filterDefinitions.contains(sender))
    {
        SoundfontFilter * filter = new SoundfontFilter();
        switch (_filterDefinitions[sender]._type)
        {
        case FilterType::CATEGORY:
            filter->setCategory(_filterDefinitions[sender]._id);
            break;
        case FilterType::PROPERTY:
            filter->setProperties(_filterDefinitions[sender]._property, QStringList(_filterDefinitions[sender]._propertyValue));
            break;
        case FilterType::TAG:
            filter->setTags(QStringList(_filterDefinitions[sender]._tag));
            break;
        }

        // Notify the click with the filter
        emit(itemClicked(filter));
    }
}

void AttributeFlow::polish(QStyle * style)
{
    foreach (ElidedPushButton * button, _filters)
        style->polish(button);
}

bool AttributeFlow::hasHeightForWidth() const
{
    return true;
}

int AttributeFlow::heightForWidth(int width) const
{
    return _layout->heightForWidth(width);
}
