#include "attribute.h"
#include "contextmanager.h"
#include <qmath.h>

Attribute::Attribute(AttributeType champ, bool isPrst) :
    _champ(champ),
    _isPrst(isPrst),
    _realValue(0)
{
    _storedValue.shValue = 0;
}

Attribute::Attribute(AttributeType champ, bool isPrst, double value) : Attribute(champ, isPrst)
{
    setRealValue(value);
}

Attribute::Attribute(AttributeType champ, bool isPrst, AttributeValue storedValue) : Attribute(champ, isPrst)
{
    setStoredValue(storedValue);
}

void Attribute::setRealValue(double realValue)
{
    _storedValue = fromRealValue(_champ, _isPrst, realValue);

    // Adapt the real value based on the new stored value
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(AttributeValue storedValue)
{
    _storedValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(qint16 storedValue)
{
    _storedValue.shValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(quint16 storedValue)
{
    _storedValue.wValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(qint8 lower, qint8 upper)
{
    _storedValue.rValue.byLo = (lower < upper ? lower : upper);
    _storedValue.rValue.byHi = (upper > lower ? upper : lower);
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

double Attribute::toRealValue(AttributeType champ, bool isPrst, AttributeValue storedValue)
{
    Q_UNUSED(isPrst);
    double realValue = 0;

    switch (champ)
    {
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        realValue = qPow(2., (double)storedValue.shValue / 1200.);
        break;
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset: case champ_endloopAddrsOffset:
    case champ_endAddrsOffset:
        realValue = storedValue.shValue;
        break;
    case champ_pan: case champ_initialAttenuation: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        realValue = (double)storedValue.shValue / 10.;
        break;
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: case champ_sampleModes:
    case champ_exclusiveClass: case champ_sampleID:
        realValue = storedValue.wValue;
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        realValue = storedValue.shValue * 32768;
        break;
    case champ_initialFilterFc: case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            realValue = qPow(2., (double)storedValue.shValue / 1200.);
        else
            realValue = qPow(2., (double)storedValue.shValue / 1200.) * 8.176;
        break;
    case champ_keyRange: case champ_velRange:
        realValue = storedValue.rValue.byLo * 1000 + storedValue.rValue.byHi;
        break;
    default:
        break;
    }

    return realValue;
}

AttributeValue Attribute::fromRealValue(AttributeType champ, bool isPrst, double realValue)
{
    AttributeValue storedValue;
    storedValue.dwValue = 0;

    switch (champ)
    {
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
        storedValue.shValue = qRound(realValue);
        break;
    case champ_pan: case champ_initialAttenuation: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        storedValue.shValue = qRound(realValue * 10.);
        break;
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: case champ_sampleModes:
    case champ_exclusiveClass: case champ_sampleID:
        storedValue.wValue = qRound(realValue);
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        storedValue.shValue = qRound(realValue) / 32768;
        break;
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
        storedValue.shValue = qRound(realValue) % 32768;
        break;
    case champ_keyRange: case champ_velRange:
        storedValue.rValue.byHi = (quint8)(realValue / 1000);
        storedValue.rValue.byLo = (quint8)(realValue - 1000 * storedValue.rValue.byHi);
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        storedValue.shValue = qRound(1200. * qLn(realValue) / 0.69314718056);
        break;
    case champ_initialFilterFc:
        if (isPrst)
            storedValue.shValue = (qint16)(1200. * qLn(realValue) / 0.69314718056);
        else
            storedValue.shValue = (qint16)(1200. * qLn(realValue / 8.176) / 0.69314718056);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            storedValue.shValue = (qint16)(1200. * qLn(realValue) / 0.69314718056);
        else
            storedValue.shValue = (qint16)(1200. * qLn(realValue / 8.176) / 0.69314718056);
        break;
        break;
    default:
        break;
    }

    return limit(champ, storedValue, isPrst);
}

double Attribute::getDefaultRealValue(AttributeType champ, bool isPrst)
{
    // Default stored value
    AttributeValue defaultStoredValue = Attribute::getDefaultStoredValue(champ);

    // Compute the corresponding real value
    return toRealValue(champ, isPrst, defaultStoredValue);
}

AttributeValue Attribute::getDefaultStoredValue(AttributeType champ)
{
    AttributeValue value;
    value.dwValue = 0;

    switch (champ)
    {
    case champ_fineTune: case champ_coarseTune: case champ_initialFilterQ:
    case champ_startAddrsOffset: case champ_endAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endloopAddrsOffset: case champ_initialAttenuation: case champ_pan:
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_sustainVolEnv: case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_sustainModEnv: case champ_modEnvToPitch: case champ_modEnvToFilterFc:
    case champ_modLfoToPitch: case champ_modLfoToFilterFc: case champ_modLfoToVolume:
    case champ_vibLfoToPitch: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_exclusiveClass: case champ_sampleModes:
        value.wValue = 0;
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        value.shValue = -12000;
        break;
    case champ_velRange: case champ_keyRange:
        value.rValue.byLo = 0;
        value.rValue.byHi = 127;
        break;
    case champ_keynum: case champ_velocity: case champ_overridingRootKey:
        value.shValue = -1;
        break;
    case champ_scaleTuning:
        value.shValue = 100;
        break;
    case champ_initialFilterFc:
        value.wValue = 13500;
        break;
    default:
        break;
    }

    return value;
}

AttributeValue Attribute::limit(AttributeType champ, AttributeValue value, bool isPrst)
{
    switch (champ)
    {
    case champ_keyRange: case champ_velRange:
        value.rValue.byLo = limit(value.rValue.byLo, 0, 127);
        value.rValue.byHi = limit(value.rValue.byHi, 0, 127);
        break;
    case champ_coarseTune:
        value.shValue = isPrst ?
                    limit(value.shValue, -240, 240) :
                    limit(value.shValue, -120, 120);
        break;
    case champ_fineTune:
        value.shValue = isPrst ?
                    limit(value.shValue, -198, 198) :
                    limit(value.shValue, -99, 99);
        break;
    case champ_scaleTuning:
        value.shValue = isPrst ?
                    limit(value.shValue, -1200, 1200) :
                    limit(value.shValue, 0, 1200);
        break;
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
        value.shValue = isPrst ?
                    limit(value.shValue, -24000, 24000) :
                    limit(value.shValue, -12000, 12000);
        break;
    case champ_pan:
        value.shValue = isPrst ?
                    limit(value.shValue, -1000, 1000) :
                    limit(value.shValue, -500, 500);
        break;
    case champ_sampleModes:
        value.wValue = limit(value.wValue, 0, 3);
        break;
    case champ_initialFilterQ:
        value.shValue = isPrst ?
                    limit(value.shValue, -960, 960) :
                    limit(value.shValue, 0, 960);
        break;
    case champ_modLfoToVolume:
        value.shValue = isPrst ?
                    limit(value.shValue, -1920, 1920) :
                    limit(value.shValue, -960, 960);
        break;
    case champ_overridingRootKey: case champ_keynum: case champ_exclusiveClass:
        value.wValue = limit(value.wValue, 0, 127);
        break;
    case champ_velocity:
        value.wValue = limit(value.wValue, 1, 127);
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
        value.shValue = limit(value.shValue, -32767, 32767);
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        value.shValue = isPrst ?
                    limit(value.shValue, -21000, 21000) :
                    limit(value.shValue, -12000, 5000);
        break;
    case champ_initialFilterFc:
        value.shValue = isPrst ?
                    limit(value.shValue, -21000, 21000) :
                    limit(value.shValue, 1500, 13500);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        value.shValue = isPrst ?
                    limit(value.shValue, -21000, 21000) :
                    limit(value.shValue, -16000, 4500);
        break;
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        value.shValue = isPrst ?
                    limit(value.shValue, -21000, 21000) :
                    limit(value.shValue, -12000, 8000);
        break;
    case champ_sustainVolEnv: case champ_initialAttenuation:
        value.shValue = isPrst ?
                    limit(value.shValue, -1440, 1440) :
                    limit(value.shValue, 0, 1440);
        break;
    case champ_sustainModEnv: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        value.shValue = isPrst ?
                    limit(value.shValue, -1000, 1000) :
                    limit(value.shValue, 0, 1000);
        break;
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
        value.shValue = isPrst ?
                    limit(value.shValue, -2400, 2400) :
                    limit(value.shValue, -1200, 1200);
        break;
    default:
        break;
    }

    return value;
}

int Attribute::limit(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

double Attribute::limit(double value, double min, double max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

QString Attribute::toString(AttributeType champ, bool isPrst, AttributeValue storedValue)
{
    QString result = "";

    switch (champ)
    {
    case champ_velRange:
        if (storedValue.rValue.byLo == storedValue.rValue.byHi)
            result = QString::number(storedValue.rValue.byLo);
        else
            result = QString::number(storedValue.rValue.byLo) + "-" + QString::number(storedValue.rValue.byHi);
        break;
    case champ_keyRange:
        if (storedValue.rValue.byLo == storedValue.rValue.byHi)
            result = ContextManager::keyName()->getKeyName(storedValue.rValue.byLo);
        else
            result = ContextManager::keyName()->getKeyName(storedValue.rValue.byLo) + "-" + ContextManager::keyName()->getKeyName(storedValue.rValue.byHi);
        break;
    case champ_initialAttenuation: case champ_pan: case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv: case champ_sustainModEnv:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        result = QString::number(toRealValue(champ, isPrst, storedValue), 'f', 1);
        break;
    case champ_keynum: case champ_overridingRootKey:
        result = ContextManager::keyName()->getKeyName(storedValue.wValue);
        break;
    case champ_sampleModes: case champ_exclusiveClass: case champ_velocity: case champ_sfModDestOper:
        result = QString::number(storedValue.wValue);
        break;
    case champ_scaleTuning: case champ_coarseTune: case champ_fineTune:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
    case champ_modAmount:
        result = QString::number(storedValue.shValue);
        break;
    case champ_initialFilterFc:
        result = QString::number(toRealValue(champ, isPrst, storedValue), 'f', isPrst ? 3 : 0);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        result = QString::number(toRealValue(champ, isPrst, storedValue), 'f', 3);
        break;
    case champ_sfModTransOper:
        if (storedValue.wValue == 2)
            result = ", " + trUtf8("valeur absolue");
        break;
    case champ_indexMod:
        result = QString::number(storedValue.wValue + 1);
        break;
    case champ_sfModAmtSrcOper: case champ_sfModSrcOper:
        result = QString::number(storedValue.sfModValue.Index);
        break;
    default: break;
    }

    return result;
}

AttributeValue Attribute::fromString(AttributeType champ, bool isPrst, QString strValue, bool &ok)
{
    AttributeValue value;
    value.wValue = 0;
    ok = true;

    switch (champ)
    {
    case champ_keyRange: case champ_velRange: {
        int posSeparator = strValue.indexOf(QRegExp("[0-9]-")) + 1;
        QString txtLeft, txtRight;
        if (posSeparator == 0)
            txtLeft = txtRight = strValue;
        else
        {
            txtLeft = strValue.left(posSeparator);
            txtRight = strValue.right(strValue.size() - posSeparator - 1);
        }

        int val1, val2;
        if (champ == champ_velRange)
        {
            bool tmp;
            val1 = txtLeft.toInt(&tmp);
            val2 = txtRight.toInt(&ok);
            ok &= tmp;
        }
        else
        {
            val1 = ContextManager::keyName()->getKeyNum(txtLeft);
            val2 = ContextManager::keyName()->getKeyNum(txtRight);
            ok = (val1 != -1) && (val2 != -1);
        }

        if (txtLeft.isEmpty())
            val1 = 0;
        if (txtRight.isEmpty())
            val2 = 127;

        if (val1 > val2)
        {
            int val3 = val1;
            val1 = val2;
            val2 = val3;
        }

        value.rValue.byLo = val1;
        value.rValue.byHi = val2;
        value = limit(champ, value, isPrst);
    }; break;
    case champ_initialAttenuation: case champ_sustainVolEnv: case champ_pan:
    case champ_coarseTune: case champ_fineTune: case champ_scaleTuning:
    case champ_initialFilterFc: case champ_initialFilterQ:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modLfoToVolume: case champ_sustainModEnv:
    case champ_delayModEnv: case champ_holdModEnv:
    case champ_delayVolEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_exclusiveClass: case champ_velocity:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
    case champ_startAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endAddrsOffset: case champ_endloopAddrsOffset:
    case champ_startAddrsCoarseOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
        value = fromRealValue(champ, isPrst, strValue.toDouble(&ok));
        break;
    case champ_sampleModes: {
        int iTmp = qRound(strValue.toDouble(&ok));
        if (iTmp != 0 && iTmp != 1 && iTmp != 3)
            iTmp = 0;
        value.wValue = iTmp;
    }break;
    case champ_overridingRootKey: case champ_keynum: {
        int keyNum = ContextManager::keyName()->getKeyNum(strValue);
        ok = (keyNum >= 0 && keyNum <= 127);
        value.wValue = keyNum;
    }break;
    default:
        ok = false;
    }

    if (!ok)
        value.dwValue = 0;

    return value;
}
