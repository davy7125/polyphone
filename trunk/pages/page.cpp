/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
#include "mainwindow.h"
#include "dialog_paramglobal.h"
#include "dialog_visualizer.h"
#include "dialog_space.h"
#include "dialog_duplication.h"
#include <QScrollBar>


//////////////////////////////////////////// PAGE ////////////////////////////////////////////

Page::Page(TypePage typePage, QWidget *parent) : QWidget(parent),
    preparation(false),
    m_typePage(typePage)
{}
// Initialisation des variables statiques
MainWindow * Page::mainWindow = NULL;
Tree * Page::tree = NULL;
QStackedWidget * Page::qStackedWidget = NULL;
Pile_sf2 * Page::sf2 = NULL;
Synth * Page::synth = NULL;

// Méthodes publiques
char * Page::getTextValue(char * T, WORD champ, genAmountType genVal)
{
    switch (champ)
    {
    case champ_keyRange: case champ_velRange:
        sprintf(T,"%d-%d", genVal.ranges.byLo, genVal.ranges.byHi);
        break;

    case champ_initialAttenuation: case champ_pan: case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv: case champ_sustainModEnv:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        sprintf(T,"%.1f", (double)genVal.shAmount/10);
        break;

    case champ_sampleModes: case champ_overridingRootKey: case champ_exclusiveClass:
    case champ_keynum: case champ_velocity:
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
        if (m_typePage == PAGE_PRST)
            sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200));
        else
            sprintf(T,"%.0f", exp2((double)genVal.shAmount/1200)*8.176);
        break;

    case champ_freqModLFO: case champ_freqVibLFO:
        if (m_typePage == PAGE_PRST)
            sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200));
        else
            sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200)*8.176);
        break;

    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200)); break;

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
char * Page::getTextValue(char * T, WORD champ, int iVal)
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
char * Page::getTextValue(char * T, WORD champ, SFModulator sfModVal)
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
QString Page::getIndexName(WORD iVal, int CC)
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
QString Page::getGenName(WORD iVal, int type)
{
    QString qStr = "";
    switch (iVal)
    {
    case 0:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset début sample"); break;
        case 2: qStr = "impossible"; break;
        default: qStr = "Sample: start offset"; break;
        }
        break;
    case 1:
        switch (type)
        {
        case 1: qStr = trUtf8("Offset fin sample"); break;
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
        case 1: qStr = trUtf8("Offset début sample"); break;
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
        case 1: qStr = trUtf8("Offset fin sample"); break;
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
        case 1: qStr = trUtf8("Vol env decay (s)"); break;
        case 2: qStr = trUtf8("Vol env decay (×)"); break;
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
        case 1: qStr = trUtf8("Vol env sustain (%)"); break;
        case 2: qStr = trUtf8("Vol env sustain (%)"); break;
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
        case 1: qStr = trUtf8("Offset début sample"); break;
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
genAmountType Page::getValue(QString texte, WORD champ, bool &ok)
{
    genAmountType genAmount;
    int iTmp;
    ok = 1;
    switch (champ)
    {
    case champ_keyRange: case champ_velRange: {
        char T[20];
        strcpy(T, texte.toStdString().c_str());
        int val1, val2, val3;
        if (sscanf(T, "%d-%d", &val1, &val2) == 2)
        {
            if (val1 > val2)
            {
                val3 = val1;
                val1 = val2;
                val2 = val3;
            }
            genAmount.ranges.byLo = limit(val1, 0, 127, 0, 127);
            genAmount.ranges.byHi = limit(val2, 0, 127, 0, 127);
        }
        else if (sscanf(T, "%d-%d", &val1, &val2) == 1)
        {
            genAmount.ranges.byLo = limit(val1, 0, 127, 0, 127);
            genAmount.ranges.byHi = limit(val1, 0, 127, 0, 127);
        }
        else
            ok = 0;
        }; break;
    case champ_initialAttenuation: case champ_sustainVolEnv:
        genAmount.shAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1440, -1440, 1440);
        break;
    case champ_pan: genAmount.shAmount = (short)limit(10*texte.toDouble(&ok), -500, 500, -1000, 1000);
        break;
    case champ_sampleModes:
        iTmp = texte.toDouble(&ok);
        if (iTmp != 0 && iTmp != 1 && iTmp != 3) iTmp = 0;
        genAmount.wAmount = (WORD)iTmp;
        break;
    case champ_overridingRootKey: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 0, 127);
        break;
    case champ_coarseTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -120, 120, -240, 240);
        break;
    case champ_fineTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -99, 99, -198, 198);
        break;
    case champ_scaleTuning: genAmount.shAmount = (WORD)limit(texte.toDouble(&ok), 0, 1200, -1200, 1200);
        break;
    case champ_initialFilterFc:
        if (m_typePage == PAGE_PRST)
            genAmount.shAmount = (WORD)limit(1200*log2(texte.toDouble(&ok)), 0, 0, -12000, 12000);
        else
            genAmount.shAmount = (WORD)limit(1200*log2(texte.toDouble(&ok)/8.176), 1500, 13500);
        break;
    case champ_initialFilterQ: genAmount.shAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 960, -960, 960);
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
        genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)), -12000, 5000, -20000, 17000);
        break;
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
        genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)), -12000, 8000, -20000, 20000);
        break;
    case champ_sustainModEnv:
        genAmount.shAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1000, -1000, 1000);
        break;
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
        genAmount.shAmount = (short)limit(texte.toDouble(&ok), -1200, 1200, -2400, 2400);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        if (m_typePage == PAGE_PRST)
            genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)), 0, 0, -21000, 20500);
        else
            genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)/8.176), -16000, 4500);
        break;
    case champ_exclusiveClass: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 1, 127);
        break;
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        genAmount.shAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1000, -1000, 1000);
        break;
    case champ_keynum: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 0, 127);
        break;
    case champ_velocity: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 1, 127);
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

// Méthodes privées
int Page::limit(int iTmp, int minInst, int maxInst, int minPrst, int maxPrst)
{
    int valRet = 0;
    if (m_typePage == PAGE_PRST)
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


//////////////////////////////////////////// PAGE TABLE ////////////////////////////////////////////

QList<PageTable::Modulator> PageTable::_modulatorCopy;

PageTable::PageTable(TypePage typePage, QWidget *parent) : Page(typePage, parent)
{
    this->table = NULL;
    this->tableMod = NULL;
}

void PageTable::afficher()
{
    int posV = this->table->verticalScrollBar()->value();
    EltID id = this->tree->getID(0);
    EltID ori = id;
    EltID id2 = id;
    EltID id3 = id;
    WORD champTmp;
    genAmountType genValTmp;
    QString qStr;
    int offsetStart = 0;
    int offsetEnd = 0;
    int offsetStartLoop = 0;
    int offsetEndLoop = 0;
    int row, nbSmplInst, numCol;
    char T[20];char str[40];char str2[40];
    this->preparation = 1;
    // Destruction des cellules précédentes
    this->table->clear();

    ////// AFFICHAGE DES PARAMETRES GLOBAUX //////
    this->table->addColumn(0, trUtf8("Global"));
    id.typeElement = this->contenantGen;
    id.indexElt2 = 0;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexMod = i;
        champTmp = this->sf2->get(id, champ_sfGenOper).wValue;
        genValTmp = this->sf2->get(id, champ_sfGenAmount).genValue;
        switch (champTmp)
        {
        case champ_startAddrsOffset:
            offsetStart += genValTmp.shAmount;
            break;
        case champ_startAddrsCoarseOffset:
            offsetStart += 32768 * genValTmp.shAmount;
            break;
        case champ_endAddrsOffset:
            offsetEnd += genValTmp.shAmount;
            break;
        case champ_endAddrsCoarseOffset:
            offsetEnd += 32768 * genValTmp.shAmount;
            break;
        case champ_startloopAddrsOffset:
            offsetStartLoop += genValTmp.shAmount;
            break;
        case champ_startloopAddrsCoarseOffset:
            offsetStartLoop += 32768 * genValTmp.shAmount;
            break;
        case champ_endloopAddrsOffset:
            offsetEndLoop += genValTmp.shAmount;
            break;
        case champ_endloopAddrsCoarseOffset:
            offsetEndLoop += 32768 * genValTmp.shAmount;
            break;
        default:
            row = this->table->getRow(champTmp);
            if (row > -1)
                this->table->item(row, 0)->setText(getTextValue(T, champTmp, genValTmp));
        }
    }
    if (offsetStart && this->table->getRow(champ_startAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_startAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_startAddrsOffset, offsetStart));
    }
    if (offsetEnd && this->table->getRow(champ_endAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_endAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_endAddrsOffset, offsetEnd));
    }
    if (offsetStartLoop && this->table->getRow(champ_startloopAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_startloopAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_startloopAddrsOffset, offsetStartLoop));
    }
    if (offsetEndLoop && this->table->getRow(champ_endloopAddrsOffset) > -1)
    {
        row = this->table->getRow(champ_endloopAddrsOffset);
        this->table->item(row, 0)->setText(getTextValue(T, champ_endloopAddrsOffset, offsetEndLoop));
    }
    this->table->setID(id, 0);
    ////// AFFICHAGE DES PARAMETRES PAR ELEMENT LIÉ //////
    nbSmplInst = 0;
    id.typeElement = this->lien;
    id2.typeElement = this->lienGen;
    id3.typeElement = this->contenu;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            id2.indexElt2 = i;
            // Ajout d'un élément lié
            numCol = 1;
            // Détermination de la colonne
            Champ cElementLie;
            if (this->contenant == elementInst)
                cElementLie = champ_sampleID;
            else
                cElementLie = champ_instrument;

            id3.indexElt = this->sf2->get(id, cElementLie).wValue;
            sprintf(str, "%.03d%.03d%s", this->sf2->get(id, champ_keyRange).rValue.byLo,
                    this->sf2->get(id, champ_velRange).rValue.byLo,
                    this->sf2->getQstr(id3, champ_name).toStdString().c_str());
            qStr = this->sf2->getQstr(id3, champ_name).left(10);
            qStr.append("\n");
            qStr.append(this->sf2->getQstr(id3, champ_name).mid(10).left(10));
            for (int j = 1; j < nbSmplInst+1; j++)
            {
                // note et vélocité basses de la colonne et prise en compte du nom de l'élément lié
                id3.indexElt = this->sf2->get(this->table->getID(j), cElementLie).wValue;
                sprintf(str2, "%.03d%.03d%s", this->sf2->get(this->table->getID(j), champ_keyRange).rValue.byLo,
                        this->sf2->get(this->table->getID(j), champ_velRange).rValue.byLo,
                        this->sf2->getQstr(id3, champ_name).toStdString().c_str());
                if (strcmp(str, str2) > 0)
                    numCol++;
            }

            nbSmplInst++;
            offsetStart = 0;
            offsetEnd = 0;
            offsetStartLoop = 0;
            offsetEndLoop = 0;
            this->table->addColumn(numCol, qStr);
            for (int j = 0; j < this->sf2->count(id2); j++)
            {
                id2.indexMod = j;
                champTmp = this->sf2->get(id2, champ_sfGenOper).wValue;
                genValTmp = this->sf2->get(id2, champ_sfGenAmount).genValue;
                switch (champTmp)
                {
                case champ_startAddrsOffset:
                    offsetStart += genValTmp.shAmount;
                    break;
                case champ_startAddrsCoarseOffset:
                    offsetStart += 32768 * genValTmp.shAmount;
                    break;
                case champ_endAddrsOffset:
                    offsetEnd += genValTmp.shAmount;
                    break;
                case champ_endAddrsCoarseOffset:
                    offsetEnd += 32768 * genValTmp.shAmount;
                    break;
                case champ_startloopAddrsOffset:
                    offsetStartLoop += genValTmp.shAmount;
                    break;
                case champ_startloopAddrsCoarseOffset:
                    offsetStartLoop += 32768 * genValTmp.shAmount;
                    break;
                case champ_endloopAddrsOffset:
                    offsetEndLoop += genValTmp.shAmount;
                    break;
                case champ_endloopAddrsCoarseOffset:
                    offsetEndLoop += 32768 * genValTmp.shAmount;
                    break;
                default:
                    row = this->table->getRow(champTmp);
                    if (row > -1)
                        this->table->item(row, numCol)->setText(getTextValue(T, champTmp, genValTmp));
                }
            }
            if (offsetStart && this->table->getRow(champ_startAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_startAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_startAddrsOffset, offsetStart));
            }
            if (offsetEnd && this->table->getRow(champ_endAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_endAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_endAddrsOffset, offsetEnd));
            }
            if (offsetStartLoop && this->table->getRow(champ_startloopAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_startloopAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_startloopAddrsOffset, offsetStartLoop));
            }
            if (offsetEndLoop && this->table->getRow(champ_endloopAddrsOffset) > -1)
            {
                row = this->table->getRow(champ_endloopAddrsOffset);
                this->table->item(row, numCol)->setText(getTextValue(T, champ_endloopAddrsOffset, offsetEndLoop));
            }
            this->table->setID(id2, numCol);
        }
    }

    ////// MISE EN FORME DE LA TABLE (couleurs, alignement) //////
    QBrush brush1(QPixmap(":/style/fond.png"));
    QBrush brush2(QPixmap(":/style/fondJaune.png"));
    if (this->contenant == elementInst)
    {
        for (int j = 0; j < this->table->rowCount(); j++)
        {
            if (j < 9)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 13)
                this->table->item(j, 0)->setBackground(brush2);
            else if (j < 15)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 23)
                this->table->item(j, 0)->setBackground(brush2);
            else if (j < 33)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 41)
                this->table->item(j, 0)->setBackground(brush2);
            else if (j < 46)
                this->table->item(j, 0)->setBackground(brush1);
            else
                this->table->item(j, 0)->setBackground(brush2);
            this->table->item(j, 0)->setTextAlignment(0x0082);
        }
        for (int i = 1; i < this->table->columnCount(); i++)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 9) {}
                else if (j < 13)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 15) {}
                else if (j < 23)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 33) {}
                else if (j < 41)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 46) {}
                else
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                this->table->item(j, i)->setTextAlignment(0x0082);
            }
        }
    }
    else
    {
        for (int j = 0; j < this->table->rowCount(); j++)
        {
            if (j < 8)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 11)
                this->table->item(j, 0)->setBackground(brush2);
            else if (j < 13)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 21)
                this->table->item(j, 0)->setBackground(brush2);
            else if (j < 31)
                this->table->item(j, 0)->setBackground(brush1);
            else if (j < 39)
                this->table->item(j, 0)->setBackground(brush2);
            else
                this->table->item(j, 0)->setBackground(brush1);
            this->table->item(j, 0)->setTextAlignment(0x0082);
        }
        for (int i = 1; i < this->table->columnCount(); i++)
        {
            for (int j = 0; j < this->table->rowCount(); j++)
            {
                if (j < 8) {}
                else if (j < 11)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 13) {}
                else if (j < 21)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                else if (j < 31) {}
                else if (j < 39)
                    this->table->item(j, i)->setBackgroundColor(QColor(255, 255, 200));
                this->table->item(j, i)->setTextAlignment(0x0082);
            }
        }
    }
    for (int i = 0; i < 4; i++)
        this->table->hideRow(i);
    this->table->resizeColumnsToContents();

    ///////////////////// REMPLISSAGE DES MODS //////////////////////////
    this->afficheMod(ori);
    // Fin de la préparation et basculement de l'affichage
    this->preparation = 0;
    this->reselect();
    this->qStackedWidget->setCurrentWidget(this);
    this->table->verticalScrollBar()->setValue(posV);
}
void PageTable::clearTable()
{
    this->table->setColumnCount(0);
}
void PageTable::afficheMod(EltID id, int selectedRow)
{
    QString qStr;
    genAmountType genValTmp;
    SFModulator sfModTmp;
    char T[60];
    if (id.typeElement == this->contenant)
        id.typeElement = this->contenantMod;
    else
        id.typeElement = this->lienMod;
    this->tableMod->clear();
    int iVal;
    QIcon icon;
    int numLigne = 0;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexMod = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            // Ajout d'un modulateur
            this->tableMod->addRow(numLigne);
            this->tableMod->setID(id, numLigne);
            this->tableMod->item(numLigne, 5)->setText(getTextValue(T, champ_indexMod, id.indexMod));

            sfModTmp = this->sf2->get(id, champ_sfModSrcOper).sfModValue;
            iVal = 4*sfModTmp.D + 8*sfModTmp.P + sfModTmp.Type + 1;
            sprintf(T, ":/icones/courbe%.2d", iVal);
            icon = QIcon(T);
            this->tableMod->item(numLigne, 6)->setIcon(icon);
            if (sfModTmp.Index == 127 && sfModTmp.CC == 0)
            {
                // On cherche le lien
                iVal = getAssociatedMod(id);
                if (iVal > -1)
                {
                    sprintf(T, "%s: #%d", trUtf8("Modulateur").toStdString().c_str(), iVal+1);
                    qStr = T;
                }
                else
                    qStr = trUtf8("Lien (invalide)");
            }
            else qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
            this->tableMod->item(numLigne, 6)->setText(qStr);
            sfModTmp = this->sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
            iVal = 4*sfModTmp.D + 8*sfModTmp.P + sfModTmp.Type + 1;
            sprintf(T, ":/icones/courbe%.2d", iVal);
            icon = QIcon(T);
            this->tableMod->item(numLigne, 7)->setIcon(icon);
            qStr = getIndexName(sfModTmp.Index, sfModTmp.CC);
            genValTmp = this->sf2->get(id, champ_modAmount).genValue;
            qStr.append(QString::fromUtf8(" × ")).append(getTextValue(T, champ_modAmount, genValTmp));
            this->tableMod->item(numLigne, 7)->setText(qStr);
            this->spinAmount->setValue(genValTmp.shAmount);
            genValTmp = this->sf2->get(id, champ_sfModDestOper).genValue;
            if (genValTmp.wAmount > 99)
            {
                // lien vers modulateur
                sprintf(T, "%s: #%d", trUtf8("Modulateur").toStdString().c_str(), genValTmp.wAmount - 32767);
                qStr = T;
            }
            else qStr = getGenName(genValTmp.wAmount);
            genValTmp = this->sf2->get(id, champ_sfModTransOper).genValue;
            qStr.append(getTextValue(T, champ_sfModTransOper, genValTmp));
            this->tableMod->item(numLigne, 8)->setText(qStr);
            numLigne++;
        }
    }
    for (int i = 0; i < 5; i++)
        this->tableMod->hideColumn(i);
    if (selectedRow > -1)
    {
        for (int i = 0; i < this->tableMod->rowCount(); i++)
        {
            if (this->tableMod->getID(i).indexMod == selectedRow)
                this->tableMod->selectRow(i);
        }
    }
    this->tableMod->resizeColumnToContents(5);
    this->tableMod->resizeColumnToContents(6);
    this->tableMod->resizeColumnToContents(7);

    this->afficheEditMod();
}
void PageTable::afficheEditMod()
{
    // Bouton nouveau
    if (this->tree->getSelectedItemsNumber() == 1)
        this->pushNouveauMod->setEnabled(true);
    else
        this->pushNouveauMod->setEnabled(false);
    EltID id = this->tableMod->getID();
    int iTmp;
    WORD wTmp;
    bool bTmp;
    SFModulator sfMod;
    if (id.typeElement != elementUnknown)
    {
        this->spinAmount->setValue(this->sf2->get(id, champ_modAmount).shValue);
        this->spinAmount->setEnabled(true);
        this->checkAbs->setChecked(this->sf2->get(id, champ_sfModTransOper).wValue == 2);
        this->checkAbs->setEnabled(true);
        this->pushSupprimerMod->setEnabled(true);
        sfMod = this->sf2->get(id, champ_sfModSrcOper).sfModValue;
        iTmp = sfMod.D + 2*sfMod.P + 4*sfMod.Type;
        this->comboSource1Courbe->setEnabled(true);
        this->comboSource1Courbe->setCurrentIndex(iTmp/4);
        this->comboSource1Courbe->setModelColumn(iTmp%4);
        this->comboSource1Courbe->setCurrentIndex(iTmp/4); // pour forcer l'initialisation
        this->comboSource1Courbe->setModelColumn(iTmp%4);
        // COMBOBOX SOURCE 1
        this->comboSource1->removeItemsAfterLim();
        addAvailableSenderMod(this->comboSource1, id);
        // Sélection et activation
        wTmp = sfMod.Index;
        bTmp = sfMod.CC;
        if (wTmp == 127 && bTmp == 0)
        {
            // On cherche le lien
            int iVal = getAssociatedMod(id);
            if (iVal > -1) this->comboSource1->selectIndex(100, 32768 + iVal);
            else this->comboSource1->selectIndex(this->getSrcIndex(0, 0));
        }
        else this->comboSource1->selectIndex(this->getSrcIndex(wTmp, bTmp));
        this->comboSource1->setEnabled(true);
        sfMod = this->sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
        iTmp = sfMod.D + 2*sfMod.P + 4*sfMod.Type;
        this->comboSource2Courbe->setEnabled(true);
        this->comboSource2Courbe->setCurrentIndex(iTmp/4);
        this->comboSource2Courbe->setModelColumn(iTmp%4);
        this->comboSource2Courbe->setCurrentIndex(iTmp/4); // pour forcer l'initialisation
        this->comboSource2Courbe->setModelColumn(iTmp%4);
        // COMBOBOX SOURCE 2
        // Sélection et activation
        wTmp = sfMod.Index;
        bTmp = sfMod.CC;
        this->comboSource2->selectIndex(this->getSrcIndex(wTmp, bTmp));
        this->comboSource2->setEnabled(true);
        // COMBOBOX DESTINATION
        this->comboDestination->removeItemsAfterLim();
        addAvailableReceiverMod(this->comboDestination, id);
        // Sélection et activation
        wTmp = this->sf2->get(id, champ_sfModDestOper).wValue;
        this->comboDestination->selectIndex(this->getDestIndex(wTmp), wTmp);
        this->comboDestination->setEnabled(true);
    }
    else
    {
        this->spinAmount->setValue(0);
        this->spinAmount->setEnabled(false);
        this->checkAbs->setChecked(false);
        this->checkAbs->setEnabled(false);
        this->pushSupprimerMod->setEnabled(false);
        this->comboSource1Courbe->setCurrentIndex(0);
        this->comboSource1Courbe->setModelColumn(0);
        this->comboSource1Courbe->setEnabled(false);
        this->comboSource2Courbe->setCurrentIndex(0);
        this->comboSource2Courbe->setModelColumn(0);
        this->comboSource2Courbe->setEnabled(false);
        this->comboDestination->setCurrentIndex(0);
        this->comboDestination->setEnabled(false);
        this->comboSource1->setCurrentIndex(0);
        this->comboSource1->setEnabled(false);
        this->comboSource2->setCurrentIndex(0);
        this->comboSource2->setEnabled(false);
    }
}
void PageTable::updateId(EltID id)
{
    // Mise à jour tableau, id a été supprimé
    EltID id2;
    if (id.typeElement == elementSf2)
    {
        // décrémentation sf2 ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexSf2 > id.indexSf2)
            {
                id2.indexSf2--;
                this->table->setID(id2, i);
            }
        }
    }
    else if (id.typeElement == elementInst || id.typeElement == elementPrst)
    {
        // décrémentation elt ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexElt > id.indexElt && id2.indexSf2 == id.indexSf2)
            {
                id2.indexElt--;
                this->table->setID(id2, i);
            }
        }
    }
    else if (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst)
    {
        // décrémentation elt2 ?
        for (int i = 0; i < this->table->columnCount(); i++)
        {
            id2 = this->table->getID(i);
            if (id2.indexElt2 > id.indexElt2  && id2.indexSf2 == id.indexSf2
                     && id2.indexElt == id.indexElt)
            {
                id2.indexElt2--;
                this->table->setID(id2, i);
            }
        }
    }
    // Affichage mod
    this->afficheMod(this->table->getID(0), this->tableMod->currentRow());
}


void PageTable::resetChamp(bool &newAction, int colonne, Champ champ1, Champ champ2)
{
    EltID id = table->getID(colonne);
    bool ok = sf2->isSet(id, champ1);
    if (champ2 != champ_unknown)
        ok = ok || sf2->isSet(id, champ2);

    if (ok)
    {
        // On efface la donnée
        if (newAction)
            sf2->prepareNewActions();
        id = table->getID(colonne);
        sf2->reset(id, champ1);
        if (champ2 != champ_unknown)
            sf2->reset(id, champ2);
        newAction = false;

        // Mise à jour fenêtre
        mainWindow->updateDo();
    }
}
void PageTable::setOffset(bool &newAction, int ligne, int colonne, Champ champ1, Champ champ2)
{
    EltID id = this->table->getID(colonne);
    bool ok;
    char T[20];
    QString texte = this->table->item(ligne, colonne)->text().left(9);
    genAmountType genAmount = getValue(texte, champ1, ok);
    if (ok)
    {
        // Enregistrement de la nouvelle valeur
        genAmountType genAmount2 = getValue(texte, champ2, ok);
        int iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ1, id);
        genAmount2.shAmount = iVal / 32768;
        genAmount.shAmount = iVal % 32768;
        if (genAmount.shAmount != this->sf2->get(id, champ1).shValue ||
            genAmount2.shAmount != this->sf2->get(id, champ2).shValue)
        {
            // Modification du sf2
            if (newAction)
                this->sf2->prepareNewActions();
            id = this->table->getID(colonne);
            Valeur value;
            value.genValue = genAmount;
            this->sf2->set(id, champ1, value);
            value.genValue = genAmount2;
            this->sf2->set(id, champ2, value);
            newAction = false;

            // Mise à jour fenêtre
            this->mainWindow->updateDo();
        }
        // Mise à jour de la valeur dans la cellule
        int offset = this->sf2->get(id, champ1).genValue.shAmount +
                32768 * this->sf2->get(id, champ2).genValue.shAmount;
        this->table->item(ligne, colonne)->setText(getTextValue(T, champ1, offset));
    }
    else
    {
        // Restauration valeur précédente
        if (this->sf2->isSet(id, champ1) || this->sf2->isSet(id, champ2))
        {
            int offset = this->sf2->get(id, champ1).genValue.shAmount +
                    32768 * this->sf2->get(id, champ2).genValue.shAmount;
            this->table->item(ligne, colonne)->setText(getTextValue(T, champ1, offset));
        }
        else
            this->table->item(ligne, colonne)->setText("");
    }
}
void PageTable::set(int ligne, int colonne, bool newAction)
{
    if (this->preparation)
        return;

    // Modification de toutes les cellules sélectionnées
    QString text = table->item(ligne, colonne)->text();
    for (int i = 0; i < table->selectedItems().size(); i++)
    {
        table->blockSignals(true);
        table->selectedItems()[i]->setText(text);
        table->blockSignals(false);
        set(table->selectedItems().at(i)->row(), table->selectedItems().at(i)->column(), newAction, true);
    }
}
void PageTable::set(int ligne, int colonne, bool &newAction, bool allowPropagation)
{
    // modification d'un élément du tableau
    Champ champ = this->table->getChamp(ligne);
    if (champ == champ_unknown)
        return;

    EltID id = this->table->getID(colonne);
    if (allowPropagation && Config::getInstance()->getRepercussionStereo() && id.typeElement == elementInstSmpl &&
            champ != champ_pan)
    {
        // Répercussion des modifications sur le sample stéréo s'il est présent
        EltID idSmpl = id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = sf2->get(id, champ_sampleID).wValue;
        SFSampleLink typeLink = sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLink == rightSample || typeLink == leftSample || typeLink == linkedSample ||
                typeLink == RomRightSample || typeLink == RomLeftSample || typeLink == RomLinkedSample)
        {
            int numSmpl2 = sf2->get(idSmpl, champ_wSampleLink).wValue;
            rangesType keyRange = sf2->get(id, champ_keyRange).rValue;
            rangesType velRange = sf2->get(id, champ_velRange).rValue;

            // Recherche d'une correspondance dans les samples liés
            bool ok = true;
            int numCol2 = -1;
            EltID idTmp = id;
            for (int i = 1; i < table->columnCount(); i++)
            {
                idTmp = this->table->getID(i);
                if (!sf2->get(idTmp, champ_hidden).bValue && i != colonne)
                {
                    rangesType keyRange2 = sf2->get(idTmp, champ_keyRange).rValue;
                    rangesType velRange2 = sf2->get(idTmp, champ_velRange).rValue;
                    if (keyRange2.byLo == keyRange.byLo && keyRange2.byHi == keyRange.byHi &&
                            velRange2.byLo == velRange.byLo && velRange2.byHi == velRange.byHi)
                    {
                        int iTmp = sf2->get(idTmp, champ_sampleID).wValue;
                        if (iTmp == idSmpl.indexElt)
                            ok = false; // ambiguité
                        else if (iTmp == numSmpl2)
                        {
                            if (numCol2 == -1)
                                numCol2 = i;
                            else
                                ok = false; // ambiguité
                        }
                    }
                }
            }

            // Application de la modification
            if (numCol2 != -1 && ok)
            {
                table->blockSignals(true);
                table->item(ligne, numCol2)->setText(table->item(ligne, colonne)->text());
                table->blockSignals(false);
                set(ligne, numCol2, newAction, false);
            }
        }
    }

    if (table->item(ligne, colonne)->text().isEmpty())
    {
        // Effacement d'un paramètre ?
        switch ((int)champ)
        {
        case champ_startAddrsOffset:
            resetChamp(newAction, colonne, champ_startAddrsOffset, champ_startAddrsCoarseOffset);
            break;
        case champ_endAddrsOffset:
            resetChamp(newAction, colonne, champ_endAddrsOffset, champ_endAddrsCoarseOffset);
            break;
        case champ_startloopAddrsOffset:
            resetChamp(newAction, colonne, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset);
            break;
        case champ_endloopAddrsOffset:
            resetChamp(newAction, colonne, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset);
            break;
        default:
            resetChamp(newAction, colonne, champ, champ_unknown);
        }
    }
    else
    {
        preparation = 1;
        switch ((int)champ)
        {
        case champ_startAddrsOffset:
            setOffset(newAction, ligne, colonne, champ_startAddrsOffset, champ_startAddrsCoarseOffset);
            break;
        case champ_endAddrsOffset:
            setOffset(newAction, ligne, colonne, champ_endAddrsOffset, champ_endAddrsCoarseOffset);
            break;
        case champ_startloopAddrsOffset:
            setOffset(newAction, ligne, colonne, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset);
            break;
        case champ_endloopAddrsOffset:
            setOffset(newAction, ligne, colonne, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset);
            break;
        default:{
            QString texte = this->table->item(ligne, colonne)->text().left(9);
            bool ok;
            char T[20];
            EltID id = this->table->getID(colonne);
            genAmountType genAmount = getValue(texte, champ, ok);
            if (ok)
            {
                // Modification champ
                if (genAmount.wAmount != this->sf2->get(id, champ).wValue || !this->sf2->isSet(id, champ))
                {
                    // Modification du sf2
                    if (newAction)
                        this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ, value);
                    newAction = false;

                    // Mise à jour fenêtre
                    mainWindow->updateDo();
                }
                // Mise à jour de la valeur dans la cellule
                table->item(ligne, colonne)->setText(getTextValue(T, champ, genAmount));
            }
            else
            {
                // Restauration valeur précédente
                if (this->sf2->isSet(id, champ))
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ, this->sf2->get(id, champ).genValue));
                else this->table->item(ligne, colonne)->setText("");
            }
            }
        }
        preparation = 0;
    }
    this->table->resizeColumnsToContents();

    // Mise à jour partie mod (car entre 2 des mods peuvent être définitivement détruits, et les index peuvent être mis à jour)
    id = this->table->getID(colonne);
    this->afficheMod(id);
}
void PageTable::setAmount()
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    val.shValue = this->spinAmount->value();
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (this->sf2->get(id2, champ_modAmount).shValue != val.shValue)
        {
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            this->sf2->set(id2, champ_modAmount, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}
void PageTable::setAbs()
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    if (this->checkAbs->isChecked())
        val.wValue = 2;
    else
        val.wValue = 0;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        if (this->sf2->get(id2, champ_sfModTransOper).wValue != val.wValue)
        {
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            this->sf2->set(id2, champ_sfModTransOper, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}
void PageTable::setSourceType(int row, int column)
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    int D = column % 2;
    int P = column / 2;
    int type = row;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = this->sf2->get(id2, champ_sfModSrcOper).sfModValue;
        if (val.sfModValue.D != D || val.sfModValue.P != P || val.sfModValue.Type != type)
        {
            val.sfModValue.D = D;
            val.sfModValue.P = P;
            val.sfModValue.Type = type;
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            this->sf2->set(id2, champ_sfModSrcOper, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}
void PageTable::setSourceAmountType(int row, int column)
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    Valeur val;
    int D = column % 2;
    int P = column / 2;
    int type = row;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = this->sf2->get(id2, champ_sfModAmtSrcOper).sfModValue;
        if (val.sfModValue.D != D || val.sfModValue.P != P || val.sfModValue.Type != type)
        {
            val.sfModValue.D = D;
            val.sfModValue.P = P;
            val.sfModValue.Type = type;
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            this->sf2->set(id2, champ_sfModAmtSrcOper, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}
void PageTable::setDest(int index)
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    if (index < 32768) index = getDestNumber(index);
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfGenValue = this->sf2->get(id2, champ_sfModDestOper).sfGenValue;
        if (val.sfGenValue != (Champ)index)
        {
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            // La valeur précédente était-elle un lien ?
            if (val.wValue >= 32768)
            {
                EltID id3 = id2;
                id3.indexMod = val.wValue - 32768;
                if (id3.indexMod < this->sf2->count(id2))
                {
                    Valeur val2;
                    val2.sfModValue = this->sf2->get(id3, champ_sfModSrcOper).sfModValue;
                    val2.sfModValue.CC = 0;
                    val2.sfModValue.Index = 0;
                    this->sf2->set(id3, champ_sfModSrcOper, val2);
                }
            }
            // Lien ?
            if (index >= 32768 && this->sf2->count(id2) > index - 32768)
            {
                EltID id3 = id2;
                id3.indexMod = index - 32768;
                // Destruction des précédents liens vers le nouvel élément
                Valeur val2;
                EltID id4 = id2;
                do
                {
                    id4.indexMod = this->getAssociatedMod(id3);
                    if (id4.indexMod != -1)
                    {
                        val2.sfGenValue = (Champ)0;
                        this->sf2->set(id4, champ_sfModDestOper, val2);
                    }
                } while (id4.indexMod != -1);
                val2.sfModValue = this->sf2->get(id3, champ_sfModSrcOper).sfModValue;
                val2.sfModValue.Index = 127;
                val2.sfModValue.CC = 0;
                this->sf2->set(id3, champ_sfModSrcOper, val2);
            }
            val.sfGenValue = (Champ)index;
            this->sf2->set(id2, champ_sfModDestOper, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}
void PageTable::setSource(int index)
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    if (index < 32768) index = getSrcNumber(index, CC);
    else CC = 0;
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = this->sf2->get(id2, champ_sfModSrcOper).sfModValue;
        if (index < 32768)
        {
            if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
            {
                // modification, pas de lien
                this->sf2->prepareNewActions();
                // Reprise des identificateurs si modification
                id = this->tableMod->getID();
                id2 = id;
                if (id2.typeElement == this->contenantMod)
                    id.typeElement = this->contenant;
                else
                    id.typeElement = this->lien;
                // Valeur précédente était un lien ?
                if (val.sfModValue.Index == 127 && val.sfModValue.CC == 0)
                {
                    // On casse le lien
                    EltID id3 = id2;
                    Valeur val2;
                    do
                    {
                        id3.indexMod = this->getAssociatedMod(id2);
                        if (id3.indexMod != -1)
                        {
                            val2.wValue = 0;
                            this->sf2->set(id3, champ_sfModDestOper, val2);
                        }
                    } while (id3.indexMod != -1);
                }
                val.sfModValue.Index = index;
                val.sfModValue.CC = CC;
                this->sf2->set(id2, champ_sfModSrcOper, val);
                if (this->tableMod->selectedItems().count())
                    this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
                else
                    this->afficheMod(id);
                this->mainWindow->updateDo();
            }
        }
        else
        {
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            if (val.sfModValue.Index != 127 || val.sfModValue.CC != CC)
            {
                // La valeur précédente n'est pas un lien
                // Modification sender
                EltID id3 = id2;
                id3.indexMod = index - 32768;
                // sender était-il lié à un autre mod ?
                Valeur val2;
                val2.wValue = this->sf2->get(id3, champ_sfModDestOper).wValue;
                if (val2.wValue >= 32768)
                {
                    // On enlève le lien précédent
                    EltID id4 = id2;
                    id4.indexMod = val2.wValue-32768;
                    if (this->sf2->count(id) > id4.indexMod)
                    {
                        val2.sfModValue = this->sf2->get(id4, champ_sfModSrcOper).sfModValue;
                        val2.sfModValue.CC = 0;
                        val2.sfModValue.Index = 0;
                        this->sf2->set(id4, champ_sfModSrcOper, val2);
                    }
                }
                val2.wValue = 32768 + id2.indexMod;
                this->sf2->set(id3, champ_sfModDestOper, val2);
                val.sfModValue.Index = 127;
                val.sfModValue.CC = CC;
                this->sf2->set(id2, champ_sfModSrcOper, val);
                if (this->tableMod->selectedItems().count())
                    this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
                else
                    this->afficheMod(id);
                this->mainWindow->updateDo();
            }
            else
            {
                // La valeur précédente était un lien
                EltID id3 = id2;
                id3.indexMod = this->getAssociatedMod(id2);
                if (id3.indexMod != index - 32768)
                {
                    // On enlève les liens précédents
                    Valeur val2;
                    do
                    {
                        id3.indexMod = this->getAssociatedMod(id2);
                        if (id3.indexMod != -1)
                        {
                            val2.wValue = 0;
                            this->sf2->set(id3, champ_sfModDestOper, val2);
                        }
                    } while (id3.indexMod != -1);
                    // Modification sender
                    id3 = id2;
                    id3.indexMod = index - 32768;
                    // sender était-il lié à un autre mod ?
                    val2.wValue = this->sf2->get(id3, champ_sfModDestOper).wValue;
                    if (val2.wValue >= 32768)
                    {
                        // On enlève le lien précédent
                        EltID id4 = id2;
                        id4.indexMod = val2.wValue-32768;
                        if (this->sf2->count(id2) > id4.indexMod)
                        {
                            val2.sfModValue = this->sf2->get(id4, champ_sfModSrcOper).sfModValue;
                            val2.sfModValue.CC = 0;
                            val2.sfModValue.Index = 0;
                            this->sf2->set(id4, champ_sfModSrcOper, val2);
                        }
                    }
                    val2.wValue = 32768 + id2.indexMod;
                    this->sf2->set(id3, champ_sfModDestOper, val2);
                    val.sfModValue.Index = 127;
                    val.sfModValue.CC = CC;
                    this->sf2->set(id2, champ_sfModSrcOper, val);
                    if (this->tableMod->selectedItems().count())
                        this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
                    else
                        this->afficheMod(id);
                    this->mainWindow->updateDo();
                }
            }
        }
    }
    this->preparation = 0;
}
void PageTable::setSource2(int index)
{
    if (this->preparation) return;
    this->preparation = 1;
    EltID id = this->tableMod->getID();
    EltID id2 = id;
    if (id2.typeElement == this->contenantMod)
        id.typeElement = this->contenant;
    else
        id.typeElement = this->lien;
    bool CC;
    index = getSrcNumber(index, CC);
    Valeur val;
    if (id2.typeElement != elementUnknown)
    {
        // Comparaison avec valeur précédente
        val.sfModValue = this->sf2->get(id2, champ_sfModAmtSrcOper).sfModValue;
        if (val.sfModValue.Index != index || val.sfModValue.CC != CC)
        {
            // modification
            this->sf2->prepareNewActions();
            // Reprise des identificateurs si modification
            id = this->tableMod->getID();
            id2 = id;
            if (id2.typeElement == this->contenantMod)
                id.typeElement = this->contenant;
            else
                id.typeElement = this->lien;
            val.sfModValue.Index = index;
            val.sfModValue.CC = CC;
            this->sf2->set(id2, champ_sfModAmtSrcOper, val);
            if (this->tableMod->selectedItems().count())
                this->afficheMod(id, this->tableMod->selectedItems().takeFirst()->row());
            else
                this->afficheMod(id);
            this->mainWindow->updateDo();
        }
    }
    this->preparation = 0;
}

void PageTable::reselect()
{
    this->selectNone();
    this->table->setSelectionMode(QAbstractItemView::MultiSelection);
    for (unsigned int i = 0; i < this->tree->getSelectedItemsNumber(); i++)
        this->select(this->tree->getID(i));
    this->table->setSelectionMode(QAbstractItemView::ExtendedSelection);
}
void PageTable::selectNone(bool refresh)
{
    this->preparation = !refresh;
    this->table->clearSelection();
    this->preparation = false;
}
void PageTable::select(EltID id, bool refresh)
{
    this->preparation = !refresh;
    EltID id2;
    int max;
    for (int i = 0; i < this->table->columnCount(); i++)
    {
        id2 = this->table->getID(i);
        if (id.typeElement == id2.typeElement && id.indexSf2 == id2.indexSf2 && \
                id.indexElt == id2.indexElt &&(id.indexElt2 == id2.indexElt2 ||
                id.typeElement == elementInst || id.typeElement == elementPrst))
        {
            this->table->item(1,i)->setSelected(true);
            max = this->table->horizontalScrollBar()->maximum();
            if (max / this->table->columnCount() > 62)
                this->table->horizontalScrollBar()->setValue((max*(i-1))/this->table->columnCount());
            else
                this->table->scrollToItem(this->table->item(10, i), QAbstractItemView::PositionAtCenter);
        }
    }
    this->preparation = false;
}
void PageTable::selected()
{
    if (this->preparation) return;
    // Mise à jour de la sélection dans l'arborescence
    this->tree->blockSignals(true);
    int compte = this->table->selectedItems().count();
    if (compte)
    {
        this->tree->selectNone();
        int colonne;
        for (int i = 0; i < compte; i++)
        {
            colonne = this->table->selectedItems().takeAt(i)->column();
            this->tree->select(this->table->getID(colonne));
        }
        this->tree->updateAtNextSelectionRequest();
        // Mise à jour des informations sur les mods
        if (compte)
        {
            this->preparation = 1;
            colonne = this->table->selectedItems().takeAt(compte-1)->column();
            this->afficheMod(this->table->getID(colonne));
            this->preparation = 0;
        }
    }
    this->tree->blockSignals(false);
}

void PageTable::addAvailableReceiverMod(ComboBox *combo, EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return;
    // Parcours des mods autour de id
    EltID id2 = id;
    EltID id3 = id;
    WORD wTmp;
    int compte;
    bool stop, found;
    char T[20];
    int nbMod = sf2->count(id);
    for (int i = 0; i < nbMod; i++)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod && !sf2->get(id2, champ_hidden).bValue)
        {
            // On regarde si id2 peut recevoir des signaux de id
            // Condition : id2 n'émet pas, directement ou indirectement, de signaux à id
            compte = 0;
            found = false;
            stop = false;
            id3.indexMod = id2.indexMod;
            while (!stop)
            {
                wTmp = sf2->get(id3, champ_sfModDestOper).wValue;
                if (wTmp >= 32768)
                {
                    if (wTmp - 32768 < nbMod)
                    {
                        // Lien valide
                        id3.indexMod = wTmp - 32768;
                        if (id3.indexMod == id.indexMod)
                        {
                            // Pas autorisé
                            stop = true;
                            found = true;
                        }
                    }
                    else stop = true; // Lien non valide
                }
                else stop = true; // Pas de lien
                compte++;
                if (compte > nbMod) stop = true; // Boucle infinie
            }
            if (!found)
            {
                // id2 peut recevoir les signaux de id
                sprintf(T, "%s: #%d", trUtf8("Modulateur").toStdString().c_str(), id2.indexMod+1);
                combo->addItem(T);
            }
        }
    }
}
void PageTable::addAvailableSenderMod(ComboBox *combo, EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return;
    // Parcours des mods autour de id
    EltID id2 = id;
    EltID id3 = id;
    EltID id4 = id;
    WORD wTmp;
    SFModulator sfMod;
    int compte, indModSender;
    bool stop, found;
    char T[20];
    int nbMod = sf2->count(id);
    for (int i = 0; i < nbMod; i++)
    {
        id2.indexMod = i;
        if (id2.indexMod != id.indexMod && !sf2->get(id2, champ_hidden).bValue)
        {
            // On regarde si id2 peut envoyer des signaux à id
            // Condition : id2 ne reçoit pas, directement ou indirectement, de signaux de id
            compte = 0;
            found = false;
            stop = false;
            id3.indexMod = id2.indexMod;
            while (!stop)
            {
                sfMod = sf2->get(id3, champ_sfModSrcOper).sfModValue;
                if (sfMod.CC == 0 && sfMod.Index == 127)
                {
                    // On cherche le mod qui envoie des signaux à id3
                    indModSender = -1;
                    for (int j = 0; j < nbMod; j++)
                    {
                        id4.indexMod = j;
                        if (!sf2->get(id4, champ_hidden).bValue)
                        {
                            wTmp = sf2->get(id4, champ_sfModDestOper).wValue;
                            if (wTmp == 32768 + id3.indexMod)
                                indModSender = j;
                        }
                    }
                    if (indModSender != -1)
                    {
                        // Le sender a été trouvé
                        if (indModSender == id.indexMod)
                        {
                            // Pas autorisé
                            stop = true;
                            found = true;
                        }
                        else id3.indexMod = indModSender;
                    }
                    else stop = true; // Lien non valide
                }
                else stop = true; // Pas de lien
                compte++;
                if (compte > nbMod) stop = true; // Boucle infinie
            }
            if (!found)
            {
                // id2 peut envoyer des signaux à id
                sprintf(T, "%s: #%d", trUtf8("Modulateur").toStdString().c_str(), id2.indexMod+1);
                combo->addItem(T);
            }
        }
    }
}
int PageTable::getAssociatedMod(EltID id)
{
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return -1;
    int iVal = -1;
    EltID id2 = id;
    for (int j = 0; j < this->sf2->count(id); j++)
    {
        id2.indexMod = j;
        if (id2.indexMod != id.indexMod  && !this->sf2->get(id2, champ_hidden).bValue)
        {
            if (this->sf2->get(id2, champ_sfModDestOper).wValue == 32768 + id.indexMod)
                iVal = j;
        }
    }
    if (iVal > -1)
    {
        id2.indexMod = iVal;
        return id2.indexMod;
    }
    else return -1;
}

void PageTable::supprimerMod()
{
    this->sf2->prepareNewActions();
    // Suppression mod
    EltID id = this->tableMod->getID();
    if (id.typeElement != elementInstMod && id.typeElement != elementInstSmplMod && \
            id.typeElement != elementPrstMod && id.typeElement != elementPrstInstMod)
        return;
    // Liens vers le mod ?
    int iVal = -1;
    Valeur val;
    EltID id2 = id;
    do
    {
        iVal = getAssociatedMod(id);
        if (iVal != -1)
        {
            // Suppression du lien
            val.sfGenValue = (Champ)0;
            id2.indexMod = iVal;
            this->sf2->set(id2, champ_sfModDestOper, val);
        }
    } while (iVal != -1);
    // Le mod est-il lié ?
    if (this->sf2->get(id, champ_sfModDestOper).wValue >= 32768)
    {
        id2.indexMod = this->sf2->get(id, champ_sfModDestOper).wValue - 32768;
        if (id2.indexMod < this->sf2->count(id))
        {
            val.sfModValue = this->sf2->get(id2, champ_sfModSrcOper).sfModValue;
            val.sfModValue.CC = 0;
            val.sfModValue.Index = 0;
            this->sf2->set(id2, champ_sfModSrcOper, val);
        }
    }
    this->sf2->remove(id);
    this->afficheMod(this->tree->getID(0));
    this->mainWindow->updateDo();
}
void PageTable::nouveauMod()
{
    this->sf2->prepareNewActions();
    // Création nouveau mod
    EltID id = this->tree->getID(0);
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;
    id.indexMod = this->sf2->add(id);
    // initialisation
    Valeur val;
    val.sfGenValue = (Champ)0;
    val.wValue = 0;
    this->sf2->set(id, champ_modAmount, val);
    this->sf2->set(id, champ_sfModTransOper, val);
    val.sfModValue.CC = 0;
    val.sfModValue.D = 0;
    val.sfModValue.Index = 0;
    val.sfModValue.P = 0;
    val.sfModValue.Type = 0;
    this->sf2->set(id, champ_sfModSrcOper, val);
    this->sf2->set(id, champ_sfModAmtSrcOper, val);
    if (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod)
        val.sfGenValue = (Champ)52;
    this->sf2->set(id, champ_sfModDestOper, val);
    this->afficheMod(this->tree->getID(0), id.indexMod);
    this->mainWindow->updateDo();
}
void PageTable::copyMod()
{
    // Sauvegarde des mods
    if (this->tree->getSelectedItemsNumber() != 1)
        return;
    EltID id = this->tree->getID(0);
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;
    _modulatorCopy.clear();
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexMod = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            Modulator modTmp;
            modTmp.modSrcOper = this->sf2->get(id, champ_sfModSrcOper).sfModValue;
            modTmp.modDestOper = this->sf2->get(id, champ_sfModDestOper).sfGenValue;
            modTmp.modAmount = this->sf2->get(id, champ_modAmount).shValue;
            modTmp.modAmtSrcOper = this->sf2->get(id, champ_sfModAmtSrcOper).sfModValue;
            modTmp.modTransOper = this->sf2->get(id, champ_sfModTransOper).sfTransValue;
            modTmp.index = i;
            _modulatorCopy.append(modTmp);
        }
    }
}
void PageTable::pasteMod()
{
    if (this->tree->getSelectedItemsNumber() != 1)
        return;
    this->sf2->prepareNewActions();
    EltID id = this->tree->getID(0);
    if (id.typeElement == elementInst) id.typeElement = elementInstMod;
    else if (id.typeElement == elementPrst) id.typeElement = elementPrstMod;
    else if (id.typeElement == elementInstSmpl) id.typeElement = elementInstSmplMod;
    else if (id.typeElement == elementPrstInst) id.typeElement = elementPrstInstMod;
    else return;
    if (id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod)
    {
        // Vérification que toutes les destinations sont possibles
        Champ champTmp;
        for (int i = 0; i < _modulatorCopy.size(); i++)
        {
            champTmp = _modulatorCopy[i].modDestOper;
            QString warnQStr = trUtf8("Action impossible : ");
            if (champTmp == champ_startAddrsOffset ||
                    champTmp == champ_startAddrsCoarseOffset ||
                    champTmp == champ_startloopAddrsOffset ||
                    champTmp == champ_startloopAddrsCoarseOffset ||
                    champTmp == champ_endAddrsOffset ||
                    champTmp == champ_endAddrsCoarseOffset ||
                    champTmp == champ_endloopAddrsOffset ||
                    champTmp == champ_endloopAddrsCoarseOffset)
            {
                QMessageBox::warning(this, trUtf8("Attention"), warnQStr +
                                     trUtf8("les offsets ne peuvent être modulés dans un preset."));
                return;
            }
            else if (champTmp == champ_keynum || champTmp == champ_velocity ||
                    champTmp == champ_sampleModes ||
                    champTmp == champ_exclusiveClass ||
                    champTmp == champ_overridingRootKey)
            {
                QMessageBox::warning(this, trUtf8("Attention"), warnQStr +
                                     "\"" + getGenName(champTmp) + trUtf8("\" ne peut être modulé dans un preset."));
                return;
            }
        }
    }
    // Suppression des mods existants
    int nbElt = this->sf2->count(id);
    for (int i = nbElt - 1; i >= 0; i--)
    {
        id.indexMod = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
            this->sf2->remove(id);
    }
    // Création de nouveaux mods
    QList<int> listIndex;
    for (int i = 0; i < _modulatorCopy.size(); i++)
        listIndex.append(this->sf2->add(id));
    // Copie des configurations des mods sauvegardés
    Valeur valTmp;
    Modulator modTmp;
    for (int i = 0; i < _modulatorCopy.size(); i++)
    {
        id.indexMod = listIndex.at(i);
        modTmp = _modulatorCopy.at(i);
        valTmp.sfModValue = modTmp.modSrcOper;
        this->sf2->set(id, champ_sfModSrcOper, valTmp);
        if (modTmp.modDestOper >= 32768)
        {
            // Lien vers un autre modulateur
            int link = modTmp.modDestOper - 32768;
            int pos = -1;
            for (int j = 0; j < _modulatorCopy.size(); j++)
            {
                if (_modulatorCopy[j].index == link)
                    pos = j;
            }
            if (pos != i && pos > -1)
                valTmp.sfGenValue = (Champ)(32768 + listIndex.at(pos));
            else
                valTmp.sfGenValue = (Champ)0;
        }
        else
            valTmp.sfGenValue = modTmp.modDestOper;
        this->sf2->set(id, champ_sfModDestOper, valTmp);
        valTmp.shValue = modTmp.modAmount;
        this->sf2->set(id, champ_modAmount, valTmp);
        valTmp.sfModValue = modTmp.modAmtSrcOper;
        this->sf2->set(id, champ_sfModAmtSrcOper, valTmp);
        valTmp.sfTransValue = modTmp.modTransOper;
        this->sf2->set(id, champ_sfModTransOper, valTmp);
    }
    this->afficheMod(this->tree->getID(0), -1);
    this->mainWindow->updateDo();
}

void PageTable::remplirComboSource(ComboBox *comboBox)
{
    comboBox->addItem(getIndexName(0, 0));
    comboBox->addItem(getIndexName(2, 0));
    comboBox->addItem(getIndexName(3, 0));
    comboBox->addItem(getIndexName(10, 0));
    comboBox->addItem(getIndexName(13, 0));
    comboBox->addItem(getIndexName(14, 0));
    comboBox->addItem(getIndexName(16, 0));
    for (int i = 1; i < 6; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 7; i < 32; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 64; i < 98; i++) comboBox->addItem(getIndexName(i, 1));
    for (int i = 102; i < 120; i++) comboBox->addItem(getIndexName(i, 1));
    comboBox->setLimite(89);
}

WORD PageTable::getSrcNumber(WORD wVal, bool &CC)
{
    if (wVal < 7) CC = false;
    else CC = true;
    WORD wRet = 0;
    switch (wVal)
    {
    case 0: wRet = 0; break;
    case 1: wRet = 2; break;
    case 2: wRet = 3; break;
    case 3: wRet = 10; break;
    case 4: wRet = 13; break;
    case 5: wRet = 14; break;
    case 6: wRet = 16; break;
    case 7: wRet = 1; break;
    case 8: wRet = 2; break;
    case 9: wRet = 3; break;
    case 10: wRet = 4; break;
    case 11: wRet = 5; break;
    case 12: wRet = 7; break;
    case 13: wRet = 8; break;
    case 14: wRet = 9; break;
    case 15: wRet = 10; break;
    case 16: wRet = 11; break;
    case 17: wRet = 12; break;
    case 18: wRet = 13; break;
    case 19: wRet = 14; break;
    case 20: wRet = 15; break;
    case 21: wRet = 16; break;
    case 22: wRet = 17; break;
    case 23: wRet = 18; break;
    case 24: wRet = 19; break;
    case 25: wRet = 20; break;
    case 26: wRet = 21; break;
    case 27: wRet = 22; break;
    case 28: wRet = 23; break;
    case 29: wRet = 24; break;
    case 30: wRet = 25; break;
    case 31: wRet = 26; break;
    case 32: wRet = 27; break;
    case 33: wRet = 28; break;
    case 34: wRet = 29; break;
    case 35: wRet = 30; break;
    case 36: wRet = 31; break;
    case 37: wRet = 64; break;
    case 38: wRet = 65; break;
    case 39: wRet = 66; break;
    case 40: wRet = 67; break;
    case 41: wRet = 68; break;
    case 42: wRet = 69; break;
    case 43: wRet = 70; break;
    case 44: wRet = 71; break;
    case 45: wRet = 72; break;
    case 46: wRet = 73; break;
    case 47: wRet = 74; break;
    case 48: wRet = 75; break;
    case 49: wRet = 76; break;
    case 50: wRet = 77; break;
    case 51: wRet = 78; break;
    case 52: wRet = 79; break;
    case 53: wRet = 80; break;
    case 54: wRet = 81; break;
    case 55: wRet = 82; break;
    case 56: wRet = 83; break;
    case 57: wRet = 84; break;
    case 58: wRet = 85; break;
    case 59: wRet = 86; break;
    case 60: wRet = 87; break;
    case 61: wRet = 88; break;
    case 62: wRet = 89; break;
    case 63: wRet = 90; break;
    case 64: wRet = 91; break;
    case 65: wRet = 92; break;
    case 66: wRet = 93; break;
    case 67: wRet = 94; break;
    case 68: wRet = 95; break;
    case 69: wRet = 96; break;
    case 70: wRet = 97; break;
    case 71: wRet = 102; break;
    case 72: wRet = 103; break;
    case 73: wRet = 104; break;
    case 74: wRet = 105; break;
    case 75: wRet = 106; break;
    case 76: wRet = 107; break;
    case 77: wRet = 108; break;
    case 78: wRet = 109; break;
    case 79: wRet = 110; break;
    case 80: wRet = 111; break;
    case 81: wRet = 112; break;
    case 82: wRet = 113; break;
    case 83: wRet = 114; break;
    case 84: wRet = 115; break;
    case 85: wRet = 116; break;
    case 86: wRet = 117; break;
    case 87: wRet = 118; break;
    case 88: wRet = 119; break;
    default: wRet = 0; CC = false; break;
    }
    return wRet;
}
WORD PageTable::getSrcIndex(WORD wVal, bool bVal)
{
    WORD wRet = 0;
    if (bVal)
    {
        switch (wVal)
        {
        case 1: wRet = 7; break;
        case 2: wRet = 8; break;
        case 3: wRet = 9; break;
        case 4: wRet = 10; break;
        case 5: wRet = 11; break;
        case 7: wRet = 12; break;
        case 8: wRet = 13; break;
        case 9: wRet = 14; break;
        case 10: wRet = 15; break;
        case 11: wRet = 16; break;
        case 12: wRet = 17; break;
        case 13: wRet = 18; break;
        case 14: wRet = 19; break;
        case 15: wRet = 20; break;
        case 16: wRet = 21; break;
        case 17: wRet = 22; break;
        case 18: wRet = 23; break;
        case 19: wRet = 24; break;
        case 20: wRet = 25; break;
        case 21: wRet = 26; break;
        case 22: wRet = 27; break;
        case 23: wRet = 28; break;
        case 24: wRet = 29; break;
        case 25: wRet = 30; break;
        case 26: wRet = 31; break;
        case 27: wRet = 32; break;
        case 28: wRet = 33; break;
        case 29: wRet = 34; break;
        case 30: wRet = 35; break;
        case 31: wRet = 36; break;
        case 64: wRet = 37; break;
        case 65: wRet = 38; break;
        case 66: wRet = 39; break;
        case 67: wRet = 40; break;
        case 68: wRet = 41; break;
        case 69: wRet = 42; break;
        case 70: wRet = 43; break;
        case 71: wRet = 44; break;
        case 72: wRet = 45; break;
        case 73: wRet = 46; break;
        case 74: wRet = 47; break;
        case 75: wRet = 48; break;
        case 76: wRet = 49; break;
        case 77: wRet = 50; break;
        case 78: wRet = 51; break;
        case 79: wRet = 52; break;
        case 80: wRet = 53; break;
        case 81: wRet = 54; break;
        case 82: wRet = 55; break;
        case 83: wRet = 56; break;
        case 84: wRet = 57; break;
        case 85: wRet = 58; break;
        case 86: wRet = 59; break;
        case 87: wRet = 60; break;
        case 88: wRet = 61; break;
        case 89: wRet = 62; break;
        case 90: wRet = 63; break;
        case 91: wRet = 64; break;
        case 92: wRet = 65; break;
        case 93: wRet = 66; break;
        case 94: wRet = 67; break;
        case 95: wRet = 68; break;
        case 96: wRet = 69; break;
        case 97: wRet = 70; break;
        case 102: wRet = 71; break;
        case 103: wRet = 72; break;
        case 104: wRet = 73; break;
        case 105: wRet = 74; break;
        case 106: wRet = 75; break;
        case 107: wRet = 76; break;
        case 108: wRet = 77; break;
        case 109: wRet = 78; break;
        case 110: wRet = 79; break;
        case 111: wRet = 80; break;
        case 112: wRet = 81; break;
        case 113: wRet = 82; break;
        case 114: wRet = 83; break;
        case 115: wRet = 84; break;
        case 116: wRet = 85; break;
        case 117: wRet = 86; break;
        case 118: wRet = 87; break;
        case 119: wRet = 88; break;
        }
    }
    else
    {
        switch (wVal)
        {
        case 0: wRet = 0; break;
        case 2: wRet = 1; break;
        case 3: wRet = 2; break;
        case 10: wRet = 3; break;
        case 13: wRet = 4; break;
        case 14: wRet = 5; break;
        case 16: wRet = 6; break;
        case 127: wRet = 100; break; // lien
        }
    }
    return wRet;
}

int PageTable::limit(int iVal, Champ champ, EltID id)
{
    int ret = iVal;
    EltID id2 = id;
    EltID id3 = id;
    if (id.typeElement != elementInst && id.typeElement != elementInstSmpl) return 0;
    int limInf, limSup;
    switch ((int)champ)
    {
    case champ_startAddrsOffset:
        // Limite inférieure
        if (ret < 0) ret = 0;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limSup = this->sf2->get(id2, champ_dwLength).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || this->sf2->get(id2, champ_dwLength).dwValue < (unsigned)limSup)
                    limSup = this->sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    case champ_endAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limInf = -this->sf2->get(id2, champ_dwLength).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)this->sf2->get(id2, champ_dwLength).dwValue > limInf)
                    limInf = -this->sf2->get(id2, champ_dwLength).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        if (ret > 0) ret = 0;
        break;
    case champ_startloopAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limInf = -this->sf2->get(id2, champ_dwStartLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)this->sf2->get(id2, champ_dwStartLoop).dwValue > limInf)
                    limInf = -this->sf2->get(id2, champ_dwStartLoop).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limSup = this->sf2->get(id2, champ_dwLength).dwValue - this->sf2->get(id2, champ_dwStartLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || \
                        this->sf2->get(id2, champ_dwLength).dwValue - \
                        this->sf2->get(id2, champ_dwStartLoop).dwValue < (unsigned)limSup)
                    limSup = this->sf2->get(id2, champ_dwLength).dwValue - \
                            this->sf2->get(id2, champ_dwStartLoop).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    case champ_endloopAddrsOffset:
        // Limite inférieure
        limInf = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limInf = -this->sf2->get(id2, champ_dwEndLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || -(signed)this->sf2->get(id2, champ_dwEndLoop).dwValue > limInf)
                    limInf = -this->sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret < limInf) ret = limInf;
        // Limite supérieure
        limSup = 0;
        if (id.typeElement == elementInstSmpl)
        {
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
            limSup = this->sf2->get(id2, champ_dwLength).dwValue - this->sf2->get(id2, champ_dwEndLoop).dwValue;
        }
        else
        {
            id3.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            for (int i = 0; i < this->sf2->count(id3); i++)
            {
                id3.indexElt2 = i;
                id2.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
                if (i == 0 || \
                        this->sf2->get(id2, champ_dwLength).dwValue - \
                        this->sf2->get(id2, champ_dwEndLoop).dwValue < (unsigned)limSup)
                    limSup = this->sf2->get(id2, champ_dwLength).dwValue - \
                            this->sf2->get(id2, champ_dwEndLoop).dwValue;
            }
        }
        if (ret > limSup) ret = limSup;
        break;
    }
    return ret;
}

void PageTable::paramGlobal()
{
    this->sf2->prepareNewActions();
    EltID id = this->tree->getID(0);
    if (m_typePage == PAGE_INST)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    // Vérification que l'élément possède au moins un élément lié, avec un keyrange valide
    int nbElt = 0;
    int posMin = 128;
    int posMax = 0;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            nbElt++;
            if (this->sf2->isSet(id, champ_keyRange))
            {
                if (this->sf2->get(id, champ_keyRange).rValue.byLo < posMin)
                    posMin = this->sf2->get(id, champ_keyRange).rValue.byLo;
                if (this->sf2->get(id, champ_keyRange).rValue.byHi > posMax)
                    posMax = this->sf2->get(id, champ_keyRange).rValue.byHi;
            }
        }
    }
    if (nbElt == 0)
    {
        if (m_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le preset doit contenir des instruments."));
        this->mainWindow->updateDo();
        return;
    }
    if (posMin > posMax)
    {
        if (m_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour l'instrument."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour le preset."));
        this->mainWindow->updateDo();
        return;
    }
    DialogParamGlobal * dialogParam = new DialogParamGlobal(this->sf2, id, this);
    dialogParam->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogParam, SIGNAL(accepted(QVector<double>,QList<EltID>,int,int,int,int)),
                  SLOT(paramGlobal(QVector<double>,QList<EltID>,int,int,int,int)));
    dialogParam->show();
}
void PageTable::paramGlobal(QVector<double> dValues, QList<EltID> listElt, int typeModif, int champ, int velMin, int velMax)
{
    // Modification de tous les éléments
    EltID id;
    if (velMin > velMax)
    {
        int tmp = velMin;
        velMin = velMax;
        velMax = tmp;
    }
    for (int numID = 0; numID < listElt.size(); numID++)
    {
        // Pos min et max sur le clavier
        id = listElt.at(numID);
        if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            id.typeElement = elementInstSmpl;
        else
            id.typeElement = elementPrstInst;

        // Modification des valeurs pour chaque élément associé à id
        double amount;
        int pos;
        char T[30];
        bool ok;
        Valeur value;
        for (int i = 0; i < this->sf2->count(id); i++)
        {
            id.indexElt2 = i;
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                int velMin2 = 0;
                int velMax2 = 127;
                if (this->sf2->isSet(id, champ_velRange))
                {
                    velMin2 = this->sf2->get(id, champ_velRange).rValue.byLo;
                    velMax2 = this->sf2->get(id, champ_velRange).rValue.byHi;
                }
                if (velMin2 > velMax2)
                {
                    int tmp = velMin2;
                    velMin2 = velMax2;
                    velMax2 = tmp;
                }
                if (velMin2 >= velMin && velMax2 <= velMax)
                {
                    amount = QString(getTextValue(T, (Champ)champ, this->sf2->get(id, (Champ)champ).genValue)).toDouble();

                    // Calcul de la modification
                    pos = (double)(this->sf2->get(id, champ_keyRange).rValue.byLo +
                           this->sf2->get(id, champ_keyRange).rValue.byHi) / 2 * dValues.size() / 127. + 0.5;
                    if (pos < 0)
                        pos = 0;
                    else if (pos >= dValues.size())
                        pos = dValues.size() - 1;

                    // Application de la modification
                    switch (typeModif)
                    {
                    case 0: // Ajout
                        amount += dValues.at(pos);
                        break;
                    case 1: // Multiplication
                        amount *= dValues.at(pos);
                        break;
                    case 2: // Remplacement
                        amount = dValues.at(pos);
                        break;
                    }
                    value.genValue = getValue(QString::number(amount), (Champ)champ, ok);
                    this->sf2->set(id, (Champ)champ, value);
                }
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void PageTable::duplication()
{
    EltID id = this->tree->getID(0);
    if (m_typePage == PAGE_INST)
    {
        id.typeElement = elementInstSmpl;
        if (this->sf2->count(id, false) == 0)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
            return;
        }
    }
    else
    {
        id.typeElement = elementPrstInst;
        if (this->sf2->count(id, false) == 0)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le preset doit contenir des instruments."));
            return;
        }
    }
    DialogDuplication * dialogDuplication = new DialogDuplication(m_typePage == PAGE_PRST, this);
    dialogDuplication->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogDuplication, SIGNAL(accepted(QVector<int>,bool,bool)),
                  SLOT(duplication(QVector<int>,bool,bool)));
    dialogDuplication->show();
}
void PageTable::duplication(QVector<int> listeVelocites, bool duplicKey, bool duplicVel)
{
    this->sf2->prepareNewActions();
    EltID id = this->tree->getID(0);
    if (m_typePage == PAGE_INST)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;

    // Duplication pour chaque note
    if (duplicKey)
    {
        int nbElementsLies = this->sf2->count(id);
        for (int i = 0; i < nbElementsLies; i++)
        {
            id.indexElt2 = i;
            if (!this->sf2->get(id, champ_hidden).bValue)
                this->duplication(id);
        }
    }

    // Duplication pour chaque velocityRange
    if (duplicVel)
    {
        int nbElementsLies = this->sf2->count(id);
        for (int i = 0; i < nbElementsLies; i++)
        {
            id.indexElt2 = i;
            if (!this->sf2->get(id, champ_hidden).bValue)
                this->duplication(id, listeVelocites);
        }
    }

    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void PageTable::duplication(EltID id)
{
    // Etendue du sample sur le clavier
    int numBas = 0;
    int numHaut = 127;
    if (this->sf2->isSet(id, champ_keyRange))
    {
        numBas = this->sf2->get(id, champ_keyRange).rValue.byLo;
        numHaut = this->sf2->get(id, champ_keyRange).rValue.byHi;
        if (numBas > numHaut)
        {
            int temp = numBas;
            numBas = numHaut;
            numHaut = temp;
        }
    }
    if (numBas != numHaut)
    {
        // Modification keyrange
        Valeur val;
        val.rValue.byLo = numBas;
        val.rValue.byHi = numBas;
        this->sf2->set(id, champ_keyRange, val);
        // Création de divisions identiques pour les autres notes
        EltID id2 = id;
        for (int i = numBas+1; i <= numHaut; i++)
        {
            if (m_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmpl;
                id2.typeElement = elementInstSmpl;
                id2.indexElt2 = this->sf2->add(id);
                // Recopiage des gens
                id.typeElement = elementInstSmplGen;
            }
            else
            {
                id.typeElement = elementPrstInst;
                id2.typeElement = elementPrstInst;
                id2.indexElt2 = this->sf2->add(id);
                // Recopiage des gens
                id.typeElement = elementPrstInstGen;
            }
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                this->sf2->set(id2, (Champ)this->sf2->get(id, champ_sfGenOper).wValue,
                               this->sf2->get(id, champ_sfGenAmount));
            }
            // Modification keyrange
            val.rValue.byLo = i;
            val.rValue.byHi = i;
            this->sf2->set(id2, champ_keyRange, val);
            // Recopiage des mods
            if (m_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmplMod;
                id2.typeElement = elementInstSmplMod;
            }
            else
            {
                id.typeElement = elementPrstInstMod;
                id2.typeElement = elementPrstInstMod;
            }
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                id2.indexMod = this->sf2->add(id2);
                this->sf2->set(id2, champ_modAmount, this->sf2->get(id, champ_modAmount));
                this->sf2->set(id2, champ_sfModSrcOper, this->sf2->get(id, champ_sfModSrcOper));
                this->sf2->set(id2, champ_sfModAmtSrcOper, this->sf2->get(id, champ_sfModAmtSrcOper));
                this->sf2->set(id2, champ_sfModDestOper, this->sf2->get(id, champ_sfModDestOper));
                this->sf2->set(id2, champ_sfModTransOper, this->sf2->get(id, champ_sfModTransOper));
            }
        }
    }
}
void PageTable::duplication(EltID id, QVector<int> listeVelocite)
{
    QVector<bool> rangeOk;
    rangeOk.fill(false, listeVelocite.size() / 2);

    // Element lié
    int idLinked;
    if (m_typePage == PAGE_INST)
        idLinked = this->sf2->get(id, champ_sampleID).wValue;
    else
        idLinked = this->sf2->get(id, champ_instrument).wValue;

    // Keyrange
    int noteMin = 0;
    int noteMax = 127;
    if (this->sf2->isSet(id, champ_keyRange))
    {
        noteMin = this->sf2->get(id, champ_keyRange).rValue.byLo;
        noteMax = this->sf2->get(id, champ_keyRange).rValue.byHi;
        if (noteMin > noteMax)
        {
            int tmp = noteMin;
            noteMin = noteMax;
            noteMax = tmp;
        }
    }

    // On scanne tous les éléments liés pour savoir si des étendues sont déjà présentes
    bool selfOk = false;
    int indexSelf = id.indexElt2;
    int nbElementsLies = this->sf2->count(id);
    for (int i = 0; i < nbElementsLies; i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            // Keyrange
            int noteMin2 = 0;
            int noteMax2 = 127;
            if (this->sf2->isSet(id, champ_keyRange))
            {
                noteMin2 = this->sf2->get(id, champ_keyRange).rValue.byLo;
                noteMax2 = this->sf2->get(id, champ_keyRange).rValue.byHi;
                if (noteMin2 > noteMax2)
                {
                    int tmp = noteMin2;
                    noteMin2 = noteMax2;
                    noteMax2 = tmp;
                }
            }
            int idLinked2;
            if (m_typePage == PAGE_INST)
                idLinked2 = this->sf2->get(id, champ_sampleID).wValue;
            else
                idLinked2 = this->sf2->get(id, champ_instrument).wValue;

            if (noteMin == noteMin2 && noteMax == noteMax2 && idLinked == idLinked2)
            {
                int velMin2 = 0;
                int velMax2 = 127;
                if (this->sf2->isSet(id, champ_keyRange))
                {
                    velMin2 = this->sf2->get(id, champ_velRange).rValue.byLo;
                    velMax2 = this->sf2->get(id, champ_velRange).rValue.byHi;
                    if (velMin2 > velMax2)
                    {
                        int tmp = velMin2;
                        velMin2 = velMax2;
                        velMax2 = tmp;
                    }
                }

                for (int j = 0; j < listeVelocite.size() / 2; j++)
                {
                    int velMin = qMin(listeVelocite.at(2 * j), listeVelocite.at(2 * j + 1));
                    int velMax = qMax(listeVelocite.at(2 * j), listeVelocite.at(2 * j + 1));

                    if (velMin == velMin2 && velMax == velMax2)
                    {
                        rangeOk[j] = true;
                        if (i == indexSelf)
                            selfOk = true;
                    }
                }
            }
        }
    }

    if (rangeOk.indexOf(false) != -1)
    {
        id.indexElt2 = indexSelf;

        if (!selfOk)
        {
            // Modification de l'étendue de vélocité de l'élément de départ
            int pos = rangeOk.indexOf(false);
            rangeOk[pos] = true;
            Valeur val;
            val.rValue.byLo = qMin(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            val.rValue.byHi = qMax(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            this->sf2->set(id, champ_velRange, val);
        }

        while (rangeOk.indexOf(false) != -1)
        {
            // Duplication
            if (m_typePage == PAGE_INST)
                id.typeElement = elementInstSmpl;
            else
                id.typeElement = elementPrstInst;
            EltID id2 = id;
            id2.indexElt2 = this->sf2->add(id);

            // Recopiage des gens
            if (m_typePage == PAGE_INST)
                id.typeElement = elementInstSmplGen;
            else
                id.typeElement = elementPrstInstGen;
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                this->sf2->set(id2, (Champ)this->sf2->get(id, champ_sfGenOper).wValue,
                               this->sf2->get(id, champ_sfGenAmount));
            }

            // Modification étendue de vélocité
            int pos = rangeOk.indexOf(false);
            rangeOk[pos] = true;
            Valeur val;
            val.rValue.byLo = qMin(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            val.rValue.byHi = qMax(listeVelocite.at(2 * pos), listeVelocite.at(2 * pos + 1));
            this->sf2->set(id2, champ_velRange, val);

            // Recopiage des mods
            if (m_typePage == PAGE_INST)
            {
                id.typeElement = elementInstSmplMod;
                id2.typeElement = elementInstSmplMod;
            }
            else
            {
                id.typeElement = elementPrstInstMod;
                id2.typeElement = elementPrstInstMod;
            }
            for (int j = 0; j < this->sf2->count(id); j++)
            {
                id.indexMod = j;
                id2.indexMod = this->sf2->add(id2);
                this->sf2->set(id2, champ_modAmount, this->sf2->get(id, champ_modAmount));
                this->sf2->set(id2, champ_sfModSrcOper, this->sf2->get(id, champ_sfModSrcOper));
                this->sf2->set(id2, champ_sfModAmtSrcOper, this->sf2->get(id, champ_sfModAmtSrcOper));
                this->sf2->set(id2, champ_sfModDestOper, this->sf2->get(id, champ_sfModDestOper));
                this->sf2->set(id2, champ_sfModTransOper, this->sf2->get(id, champ_sfModTransOper));
            }
        }
    }
}
void PageTable::spatialisation()
{
    EltID id = this->tree->getID(0);
    if (m_typePage == PAGE_INST)
    {
        id.typeElement = elementInstSmpl;
        if (this->sf2->count(id, false) == 0)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
            return;
        }
    }
    else
    {
        id.typeElement = elementPrstInst;
        if (this->sf2->count(id, false) == 0)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le preset doit contenir des instruments."));
            return;
        }
    }
    DialogSpace * dialogSpace = new DialogSpace(m_typePage == PAGE_PRST, this);
    dialogSpace->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogSpace, SIGNAL(accepted(int,int,int,int,int,int,int)),
                  SLOT(spatialisation(int,int,int,int,int,int,int)));
    dialogSpace->show();
}
void PageTable::spatialisation(int motif, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2)
{
    this->sf2->prepareNewActions();
    // Sauvegarde des valeurs
    Config * conf = Config::getInstance();
    bool isPrst = m_typePage == PAGE_PRST;
    conf->setTools_space_motif(isPrst, motif);
    conf->setTools_space_divisions(isPrst, nbDiv);
    conf->setTools_space_etalement(isPrst, etalement);
    conf->setTools_space_occupation(isPrst, occupation);
    conf->setTools_space_offset(isPrst, offset);
    conf->setTools_space_renversement1(isPrst, sens);
    conf->setTools_space_renversement2(isPrst, sens2);

    // Liste des éléments liés avec leur lien (stéréo) le cas échéant
    QList<EltID> list1;
    QList<genAmountType> listRange;
    QList<EltID> list2;
    EltID id = this->tree->getID(0);
    genAmountType amount;
    bool found;
    int pos;
    int noteMin = 128;
    int noteMax = 0;
    if (isPrst)
        id.typeElement = elementPrstInst;
    else
        id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (this->sf2->isSet(id, champ_keyRange))
                amount = this->sf2->get(id, champ_keyRange).genValue;
            else
            {
                amount.ranges.byLo = 0;
                amount.ranges.byHi = 127;
            }
            if (amount.ranges.byLo < noteMin) noteMin = amount.ranges.byLo;
            if (amount.ranges.byHi > noteMax) noteMax = amount.ranges.byHi;

            // Recherche d'une note liée ayant la même étendue sur le clavier
            found = false;
            if (!isPrst)
            {
                pos = 0;
                while (pos < list1.size() && !found)
                {
                    if (amount.ranges.byHi == listRange.at(pos).ranges.byHi &&
                        amount.ranges.byLo == listRange.at(pos).ranges.byLo &&
                        list2.at(pos).indexElt2 == -1)
                    {
                        // Les samples sont-ils liés ?
                        EltID idSmpl1 = id;
                        idSmpl1.indexElt = this->sf2->get(id, champ_sampleID).wValue;
                        idSmpl1.typeElement = elementSmpl;
                        EltID idSmpl2 = list1.at(pos);
                        idSmpl2.indexElt = this->sf2->get(idSmpl2, champ_sampleID).wValue;
                        idSmpl2.typeElement = elementSmpl;
                        if (idSmpl1.indexElt == this->sf2->get(idSmpl2, champ_wSampleLink).wValue)
                        {
                            SFSampleLink type1 = this->sf2->get(idSmpl1, champ_sfSampleType).sfLinkValue;
                            SFSampleLink type2 = this->sf2->get(idSmpl2, champ_sfSampleType).sfLinkValue;
                            if (((type1 == rightSample || type1 == RomRightSample) && (type2 == leftSample || type2 == RomLeftSample)) ||
                                ((type1 == leftSample || type1 == RomLeftSample) && (type2 == rightSample || type2 == RomRightSample)))
                                found = true;
                        }
                    }
                    if (!found)
                        pos++;
                }
            }
            if (found)
            {
                // Lien
                list2[pos] = id;
            }
            else
            {
                // Ajout à liste 1
                list1.append(id);
                // Element nul dans liste 2
                id.indexElt2 = -1;
                list2.append(id);
                // Etendue
                listRange.append(amount);
            }
        }
    }
    // Spatialisation
    double pan = 0;
    int note = 64;
    EltID id2, id3;
    int sampleG;
    Valeur val;
    for (int i = 0; i < list1.size(); i++)
    {
        note = (listRange.at(i).ranges.byLo + listRange.at(i).ranges.byHi) / 2;
        pan = DialogSpace::space(noteMin, noteMax, note, motif, nbDiv,
                                 etalement, occupation, offset, sens, sens2);
        // Lien ?
        if (list2.at(i).indexElt2 == -1)
        {
            // pas de lien
            val.genValue.shAmount = 1000 * pan - 500;
            this->sf2->set(list1.at(i), champ_pan, val);
        }
        else
        {
            // Quel sample est à gauche ?
            sampleG = 0;
            // Sample correspondant 1
            id2 = list1.at(i);
            id2.indexElt = this->sf2->get(id2, champ_sampleID).wValue;
            id2.typeElement = elementSmpl;
            SFSampleLink type1 = this->sf2->get(id2, champ_sfSampleType).sfLinkValue;
            // Sample correspondant 2
            id3 = list2.at(i);
            id3.indexElt = this->sf2->get(id3, champ_sampleID).wValue;
            id3.typeElement = elementSmpl;
            SFSampleLink type2 = this->sf2->get(id3, champ_sfSampleType).sfLinkValue;
            if ((type1 == leftSample || type1 == RomLeftSample) &&
                 type2 != leftSample && type2 != RomLeftSample)
            {
                sampleG = 0;
            }
            else if ((type1 == rightSample || type1 == RomRightSample) &&
                     type2 != rightSample && type2 != RomRightSample)
            {
                sampleG = 1;
            }
            else
            {
                if (this->sf2->get(list1.at(i), champ_pan).shValue <
                        this->sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 0;
                else if (this->sf2->get(list1.at(i), champ_pan).shValue >
                        this->sf2->get(list2.at(i), champ_pan).shValue)
                    sampleG = 1;
            }
            if (sampleG == 0)
            {
                // Inversion
                id2 = list1.at(i);
                list1[i] = list2.at(i);
                list2[i] = id2;
            }
            // lien
            if (pan < 0.5)
            {
                // Gauche
                val.genValue.shAmount = -500;
                this->sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 2000 * pan - 500;
                this->sf2->set(list2.at(i), champ_pan, val);
            }
            else
            {
                // Gauche
                val.genValue.shAmount = 2000 * pan - 1500;
                this->sf2->set(list1.at(i), champ_pan, val);
                // Droite
                val.genValue.shAmount = 500;
                this->sf2->set(list2.at(i), champ_pan, val);
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}

void PageTable::visualize()
{
    EltID id = this->tree->getID(0);
    if (m_typePage == PAGE_INST)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    // Vérification que l'élément possède au moins un élément lié, avec un keyrange valide
    int nbElt = 0;
    int posMin = 128;
    int posMax = 0;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            nbElt++;
            if (this->sf2->isSet(id, champ_keyRange))
            {
                if (this->sf2->get(id, champ_keyRange).rValue.byLo < posMin)
                    posMin = this->sf2->get(id, champ_keyRange).rValue.byLo;
                if (this->sf2->get(id, champ_keyRange).rValue.byHi > posMax)
                    posMax = this->sf2->get(id, champ_keyRange).rValue.byHi;
            }
        }
    }
    if (nbElt == 0)
    {
        if (m_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le preset doit contenir des instruments."));
        return;
    }
    if (posMin > posMax)
    {
        if (m_typePage == PAGE_INST)
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour l'instrument."));
        else
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucune étendue de notes spécifiée pour le preset."));
        return;
    }
    DialogVisualizer * dialogVisu = new DialogVisualizer(this->sf2, id, this);
    dialogVisu->setAttribute(Qt::WA_DeleteOnClose, true);
    dialogVisu->show();
}


int PageTable::getDestNumber(int i)
{
    if (m_typePage == PAGE_PRST)
    {
        switch (i)
        {
        case 0:  return 52; // Pitch/Effects
        case 1:  return 51;
        case 2:  return 56;
        case 3:  return 8;
        case 4:  return 9;
        case 5:  return 17;
        case 6:  return 15;
        case 7:  return 16;
        case 8:  return 48; // Volume envelope
        case 9:  return 33;
        case 10: return 34;
        case 11: return 35;
        case 12: return 36;
        case 13: return 37;
        case 14: return 38;
        case 15: return 39;
        case 16: return 40;
        case 17: return 25; // Modulation envelope
        case 18: return 26;
        case 19: return 27;
        case 20: return 28;
        case 21: return 29;
        case 22: return 30;
        case 23: return 7;
        case 24: return 11;
        case 25: return 31;
        case 26: return 32;
        case 27: return 21; // Modulation LFO
        case 28: return 22;
        case 29: return 5;
        case 30: return 13;
        case 31: return 10;
        case 32: return 23; // Vibrato LFO
        case 33: return 24;
        case 34: return 6;
        default: return 100;
        }
    }
    else
    {
        switch (i)
        {
        case 0:  return 0; // Sample
        case 1:  return 4;
        case 2:  return 1;
        case 3:  return 12;
        case 4:  return 2;
        case 5:  return 45;
        case 6:  return 3;
        case 7:  return 50;
        case 8:  return 58; // Pitch/Effects
        case 9:  return 52;
        case 10: return 51;
        case 11: return 56;
        case 12: return 8;
        case 13: return 9;
        case 14: return 17;
        case 15: return 15;
        case 16: return 16;
        case 17: return 46;
        case 18: return 57;
        case 19: return 48; // Volume envelope
        case 20: return 33;
        case 21: return 34;
        case 22: return 35;
        case 23: return 36;
        case 24: return 37;
        case 25: return 38;
        case 26: return 39;
        case 27: return 40;
        case 28: return 47;
        case 29: return 54;
        case 30: return 25; // Modulation envelope
        case 31: return 26;
        case 32: return 27;
        case 33: return 28;
        case 34: return 29;
        case 35: return 30;
        case 36: return 7;
        case 37: return 11;
        case 38: return 31;
        case 39: return 32;
        case 40: return 21; // Modulation LFO
        case 41: return 22;
        case 42: return 5;
        case 43: return 13;
        case 44: return 10;
        case 45: return 23; // Vibrato LFO
        case 46: return 24;
        case 47: return 6;
        default: return 100;
        }
    }
}
int PageTable::getDestIndex(int i)
{
    if (m_typePage == PAGE_PRST)
    {
        switch (i)
        {
        case 52: return 0; // Pitch/Effects
        case 51: return 1;
        case 56: return 2;
        case 8:  return 3;
        case 9:  return 4;
        case 17: return 5;
        case 15: return 6;
        case 16: return 7;
        case 48: return 8; // Volume envelope
        case 33: return 9;
        case 34: return 10;
        case 35: return 11;
        case 36: return 12;
        case 37: return 13;
        case 38: return 14;
        case 39: return 15;
        case 40: return 16;
        case 25: return 17; // Modulation envelope
        case 26: return 18;
        case 27: return 19;
        case 28: return 20;
        case 29: return 21;
        case 30: return 22;
        case 7:  return 23;
        case 11: return 24;
        case 31: return 25;
        case 32: return 26;
        case 21: return 27; // Modulation LFO
        case 22: return 28;
        case 5:  return 29;
        case 13: return 30;
        case 10: return 31;
        case 23: return 32; // Vibrato LFO
        case 24: return 33;
        case 6:  return 34;
        default: return 100;
        }
    }
    else
    {
        switch (i)
        {
        case 0:  return 0; // Sample
        case 4:  return 1;
        case 1:  return 2;
        case 12: return 3;
        case 2:  return 4;
        case 45: return 5;
        case 3:  return 6;
        case 50: return 7;
        case 58: return 8;
        case 52: return 9; // Pitch/Effects
        case 51: return 10;
        case 56: return 11;
        case 8:  return 12;
        case 9:  return 13;
        case 17: return 14;
        case 15: return 15;
        case 16: return 16;
        case 46: return 17;
        case 57: return 18;
        case 48: return 19; // Volume envelope
        case 33: return 20;
        case 34: return 21;
        case 35: return 22;
        case 36: return 23;
        case 37: return 24;
        case 38: return 25;
        case 39: return 26;
        case 40: return 27;
        case 47: return 28;
        case 54: return 29;
        case 25: return 30; // Modulation envelope
        case 26: return 31;
        case 27: return 32;
        case 28: return 33;
        case 29: return 34;
        case 30: return 35;
        case 7:  return 36;
        case 11: return 37;
        case 31: return 38;
        case 32: return 39;
        case 21: return 40; // Modulation LFO
        case 22: return 41;
        case 5:  return 42;
        case 13: return 43;
        case 10: return 44;
        case 23: return 45; // Vibrato LFO
        case 24: return 46;
        case 6:  return 47;
        default: return 100;
        }
    }
}

void PageTable::enlightColumn(int key, bool isEnlighted)
{
    // Mise à jour éléments enclenchés
    if (isEnlighted && _listKeyEnlighted.indexOf(key) == -1)
        _listKeyEnlighted.append(key);
    else
        _listKeyEnlighted.removeAll(key);

    // Mise à jour de la table
    for (int i = 1; i < this->table->columnCount(); i++)
    {
        EltID id = this->table->getID(i);
        if (sf2->isValide(id))
        {
            bool enlighted = false;
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                int key1 = this->sf2->get(id, champ_keyRange).rValue.byLo;
                int key2 = this->sf2->get(id, champ_keyRange).rValue.byHi;
                if (!this->sf2->isSet(id, champ_keyRange))
                {
                    key1 = 0;
                    key2 = 128;
                }
                for (int j = 0; j < _listKeyEnlighted.size(); j++)
                    enlighted = enlighted || (qMin(key1, key2) <= _listKeyEnlighted.at(j)
                                              && qMax(key1, key2) >= _listKeyEnlighted.at(j));
            }
            this->table->setEnlighted(i, enlighted);
        }
    }
}

//////////////////////////////////////////// TABLE WIDGET ////////////////////////////////////////////

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    KeyPressCatcher * keyPressCatcher = new KeyPressCatcher(this);
    this->installEventFilter(keyPressCatcher);
    connect(keyPressCatcher, SIGNAL(set(int, int, bool)), this, SLOT(emitSet(int, int, bool)));
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
}
TableWidget::~TableWidget()
{
}
void TableWidget::emitSet(int ligne, int colonne, bool newAction)
{
    bool enabled = this->signalsBlocked();
    this->blockSignals(false);
    emit(set(ligne, colonne, newAction));
    this->blockSignals(enabled);
}
void TableWidget::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
    {
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    }
    this->setColumnCount(0);
}
void TableWidget::addColumn(int column, QString title)
{
    // Ajout d'une colonne
    this->insertColumn(column);
    // Création d'éléments
    for (int i = 0; i < this->rowCount(); i++)
        this->setItem(i, column, new QTableWidgetItem());
    // Modification du titre
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));
    // Ajout d'un élément couleur
    _listColors.insert(column, QColor(0, 0, 0));
}
void TableWidget::setID(EltID id, int colonne)
{
    char T[20];
    switch(id.typeElement)
    {
    case elementInstGen: case elementInst: strcpy(T, "Inst"); break;
    case elementInstSmplGen: case elementInstSmpl: strcpy(T, "InstSmpl"); break;
    case elementPrstGen: case elementPrst: strcpy(T, "Prst"); break;
    case elementPrstInstGen: case elementPrstInst: strcpy(T, "PrstInst"); break;
    default: break;
    }
    this->item(0, colonne)->setText(T);
    sprintf(T, "%d", id.indexSf2);
    this->item(1, colonne)->setText(T);
    sprintf(T, "%d", id.indexElt);
    this->item(2, colonne)->setText(T);
    sprintf(T, "%d", id.indexElt2);
    this->item(3, colonne)->setText(T);
}
EltID TableWidget::getID(int colonne)
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->columnCount() > colonne)
    {
        if (strcmp(this->item(0, colonne)->text().toStdString().c_str(), "Inst") == 0)
            id.typeElement = elementInst;
        else if (strcmp(this->item(0, colonne)->text().toStdString().c_str(), "InstSmpl") == 0)
            id.typeElement = elementInstSmpl;
        else if (strcmp(this->item(0, colonne)->text().toStdString().c_str(), "Prst") == 0)
            id.typeElement = elementPrst;
        else if (strcmp(this->item(0, colonne)->text().toStdString().c_str(), "PrstInst") == 0)
            id.typeElement = elementPrstInst;
        sscanf(this->item(1, colonne)->text().toStdString().c_str(), "%d", &id.indexSf2);
        sscanf(this->item(2, colonne)->text().toStdString().c_str(), "%d", &id.indexElt);
        sscanf(this->item(3, colonne)->text().toStdString().c_str(), "%d", &id.indexElt2);
        //sscanf(this->item(4, colonne)->text().toStdString().c_str(), "%d", &id.indexMod);
        id.indexMod = 0;
    }
    return id;
}
void TableWidget::setEnlighted(int colonne, bool isEnlighted)
{
    if (colonne >= this->columnCount())
        return;

    if (isEnlighted)
        _listColors[colonne] = QColor(70, 120, 210);
    else
        _listColors[colonne] = QColor(0, 0, 0);

    _timer->start(30);
}
void TableWidget::updateColors()
{
    int minChange = 40;
    bool toutPareil = true;
    for (int i = 0; i < this->columnCount(); i++)
    {
        if (this->horizontalHeaderItem(i))
        {
            QColor couleur1 = this->horizontalHeaderItem(i)->foreground().color();
            QColor couleur2 = _listColors.at(i);
            if (couleur1 != couleur2)
            {
                int deltaRouge = qMax(-minChange, qMin(minChange, couleur2.red() - couleur1.red()));
                int deltaVert = qMax(-minChange, qMin(minChange, couleur2.green() - couleur1.green()));
                int deltaBleu = qMax(-minChange, qMin(minChange, couleur2.blue() - couleur1.blue()));
                couleur1.setRed(couleur1.red() + deltaRouge);
                couleur1.setGreen(couleur1.green() + deltaVert);
                couleur1.setBlue(couleur1.blue() + deltaBleu);
                this->horizontalHeaderItem(i)->setForeground(couleur1);
            }
            if (couleur1 != couleur2)
                toutPareil = false;
        }
    }
    if (toutPareil)
        _timer->stop();
}
void TableWidget::setColumnCount(int columns)
{
    QTableWidget::setColumnCount(columns);
    _listColors.clear();
    for (int i = 0; i < columns; i++)
        _listColors << QColor(0, 0, 0);
}
void TableWidget::removeColumn(int column)
{
    QTableWidget::removeColumn(column);
    _listColors.removeAt(column);
}

//////////////////////////////////////////// TABLE WIDGET MOD ////////////////////////////////////////////

TableWidgetMod::TableWidgetMod(QWidget *parent) : QTableWidget(parent) {}
TableWidgetMod::~TableWidgetMod() {}
// Méthodes publiques
void TableWidgetMod::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
    {
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    }
    this->setRowCount(0);
}
void TableWidgetMod::addRow(int row)
{
    // Ajout d'une ligne
    this->insertRow(row);
    // Création d'éléments
    for (int i = 0; i < this->columnCount(); i++)
    {
        this->setItem(row, i, new QTableWidgetItem);
    }
}
void TableWidgetMod::setID(EltID id, int row)
{
    char T[20];
    switch(id.typeElement)
    {
    case elementInstMod: case elementInst: strcpy(T, "Inst"); break;
    case elementInstSmplMod: case elementInstSmpl: strcpy(T, "InstSmpl"); break;
    case elementPrstMod: case elementPrst: strcpy(T, "Prst"); break;
    case elementPrstInstMod: case elementPrstInst: strcpy(T, "PrstInst"); break;
    default: break;
    }
    this->item(row, 0)->setText(T);
    sprintf(T, "%d", id.indexSf2);
    this->item(row, 1)->setText(T);
    sprintf(T, "%d", id.indexElt);
    this->item(row, 2)->setText(T);
    sprintf(T, "%d", id.indexElt2);
    this->item(row, 3)->setText(T);
    sprintf(T, "%d", id.indexMod);
    this->item(row, 4)->setText(T);
}
EltID TableWidgetMod::getID(int row)
{
    EltID id;
    if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "Inst") == 0)
        id.typeElement = elementInstMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "InstSmpl") == 0)
        id.typeElement = elementInstSmplMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "Prst") == 0)
        id.typeElement = elementPrstMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "PrstInst") == 0)
        id.typeElement = elementPrstInstMod;
    sscanf(this->item(row, 1)->text().toStdString().c_str(), "%d", &id.indexSf2);
    sscanf(this->item(row, 2)->text().toStdString().c_str(), "%d", &id.indexElt);
    sscanf(this->item(row, 3)->text().toStdString().c_str(), "%d", &id.indexElt2);
    sscanf(this->item(row, 4)->text().toStdString().c_str(), "%d", &id.indexMod);
    return id;
}
EltID TableWidgetMod::getID()
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->selectedItems().count())
        id = getID(this->selectedItems().takeAt(0)->row());
    return id;
}
