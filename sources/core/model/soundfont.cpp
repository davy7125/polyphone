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

#include "soundfont.h"
#include "smpl.h"
#include "instprst.h"
#include "soundfonts.h"
#include "treemodel.h"
#include "treeitemfirstlevel.h"
#include "treeitemroot.h"
#include "treesortfilterproxy.h"

Soundfont::Soundfont(EltID id) :
    _id(id),
    _smpl(IndexedElementList<Smpl *>())
{
    // Prepare the root items and the model
    _rootItem = new TreeItemRoot(EltID(elementUnknown));
    TreeModel * sourceModel = new TreeModel(_rootItem);
    _model = sourceModel;

    _rootItem->attachModel(sourceModel);

    // Built the tree
    _generalTreeItem = new TreeItemFirstLevel(QObject::trUtf8("General"), nullptr, _rootItem, EltID(elementSf2, _id.indexSf2, -1, -1, -1));
    _sampleTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Samples"), (IndexedElementList<TreeItem *> *)&_smpl, _rootItem, EltID(elementRootSmpl, _id.indexSf2, -1, -1, -1));
    _instrumentTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Instruments"), (IndexedElementList<TreeItem *> *)&_inst, _rootItem, EltID(elementRootInst, _id.indexSf2, -1, -1, -1));
    _presetTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Presets"), (IndexedElementList<TreeItem *> *)&_prst, _rootItem, EltID(elementRootPrst, _id.indexSf2, -1, -1, -1));

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
    _nameSort = "";
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
    for (int i = _prst.indexCount() - 1; i >= 0; i--)
    {
        InstPrst * elt = _prst.atIndex(i);
        if (elt != nullptr)
        {
            elt->notifyDeletion(false);
            delete elt;
            _prst.takeAtIndex(i);
        }
    }

    // Delete all instruments
    for (int i = _inst.indexCount() - 1; i >= 0; i--)
    {
        InstPrst * elt = _inst.atIndex(i);
        if (elt != nullptr)
        {
            elt->notifyDeletion(false);
            delete elt;
            _inst.takeAtIndex(i);
        }
    }

    // Delete all samples
    for (int i = _smpl.indexCount() - 1; i >= 0; i--)
    {
        Smpl * elt = _smpl.atIndex(i);
        if (elt != nullptr)
        {
            elt->notifyDeletion(false);
            delete _smpl.takeAtIndex(i);
        }
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
    int index = _smpl.add(new Smpl(_smpl.positionCount(), _sampleTreeItem, EltID(elementSmpl, _id.indexSf2, _smpl.indexCount(), -1, -1)));
    _smpl.atIndex(index)->notifyCreated();
    return index;
}

Smpl * Soundfont::getSample(int index)
{
    if (index < _smpl.indexCount())
        return _smpl.atIndex(index);
    return nullptr;
}

bool Soundfont::deleteSample(int index)
{
    // Check that the index is valid
    if (index >= _smpl.indexCount())
        return false;

    Smpl * smpl = _smpl.atIndex(index);
    if (smpl != nullptr)
    {
        // Delete the sample
        smpl->notifyDeletion();
        delete _smpl.takeAtIndex(index);

        // Update the row number of the following samples
        for (int i = index + 1; i < _smpl.indexCount(); i++)
        {
            Smpl * elt = _smpl.atIndex(index);
            if (elt != nullptr)
                elt->decrementRow();
        }

        return true;
    }

    return false;
}

int Soundfont::addInstrument()
{
    int index = _inst.add(new InstPrst(this, _inst.positionCount(), _instrumentTreeItem,
                                       EltID(elementInst, _id.indexSf2, _inst.indexCount(), -1, -1)));
    _inst.atIndex(index)->notifyCreated();
    return index;
}

InstPrst * Soundfont::getInstrument(int index)
{
    if (index < _inst.indexCount())
        return _inst.atIndex(index);
    return nullptr;
}

bool Soundfont::deleteInstrument(int index)
{
    // Check that the index is valid
    if (index >= _inst.indexCount())
        return false;

    InstPrst * inst = _inst.atIndex(index);
    if (inst != nullptr)
    {
        // Delete the instrument
        inst->notifyDeletion();
        delete _inst.takeAtIndex(index);

        // Update the row number of the following samples
        for (int i = index + 1; i < _inst.indexCount(); i++)
        {
            InstPrst * elt = _inst.atIndex(index);
            if (elt != nullptr)
                elt->decrementRow();
        }

        return true;
    }

    return false;
}

int Soundfont::addPreset()
{
    int index = _prst.add(new InstPrst(this, _prst.positionCount(), _presetTreeItem,
                                       EltID(elementPrst, _id.indexSf2, _prst.indexCount(), -1, -1)));
    _prst.atIndex(index)->notifyCreated();

    // Extra fields for presets
    _prst.atIndex(index)->setExtraField(champ_wPreset, 0);
    _prst.atIndex(index)->setExtraField(champ_wBank, 0);
    _prst.atIndex(index)->setExtraField(champ_dwLibrary, 0);
    _prst.atIndex(index)->setExtraField(champ_dwGenre, 0);
    _prst.atIndex(index)->setExtraField(champ_dwMorphology, 0);

    return index;
}

InstPrst * Soundfont::getPreset(int index)
{
    if (index < _prst.indexCount())
        return _prst.atIndex(index);
    return nullptr;
}

bool Soundfont::deletePreset(int index)
{
    // Check that the index is valid
    if (index >= _prst.indexCount())
        return false;

    InstPrst * prst = _prst.atIndex(index);
    if (prst != nullptr)
    {
        // Delete the preset
        prst->notifyDeletion();
        delete _prst.takeAtIndex(index);

        // Update the row number of the following samples
        for (int i = index + 1; i < _prst.indexCount(); i++)
        {
            InstPrst * elt = _prst.atIndex(index);
            if (elt != nullptr)
                elt->decrementRow();
        }

        return true;
    }

    return false;
}
