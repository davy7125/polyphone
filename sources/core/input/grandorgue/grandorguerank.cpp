/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "grandorguerank.h"
#include "grandorguepipe.h"
#include "grandorguedatathrough.h"
#include "soundfontmanager.h"
#include "utils.h"

GrandOrgueRank::GrandOrgueRank(QString rootDir, GrandOrgueDataThrough *godt, int id) :
    _rootDir(rootDir),
    _godt(godt),
    _id(id),
    _gain(0),
    _tuning(0)
{

}

GrandOrgueRank::~GrandOrgueRank()
{
    while (!_pipes.isEmpty())
        delete _pipes.take(_pipes.firstKey());
}

void GrandOrgueRank::readData(QString key, QString value)
{
    if (key.startsWith("pipe"))
    {
        if (key.length() < 7)
            return;

        // Extract the number of the pipe
        key = key.right(key.length() - 4);
        bool ok = false;
        int number = key.left(3).toInt(&ok);
        if (!ok || number < 0)
            return;

        // Property
        QString property = key.length() > 3 ? key.right(key.length() - 3) : "#";

        // Store data
        if (!_pipes.contains(number))
            _pipes[number] = new GrandOrguePipe(_rootDir, _godt);
        _pipes[number]->readData(property, value);
    }
    else if (key == "gain")
    {
        bool ok = false;
        _gain = value.toDouble(&ok);
        if (!ok)
        {
            qDebug() << "couldn't read rank gain:" << value;
            _gain = 0;
        }
    }
    else if (key == "amplitudelevel")
    {
        bool ok = false;
        int amplitude = value.toInt(&ok);
        if (ok)
            this->mergeAmplitude(amplitude);
        else
            qDebug() << "couldn't read rank amplitude:" << value;
    }
    else if (key == "pitchtuning")
    {
        bool ok = false;
        _tuning = value.toInt(&ok);
        if (!ok)
        {
            qDebug() << "couldn't read rank tuning:" << value;
            _tuning = 0;
        }
    }
    else
        _properties[key] = value;
}

void GrandOrgueRank::preProcess()
{
    if (!_pipes.isEmpty())
    {
        // Include the gain and the tuning of the rank in the pipes
        foreach (GrandOrguePipe * pipe, _pipes.values())
        {
            pipe->addGain(_gain);
            pipe->addTuning(_tuning);
        }

        // Maximum gain of the pipes
        bool first = true;
        double maxGain = 0;
        foreach (GrandOrguePipe * pipe, _pipes.values())
        {
            if (first)
                maxGain = pipe->gain();
            else
                maxGain = qMax(maxGain, pipe->gain());
        }
        _godt->setMaxRankGain(_id, maxGain);
    }
}

EltID GrandOrgueRank::process(SoundfontManager * sm, int sf2Index, int indexOfFirstSample, int keyOfFirstSample)
{
    // At least one valid pipe?
    if (!isValid())
        return EltID();

    // New instrument
    EltID idInst(elementInst, sf2Index);
    idInst.indexElt = sm->add(idInst);

    // Name
    sm->set(idInst, champ_name, _properties.contains("name") ? _properties["name"] : QObject::tr("untitled"));

    // Instrument gain
    AttributeValue val;
    val.wValue = static_cast<quint16>(25. * (_gain - _godt->getMaxGain()) + 0.5); // 25 is 10 divided by 0.4;
    sm->set(idInst, champ_initialAttenuation, val);

    // Instrument tuning
    int fineTune = _tuning % 100;
    int coarseTune = _tuning / 100;
    if (fineTune > 50)
    {
        fineTune -= 100;
        coarseTune += 1;
    }
    else if (fineTune < -50)
    {
        fineTune += 100;
        coarseTune -= 1;
    }
    val.shValue = fineTune;
    sm->set(idInst, champ_fineTune, val);
    val.shValue = coarseTune;
    sm->set(idInst, champ_coarseTune, val);

    // Disable default modulators
    disableModulators(sm, idInst);

    // Loop mode
    val.wValue = (_properties.contains("percussive") && _properties["percussive"].toLower() == "y") ? 0 : 1;
    sm->set(idInst, champ_sampleModes, val);

    // Associate samples
    foreach (int index, _pipes.keys())
        _pipes[index]->process(idInst, index - indexOfFirstSample + keyOfFirstSample);

    // Simplifications
    sm->simplify(idInst, champ_fineTune);
    sm->simplify(idInst, champ_coarseTune);
    sm->simplify(idInst, champ_initialAttenuation);
    sm->simplify(idInst, champ_sampleModes);

    return idInst;
}

void GrandOrgueRank::mergeAmplitude(int amplitude)
{
    // Translate into a gain in dB
    double coef = 0.01 * static_cast<double>(amplitude);
    _gain += 20. * log10(coef);
}

bool GrandOrgueRank::isValid()
{
    // The rank must have at least one valid pipe
    foreach (GrandOrguePipe * pipe, _pipes.values())
        if (pipe->isValid())
            return true;
    return false;
}

void GrandOrgueRank::disableModulators(SoundfontManager * sm, EltID idInst)
{
    EltID idMod(elementInstMod, idInst.indexSf2, idInst.indexElt);
    AttributeValue val;

    // Disable "MIDI Note-On Velocity to Initial Attenuation"
    idMod.indexMod = sm->add(idMod);
    val.sfModValue = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeConcave, true, false);
    sm->set(idMod, champ_sfModSrcOper, val);
    val.wValue = champ_initialAttenuation;
    sm->set(idMod, champ_sfModDestOper, val);
    val.wValue = 0;
    sm->set(idMod, champ_modAmount, val);
    val.sfModValue = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
    sm->set(idMod, champ_sfModAmtSrcOper, val);
    val.sfTransValue = SFTransform::linear;
    sm->set(idMod, champ_sfModTransOper, val);

    // Disable "MIDI Note-On Velocity to Filter Cutoff"
    idMod.indexMod = sm->add(idMod);
    val.sfModValue = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, true, false);
    sm->set(idMod, champ_sfModSrcOper, val);
    val.wValue = champ_initialFilterFc;
    sm->set(idMod, champ_sfModDestOper, val);
    val.wValue = 0;
    sm->set(idMod, champ_modAmount, val);
    val.sfModValue = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
    sm->set(idMod, champ_sfModAmtSrcOper, val);
    val.sfTransValue = SFTransform::linear;
    sm->set(idMod, champ_sfModTransOper, val);
}
