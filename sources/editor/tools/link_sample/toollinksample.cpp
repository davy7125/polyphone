#include "toollinksample.h"
#include "soundfontmanager.h"

void ToolLinkSample::beforeProcess()
{
    _smplIds.clear();
    _names.clear();
    _leftRight.clear();
    _processedIds.clear();
    _sampleErrors.clear();
    _scanned = false;
}

void ToolLinkSample::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);

    // Scan if not done
    if (!_scanned)
    {
        this->scan(sm, id.indexSf2);
        _scanned = true;
    }

    // Return immediately if the sample has already been processed
    if (_processedIds.contains(id.indexElt))
        return;

    // Try to find the sample in the list
    int position = -1;
    for (int i = 0; i < _smplIds.count(); i++)
    {
        if (_smplIds[i] == id.indexElt)
        {
            position = i;
            break;
        }
    }

    // If not, the sample is not suitable for a stereo match
    if (position == -1)
    {
        _sampleErrors << sm->getQstr(id, champ_name);
        return;
    }

    // Try to find a match
    QString nameToFind = _names.takeAt(position);
    bool leftRightToFind = !_leftRight.takeAt(position);
    _smplIds.removeAt(position);
    int match = -1;
    for (int i = 0; i < _smplIds.count(); i++)
    {
        if (_names[i] == nameToFind && _leftRight[i] == leftRightToFind)
        {
            match = i;
            break;
        }
    }

    // Match found?
    if (match == -1)
        _sampleErrors << sm->getQstr(id, champ_name);
    else
    {
        // A match has been found
        int otherId = _smplIds.takeAt(match);
        _names.removeAt(match);
        _leftRight.removeAt(match);
        _processedIds << id.indexElt << otherId;

        // Associate two samples
        if (leftRightToFind)
            this->associate(sm, id, EltID(elementSmpl, id.indexSf2, otherId));
        else
            this->associate(sm, EltID(elementSmpl, id.indexSf2, otherId), id);
    }
}

void ToolLinkSample::scan(SoundfontManager * sm, int indexSf2)
{
    // Browse all sample names and classify them
    QList<int> allSamples = sm->getSiblings(EltID(elementSmpl, indexSf2));
    foreach (int i, allSamples)
    {
        QString sampleName = sm->getQstr(EltID(elementSmpl, indexSf2, i), champ_name);
        if (sampleName.right(1).toUpper() == "L")
        {
            _smplIds.append(i);
            _names.append(sampleName.left(sampleName.size() - 1));
            _leftRight.append(false);
        }
        else if (sampleName.right(1).toUpper() == "R")
        {
            _smplIds.append(i);
            _names.append(sampleName.left(sampleName.size() - 1));
            _leftRight.append(true);
        }
    }
}

void ToolLinkSample::associate(SoundfontManager * sm, EltID leftSmpl, EltID rightSmpl)
{
    // First disconnect samples that could be linked to either the left or right sample
    this->disconnectTarget(sm, leftSmpl);
    this->disconnectTarget(sm, rightSmpl);

    // Then link left with right sample
    Valeur value;
    value.sfLinkValue = leftSample;
    sm->set(leftSmpl, champ_sfSampleType, value);
    value.wValue = rightSmpl.indexElt;
    sm->set(leftSmpl, champ_wSampleLink, value);

    value.sfLinkValue = rightSample;
    sm->set(rightSmpl, champ_sfSampleType, value);
    value.wValue = leftSmpl.indexElt;
    sm->set(rightSmpl, champ_wSampleLink, value);
}

void ToolLinkSample::disconnectTarget(SoundfontManager * sm, EltID id)
{
    SFSampleLink type = sm->get(id, champ_sfSampleType).sfLinkValue;
    if (type != linkInvalid && type != monoSample)
    {
        int otherSample = sm->get(id, champ_wSampleLink).wValue;
        EltID otherID(elementSmpl, id.indexSf2, otherSample);
        if (sm->isValid(otherID))
        {
            Valeur value;
            value.sfLinkValue = monoSample;
            sm->set(otherID, champ_sfSampleType, value);
            value.wValue = 0;
            sm->set(otherID, champ_wSampleLink, value);
        }
    }
}
