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

#include "sfzparameter.h"
#include "contextmanager.h"
#include <QDebug>

QString SfzParameter::DEFAULT_PATH = "";

SfzParameter::SfzParameter(QString opcode, QString valeur) :
    _opcode(op_unknown),
    _intValue(0),
    _dblValue(0.)
{
    QString valeurLow = valeur.toLower();
    opcode = opcode.remove('_');
    if (opcode == "sample")
    {
        _opcode = op_sample;
        _strValue = valeur.replace("\\", "/");
        if (!_strValue.isEmpty() && _strValue[0] == '/')
            _strValue = _strValue.right(_strValue.size() - 1);
        if (!DEFAULT_PATH.isEmpty())
            _strValue = DEFAULT_PATH + "/" + _strValue;
    }
    else if (opcode == "key")
    {
        _opcode = op_key;
        _intValue = ContextManager::keyName()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "lokey")
    {
        _opcode = op_keyMin;
        _intValue = ContextManager::keyName()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "hikey")
    {
        _opcode = op_keyMax;
        _intValue = ContextManager::keyName()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "lovel")
    {
        _opcode = op_velMin;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "hivel")
    {
        _opcode = op_velMax;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchkeycenter")
    {
        _opcode = op_rootKey;
        _intValue = ContextManager::keyName()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "group")
    {
        _opcode = op_exclusiveClass;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "offby")
    {
        _opcode = op_off_by;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "tune")
    {
        _opcode = op_tuningFine;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "transpose")
    {
        _opcode = op_tuningCoarse;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "delay")
    {
        _opcode = op_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "offset")
    {
        _opcode = op_offset;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "end")
    {
        _opcode = op_end;
        _intValue = valeurLow.toInt() + 1;
    }
    else if (opcode == "loopstart")
    {
        _opcode = op_loop_start;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "loopend")
    {
        _opcode = op_loop_end;
        _intValue = valeurLow.toInt() + 1;
    }
    else if (opcode == "loopmode")
    {
        _opcode = op_loop_mode;
        _strValue = valeurLow;
    }
    else if (opcode == "pan")
    {
        _opcode = op_pan;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "width")
    {
        _opcode = op_width;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "position")
    {
        _opcode = op_position;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "volume")
    {
        _opcode = op_volume;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchkeytrack")
    {
        _opcode = op_tuningScale;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "ampegdelay")
    {
        _opcode = op_ampeg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegattack")
    {
        _opcode = op_ampeg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampeghold")
    {
        _opcode = op_ampeg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegdecay")
    {
        _opcode = op_ampeg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegsustain")
    {
        _opcode = op_ampeg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegrelease")
    {
        _opcode = op_ampeg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegholdcc133")
    {
        _opcode = op_noteToVolEnvHold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegdecaycc133")
    {
        _opcode = op_noteToVolEnvDecay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "effect1")
    {
        _opcode = op_reverb;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "effect2")
    {
        _opcode = op_chorus;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filtype")
    {
        _opcode = op_filterType;
        _strValue = valeurLow;
    }
    else if (opcode == "cutoff")
    {
        _opcode = op_filterFreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "resonance")
    {
        _opcode = op_filterQ;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfodelay")
    {
        _opcode = op_vibLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfofreq")
    {
        _opcode = op_vibLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfodepth")
    {
        _opcode = op_vibLFOtoTon;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchegdelay")
    {
        _opcode = op_pitcheg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegattack")
    {
        _opcode = op_pitcheg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitcheghold")
    {
        _opcode = op_pitcheg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdecay")
    {
        _opcode = op_pitcheg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegsustain")
    {
        _opcode = op_pitcheg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegrelease")
    {
        _opcode = op_pitcheg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdepth")
    {
        _opcode = op_modEnvToTon;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchegholdcc133")
    {
        _opcode = op_noteToModEnvHold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdecaycc133")
    {
        _opcode = op_noteToModEnvDecay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfodelay")
    {
        _opcode = op_modLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfofreq")
    {
        _opcode = op_modLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfodepth")
    {
        _opcode = op_modLFOtoVolume;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampvelcurve1")
    {
        _opcode = op_amp_velcurve_1;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampvelcurve127")
    {
        _opcode = op_amp_velcurve_127;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdelay")
    {
        _opcode = op_fileg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegattack")
    {
        _opcode = op_fileg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fileghold")
    {
        _opcode = op_fileg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdecay")
    {
        _opcode = op_fileg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegsustain")
    {
        _opcode = op_fileg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegrelease")
    {
        _opcode = op_fileg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdepth")
    {
        _opcode = op_modEnvToFilter;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "filegholdcc133")
    {
        _opcode = op_fileg_holdcc133;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdecaycc133")
    {
        _opcode = op_fileg_decaycc133;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfodelay")
    {
        _opcode = op_filLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfofreq")
    {
        _opcode = op_filLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfodepth")
    {
        _opcode = op_modLFOtoFilter;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "lochan")
    {
        _opcode = op_chanMin;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "hichan")
    {
        _opcode = op_chanMax;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "trigger" && valeur == "attack")
    {
        // Nothing to do, no warning
    }
    else
        qWarning() << "opcode not supported: " + opcode + " (" + valeur + ")";
}
