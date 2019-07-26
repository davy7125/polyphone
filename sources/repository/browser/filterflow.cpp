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

#include "filterflow.h"
#include "flowlayout.h"
#include "contextmanager.h"
#include <QPushButton>

FilterFlow::FilterFlow(QWidget *parent) : QWidget(parent)
{
    _layout = new FlowLayout(this);
    _layout->setMargin(0);
    this->setLayout(_layout);

    // Style of the buttons
    QColor buttonBackground = ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND);
    QColor buttonBorder = ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
                buttonBackground.lighter(180) : buttonBackground.darker(130);
    QColor buttonTextHover = ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
                buttonBorder.lighter(180) : buttonBorder.darker(200);
    QColor buttonSelectedBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor buttonSelectedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor buttonSelectedHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    this->setStyleSheet("QPushButton{background-color:" + buttonBackground.name() + "; color:" + buttonBorder.name() +
                        ";padding:5px;border-radius:4px;border:1px solid " + buttonBorder.name() + "}" +
                        "QPushButton:hover{color:" + buttonTextHover.name() + "}" +
                        "QPushButton:checked{ background-color:" + buttonSelectedBackground.name() + ";color:" + buttonSelectedText.name() +
                        ";border:1px solid " + buttonSelectedBackground.name() + "}" +
                        "QPushButton:checked:hover{ background-color:" + buttonSelectedHover.name() + ";border:1px solid " + buttonSelectedHover.name() + "}");
}

void FilterFlow::initialize(bool singleSelection, QString firstElementText)
{
    _singleSelection = singleSelection;
    QLayoutItem *item;
    while ((item = _layout->takeAt(0)))
    {
        delete item->widget();
        delete item;
    }

    // Add the element "All"
    this->addElement(firstElementText.isEmpty() ? trUtf8("All") : firstElementText, -1, true);
}

void FilterFlow::addElement(QString text, int id, bool selected)
{
    _layout->addWidget(createItem(text, id, selected));
}

void FilterFlow::select(QStringList elements)
{
    this->blockSignals(true);
    if (elements.empty())
    {
        for (int i = 0; i < _layout->count(); i++)
        {
            QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
            buttonTmp->setChecked(i == 0);
        }
    }
    else
    {
        for (int i = 0; i < _layout->count(); i++)
        {
            QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
            buttonTmp->setChecked(i > 0 && elements.contains(buttonTmp->text()));
        }
    }
    this->blockSignals(false);
}

void FilterFlow::select(QList<int> elements)
{
    this->blockSignals(true);
    if (elements.empty())
    {
        for (int i = 0; i < _layout->count(); i++)
        {
            QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
            buttonTmp->setChecked(i == 0);
        }
    }
    else
    {
        for (int i = 0; i < _layout->count(); i++)
        {
            QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
            buttonTmp->setChecked(i > 0 && elements.contains(buttonTmp->property("id").toInt()));
        }
    }
    this->blockSignals(false);
}

void FilterFlow::select(int element)
{
    this->blockSignals(true);
    for (int i = 0; i < _layout->count(); i++)
    {
        QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
        buttonTmp->setChecked(buttonTmp->property("id").toInt() == element);
    }
    this->blockSignals(false);
}

QStringList FilterFlow::getSelectedElements()
{
    QStringList result;
    for (int i = 1; i < _layout->count(); i++)
    {
        QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
        if (buttonTmp->isChecked())
            result.append(buttonTmp->text());
    }
    return result;
}

QList<int> FilterFlow::getSelectedIds()
{
    QList<int> result;
    for (int i = 1; i < _layout->count(); i++)
    {
        QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
        if (buttonTmp->isChecked())
            result.append(buttonTmp->property("id").toInt());
    }
    return result;
}

int FilterFlow::getSelectedId()
{
    for (int i = 0; i < _layout->count(); i++)
    {
        QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
        if (buttonTmp->isChecked())
            return buttonTmp->property("id").toInt();
    }
    return -1;
}

QWidget * FilterFlow::createItem(QString text, int id, bool selected)
{
    QPushButton * button = new QPushButton(this);
    button->setText(text);
    button->setCursor(Qt::PointingHandCursor);
    button->setCheckable(true);
    button->setChecked(selected);
    button->setProperty("id", id);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
    return button;
}

void FilterFlow::buttonClicked()
{
    // Sender
    QPushButton* currentButton = dynamic_cast<QPushButton *>(QObject::sender());

    this->blockSignals(true);

    // Button "all" clicked?
    bool updateNeeded = false;
    if (_layout->itemAt(0)->widget() == currentButton)
    {
        currentButton->setChecked(true);
        for (int i = 1; i < _layout->count(); i++)
        {
            QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
            if (buttonTmp->isChecked())
            {
                updateNeeded = true;
                buttonTmp->setChecked(false);
            }
        }
    }
    else
    {
        if (_singleSelection)
        {
            currentButton->setChecked(true);
            for (int i = 0; i < _layout->count(); i++)
            {
                QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
                if (buttonTmp != currentButton && buttonTmp->isChecked())
                {
                    updateNeeded = true;
                    buttonTmp->setChecked(false);
                }
            }
        }
        else
        {
            updateNeeded = true;
            bool allSelected = true;
            for (int i = 1; i < _layout->count(); i++)
            {
                QPushButton * buttonTmp = dynamic_cast<QPushButton *>(_layout->itemAt(i)->widget());
                if (buttonTmp->isChecked())
                {
                    allSelected = false;
                    break;
                }
            }
            (dynamic_cast<QPushButton *>(_layout->itemAt(0)->widget()))->setChecked(allSelected);
        }
    }
    this->blockSignals(false);

    // Notify about the change
    if (updateNeeded)
        emit(selectionChanged());
}
