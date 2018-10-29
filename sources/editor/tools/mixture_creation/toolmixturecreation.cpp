#include "toolmixturecreation.h"
#include "toolmixturecreation_gui.h"
#include "soundfontmanager.h"
#include "runnablesamplecreator.h"

ToolMixtureCreation::ToolMixtureCreation() : AbstractTool(new ToolMixtureCreation_parameters(), new ToolMixtureCreation_gui()),
    _waitingDialog(NULL)
{
    connect(this, SIGNAL(elementProcessed(EltID, int, int, double)), this, SLOT(onElementProcessed(EltID, int, int, double)), Qt::QueuedConnection);
}

ToolMixtureCreation::~ToolMixtureCreation()
{
    delete _waitingDialog;
}

bool ToolMixtureCreation::isCompatible(IdList ids)
{
    // Ok if there is only one instrument
    return ids.getSelectedIds(elementInst).count() == 1;
}

void ToolMixtureCreation::run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *)parameters;
    _sampleKey.clear();
    _warning = "";

    // Current instrument
    EltID idInst = ids.getSelectedIds(elementInst)[0];
    idInst.typeElement = elementInst;
    if (sm->getSiblings(EltID(elementInstSmpl, idInst.indexSf2, idInst.indexElt)).empty())
    {
        _warning = trUtf8("L'instrument doit contenir des sons.");
        return;
    }

    // Get the divisions, remove those that are empty
    QList<DivisionInfo> dis = params->getDivisions();
    for (int i = dis.count() - 1; i >= 0; i--)
        if (dis[0].getRanks().empty())
            dis.removeAt(i);
    if (dis.empty())
        return;

    // Number of keys per sample
    int keyNumber = 1;
    switch (params->getDensityType())
    {
    case 0: keyNumber = 1; break;
    case 1: keyNumber = 3; break;
    case 2: keyNumber = 6; break;
    default:
        return;
    }

    // Other parameters
    bool loopEnabled = params->getLoopSample();
    _stereoSamples = params->getStereoSample();
    _instrumentName = params->getInstrumentName();

    // Compute the number of steps (one for each sample)
    _currentStep = 0;
    _steps = 0;
    for (int i = 0; i < dis.length(); i++)
        _steps += qAbs(dis[i].getMaxKey() - dis[i].getMinKey()) / keyNumber + 1;
    if (_stereoSamples)
        _steps *= 2;

    // Create a new instrument
    _idNewInst = idInst;
    _idNewInst.indexElt = sm->add(_idNewInst);
    sm->set(_idNewInst, champ_name, _instrumentName.left(20));
    if (loopEnabled)
    {
        AttributeValue value;
        value.wValue = 1;
        sm->set(_idNewInst, champ_sampleModes, value);
    }

    // Create and open a progress dialog
    if (_waitingDialog != NULL)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // For each division
    foreach (DivisionInfo di, dis)
    {
        // Corresponding key range
        int noteStart2 = qMin(di.getMinKey(), di.getMaxKey());
        int noteEnd = qMax(di.getMinKey(), di.getMaxKey());
        int noteStart = noteStart2 + (noteEnd - noteStart2) % keyNumber;

        // For each key
        for (int key = noteStart; key <= noteEnd; key += keyNumber)
        {
            // For each side (left / right)
            for (int side = 0; side < 1 + (_stereoSamples ? 1 : 0); side++)
            {
                RunnableSampleCreator * rsc = new RunnableSampleCreator(this, idInst, di, key,
                                                                        qMax(noteStart2, key - keyNumber + 1), // min key
                                                                        loopEnabled, _stereoSamples, side);
                QThreadPool::globalInstance()->start(rsc);
            }
        }
    }
}

void ToolMixtureCreation::sampleDataReady(EltID idSmpl, int key, int minKey, double minAtt)
{
    emit(elementProcessed(idSmpl, key, minKey, minAtt));
}

void ToolMixtureCreation::onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt)
{
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Sample name
    QString name;
    if (_stereoSamples)
        name = _instrumentName.left(15);
    else
        name = _instrumentName.left(16);
    QString str2 = QString("%1").arg(key, 3, 10, QChar('0'));
    bool isRight;
    if (_stereoSamples)
    {
        isRight = (sm->get(idSmpl, champ_sfSampleType).sfLinkValue == rightSample);
        if (isRight)
            name = name + ' ' + str2 + 'R';
        else
            name = name + ' ' + str2 + 'L';
    }
    else
        name = name + ' ' + str2;
    qDebug() << "sample name" << name;
    sm->set(idSmpl, champ_name, name);

    // Link samples

    //    if (side == 1)
    //    {
    //        EltID idLink = idSmpl;
    //        idLink.indexElt = idSmpl.indexElt - 1;
    //        value.wValue = idLink.indexElt;
    //        sm->set(idSmpl, champ_wSampleLink, value);
    //        value.wValue = idSmpl.indexElt;
    //        sm->set(idLink, champ_wSampleLink, value);
    //    }

    // Ajout du sample dans l'instrument
    EltID idInstSmpl = _idNewInst;
    idInstSmpl.typeElement = elementInstSmpl;
    idInstSmpl.indexElt2 = sm->add(idInstSmpl);

    // Configuration
    AttributeValue value;
    value.wValue = idSmpl.indexElt;
    sm->set(idInstSmpl, champ_sampleID, value);
    value.rValue.byLo = minKey;
    value.rValue.byHi = key;
    sm->set(idInstSmpl, champ_keyRange, value);
    if (_stereoSamples)
    {
        if (isRight)
            value.shValue = 500;
        else
            value.shValue = -500;
    }
    else
        value.shValue = 0;
    sm->set(idInstSmpl, champ_pan, value);
    value.wValue = minAtt * 10;
    sm->set(idInstSmpl, champ_initialAttenuation, value);





    if (_waitingDialog == NULL)
        return; // Just in case

    _waitingDialog->setValue(++_currentStep);
    if (_currentStep >= _steps)
    {
        delete _waitingDialog;
        _waitingDialog = NULL;
        if (_canceled)
        {
            SoundfontManager::getInstance()->revertNewEditing();
            emit(finished(false));
        }
        else
            emit(finished(true));
    }
}

void ToolMixtureCreation::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

QString ToolMixtureCreation::getWarning()
{
    return _warning;
}
