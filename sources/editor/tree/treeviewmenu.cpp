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

#include "treeviewmenu.h"
#include "soundfontmanager.h"
#include "dialog_list.h"
#include "dialog_rename.h"
#include "QInputDialog"
#include <QMessageBox>
#include "contextmanager.h"
#include "duplicator.h"
#include "dialogquestion.h"
#include "utils.h"
#include "auto_distribution/toolautodistribution.h"
#include "solomanager.h"

IdList TreeViewMenu::s_copy = IdList();

TreeViewMenu::TreeViewMenu(QWidget * parent) : QMenu(parent),
    _dialogList(new DialogList(parent))
{   
    // Style
    this->setStyleSheet(ContextManager::theme()->getMenuTheme());

    // Associate
    _associateAction = new QAction(tr("&Bind to..."), this);
    connect(_associateAction, SIGNAL(triggered()), this, SLOT(associate()));
    this->addAction(_associateAction);

    // Replace
    _replaceAction = new QAction(tr("&Replace by..."), this);
    connect(_replaceAction, SIGNAL(triggered()), this, SLOT(replace()));
    this->addAction(_replaceAction);
    this->addSeparator();

    connect(_dialogList, SIGNAL(elementSelected(EltID, bool)), this, SLOT(itemSelectedFromList(EltID, bool)));

    /// ----------- ///

    // Copy
    _copyAction = new QAction(tr("&Copy"), this);
    _copyAction->setShortcut(QString("Ctrl+C"));
    connect(_copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    this->addAction(_copyAction);

    // Paste
    _pasteAction = new QAction(tr("&Paste"), this);
    _pasteAction->setShortcut(QString("Ctrl+V"));
    connect(_pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    this->addAction(_pasteAction);

    // Duplicate
    _duplicateAction = new QAction(tr("D&uplicate"), this);
    _duplicateAction->setShortcut(QString("Ctrl+D"));
    connect(_duplicateAction, SIGNAL(triggered()), this, SLOT(duplicate()));
    this->addAction(_duplicateAction);

    // Delete
    _removeAction = new QAction(tr("&Delete"), this);
    _removeAction->setShortcut(QString("Del"));
    connect(_removeAction, SIGNAL(triggered()), this, SLOT(remove()));
    this->addAction(_removeAction);
    this->addSeparator();

    /// ----------- ///

    // Mute
    _muteAction = new QAction(tr("Mute"), this);
    _muteAction->setCheckable(true);
    connect(_muteAction, SIGNAL(triggered(bool)), this, SLOT(onMute(bool)));
    this->addAction(_muteAction);

    // Always play
    _alwaysPlayAction = new QAction(tr("Always play"), this);
    _alwaysPlayAction->setCheckable(true);
    connect(_alwaysPlayAction, SIGNAL(triggered(bool)), this, SLOT(onAlwaysPlay(bool)));
    this->addAction(_alwaysPlayAction);

    // Restore playback
    _restorePlaybackAction = new QAction(tr("Restore playback"), this);
    connect(_restorePlaybackAction, SIGNAL(triggered()), this, SLOT(onRestorePlayback()));
    this->addAction(_restorePlaybackAction);
    this->addSeparator();

    /// ----------- ///

    // Rename
    _renameAction = new QAction(tr("Re&name..."), this);
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
    bool renameInstOrPrst = false;
    bool renameElementInInstOrPrst = false;
    foreach (EltID id, ids)
    {
        if (id.typeElement == elementSmpl || id.typeElement == elementInst || id.typeElement == elementPrst)
            renameInstOrPrst = true;
        else if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
            renameElementInInstOrPrst = true;
        else
        {
            rename = false;
            break;
        }
    }
    if (rename && ((renameInstOrPrst && !renameElementInInstOrPrst) || (!renameInstOrPrst && renameElementInInstOrPrst)))
    {
        _renameAction->setEnabled(true);
        _renameAction->setText(ids.count() == 1 ? tr("Re&name...") : tr("Bulk re&name..."));
    }
    else
    {
        _renameAction->setText(tr("Re&name..."));
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

    // Mute / Always play / Restore
    bool everythingMute = true;
    bool everythingAlwaysPlayed = true;
    bool hidePlaybackSection = false;
    bool enableMute = true;
    bool enableAlwaysPlayed = true;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, ids)
    {
        if (!sm->solo()->isMute(id))
            everythingMute = false;
        if (!sm->solo()->isAlwaysPlayed(id))
            everythingAlwaysPlayed = false;

        if (id.typeElement == elementInst || id.typeElement == elementPrst)
            enableMute = false;
        else if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
            enableAlwaysPlayed = false;
        else
        {
            hidePlaybackSection = true;
            break;
        }
    }
    if (hidePlaybackSection)
    {
        _muteAction->setVisible(false);
        _alwaysPlayAction->setVisible(false);
        _restorePlaybackAction->setVisible(false);
    }
    else
    {
        _muteAction->setVisible(true);
        _alwaysPlayAction->setVisible(true);
        _restorePlaybackAction->setVisible(true);

        _muteAction->setChecked(everythingMute);
        _muteAction->setEnabled(enableMute);
        _alwaysPlayAction->setChecked(everythingAlwaysPlayed);
        _alwaysPlayAction->setEnabled(enableAlwaysPlayed);
    }
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
        QMessageBox::warning(dynamic_cast<QWidget*>(this->parent()), tr("Warning"),
                             tr("Cannot delete a sample used by another instrument."));
    if (message % 3 == 0)
        QMessageBox::warning(dynamic_cast<QWidget*>(this->parent()), tr("Warning"),
                             tr("Cannot delete an instrument used by another preset."));

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
    AttributeValue val;

    // For each element to associate
    SoundfontManager * sm = SoundfontManager::getInstance();
    Duplicator duplicator;
    if (idDest.typeElement == elementInst)
    {
        // If the instrument is empty, the distribution tool will be triggered
        EltID idInstSmpl = idDest;
        idInstSmpl.typeElement = elementInstSmpl;
        bool launchDistribution = sm->getSiblings(idInstSmpl).isEmpty();
        bool samePitch = false;
        QMap<unsigned char, QPair<bool, bool> > pitches;

        // Link all dragged samples
        foreach (EltID id, ids)
        {
            // Store the pitch
            int bPitch = sm->get(id, champ_byOriginalPitch).bValue;
            SFSampleLink sampleType = sm->get(id, champ_sfSampleType).sfLinkValue;
            if (!pitches.contains(bPitch))
            {
                pitches[bPitch].first = false;
                pitches[bPitch].second = false;
            }
            if (sampleType != rightSample && sampleType != RomRightSample)
            {
                if (pitches[bPitch].first)
                    samePitch = true;
                else
                    pitches[bPitch].first = true;
            }
            if (sampleType != leftSample && sampleType != RomLeftSample)
            {
                if (pitches[bPitch].second)
                    samePitch = true;
                else
                    pitches[bPitch].second = true;
            }

            duplicator.copy(id, idDest);
        }

        // If there is more than 1 pitch and no more than 1 sample per pitch / side, distribute them
        if (launchDistribution && !samePitch && pitches.count() > 1)
        {
            ToolAutoDistribution tool;
            tool.process(sm, idDest, NULL);
        }
    }
    else
    {
        // Link all instruments in the preset
        foreach (EltID idSrc, ids)
            duplicator.copy(idSrc, idDest);
    }
    sm->endEditing("command:associate");

    // Select the parent element of all children that have been linked
    emit(selectionChanged(idDest));
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
    // Checks
    if (_currentIds.empty())
        return;
    ElementType type = _currentIds[0].typeElement;
    if (type != elementSmpl && type != elementInst && type != elementPrst &&
            type != elementInstSmpl && type != elementPrstInst)
        return;

    if (_currentIds.count() > 1)
    {
        // Default name
        QString defaultName = "";
        if (type == elementSmpl || type == elementInst || type == elementPrst)
        {
            // List of all names
            QStringList currentNames;
            foreach (EltID id, _currentIds)
                currentNames << SoundfontManager::getInstance()->getQstr(id, champ_name);
            defaultName = Utils::commonPart(currentNames);
        }
        else
        {
            EltID idParent = _currentIds[0];
            idParent.typeElement = (type == elementInstSmpl ? elementInst : elementPrst);
            defaultName = SoundfontManager::getInstance()->getQstr(idParent, champ_name);
        }

        DialogRename * dial = new DialogRename(type, defaultName,
                                               dynamic_cast<QWidget*>(this->parent()));
        connect(dial, SIGNAL(updateNames(int, QString, QString, int, int)),
                this, SLOT(bulkRename(int, QString, QString, int, int)));
        dial->show();
    }
    else
    {
        QString msg;
        if (type == elementSmpl || type == elementInstSmpl)
            msg = tr("Sample name");
        else if (type == elementInst || type == elementPrstInst)
            msg = tr("Instrument name");
        else if (type == elementPrst)
            msg = tr("Preset name");

        // Default name
        QString defaultName = "";
        if (type == elementSmpl || type == elementInst || type == elementPrst)
            defaultName = SoundfontManager::getInstance()->getQstr(_currentIds[0], champ_name);
        else if (type == elementInstSmpl)
        {
            EltID idSmpl = _currentIds[0];
            idSmpl.indexElt = SoundfontManager::getInstance()->get(idSmpl, champ_sampleID).wValue;
            idSmpl.typeElement = elementSmpl;
            defaultName = SoundfontManager::getInstance()->getQstr(idSmpl, champ_name);
        }
        else if (type == elementPrstInst)
        {
            EltID idInst = _currentIds[0];
            idInst.indexElt = SoundfontManager::getInstance()->get(idInst, champ_instrument).wValue;
            idInst.typeElement = elementInst;
            defaultName = SoundfontManager::getInstance()->getQstr(idInst, champ_name);
        }

        DialogQuestion * dial = new DialogQuestion(dynamic_cast<QWidget*>(this->parent()));
        dial->initialize(tr("Rename"), msg + "...", defaultName);
        dial->setTextLimit(20);
        connect(dial, SIGNAL(onOk(QString)), this, SLOT(onRename(QString)));
        dial->show();
    }
}

void TreeViewMenu::onRename(QString txt)
{
    if (txt.isEmpty() || _currentIds.empty())
        return;

    // Element to rename
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id = _currentIds[0];
    if (id.typeElement == elementInstSmpl)
    {
        id.indexElt = sm->get(id, champ_sampleID).wValue;
        id.typeElement = elementSmpl;
    }
    else if (id.typeElement == elementPrstInst)
    {
        id.indexElt = sm->get(id, champ_instrument).wValue;
        id.typeElement = elementInst;
    }

    sm->set(id, champ_name, txt);
    sm->endEditing("command:rename");
}

void TreeViewMenu::bulkRename(int renameType, QString text1, QString text2, int val1, int val2)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    for (int i = 0; i < _currentIds.size(); i++)
    {
        EltID id = _currentIds.at(i);
        int rootkey = -1;
        int velocity = -1;
        if (id.typeElement == elementInstSmpl)
        {
            // Rootkey defined?
            if (sm->isSet(id, champ_overridingRootKey))
                rootkey = sm->get(id, champ_overridingRootKey).wValue;
            // Velocity
            if (sm->isSet(id, champ_velRange))
                velocity = sm->get(id, champ_velRange).rValue.byLo;
            else
                velocity = 0;

            // The element to rename is a sample
            id.indexElt = sm->get(id, champ_sampleID).wValue;
            id.typeElement = elementSmpl;
        }
        else if (id.typeElement == elementPrstInst)
        {
            // The element to rename is an instrument
            id.indexElt = sm->get(id, champ_instrument).wValue;
            id.typeElement = elementInst;
        }

        // Compute the name
        QString newName = sm->getQstr(id, champ_name);
        switch (renameType)
        {
        case 0:{
            // Overwrite existing name with key name as suffix
            if (rootkey == -1)
                rootkey = sm->get(id, champ_byOriginalPitch).bValue;
            QString suffix = ContextManager::keyName()->getKeyName(rootkey, false, true);
            SFSampleLink pos = sm->get(id, champ_sfSampleType).sfLinkValue;
            if (pos == rightSample || pos == RomRightSample)
                suffix += 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                suffix += 'L';

            if (text1.isEmpty())
                newName = suffix;
            else
            {
                suffix = " " + suffix;
                newName = text1.left(20 - suffix.size()) + suffix;
            }
        }break;
        case 1:{
            // Overwrite existing name with key name and velocity as suffix
            if (rootkey == -1)
                rootkey = sm->get(id, champ_byOriginalPitch).bValue;
            QString suffix = ContextManager::keyName()->getKeyName(rootkey, false, true);
            SFSampleLink pos = sm->get(id, champ_sfSampleType).sfLinkValue;
            if (pos == rightSample || pos == RomRightSample)
                suffix += 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                suffix += 'L';
            else
                suffix += "_";

            if (velocity < 10)
                suffix += "00" + QString::number(velocity);
            else if (velocity < 100)
                suffix += "0" + QString::number(velocity);
            else
                suffix += QString::number(velocity);

            if (text1.isEmpty())
                newName = suffix;
            else
            {
                suffix = " " + suffix;
                newName = text1.left(20 - suffix.size()) + suffix;
            }
        } break;
        case 2:
            // Overwrite existing name with numerical ascending suffix
            if (text1.isEmpty())
            {
                if ((i+1) % 100 < 10)
                    newName = "0" + QString::number((i+1) % 100);
                else
                    newName = QString::number((i+1) % 100);
            }
            else
            {
                if ((i+1) % 100 < 10)
                    newName = text1.left(17) + "-0" + QString::number((i+1) % 100);
                else
                    newName = text1.left(17) + "-" + QString::number((i+1) % 100);
            }
            break;
        case 3:
            // Replace characters
            newName.replace(text1, text2, Qt::CaseInsensitive);
            break;
        case 4:
            // Insert after a specific position
            if (text1.isEmpty())
                return;
            if (val1  < 1)
                val1 = 1;
            else if (val1 > newName.size() + 1)
                val1 = newName.size() + 1;
            newName.insert(val1 - 1, text1);
            break;
        case 5:
            // Delete character range
            if (val2 > val1)
                newName.remove(val1 - 1, val2 - val1 + 1);
            else
                newName.remove(val2 - 1, val1 - val2 + 1);
            break;
        }

        newName = newName.left(20);

        if (sm->getQstr(id, champ_name).compare(newName, Qt::CaseSensitive) != 0)
            sm->set(id, champ_name, newName);
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
        IdList newIds;
        foreach (EltID idSource, s_copy)
        {
            if ((idSource.typeElement == elementSmpl || idSource.typeElement == elementInst || idSource.typeElement == elementPrst ||
                 idSource.typeElement == elementInstSmpl || idSource.typeElement == elementPrstInst) && sm->isValid(idSource))
            {
                EltID id = duplicator.copy(idSource, idDest);
                if (id.typeElement != elementUnknown)
                    newIds << id;
            }
        }

        if (!newIds.isEmpty())
        {
            sm->endEditing("command:paste");
            emit(selectionChanged(newIds));
        }
    }
}

void TreeViewMenu::duplicate()
{
    if (_currentIds.empty())
        return;

    // Duplicate all elements
    SoundfontManager * sm = SoundfontManager::getInstance();
    Duplicator duplicator;
    IdList newIds;
    foreach (EltID idSource, _currentIds)
    {
        if (sm->isValid(idSource))
        {
            EltID id = duplicator.duplicate(idSource);
            if (id.typeElement != elementUnknown)
                newIds << id;
        }
    }

    if (!newIds.isEmpty())
    {
        sm->endEditing("command:duplicate");
        emit selectionChanged(newIds);
    }
}

void TreeViewMenu::onMute(bool isOn)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<int> sf2Indexes;
    foreach (EltID id, _currentIds)
    {
        sm->solo()->setMute(id, isOn);
        if (!sf2Indexes.contains(id.indexSf2))
            sf2Indexes << id.indexSf2;
    }

    // For updating the pages
    if (!sf2Indexes.isEmpty())
        emit sm->editingDone("command:mute", sf2Indexes);
}

void TreeViewMenu::onAlwaysPlay(bool isOn)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, _currentIds)
        sm->solo()->setAlwaysPlayed(id, isOn);
}

void TreeViewMenu::onRestorePlayback()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<int> sf2Indexes;
    foreach (EltID id, _currentIds)
    {
        if (!sf2Indexes.contains(id.indexSf2))
        {
            sm->solo()->restorePlayback(id.indexSf2);
            sf2Indexes << id.indexSf2;
        }
    }

    // For updating the pages
    if (!sf2Indexes.isEmpty())
        emit sm->editingDone("command:mute", sf2Indexes);
}
