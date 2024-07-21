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

#include "pushstereoediting.h"
#include "contextmanager.h"

QList<PushStereoEditing *> PushStereoEditing::s_instances;

PushStereoEditing::PushStereoEditing(QWidget * parent) : QPushButton(parent)
{
    // Button configuration
    this->setCheckable(true);
    this->setToolTip(ContextManager::translation()->getTranslation(TranslationManager::TRANSLATION_STEREO_EDITING_HELP));

    // Style
    this->setStyleSheet("QPushButton{border: 1px solid " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                        "; border-top: 0; border-right: 0; border-bottom: 0} QPushButton:hover {background-color:" +
                        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED).name() + "}");
    SetStateInstance(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool());

    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggle(bool)));

    s_instances << this;
}

PushStereoEditing::~PushStereoEditing()
{
    s_instances.removeOne(this);
}

void PushStereoEditing::SetState(bool isOn)
{
    // Update the state of all buttons
    foreach (PushStereoEditing * button, s_instances)
        button->SetStateInstance(isOn);
}

void PushStereoEditing::SetStateInstance(bool isOn)
{
    this->blockSignals(true);
    if (isOn)
    {
        this->setIcon(ContextManager::theme()->getColoredSvg(":/tool/link.svg", QSize(18, 18), ThemeManager::HIGHLIGHTED_TEXT));
        this->setChecked(true);
    }
    else
    {
        QMap<QString, QString> replacement;
        replacement["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT), 0.4).name();
        this->setIcon(ContextManager::theme()->getColoredSvg(":/tool/unlink.svg", QSize(18, 18), replacement));
        this->setChecked(false);
    }
    this->blockSignals(false);
}

void PushStereoEditing::onToggle(bool checked)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "stereo_modification", checked);
}
