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

#include "treeviewmenu.h"
#include "soundfontmanager.h"
#include "dialog_list.h"
#include "dialog_rename.h"
#include "QInputDialog"
#include <QMessageBox>
#include "contextmanager.h"
#include "duplicator.h"

IdList TreeViewMenu::s_copy = IdList();

TreeViewMenu::TreeViewMenu(QWidget * parent) : QMenu(parent),
    _dialogList(new DialogList(parent))
{   
    // Style
    this->setStyleSheet(QString("QMenu::separator {background: ") +
                        ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                          ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.5).name() +
                        ";margin: 10px 45px; height: 1px}");

    // Associate
    _associateAction = new QAction(trUtf8("&Bind to..."), this);
    connect(_associateAction, SIGNAL(triggered()), this, SLOT(associate()));
    this->addAction(_associateAction);

    // Replace
    _replaceAction = new QAction(trUtf8("&Replace by..."), this);
    connect(_replaceAction, SIGNAL(triggered()), this, SLOT(replace()));
    this->addAction(_replaceAction);
    this->addSeparator();

    connect(_dialogList, SIGNAL(elementSelected(EltID, bool)), this, SLOT(itemSelectedFromList(EltID, bool)));

    // Copy
    _copyAction = new QAction(trUtf8("&Copy"), this);
    _copyAction->setShortcut(QString("Ctrl+C"));
    connect(_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    this->addAction(_copyAction);

    // Paste
    _pasteAction = new QAction(trUtf8("&Paste"), this);
    _pasteAction->setShortcut(QString("Ctrl+V"));
    connect(_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    this->addAction(_pasteAction);

    // Duplicate
    _duplicateAction = new QAction(trUtf8("D&uplicate"), this);
    _duplicateAction->setShortcut(QString("Ctrl+D"));
    connect(_duplicateAction, SIGNAL(triggered()), this, SLOT(duplicate()));
    this->addAction(_duplicateAction);

    // Delete
    _removeAction = new QAction(trUtf8("&Delete"), this);
    _removeAction->setShortcut(QString("Del"));
    connect(_removeAction, SIGNAL(triggered()), this, SLOT(remove()));
    this->addAction(_removeAction);
    this->addSeparator();

    // Rename
    _renameAction = new QAction(trUtf8("Re&name..."), this);
    _renameAction->setShortcut(Qt::Key_F2);
    connect(_renameAction, SIGNAL(triggered()), this, SLOT(rename()));
    this->addAction(_renameAction);
}

TreeViewMenu::~TreeViewMenu()
{
    delete _dialogList;
}

void TreeViewMenu::initialize(IdList ids)
{
    _currentIds = ids;

    // All ids with the same element?
    ElementType type = elementUnknown;
    bool sameElement = true;
    foreach (EltID id, ids)
    {
        if (type == elementUnknown)
            type = id.typeElement;
        else if (type != id.typeElement)
        {
            sameElement = false;
            break;
        }
    }

    // Associate
    bool associate = sameElement;
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl && id.typeElement != elementInst)
        {
            associate = false;
            break;
        }
    }
    _associateAction->setEnabled(associate);

    // Replace
    _replaceAction->setEnabled(ids.count() == 1 && (ids[0].typeElement == elementInstSmpl || ids[0].typeElement == elementPrstInst));

    // Rename
    bool rename = true;
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl && id.typeElement != elementInst && id.typeElement != elementPrst)
        {
            rename = false;
            break;
        }
    }
    if (rename)
    {
        _renameAction->setEnabled(true);
        _renameAction->setText(ids.count() == 1 ? trUtf8("Re&name...") : trUtf8("Bulk re&name..."));
    }
    else
    {
        _renameAction->setText(trUtf8("Re&name..."));
        _renameAction->setEnabled(false);
    }

    // Copy
    _copyAction->setEnabled(sameElement);

    // Paste
    _pasteAction->setEnabled(ids.count() == 1);

    // Duplicate
    _duplicateAction->setEnabled(sameElement);

    // Delete
    _removeAction->setEnabled(sameElement);
}

void TreeViewMenu::associate()
{
    if (!_currentIds.empty())
        _dialogList->showDialog(_currentIds[0], true);
}

void TreeViewMenu::replace()
{
    if (_currentIds.count() == 1)
        _dialogList->showDialog(_currentIds[0], false);
}

void TreeViewMenu::remove()
{
    // Remove all the selected elements
    int message = 1;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, _currentIds)
    {
        if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementInstSmpl ||
                id.typeElement == elementPrst || id.typeElement == elementPrstInst)
            sm->remove(id, &message);
    }

    if (message % 2 == 0)
        QMessageBox::warning((QWidget*)this->parent(), trUtf8("Warning"),
                             trUtf8("Cannot delete a sample used by another instrument."));
    if (message % 3 == 0)
        QMessageBox::warning((QWidget*)this->parent(), trUtf8("Warning"),
                             trUtf8("Cannot delete an instrument used by another preset."));

    sm->endEditing("tree:remove");
}

void TreeViewMenu::itemSelectedFromList(EltID id, bool isAssociation)
{
    if (isAssociation)
    {
        if (!_currentIds.empty())
            associate(_currentIds, id);
    }
    else
    {
        if (_currentIds.count() == 1)
            replace(id, _currentIds[0]);
    }
}

void TreeViewMenu::associate(IdList ids, EltID idDest)
{
    // Type of the element(s) that will be created
    AttributeType champ;
    if (idDest.typeElement == elementInst)
    {
        idDest.typeElement = elementInstSmpl;
        champ = champ_sampleID;
    }
    else
    {
        idDest.typeElement = elementPrstInst;
        champ = champ_instrument;
    }
    AttributeValue val;

    // For each element to associate
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID idSrc, ids)
    {
        // Create a division
        idDest.indexElt2 = sm->add(idDest);

        // Association of idSrc in idDest
        val.wValue = idSrc.indexElt;
        sm->set(idDest, champ, val);
        if (champ == champ_sampleID)
        {
            // Pan
            if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample ||
                    sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
                val.shValue = 500;
            else if (sm->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample ||
                     sm->get(idSrc, champ_sfSampleType).sfLinkValue == RomLeftSample)
                val.shValue = -500;
            else
                val.shValue = 0;
            sm->set(idDest, champ_pan, val);
        }
        else
        {
            // Key range
            int keyMin = 127;
            int keyMax = 0;
            EltID idLinked = idSrc;
            idLinked.typeElement = elementInstSmpl;
            foreach (int i, sm->getSiblings(idLinked))
            {
                idLinked.indexElt2 = i;
                if (sm->isSet(idLinked, champ_keyRange))
                {
                    keyMin = qMin(keyMin, (int)sm->get(idLinked, champ_keyRange).rValue.byLo);
                    keyMax = qMax(keyMax, (int)sm->get(idLinked, champ_keyRange).rValue.byHi);
                }
            }
            AttributeValue value;
            if (keyMin < keyMax)
            {
                value.rValue.byLo = keyMin;
                value.rValue.byHi = keyMax;
            }
            else
            {
                value.rValue.byLo = 0;
                value.rValue.byHi = 127;
            }
            sm->set(idDest, champ_keyRange, value);
        }
    }

    sm->endEditing("command:associate");
}

void TreeViewMenu::replace(EltID idSrc, EltID idDest)
{
    // Checks
    if (idDest.typeElement != elementInstSmpl && idDest.typeElement != elementPrstInst)
        return;
    if (idSrc.typeElement != elementSmpl && idSrc.typeElement != elementInst)
        return;

    // Type of value to change
    AttributeType champ;
    if (idSrc.typeElement == elementSmpl)
        champ = champ_sampleID;
    else
        champ = champ_instrument;

    // Replace the link of a division
    AttributeValue val;
    val.wValue = idSrc.indexElt;
    SoundfontManager::getInstance()->set(idDest, champ, val);
    SoundfontManager::getInstance()->endEditing("command:replace");
}

void TreeViewMenu::rename()
{
    // Checkes
    if (_currentIds.empty())
        return;
    ElementType type = _currentIds[0].typeElement;
    if (type != elementSmpl && type != elementInst && type != elementPrst)
        return;

    if (_currentIds.count() > 1)
    {
        DialogRename * dial = new DialogRename(type == elementSmpl, (QWidget*)this->parent());
        connect(dial, SIGNAL(updateNames(int, QString, QString, int, int)),
                this, SLOT(bulkRename(int, QString, QString, int, int)));
        dial->show();
    }
    else
    {
        QString msg;
        if (type == elementSmpl)
            msg = trUtf8("Sample name (max 20 characters):");
        else if (type == elementInst)
            msg = trUtf8("Instrument name (max 20 characters):");
        else if (type == elementPrst)
            msg = trUtf8("Preset name (max 20 characters):");

        SoundfontManager * sm = SoundfontManager::getInstance();
        bool ok = true;
        QString text = QInputDialog::getText((QWidget*)this->parent(), trUtf8("Question"), msg, QLineEdit::Normal,
                                             sm->getQstr(_currentIds[0], champ_name), &ok,
                Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        if (ok && !text.isEmpty())
            sm->set(_currentIds[0], champ_name, text);
        sm->endEditing("command:rename");
    }
}

void TreeViewMenu::bulkRename(int renameType, QString text1, QString text2, int val1, int val2)
{
    if (renameType == 4)
    {
        if (val1 == val2)
            return;
    }
    else
    {
        if (text1.isEmpty())
            return;
    }

    SoundfontManager * sm = SoundfontManager::getInstance();
    for (int i = 0; i < _currentIds.size(); i++)
    {
        EltID ID = _currentIds.at(i);

        // Compute the name
        QString newName = sm->getQstr(ID, champ_name);
        switch (renameType)
        {
        case 0:{
            // Replace with the name as a suffix
            QString suffix = " " + ContextManager::keyName()->getKeyName(sm->get(ID, champ_byOriginalPitch).bValue, false, true);
            SFSampleLink pos = sm->get(ID, champ_sfSampleType).sfLinkValue;
            if (pos == rightSample || pos == RomRightSample)
                suffix += 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                suffix += 'L';

            newName = text1.left(20 - suffix.size()) + suffix;
        }break;
        case 1:
            // Replace with an index an a suffix
            if ((i+1) % 100 < 10)
                newName = text1.left(17) + "-0" + QString::number((i+1) % 100);
            else
                newName = text1.left(17) + "-" + QString::number((i+1) % 100);
            break;
        case 2:
            // Replace a string
            newName.replace(text1, text2, Qt::CaseInsensitive);
            break;
        case 3:
            // Insert a string
            if (val1 > newName.size())
                val1 = newName.size();
            newName.insert(val1, text1);
            break;
        case 4:
            // Delete a part
            if (val2 > val1)
                newName.remove(val1, val2 - val1);
            else
                newName.remove(val2, val1 - val2);
            break;
        }

        newName = newName.left(20);

        if (sm->getQstr(ID, champ_name).compare(newName, Qt::CaseInsensitive))
            sm->set(ID, champ_name, newName);
    }
    sm->endEditing("command:bulkRename");
}

void TreeViewMenu::copy()
{
    s_copy = _currentIds;
}

void TreeViewMenu::paste()
{
    if (_currentIds.count() == 1 && s_copy.count() > 0)
    {
        // Destination of all copied elements
        EltID idDest = _currentIds[0];

        // Paste all copied elements
        SoundfontManager * sm = SoundfontManager::getInstance();
        Duplicator duplicator;
        foreach (EltID idSource, s_copy)
        {
            if ((idSource.typeElement == elementSmpl || idSource.typeElement == elementInst || idSource.typeElement == elementPrst ||
                 idSource.typeElement == elementInstSmpl || idSource.typeElement == elementPrstInst) && sm->isValid(idSource))
                duplicator.copy(idSource, idDest);
        }
        sm->endEditing("command:paste");
    }
}

void TreeViewMenu::duplicate()
{
    if (_currentIds.empty())
        return;

    // Duplicate all elements
    SoundfontManager * sm = SoundfontManager::getInstance();
    Duplicator duplicator;
    foreach (EltID idSource, _currentIds)
    {
        if (sm->isValid(idSource))
            duplicator.duplicate(idSource);
    }
    sm->endEditing("command:duplicate");
}
