#ifndef SFZPARAMETER_H
#define SFZPARAMETER_H

#include <QString>

class SfzParameter
{
public:
    enum OpCode
    {
        op_unknown,
        op_sample,
        op_key,
        op_keyMin,
        op_keyMax,
        op_velMin,
        op_velMax,
        op_chanMin,
        op_chanMax,
        op_rootKey,
        op_exclusiveClass,
        op_off_by,
        op_tuningFine,
        op_tuningCoarse,
        op_delay,
        op_offset,
        op_end,
        op_loop_start,
        op_loop_end,
        op_loop_mode,
        op_volume,
        op_pan,
        op_width,
        op_position,
        op_tuningScale,
        op_chorus,
        op_reverb,
        op_filterType,
        op_filterFreq,
        op_filterQ,
        op_amp_velcurve_1,
        op_amp_velcurve_127,

        // Enveloppe volume
        op_ampeg_delay,
        op_ampeg_attack,
        op_ampeg_hold,
        op_ampeg_decay,
        op_ampeg_sustain,
        op_ampeg_release,
        op_noteToVolEnvHold,
        op_noteToVolEnvDecay,

        // LFO volume
        op_modLFOdelay,
        op_modLFOfreq,
        op_modLFOtoVolume,

        // Enveloppe pitch
        op_pitcheg_delay,
        op_pitcheg_attack,
        op_pitcheg_hold,
        op_pitcheg_decay,
        op_pitcheg_sustain,
        op_pitcheg_release,
        op_noteToModEnvHold,
        op_noteToModEnvDecay,
        op_modEnvToTon,

        // LFO pitch
        op_vibLFOdelay,
        op_vibLFOfreq,
        op_vibLFOtoTon,

        // Enveloppe filtre (si équivalent à enveloppe pitch, ou si enveloppe pitch non utilisé)
        op_fileg_delay,
        op_fileg_attack,
        op_fileg_hold,
        op_fileg_decay,
        op_fileg_sustain,
        op_fileg_release,
        op_modEnvToFilter,
        op_fileg_holdcc133,
        op_fileg_decaycc133,

        // LFO filtre (si équivalent à LFO volume ou si LFO volume non utilisé)
        op_filLFOdelay,
        op_filLFOfreq,
        op_modLFOtoFilter
    };

    SfzParameter(QString opcode, QString valeur);
    SfzParameter(OpCode opcode, int valeur) :
        _opcode(opcode),
        _intValue(valeur),
        _dblValue(0.)
    {}
    SfzParameter(OpCode opcode, double valeur) :
        _opcode(opcode),
        _intValue(0),
        _dblValue(valeur)
    {}
    SfzParameter(OpCode opcode, QString valeur) :
        _opcode(opcode),
        _intValue(0),
        _dblValue(0),
        _strValue(valeur)
    {}
    OpCode  getOpCode()      const       { return _opcode;   }
    int     getIntValue()    const       { return _intValue; }
    double  getDoubleValue() const       { return _dblValue; }
    QString getStringValue() const       { return _strValue; }
    void    setIntValue(int value)       { _intValue = value; }
    void    setDoubleValue(double value) { _dblValue = value; }

private:
    OpCode  _opcode;
    int     _intValue;
    double  _dblValue;
    QString _strValue;
};

#endif // SFZPARAMETER_H
