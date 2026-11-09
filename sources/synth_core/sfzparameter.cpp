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


#include "sfzparameter.h"
#include "textconversions.h"
using namespace TextConversions;

static int getKeyNumber(const SfzContext * context, const std::string &keyName)
{
    return (context != nullptr && context->getKeyNumber) ? context->getKeyNumber(keyName) :
                                                           SfzParameter::getDefaultKeyNumber(keyName);
}

SfzParameter::SfzParameter(std::string opcode, std::string valeur, const SfzContext * context) :
    _opcode(op_unknown),
    _intValue(0),
    _dblValue(0.)
{
    std::string valeurLow = toLower(valeur);
    opcode = replaceAll(opcode, "_", "");
    if (opcode == "sample")
    {
        _opcode = op_sample;
        _strValue = replaceAll(valeur, "\\", "/");
        if (!_strValue.empty() && _strValue[0] == '/')
            _strValue = _strValue.substr(1);
        if (context != nullptr && !context->defaultPath.empty())
            _strValue = context->defaultPath + "/" + _strValue;
    }
    else if (opcode == "key")
    {
        _opcode = op_key;
        _intValue = getKeyNumber(context, valeurLow);
    }
    else if (opcode == "lokey")
    {
        _opcode = op_keyMin;
        _intValue = getKeyNumber(context, valeurLow);
    }
    else if (opcode == "hikey")
    {
        _opcode = op_keyMax;
        _intValue = getKeyNumber(context, valeurLow);
    }
    else if (opcode == "lovel")
    {
        _opcode = op_velMin;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "hivel")
    {
        _opcode = op_velMax;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "pitchkeycenter")
    {
        _opcode = op_rootKey;
        _intValue = getKeyNumber(context, valeurLow);
    }
    else if (opcode == "group")
    {
        _opcode = op_exclusiveClass;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "offby")
    {
        _opcode = op_off_by;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "tune")
    {
        _opcode = op_tuningFine;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "transpose")
    {
        _opcode = op_tuningCoarse;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "delay")
    {
        _opcode = op_delay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "offset")
    {
        _opcode = op_offset;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "end")
    {
        _opcode = op_end;
        _intValue = toInt(valeurLow) + 1;
    }
    else if (opcode == "loopstart")
    {
        _opcode = op_loop_start;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "loopend")
    {
        _opcode = op_loop_end;
        _intValue = toInt(valeurLow) + 1;
    }
    else if (opcode == "loopmode")
    {
        _opcode = op_loop_mode;
        _strValue = valeurLow;
    }
    else if (opcode == "trigger")
    {
        _opcode = op_trigger;
        _strValue = valeurLow;
    }
    else if (opcode == "pan")
    {
        _opcode = op_pan;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "width")
    {
        _opcode = op_width;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "position")
    {
        _opcode = op_position;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "volume")
    {
        _opcode = op_volume;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchkeytrack")
    {
        _opcode = op_tuningScale;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "ampegdelay")
    {
        _opcode = op_ampeg_delay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegattack")
    {
        _opcode = op_ampeg_attack;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampeghold")
    {
        _opcode = op_ampeg_hold;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegdecay")
    {
        _opcode = op_ampeg_decay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegsustain")
    {
        _opcode = op_ampeg_sustain;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegrelease")
    {
        _opcode = op_ampeg_release;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegholdcc133")
    {
        _opcode = op_noteToVolEnvHold;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampegdecaycc133")
    {
        _opcode = op_noteToVolEnvDecay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "effect1")
    {
        _opcode = op_reverb;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "effect2")
    {
        _opcode = op_chorus;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filtype")
    {
        _opcode = op_filterType;
        _strValue = valeurLow;
    }
    else if (opcode == "cutoff")
    {
        _opcode = op_filterFreq;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "resonance")
    {
        _opcode = op_filterQ;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchlfodelay")
    {
        _opcode = op_vibLFOdelay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchlfofreq")
    {
        _opcode = op_vibLFOfreq;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchlfodepth")
    {
        _opcode = op_vibLFOtoTon;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "pitchegdelay")
    {
        _opcode = op_pitcheg_delay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegattack")
    {
        _opcode = op_pitcheg_attack;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitcheghold")
    {
        _opcode = op_pitcheg_hold;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegdecay")
    {
        _opcode = op_pitcheg_decay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegsustain")
    {
        _opcode = op_pitcheg_sustain;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegrelease")
    {
        _opcode = op_pitcheg_release;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegdepth")
    {
        _opcode = op_modEnvToTon;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "pitchegholdcc133")
    {
        _opcode = op_noteToModEnvHold;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "pitchegdecaycc133")
    {
        _opcode = op_noteToModEnvDecay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "amplfodelay")
    {
        _opcode = op_modLFOdelay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "amplfofreq")
    {
        _opcode = op_modLFOfreq;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "amplfodepth")
    {
        _opcode = op_modLFOtoVolume;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampveltrack")
    {
        _opcode = op_amp_veltrack;
        _intValue = toDouble(valeurLow);
    }
    else if (opcode == "ampvelcurve1")
    {
        _opcode = op_amp_velcurve_1;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "ampvelcurve127")
    {
        _opcode = op_amp_velcurve_127;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegdelay")
    {
        _opcode = op_fileg_delay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegattack")
    {
        _opcode = op_fileg_attack;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "fileghold")
    {
        _opcode = op_fileg_hold;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegdecay")
    {
        _opcode = op_fileg_decay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegsustain")
    {
        _opcode = op_fileg_sustain;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegrelease")
    {
        _opcode = op_fileg_release;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegdepth")
    {
        _opcode = op_modEnvToFilter;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "filegholdcc133")
    {
        _opcode = op_fileg_holdcc133;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "filegdecaycc133")
    {
        _opcode = op_fileg_decaycc133;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "fillfodelay")
    {
        _opcode = op_filLFOdelay;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "fillfofreq")
    {
        _opcode = op_filLFOfreq;
        _dblValue = toDouble(valeurLow);
    }
    else if (opcode == "fillfodepth")
    {
        _opcode = op_modLFOtoFilter;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "lochan")
    {
        _opcode = op_chanMin;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "hichan")
    {
        _opcode = op_chanMax;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "filveltrack")
    {
        _opcode = op_fil_veltrack;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "filkeytrack")
    {
        _opcode = op_fil_keytrack;
        _intValue = toInt(valeurLow);
    }
    else if (opcode == "filkeycenter")
    {
        _opcode = op_fil_keycenter;
        _intValue = getKeyNumber(context, valeurLow);
    }
    else if (context != nullptr && context->onUnsupportedOpcode)
        context->onUnsupportedOpcode(opcode, valeur);
}

int SfzParameter::getDefaultKeyNumber(std::string keyName)
{
    keyName = toLower(replaceAll(replaceAll(replaceAll(keyName, ",", "."), "\xE2\x99\xAF", "#"), "\xE2\x99\xAD", "b"));
    bool isDouble;
    int note = qtRound(toDouble(keyName, &isDouble));
    if (!isDouble && utf16Length(keyName) >= 2)
    {
        // Separate the keyname from the octave number
        size_t split = keyName.find('-');
        if (split == std::string::npos)
            split = keyName.size() - 1;
        std::string octaveStr = keyName.substr(split);
        keyName = keyName.substr(0, split);

        // Which key?
        if (keyName == "c")
            note = 60;
        else if (keyName == "c#" || keyName == "db")
            note = 61;
        else if (keyName == "d")
            note = 62;
        else if (keyName == "d#" || keyName == "eb")
            note = 63;
        else if (keyName == "e")
            note = 64;
        else if (keyName == "f")
            note = 65;
        else if (keyName == "f#" || keyName == "gb")
            note = 66;
        else if (keyName == "g")
            note = 67;
        else if (keyName == "g#" || keyName == "ab" || keyName == "hb")
            note = 68;
        else if (keyName == "a" || keyName == "h")
            note = 69;
        else if (keyName == "a#" || keyName == "h#" || keyName == "bb")
            note = 70;
        else if (keyName == "b")
            note = 71;
        else
            return -1;

        // Include the octave
        if (!octaveStr.empty())
        {
            int octave = toInt(octaveStr);
            if (octave == 0 && octaveStr != "0")
                return -1;
            else
                note += (octave - 4) * 12;
        }
    }
    return (note > 127 || note < -1) ? -1 : note;
}
