/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "page.h"
#include "pile_sf2.h"
#include "config.h"
#include "mainwindow.h"
#include <qmath.h>

Page::Page(TypePage typePage, QWidget *parent) : QWidget(parent),
    _preparation(false),
    _typePage(typePage)
{}

MainWindow * Page::_mainWindow = NULL;
Tree * Page::_tree = NULL;
QStackedWidget * Page::_qStackedWidget = NULL;
Pile_sf2 * Page::_sf2 = NULL;
Synth * Page::_synth = NULL;

char * Page::getTextValue(char * T, quint16 champ, genAmountType genVal)
{
    switch (champ)
    {
    case champ_velRange:
        if (genVal.ranges.byLo == genVal.ranges.byHi)
            sprintf(T, "%d", genVal.ranges.byLo);
        else
            sprintf(T, "%d-%d", genVal.ranges.byLo, genVal.ranges.byHi);
        break;
    case champ_keyRange:
        if (genVal.ranges.byLo == genVal.ranges.byHi)
            sprintf(T, "%s", Config::getInstance()->getKeyName(genVal.ranges.byLo).toStdString().c_str());
        else
            sprintf(T, "%s-%s", Config::getInstance()->getKeyName(genVal.ranges.byLo).toStdString().c_str(),
                    Config::getInstance()->getKeyName(genVal.ranges.byHi).toStdString().c_str());
        break;
    case champ_initialAttenuation: case champ_pan: case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv: case champ_sustainModEnv:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        sprintf(T, "%.1f", (double)genVal.shAmount/10);
        break;

    case champ_keynum: case champ_overridingRootKey:
        sprintf(T, "%s", Config::getInstance()->getKeyName(genVal.wAmount).toStdString().c_str());
        break;

    case champ_sampleModes: case champ_exclusiveClass: case champ_velocity:
        sprintf(T,"%d", genVal.wAmount);
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
        sprintf(T,"%d", genVal.shAmount);
        break;

    case champ_initialFilterFc:
        if (_typePage == PAGE_PRST)
            sprintf(T,"%.3f", qPow(2., (double)genVal.shAmount/1200));
        else
            sprintf(T,"%.0f", qPow(2., (double)genVal.shAmount/1200)*8.176);
        break;

    case champ_freqModLFO: case champ_freqVibLFO:
        if (_typePage == PAGE_PRST)
            sprintf(T,"%.3f", qPow(2., (double)genVal.shAmount/1200));
        else
            sprintf(T,"%.3f", qPow(2., (double)genVal.shAmount/1200)*8.176);
        break;

    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        sprintf(T,"%.3f", qPow(2., (double)genVal.shAmount/1200)); break;

    case champ_sfModDestOper:
        sprintf(T,"%d", genVal.wAmount);
        break;
    case champ_sfModTransOper:
        switch (genVal.wAmount)
        {
        case 2: strcpy(T, trUtf8(", valeur absolue").toStdString().c_str()); break;
        default: T[0] = '\0';
        }
        break;
    case champ_modAmount:
        sprintf(T,"%d", genVal.shAmount);
        break;
    case champ_indexMod:
        sprintf(T,"%d", genVal.wAmount+1);
        break;
    default: break;
    }
    return T;
}

char * Page::getTextValue(char * T, quint16 champ, int iVal)
{
    switch (champ)
    {
    case champ_startAddrsOffset:
        sprintf(T,"%d", iVal); break;
    case champ_endAddrsOffset:
        sprintf(T,"%d", iVal); break;
    case champ_startloopAddrsOffset:
        sprintf(T,"%d", iVal); break;
    case champ_endloopAddrsOffset:
        sprintf(T,"%d", iVal); break;
    case champ_indexMod:
        sprintf(T,"%d", iVal+1); break;
        break;
    default:
        break;
    }
    return T;
}

char * Page::getTextValue(char * T, quint16 champ, SFModulator sfModVal)
{
    switch (champ)
    {
    case champ_sfModAmtSrcOper: case champ_sfModSrcOper:
        sprintf(T, "%d", sfModVal.Index);
        return T;
        break;
    default:
        break;
    };
    return T;
}

QString Page::getIndexName(quint16 iVal, int CC)
{
    char T[20];
    QString qStr = "";
    if (CC)
    {
        switch (iVal)
        {
        case 0: qStr = "Bank select"; break;
        case 1: qStr = "Modulation wheel"; break;
        case 2: qStr = "Breath controller"; break;
        case 4: qStr = "Foot pedal"; break;
        case 5: qStr = "Portamento time"; break;
        case 6: qStr = "Data entry"; break;
        case 7: qStr = "Main volume"; break;
        case 8: qStr = "Balance"; break;
        case 10: qStr = "Pan position"; break;
        case 11: qStr = "Expression pedal"; break;
        case 12: qStr = "Effect control 1"; break;
        case 13: qStr = "Effect control 2"; break;
        case 16: qStr = "General purpose 1"; break;
        case 17: qStr = "General purpose 2"; break;
        case 18: qStr = "General purpose 3"; break;
        case 19: qStr = "General purpose 4"; break;
        case 32: qStr = "Bank select (fine)"; break;
        case 33: qStr = "Modulation wheel (fine)"; break;
        case 34: qStr = "Breath controller (fine)"; break;
        case 36: qStr = "Foot pedal (fine)"; break;
        case 37: qStr = "Portamento time (fine)"; break;
        case 38: qStr = "Data entry (fine)"; break;
        case 39: qStr = "Main volume (fine)"; break;
        case 40: qStr = "Balance (fine)"; break;
        case 42: qStr = "Pan position (fine)"; break;
        case 43: qStr = "Expression pedal (fine)"; break;
        case 44: qStr = "Effect control 1 (fine)"; break;
        case 45: qStr = "Effect control 2 (fine)"; break;
        case 48: qStr = "General purpose 1 (fine)"; break;
        case 49: qStr = "General purpose 2 (fine)"; break;
        case 50: qStr = "General purpose 3 (fine)"; break;
        case 51: qStr = "General purpose 4 (fine)"; break;
        case 64: qStr = "Hold pedal (on/off)"; break;
        case 65: qStr = "Portamento (on/off)"; break;
        case 66: qStr = "Sostenuto (on/off)"; break;
        case 67: qStr = "Soft pedal (on/off)"; break;
        case 68: qStr = "Legato pedal (on/off)"; break;
        case 69: qStr = "Hold 2 pedal (on/off)"; break;
        case 70: qStr = "Sound controller 1 (variation)"; break;
        case 71: qStr = "Sound controller 2 (timbre)"; break;
        case 72: qStr = "Sound controller 3 (release time)"; break;
        case 73: qStr = "Sound controller 4 (attack time)"; break;
        case 74: qStr = "Sound controller 5 (brightness)"; break;
        case 75: qStr = "Sound controller 6"; break;
        case 76: qStr = "Sound controller 7"; break;
        case 77: qStr = "Sound controller 8"; break;
        case 78: qStr = "Sound controller 9"; break;
        case 79: qStr = "Sound controller 10"; break;
        case 80: qStr = "General purpose 5"; break;
        case 81: qStr = "General purpose 6"; break;
        case 82: qStr = "General purpose 7"; break;
        case 83: qStr = "General purpose 8"; break;
        case 84: qStr = "Portamento control"; break;
        case 91: qStr = "Effects 1 depth (reverb)"; break;
        case 92: qStr = "Effects 2 depth (tremolo)"; break;
        case 93: qStr = "Effects 3 depth (chorus)"; break;
        case 94: qStr = "Effects 4 depth (celeste)"; break;
        case 95: qStr = "Effects 5 depth (phaser)"; break;
        case 96: qStr = "Data increment"; break;
        case 97: qStr = "Data decrement"; break;
        case 98: qStr = "Non-registered parameter (fine)"; break;
        case 99: qStr = "Non-registered parameter"; break;
        case 100: qStr = "Registered parameter (fine)"; break;
        case 101: qStr = "Registered parameter"; break;
        case 120: qStr = "All sound off"; break;
        case 121: qStr = "All controller off"; break;
        case 122: qStr = "Local keyboard (on/off)"; break;
        case 123: qStr = "All notes off"; break;
        case 124: qStr = "Omni mode off"; break;
        case 125: qStr = "Omni mode on"; break;
        case 126: qStr = "Mono on (poly off)"; break;
        case 127: qStr = "Poly on (mono off)"; break;
        default: qStr = "Undefined"; break;
        }
        sprintf(T, "CC-%.3d ", iVal);
        qStr.prepend(T);
    }
    else
    {
        switch (iVal)
        {
        case 0: qStr = "No controller, = 1"; break;
        case 2: qStr = "Note-On velocity"; break;
        case 3: qStr = "Note-On key number"; break;
        case 10: qStr = "Poly pressure"; break;
        case 13: qStr = "Channel pressure"; break;
        case 14: qStr = "Pitch wheel"; break;
        case 16: qStr = "Pitch wheel sensitivity"; break;
        case 127: qStr = "Link"; break;
        }
    }
    return qStr;
}

QString Page::getGenName(quint16 iVal, int type)
{
    QString qStr = "";
    switch (iVal)
    {
    case 0:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset début échantillon"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: start offset"; break;
        }
        break;
    case 1:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset fin échantillon"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: end offset"; break;
        }
        break;
    case 2:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset début boucle"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: loop start offset"; break;
        }
        break;
    case 3:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset fin boucle"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: loop end offset"; break;
        }
        break;
    case 4:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset début échantillon"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: start offset (coarse)"; break;
        }
        break;
    case 5:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod LFO → ton (c)"); break;
        case 2: qStr = trUtf8("Mod LFO → ton (c)"); break;
        default: qStr = "Modulation LFO: to pitch"; break;
        }
        break;
    case 6:
        switch (type)
        {
        case 1: qStr = trUtf8("Vib LFO → ton (c)"); break;
        case 2: qStr = trUtf8("Vib LFO → ton (c)"); break;
        default: qStr = "Vibrato LFO: to pitch"; break;
        }
        break;
    case 7:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env → ton (c)"); break;
        case 2: qStr = trUtf8("Mod env → ton (c)"); break;
        default: qStr = "Modulation envelope: to pitch"; break;
        }
        break;
    case 8:
        switch (type)
        {
        case 1: qStr = trUtf8("Filtre, fréquence (Hz)"); break;
        case 2: qStr = trUtf8("Filtre, fréquence (×)"); break;
        default: qStr = "Pitch/Effects: filter cutoff"; break;
        }
        break;
    case 9:
        switch (type)
        {
        case 1: qStr = trUtf8("Filtre, résonance (dB)"); break;
        case 2: qStr = trUtf8("Filtre, résonance (dB)"); break;
        default: qStr = "Pitch/Effects: filter Q"; break;
        }
        break;
    case 10:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod LFO → filtre (c)"); break;
        case 2: qStr = trUtf8("Mod LFO → filtre (c)"); break;
        default: qStr = "Modulation LFO: to filter cutoff"; break;
        }
        break;
    case 11:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env → filtre (c)"); break;
        case 2: qStr = trUtf8("Mod env → filtre (c)"); break;
        default: qStr = "Modulation envelope: to filter cutoff"; break;
        }
        break;
    case 12:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset fin échantillon"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: end offset (coarse)"; break;
        }
        break;
    case 13:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod LFO → volume (dB)"); break;
        case 2: qStr = trUtf8("Mod LFO → volume (dB)"); break;
        default: qStr = "Modulation LFO: to volume"; break;
        }
        break;
    case 15:
        switch (type)
        {
        case 1: qStr = trUtf8("Chorus (%)"); break;
        case 2: qStr = trUtf8("Chorus (%)"); break;
        default: qStr = "Pitch/Effects: chorus"; break;
        }
        break;
    case 16:
        switch (type)
        {
        case 1: qStr = trUtf8("Réverbération (%)"); break;
        case 2: qStr = trUtf8("Réverbération (%)"); break;
        default: qStr = "Pitch/Effects: reverb"; break;
        }
        break;
    case 17:
        switch (type)
        {
        case 1: qStr = trUtf8("Balance [-50;50]"); break;
        case 2: qStr = trUtf8("Balance [-100;100]"); break;
        default: qStr = "Pitch/Effects: pan"; break;
        }
        break;
    case 21:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod LFO delay (s)"); break;
        case 2: qStr = trUtf8("Mod LFO delay (×)"); break;
        default: qStr = "Modulation LFO: delay"; break;
        }
        break;
    case 22:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod LFO freq (Hz)"); break;
        case 2: qStr = trUtf8("Mod LFO freq (×)"); break;
        default: qStr = "Modulation LFO: frequency"; break;
        }
        break;
    case 23:
        switch (type)
        {
        case 1: qStr = trUtf8("Vib LFO delay (s)"); break;
        case 2: qStr = trUtf8("Vib LFO delay (×)"); break;
        default: qStr = "Vibrato LFO: delay"; break;
        }
        break;
    case 24:
        switch (type)
        {
        case 1: qStr = trUtf8("Vib LFO freq (Hz)"); break;
        case 2: qStr = trUtf8("Vib LFO freq (×)"); break;
        default: qStr = "Vibrato LFO: frequency"; break;
        }
        break;
    case 25:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env delay (s)"); break;
        case 2: qStr = trUtf8("Mod env delay (×)"); break;
        default: qStr = "Modulation envelope: delay"; break;
        }
        break;
    case 26:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env attack (s)"); break;
        case 2: qStr = trUtf8("Mod env attack (×)"); break;
        default: qStr = "Modulation envelope: attack"; break;
        }
        break;
    case 27:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env hold (s)"); break;
        case 2: qStr = trUtf8("Mod env hold (×)"); break;
        default: qStr = "Modulation envelope: hold"; break;
        }
        break;
    case 28:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env decay (s)"); break;
        case 2: qStr = trUtf8("Mod env decay (×)"); break;
        default: qStr = "Modulation envelope: decay"; break;
        }
        break;
    case 29:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env sustain (%)"); break;
        case 2: qStr = trUtf8("Mod env sustain (%)"); break;
        default: qStr = "Modulation envelope: sustain"; break;
        }
        break;
    case 30:
        switch (type)
        {
        case 1: qStr = trUtf8("Mod env release (s)"); break;
        case 2: qStr = trUtf8("Mod env release (×)"); break;
        default: qStr = "Modulation envelope: release"; break;
        }
        break;
    case 31:
        switch (type)
        {
        case 1: qStr = trUtf8("Note → Mod env hold (c)"); break;
        case 2: qStr = trUtf8("Note → Mod env hold (c)"); break;
        default: qStr = "Modulation envelope: note to hold"; break;
        }
        break;
    case 32:
        switch (type)
        {
        case 1: qStr = trUtf8("Note → Mod env decay (c)"); break;
        case 2: qStr = trUtf8("Note → Mod env decay (c)"); break;
        default: qStr = "Modulation envelope: note to decay"; break;
        }
        break;
    case 33:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env delay (s)"); break;
        case 2: qStr = trUtf8("Vol env delay (×)"); break;
        default: qStr = "Volume envelope: delay"; break;
        }
        break;
    case 34:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env attack (s)"); break;
        case 2: qStr = trUtf8("Vol env attack (×)"); break;
        default: qStr = "Volume envelope: attack"; break;
        }
        break;
    case 35:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env hold (s)"); break;
        case 2: qStr = trUtf8("Vol env hold (×)"); break;
        default: qStr = "Volume envelope: hold"; break;
        }
        break;
    case 36:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env decay (s)"); break;
        case 2: qStr = trUtf8("Vol env decay (×)"); break;
        default: qStr = "Volume envelope: decay"; break;
        }
        break;
    case 37:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env sustain (dB)"); break;
        case 2: qStr = trUtf8("Vol env sustain (dB)"); break;
        default: qStr = "Volume envelope: sustain"; break;
        }
        break;
    case 38:
        switch (type)
        {
        case 1: qStr = trUtf8("Vol env release (s)"); break;
        case 2: qStr = trUtf8("Vol env release (×)"); break;
        default: qStr = "Volume envelope: release"; break;
        }
        break;
    case 39:
        switch (type)
        {
        case 1: qStr = trUtf8("Note → Vol env hold (c)"); break;
        case 2: qStr = trUtf8("Note → Vol env hold (c)"); break;
        default: qStr = "Volume envelope: note to hold"; break;
        }
        break;
    case 40:
        switch (type)
        {
        case 1: qStr = trUtf8("Note → Vol env decay (c)"); break;
        case 2: qStr = trUtf8("Note → Vol env decay (c)"); break;
        default: qStr = "Volume envelope: note to decay"; break;
        }
        break;
    case 43:
        switch (type)
        {
        case 1: qStr = trUtf8("Étendue note"); break;
        case 2: qStr = trUtf8("Étendue note"); break;
        default: qStr = "Key range"; break;
        }
        break;
    case 44:
        switch (type)
        {
        case 1: qStr = trUtf8("Étendue vélocité"); break;
        case 2: qStr = trUtf8("Étendue vélocité"); break;
        default: qStr = "Velocity range"; break;
        }
        break;
    case 45:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset début échantillon"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: loop start offset (coarse)"; break;
        }
        break;
    case 46:
        switch (type)
        {
        case 1: qStr = trUtf8("Note fixe"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Pitch/Effects: forced midi note"; break;
        }
        break;
    case 47:
        switch (type)
        {
        case 1: qStr = trUtf8("Vélocité fixe"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Volume envelope: forced midi velocity"; break;
        }
        break;
    case 48:
        switch (type)
        {
        case 1: qStr = trUtf8("Atténuation (dB)"); break;
        case 2: qStr = trUtf8("Atténuation (dB)"); break;
        default: qStr = "Volume envelope: attenuation"; break;
        }
        break;
    case 50:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset fin boucle"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: loop end offset (coarse)"; break;
        }
        break;
    case 51:
        switch (type)
        {
        case 1: qStr = trUtf8("Accordage (demi-tons)"); break;
        case 2: qStr = trUtf8("Accordage (demi-tons)"); break;
        default: qStr = "Pitch/Effects: tune (coarse)"; break;
        }
        break;
    case 52:
        switch (type)
        {
        case 1: qStr = trUtf8("Accordage (centièmes)"); break;
        case 2: qStr = trUtf8("Accordage (centièmes)"); break;
        default: qStr = "Pitch/Effects: tune (fine)"; break;
        }
        break;
    case 54:
        switch (type)
        {
        case 1: qStr = trUtf8("Lecture en boucle"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Volume envelope: sample mode"; break;
        }
        break;
    case 56:
        switch (type)
        {
        case 1: qStr = trUtf8("Accordage (scale)"); break;
        case 2: qStr = trUtf8("Accordage (scale)"); break;
        default: qStr = "Pitch/Effects: scale tuning"; break;
        }
        break;
    case 57:
        switch (type)
        {
        case 1: qStr = trUtf8("Classe exclusive"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Pitch/Effects: exclusive class"; break;
        }
        break;
    case 58:
        switch (type)
        {
        case 1: qStr = trUtf8("Note de base"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Pitch/Effects: root note"; break;
        }
        break;
    default: qStr = "Unknown"; break;
    }
    return qStr;
}

genAmountType Page::getValue(QString texte, quint16 champ, bool &ok)
{
    genAmountType genAmount;
    int iTmp;
    ok = true;
    switch (champ)
    {
    case champ_keyRange: case champ_velRange: {
        int posSeparator = texte.indexOf(QRegExp("[0-9]-")) + 1;
        QString txtLeft, txtRight;
        if (posSeparator == 0)
            txtLeft = txtRight = texte;
        else
        {
            txtLeft = texte.left(posSeparator);
            txtRight = texte.right(texte.size() - posSeparator - 1);
        }

        int val1, val2;

        if (champ == champ_velRange)
        {
            val1 = txtLeft.toInt(&ok);
            bool tmp = ok;
            val2 = txtRight.toInt(&ok);
            ok = ok && tmp;
        }
        else
        {
            val1 = Config::getInstance()->getKeyNum(txtLeft);
            val2 = Config::getInstance()->getKeyNum(txtRight);
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

        genAmount.ranges.byLo = limit(val1, 0, 127, 0, 127);
        genAmount.ranges.byHi = limit(val2, 0, 127, 0, 127);
    }; break;
    case champ_initialAttenuation: case champ_sustainVolEnv:
        genAmount.shAmount = (quint16)limit(10*texte.toDouble(&ok), 0, 1440, -1440, 1440);
        break;
    case champ_pan: genAmount.shAmount = (short)limit(10*texte.toDouble(&ok), -500, 500, -1000, 1000);
        break;
    case champ_sampleModes:
        iTmp = texte.toDouble(&ok);
        if (iTmp != 0 && iTmp != 1 && iTmp != 3) iTmp = 0;
        genAmount.wAmount = (quint16)iTmp;
        break;
    case champ_overridingRootKey: case champ_keynum:{
        int keyNum = Config::getInstance()->getKeyNum(texte);
        ok = (keyNum >= 0);
        genAmount.wAmount = (quint16)limit(keyNum, 0, 127);
    }break;
    case champ_coarseTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -120, 120, -240, 240);
        break;
    case champ_fineTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -99, 99, -198, 198);
        break;
    case champ_scaleTuning: genAmount.shAmount = (quint16)limit(texte.toDouble(&ok), 0, 1200, -1200, 1200);
        break;
    case champ_initialFilterFc:
        if (_typePage == PAGE_PRST)
            genAmount.shAmount = (quint16)limit(1200. * qLn(texte.toDouble(&ok)) / 0.69314718056, 0, 0, -21000, 21000);
        else
            genAmount.shAmount = (quint16)limit(1200. * qLn(texte.toDouble(&ok) / 8.176) / 0.69314718056, 1500, 13500);
        break;
    case champ_initialFilterQ: genAmount.shAmount = (quint16)limit(10*texte.toDouble(&ok), 0, 960, -960, 960);
        break;
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
        genAmount.shAmount = (short)limit(texte.toDouble(&ok), -12000, 12000, -24000, 24000);
        break;
    case champ_modLfoToVolume: genAmount.shAmount = (short)limit(10*texte.toDouble(&ok), -960, 960, -1920, 1920);
        break;
    case champ_delayModEnv: case champ_holdModEnv:
    case champ_delayVolEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        genAmount.shAmount = (short)limit(1200. * qLn(texte.toDouble(&ok)) / 0.69314718056, -12000, 5000, -21000, 21000);
        break;
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
        genAmount.shAmount = (short)limit(1200. * qLn(texte.toDouble(&ok)) / 0.69314718056, -12000, 8000, -21000, 21000);
        break;
    case champ_sustainModEnv:
        genAmount.shAmount = (quint16)limit(10. * texte.toDouble(&ok), 0, 1000, -1000, 1000);
        break;
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
        genAmount.shAmount = (short)limit(texte.toDouble(&ok), -1200, 1200, -2400, 2400);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        if (_typePage == PAGE_PRST)
            genAmount.shAmount = (short)limit(1200. * qLn(texte.toDouble(&ok)) / 0.69314718056, 0, 0, -21000, 21000);
        else
            genAmount.shAmount = (short)limit(1200. * qLn(texte.toDouble(&ok)/8.176) / 0.69314718056, -16000, 4500);
        break;
    case champ_exclusiveClass: genAmount.wAmount = (quint16)limit(texte.toDouble(&ok), 1, 127);
        break;
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        genAmount.shAmount = (quint16)limit(10*texte.toDouble(&ok), 0, 1000, -1000, 1000);
        break;
    case champ_velocity: genAmount.wAmount = (quint16)limit(texte.toDouble(&ok), 1, 127);
        break;
    case champ_startAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endAddrsOffset: case champ_endloopAddrsOffset:
        genAmount.shAmount = (short)(limit(texte.toInt(&ok)%32768, -32767, 32767));
        break;
    case champ_startAddrsCoarseOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
        genAmount.shAmount = (short)(limit(texte.toInt(&ok)/32768, -32767, 32767));
        break;
    default:
        ok = 0;
    }
    if (!ok)
    {
        genAmount.ranges.byHi = 0;
        genAmount.ranges.byLo = 0;
    }
    return genAmount;
}

int Page::limit(int iTmp, int minInst, int maxInst, int minPrst, int maxPrst)
{
    int valRet = 0;
    if (_typePage == PAGE_PRST)
    {
        if (iTmp < minPrst)
            valRet = minPrst;
        else if (iTmp > maxPrst)
            valRet = maxPrst;
        else
            valRet = iTmp;
    }
    else
    {
        if (iTmp < minInst)
            valRet = minInst;
        else if (iTmp > maxInst)
            valRet = maxInst;
        else
            valRet = iTmp;
    }
    return valRet;
}

void Page::updateMainwindow()
{
    _mainWindow->updateDo();
}

void Page::playKey(int key, int velocity)
{
    _mainWindow->triggerNote(key, velocity);
}

void Page::selectInTree(QList<EltID> ids)
{
    bool previousBlockState = this->_tree->blockSignals(true);
    this->_tree->selectNone();
    foreach (EltID id, ids)
        this->_tree->select(id);
    this->_tree->updateAtNextSelectionRequest();
    this->_tree->blockSignals(previousBlockState);
}
