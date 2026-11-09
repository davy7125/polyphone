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

#include "interactiveduplicator.h"
#include "contextmanager.h"
#include <QApplication>
#include <QMessageBox>
#include <QAbstractButton>

InteractiveDuplicator::InteractiveDuplicator() : Duplicator(),
    _parent(QApplication::activeWindow())
{

}

Duplicator::EtatMessage InteractiveDuplicator::askReplacement(QString question)
{
    QMessageBox msgBox(_parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(question);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save |
                              QMessageBox::NoAll | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(Duplicator::tr("&Replace"));
    msgBox.button(QMessageBox::YesAll)->setText(Duplicator::tr("R&eplace all"));
    msgBox.button(QMessageBox::Save)->setText(Duplicator::tr("&Duplicate"));
    msgBox.button(QMessageBox::SaveAll)->setText(Duplicator::tr("D&uplicate all"));
    msgBox.button(QMessageBox::No)->setText(Duplicator::tr("&Ignore"));
    msgBox.button(QMessageBox::NoAll)->setText(Duplicator::tr("I&gnore all"));
    msgBox.setDefaultButton(QMessageBox::YesAll);
    EtatMessage reponse = IGNORER_TOUT;
    switch (msgBox.exec())
    {
    case QMessageBox::NoAll:    reponse = IGNORER_TOUT;   break;
    case QMessageBox::No:       reponse = IGNORER;        break;
    case QMessageBox::YesAll:   reponse = REMPLACER_TOUT; break;
    case QMessageBox::Yes:      reponse = REMPLACER;      break;
    case QMessageBox::Save:     reponse = DUPLIQUER;      break;
    case QMessageBox::SaveAll:  reponse = DUPLIQUER_TOUT; break;
    }
    return reponse;
}

void InteractiveDuplicator::warnGlobalNotCopied()
{
    if (!ContextManager::configuration()->getValue(ConfManager::SECTION_WARNINGS, "global_not_overwritten", true).toBool())
        return;

    QMessageBox msgBox(_parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("<b>" + Duplicator::tr("Global parameters are already filled.") + "</b>");
    msgBox.setInformativeText(Duplicator::tr("The global division will not be copied."));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::YesAll);
    msgBox.button(QMessageBox::Yes)->setText(Duplicator::tr("&Ok"));
    msgBox.button(QMessageBox::YesAll)->setText(Duplicator::tr("Ok, &disable this message"));
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() == QMessageBox::YesAll)
        ContextManager::configuration()->setValue(ConfManager::SECTION_WARNINGS, "global_not_overwritten", false);
}

void InteractiveDuplicator::warnNoPresetAvailable()
{
    QMessageBox::warning(_parent, Duplicator::tr("Warning"), Duplicator::tr("No preset available."));
}
