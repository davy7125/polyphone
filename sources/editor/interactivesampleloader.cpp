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

#include "interactivesampleloader.h"
#include "contextmanager.h"
#include <QMessageBox>
#include <QAbstractButton>

InteractiveSampleLoader::InteractiveSampleLoader(QWidget * parent) : SampleLoader(),
    _parent(parent)
{
    // Automatically remove the blank at the start and at the end
    setProcessing(ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool(),
                  ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank_end", false).toBool());
}

void InteractiveSampleLoader::loading(QString path)
{
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, path);
}

void InteractiveSampleLoader::showError(QString error)
{
    QMessageBox::warning(_parent, QObject::tr("Warning"), error);
}

int InteractiveSampleLoader::askReplacement(QString question, int replace)
{
    QMessageBox msgBox(_parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(question);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save |
                              QMessageBox::NoAll | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(QObject::tr("&Replace"));
    msgBox.button(QMessageBox::YesAll)->setText(QObject::tr("R&eplace all"));
    msgBox.button(QMessageBox::Save)->setText(QObject::tr("&Duplicate"));
    msgBox.button(QMessageBox::SaveAll)->setText(QObject::tr("D&uplicate all"));
    msgBox.button(QMessageBox::No)->setText(QObject::tr("&Ignore"));
    msgBox.button(QMessageBox::NoAll)->setText(QObject::tr("I&gnore all"));
    msgBox.setDefaultButton(QMessageBox::YesAll);
    switch (msgBox.exec())
    {
    case QMessageBox::NoAll:    replace =  4; break;
    case QMessageBox::No:       replace =  3; break;
    case QMessageBox::YesAll:   replace =  2; break;
    case QMessageBox::Yes:      replace =  1; break;
    case QMessageBox::Save:     replace =  0; break;
    case QMessageBox::SaveAll:  replace = -1; break;
    }
    return replace;
}
