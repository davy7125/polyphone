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

#include "linkedtowidget.h"
#include "soundfontmanager.h"
#include "flowlayout.h"
#include <QPushButton>

LinkedToWidget::LinkedToWidget(QWidget *parent) :
    QWidget(parent)
{
    _layout = new FlowLayout(this);
    _layout->setMargin(0);
    this->setLayout(_layout);
}

LinkedToWidget::~LinkedToWidget()
{
    this->clear();
    delete _layout;
}

void LinkedToWidget::clear()
{
    QList<QWidget*> buttons = _buttonIds.keys();
    while (!buttons.isEmpty())
        delete buttons.takeFirst();
    _buttonIds.clear();
}

void LinkedToWidget::initialize(EltID id)
{
    this->clear();
    if (id.typeElement != elementSmpl && id.typeElement != elementInst)
        return;

    EltID id2 = id;
    id2.typeElement = (id.typeElement == elementSmpl ? elementInst : elementPrst);
    EltID id3 = id;
    id3.typeElement = (id.typeElement == elementSmpl ? elementInstSmpl : elementPrstInst);
    AttributeType elementToFind = (id.typeElement == elementSmpl ? champ_sampleID : champ_instrument);

    // Browse all instruments or presets
    SoundfontManager * sf2 = SoundfontManager::getInstance();
    foreach (int i, sf2->getSiblings(id2))
    {
        id2.indexElt = i;
        id3.indexElt = i;

        // Browse all linked elements
        foreach (int j, sf2->getSiblings(id3))
        {
            id3.indexElt2 = j;
            if (sf2->get(id3, elementToFind).wValue == id.indexElt)
            {
                // Add a link
                this->addLink(id2, sf2->getQstr(id2, champ_name));
                break;
            }
        }
    }
}

void LinkedToWidget::addLink(EltID id, QString text)
{
    QPushButton * button = new QPushButton(this);
    button->setText(text);
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(24);
    button->setMaximumHeight(24);
    _buttonIds[button] = id;
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    _layout->addWidget(button);
}

int LinkedToWidget::getLinkNumber()
{
    return _layout->count();
}

void LinkedToWidget::onClick(bool isClicked)
{
    Q_UNUSED(isClicked)
    emit(itemClicked(_buttonIds[(QWidget*)QObject::sender()]));
}
