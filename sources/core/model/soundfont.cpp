/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#include "soundfont.h"
#include "smpl.h"
#include "instprst.h"
#include "soundfonts.h"
#include "treemodel.h"
#include "treeitemfirstlevel.h"
#include "treeitemroot.h"
#include "treesortfilterproxy.h"

Soundfont::Soundfont(EltID id) :
    _smplCounter(0),
    _instCounter(0),
    _prstCounter(0),
    _id(id)
{
    // Prepare the root items and the model
    _rootItem = new TreeItemRoot(EltID(elementUnknown));
    TreeModel * sourceModel = new TreeModel(_rootItem);
    _model = sourceModel;

    _rootItem->attachModel(sourceModel);

    // Built the tree
    _generalTreeItem = new TreeItemFirstLevel(QObject::trUtf8("General"), nullptr, _rootItem, EltID(elementSf2, _id.indexSf2, -1, -1, -1));
    _sampleTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Samples"), (QMap<int, TreeItem*> *)&_smpl, _rootItem, EltID(elementRootSmpl, _id.indexSf2, -1, -1, -1));
    _instrumentTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Instruments"), (QMap<int, TreeItem*> *)&_inst, _rootItem, EltID(elementRootInst, _id.indexSf2, -1, -1, -1));
    _presetTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Presets"), (QMap<int, TreeItem*> *)&_prst, _rootItem, EltID(elementRootPrst, _id.indexSf2, -1, -1, -1));

    // Default attributes for a soundfont
    SfVersionTag sfVersionTmp;
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    _IFIL = sfVersionTmp;
    sfVersionTmp.wMajor = 0;
    sfVersionTmp.wMinor = 0;
    _IVER = sfVersionTmp;
    _ISNG = "EMU8000";
    _INAM = "";
    _IROM = "";
    _ICRD = "";
    _IENG = "";
    _IPRD = "";
    _ICOP = "";
    _ICMT = "";
    _ISFT = "";
    _fileNameForData = "";
    _fileNameInitial = "";
    _numEdition = 0;
}

Soundfont::~Soundfont()
{
    // Possibly delete a temporary file associated to the soundfont
    if (!_fileNameForData.isEmpty() && _fileNameForData != _fileNameInitial)
    {
        // Delete the temporary file
        QFile::remove(_fileNameForData);
    }

    // Delete all presets
    QList<int> keys = _prst.keys();
    foreach (int key, keys)
    {
        _prst[key]->notifyDeletion(false);
        delete _prst[key];
        _prst.remove(key);
    }

    // Delete all instruments
    keys = _inst.keys();
    foreach (int key, keys)
    {
        _inst[key]->notifyDeletion(false);
        delete _inst[key];
        _inst.remove(key);
    }

    // Delete all samples
    keys = _smpl.keys();
    foreach (int key, keys)
    {
        _smpl[key]->notifyDeletion(false);
        delete _smpl.take(key);
    }

    // Delete headers
    _presetTreeItem->notifyDeletion(false);
    delete _presetTreeItem;
    _instrumentTreeItem->notifyDeletion(false);
    delete _instrumentTreeItem;
    _sampleTreeItem->notifyDeletion(false);
    delete _sampleTreeItem;
    _generalTreeItem->notifyDeletion(false);
    delete _generalTreeItem;

    // Root item
    _model->removeRow(0);
    _rootItem->attachModel(nullptr);
    delete _rootItem;

    // Delete the model
    delete _model;
}

int Soundfont::addSample()
{
    _smpl[_smplCounter] = new Smpl(this, _sampleTreeItem, EltID(elementSmpl, _id.indexSf2, _smplCounter, -1, -1));
    _smpl[_smplCounter]->notifyCreated();
    return _smplCounter++;
}

Smpl * Soundfont::getSample(int index)
{
    if (_smpl.contains(index))
        return _smpl[index];
    return nullptr;
}

bool Soundfont::deleteSample(int index)
{
    if (_smpl.contains(index))
    {
        _smpl[index]->notifyDeletion();
        delete _smpl.take(index);
        return true;
    }
    return false;
}

int Soundfont::indexOfSample(Smpl * smpl)
{
    return _smpl.values().indexOf(smpl);
}

int Soundfont::addInstrument()
{
    _inst[_instCounter] = new InstPrst(this, _instrumentTreeItem, EltID(elementInst, _id.indexSf2, _instCounter, -1, -1));
    _inst[_instCounter]->notifyCreated();
    return _instCounter++;
}

InstPrst * Soundfont::getInstrument(int index)
{
    if (_inst.contains(index))
        return _inst[index];
    return nullptr;
}

bool Soundfont::deleteInstrument(int index)
{
    if (_inst.contains(index))
    {
        _inst[index]->notifyDeletion();
        delete _inst.take(index);
        return true;
    }

    return false;
}

int Soundfont::indexOfInstrument(InstPrst * inst)
{
    return _inst.values().indexOf(inst);
}

int Soundfont::addPreset()
{
    _prst[_prstCounter] = new InstPrst(this, _presetTreeItem, EltID(elementPrst, _id.indexSf2, _prstCounter, -1, -1));
    _prst[_prstCounter]->notifyCreated();

    // Extra fields for presets
    _prst[_prstCounter]->setExtraField(champ_wPreset, 0);
    _prst[_prstCounter]->setExtraField(champ_wBank, 0);
    _prst[_prstCounter]->setExtraField(champ_dwLibrary, 0);
    _prst[_prstCounter]->setExtraField(champ_dwGenre, 0);
    _prst[_prstCounter]->setExtraField(champ_dwMorphology, 0);

    return _prstCounter++;
}

InstPrst * Soundfont::getPreset(int index)
{
    if (_prst.contains(index))
        return _prst[index];
    return nullptr;
}

bool Soundfont::deletePreset(int index)
{
    if (_prst.contains(index))
    {
        _prst[index]->notifyDeletion();
        delete _prst.take(index);
        return true;
    }
    return false;
}

int Soundfont::indexOfPreset(InstPrst * prst)
{
    return _prst.values().indexOf(prst);
}
