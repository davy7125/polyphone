/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "enginevoiceparameters.h"
#include "voiceparam.h"
#include "offlinerenderer.h"
#include "synthcoreconfig.h"
#include "soundfont.h"
#include "instprst.h"
#include "division.h"
#include "smpl.h"

EngineVoiceParameters::EngineVoiceParameters(int indexSf2, Soundfont * soundfont) :
    _indexSf2(indexSf2),
    _soundfont(soundfont)
{
    if (_soundfont != nullptr)
        OfflineRenderer::applyDefaultControllerValues(_soundfont, _midiValues);
    _context.setMidiValues(&_midiValues);
    _context.setModulatorVelToFilterType(SynthCoreConfig().modulatorVelToFilterType);
    _param = new VoiceParam(&_context);
}

EngineVoiceParameters::~EngineVoiceParameters()
{
    delete _param;
}

VoiceParam * EngineVoiceParameters::compute(InstPrst * prst, int prstDivIndex, InstPrst * inst, int instDivIndex,
                                            int key, int velocity)
{
    if (_soundfont == nullptr || inst == nullptr)
        return nullptr;
    Division * prstDiv = prst != nullptr ? prst->getDivision(prstDivIndex) : nullptr;
    Division * instDiv = inst->getDivision(instDivIndex);
    if (instDiv == nullptr || !instDiv->getAttributeSet()[champ_sampleID])
        return nullptr;
    Smpl * smpl = _soundfont->getSample(instDiv->getAttributeValues()[champ_sampleID].wValue);
    if (smpl == nullptr)
        return nullptr;

    _param->initialize(_indexSf2, prst, prstDiv, inst, instDiv, smpl, static_cast<int8_t>(prst != nullptr ? 0 : -1),
                       static_cast<int8_t>(key), static_cast<int8_t>(velocity), 0);
    return _param;
}

int EngineVoiceParameters::getStaticPitchShift(VoiceParam * param)
{
    return (param->getCurrentInteger(champ_keynum) - param->getCurrentInteger(champ_overridingRootKey)) *
            param->getCurrentInteger(champ_scaleTuning) + param->getCurrentInteger(champ_fineTune) +
            100 * param->getCurrentInteger(champ_coarseTune);
}
