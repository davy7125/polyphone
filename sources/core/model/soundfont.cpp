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
    _generalTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Général"), NULL, _rootItem, EltID(elementSf2, _id.indexSf2, -1, -1, -1));
    _sampleTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Échantillons"), (QMap<int, TreeItem*> *)&_smpl, _rootItem, EltID(elementRootSmpl, _id.indexSf2, -1, -1, -1));
    _instrumentTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Instruments"), (QMap<int, TreeItem*> *)&_inst, _rootItem, EltID(elementRootInst, _id.indexSf2, -1, -1, -1));
    _presetTreeItem = new TreeItemFirstLevel(QObject::trUtf8("Presets"), (QMap<int, TreeItem*> *)&_prst, _rootItem, EltID(elementRootPrst, _id.indexSf2, -1, -1, -1));

    // Default attributes for a soundfont
    SfVersionTag sfVersionTmp;
    sfVersionTmp.wMajor = 2;
    sfVersionTmp.wMinor = 4;
    this->_IFIL = sfVersionTmp;
    sfVersionTmp.wMajor = 0;
    sfVersionTmp.wMinor = 0;
    this->_IVER = sfVersionTmp;
    this->_ISNG = "EMU8000";
    this->_INAM = "";
    this->_IROM = "";
    this->_ICRD = "";
    this->_IENG = "";
    this->_IPRD = "";
    this->_ICOP = "";
    this->_ICMT = "";
    this->_ISFT = "";
    this->_fileNameForData = "";
    this->_fileNameInitial = "";
    this->_numEdition = 0;
}

Soundfont::~Soundfont()
{
    // Delete all presets
    QList<int> keys = _prst.keys();
    foreach (int key, keys)
        delete _prst.take(key);

    // Delete all instruments
    keys = _inst.keys();
    foreach (int key, keys)
        delete _inst.take(key);

    // Delete all samples
    keys = _smpl.keys();
    foreach (int key, keys)
        delete _smpl.take(key);

    // Delete headers
    _presetTreeItem->notifyDeletion();
    delete _presetTreeItem;
    _instrumentTreeItem->notifyDeletion();
    delete _instrumentTreeItem;
    _sampleTreeItem->notifyDeletion();
    delete _sampleTreeItem;
    _generalTreeItem->notifyDeletion();
    delete _generalTreeItem;

    // Delete the model
    delete _model;
}

int Soundfont::addSample()
{
    _smpl[_smplCounter] = new Smpl(this, _sampleTreeItem, EltID(elementSmpl, _id.indexSf2, _smplCounter, -1, -1));
    _smpl[_smplCounter]->notifyCreation();
    return _smplCounter++;
}

Smpl * Soundfont::getSample(int index)
{
    if (_smpl.contains(index))
        return _smpl[index];
    return NULL;
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
    _mutexInst.lock();
    _inst[_instCounter] = new InstPrst(this, _instrumentTreeItem, EltID(elementInst, _id.indexSf2, _instCounter, -1, -1));
    _inst[_instCounter]->notifyCreation();
    int result = _instCounter++;
    _mutexInst.unlock();

    return result;
}

InstPrst * Soundfont::getInstrument(int index)
{
    _mutexInst.lock();
    InstPrst * result = NULL;
    if (_inst.contains(index))
        result = _inst[index];
    _mutexInst.unlock();

    return result;
}

bool Soundfont::deleteInstrument(int index)
{
    _mutexInst.lock();
    if (_inst.contains(index))
    {
        _inst[index]->notifyDeletion();
        delete _inst.take(index);
        _mutexInst.unlock();
        return true;
    }

    _mutexInst.unlock();
    return false;
}

int Soundfont::indexOfInstrument(InstPrst * inst)
{
    _mutexInst.lock();
    int result = _inst.values().indexOf(inst);
    _mutexInst.unlock();

    return result;
}

int Soundfont::addPreset()
{
    _prst[_prstCounter] = new InstPrst(this, _presetTreeItem, EltID(elementPrst, _id.indexSf2, _prstCounter, -1, -1));
    _prst[_prstCounter]->notifyCreation();

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
    return NULL;
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
