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
****************************************************************************/

#include "page.h"
#include "pile_sf2.h"
#include "mainwindow.h"
#include <QScrollBar>


//////////////////////////////////////////// PAGE ////////////////////////////////////////////

Page::Page(QWidget *parent) : QWidget(parent)
{
}
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
        sprintf(T,"%d-%d", genVal.ranges.byLo, genVal.ranges.byHi); break;
    case champ_initialAttenuation:
        sprintf(T,"%.1f", (double)genVal.wAmount/10); break;
    case champ_pan:
        sprintf(T,"%.1f", (double)genVal.shAmount/10); break;
    case champ_sampleModes:
        sprintf(T,"%d", genVal.wAmount); break;
    case champ_overridingRootKey: case champ_scaleTuning:
        sprintf(T,"%d", genVal.wAmount); break;
    case champ_coarseTune: case champ_fineTune:
        sprintf(T,"%d", genVal.shAmount); break;
    case champ_initialFilterFc:
        sprintf(T,"%.0f", exp2((double)genVal.wAmount/1200)*8.176); break;
    case champ_initialFilterQ:
        sprintf(T,"%.1f", (double)genVal.wAmount/10); break;
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
        sprintf(T,"%d", genVal.shAmount); break;
    case champ_modLfoToVolume:
        sprintf(T,"%.1f", (double)genVal.shAmount/10); break;
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200)); break;
    case champ_sustainVolEnv: case champ_sustainModEnv:
        sprintf(T,"%.1f", (double)genVal.wAmount/10); break;
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
        sprintf(T,"%d", genVal.shAmount); break;
    case champ_freqModLFO: case champ_freqVibLFO:
        sprintf(T,"%.3f", exp2((double)genVal.shAmount/1200)*8.176); break;
    case champ_exclusiveClass:
        sprintf(T,"%d", genVal.wAmount); break;
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        sprintf(T,"%.1f", (float)genVal.wAmount/10); break;
    case champ_keynum:
        sprintf(T,"%d", genVal.wAmount); break;
    case champ_velocity:
        sprintf(T,"%d", genVal.wAmount); break;
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
        sprintf(T,"%d", genVal.shAmount); break;
    case champ_sfModDestOper:
        sprintf(T,"%d", genVal.wAmount); break;
        break;
    case champ_sfModTransOper:
        switch (genVal.wAmount)
        {
        case 2: strcpy(T, tr(", valeur absolue").toStdString().c_str()); break;
        default: T[0] = '\0';
        }
        break;
    case champ_modAmount:
        sprintf(T,"%d", genVal.shAmount); break;
        break;
    case champ_indexMod:
        sprintf(T,"%d", genVal.wAmount+1); break;
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
QString Page::getGenName(WORD iVal)
{
    QString qStr = "";
    switch (iVal)
    {
    case 0: qStr = "Sample: start offset"; break;
    case 1: qStr = "Sample: end offset"; break;
    case 2: qStr = "Sample: loop start offset"; break;
    case 3: qStr = "Sample: loop end offset"; break;
    case 4: qStr = "Sample: start offset (coarse)"; break;
    case 5: qStr = "Modulation LFO: to pitch"; break;
    case 6: qStr = "Vibrato LFO: to pitch"; break;
    case 7: qStr = "Modulation envelope: to pitch"; break;
    case 8: qStr = "Pitch/Effects: filter cutoff"; break;
    case 9: qStr = "Pitch/Effects: filter Q"; break;
    case 10: qStr = "Modulation LFO: to filter cutoff"; break;
    case 11: qStr = "Modulation envelope: to filter cutoff"; break;
    case 12: qStr = "Sample: end offset (coarse)"; break;
    case 13: qStr = "Modulation LFO: to volume"; break;
    case 15: qStr = "Pitch/Effects: chorus"; break;
    case 16: qStr = "Pitch/Effects: reverb"; break;
    case 17: qStr = "Pitch/Effects: pan"; break;
    case 21: qStr = "Modulation LFO: delay"; break;
    case 22: qStr = "Modulation LFO: frequency"; break;
    case 23: qStr = "Vibrato LFO: delay"; break;
    case 24: qStr = "Vibrato LFO: frequency"; break;
    case 25: qStr = "Modulation envelope: delay"; break;
    case 26: qStr = "Modulation envelope: attack"; break;
    case 27: qStr = "Modulation envelope: hold"; break;
    case 28: qStr = "Modulation envelope: decay"; break;
    case 29: qStr = "Modulation envelope: sustain"; break;
    case 30: qStr = "Modulation envelope: release"; break;
    case 31: qStr = "Modulation envelope: note to hold"; break;
    case 32: qStr = "Modulation envelope: note to decay"; break;
    case 33: qStr = "Volume envelope: delay"; break;
    case 34: qStr = "Volume envelope: attack"; break;
    case 35: qStr = "Volume envelope: hold"; break;
    case 36: qStr = "Volume envelope: decay"; break;
    case 37: qStr = "Volume envelope: sustain"; break;
    case 38: qStr = "Volume envelope: release"; break;
    case 39: qStr = "Volume envelope: note to hold"; break;
    case 40: qStr = "Volume envelope: note to decay"; break;
    case 43: qStr = "Key range"; break;
    case 44: qStr = "Velocity range"; break;
    case 45: qStr = "Sample: loop start offset (coarse)"; break;
    case 46: qStr = "Pitch/Effects: forced midi note"; break;
    case 47: qStr = "Volume envelope: forced midi velocity"; break;
    case 48: qStr = "Volume envelope: attenuation"; break;
    case 50: qStr = "Sample: loop end offset (coarse)"; break;
    case 51: qStr = "Pitch/Effects: tune (coarse)"; break;
    case 52: qStr = "Pitch/Effects: tune (fine)"; break;
    case 54: qStr = "Volume envelope: sample mode"; break;
    case 56: qStr = "Pitch/Effects: scale tuning"; break;
    case 57: qStr = "Pitch/Effects: exclusive class"; break;
    case 58: qStr = "Pitch/Effects: root note"; break;
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
        char T[11];
        strcpy(T, texte.toStdString().c_str());
        int val1, val2, val3;
        if (sscanf(T, "%d-%d", &val1, &val2) != 2)
            ok = 0;
        else
        {
            if (val1 > val2)
            {
                val3 = val1;
                val1 = val2;
                val2 = val3;
            }
            genAmount.ranges.byLo = limit(val1, 0, 127);
            genAmount.ranges.byHi = limit(val2, 0, 127);
        }
        }; break;
    case champ_initialAttenuation: genAmount.wAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1440);
        break;
    case champ_pan: genAmount.shAmount = (short)limit(10*texte.toDouble(&ok), -500, 500);
        break;
    case champ_sampleModes:
        iTmp = texte.toDouble(&ok);
        if (iTmp != 0 && iTmp != 1 && iTmp != 3) iTmp = 0;
        genAmount.wAmount = (WORD)iTmp;
        break;
    case champ_overridingRootKey: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 0, 127);
        break;
    case champ_coarseTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -120, 120);
        break;
    case champ_fineTune: genAmount.shAmount = (short)limit(texte.toDouble(&ok), -99, 99);
        break;
    case champ_scaleTuning: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 0, 99);
        break;
    case champ_initialFilterFc:
        genAmount.wAmount = (WORD)limit(1200*log2(texte.toDouble(&ok)/8.176), 1500, 13500);
        break;
    case champ_initialFilterQ: genAmount.wAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 960);
        break;
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
        genAmount.shAmount = (short)limit(texte.toDouble(&ok), -12000, 12000);
        break;
    case champ_modLfoToVolume: genAmount.shAmount = (short)limit(10*texte.toDouble(&ok), -960, 960);
        break;
    case champ_delayModEnv: case champ_holdModEnv:
    case champ_delayVolEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)), -12000, 5000);
        break;
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
        genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)), -12000, 8000);
        break;
    case champ_sustainVolEnv: case champ_sustainModEnv:
        genAmount.wAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1000);
        break;
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
        genAmount.shAmount = (short)limit(texte.toDouble(&ok), -1200, 1200);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        genAmount.shAmount = (short)limit(1200*log2(texte.toDouble(&ok)/8.176), -16000, 4500);
        break;
    case champ_exclusiveClass: genAmount.wAmount = (WORD)limit(texte.toDouble(&ok), 1, 127);
        break;
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        genAmount.wAmount = (WORD)limit(10*texte.toDouble(&ok), 0, 1000);
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
int Page::limit(int iTmp, int min, int max)
{
    if (iTmp < min)
        return min;
    else if (iTmp > max)
        return max;
    else
        return iTmp;
}


//////////////////////////////////////////// PAGE TABLE ////////////////////////////////////////////

PageTable::PageTable(QWidget *parent) : Page(parent)
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
    this->table->addColumn(0, tr("Global"));
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
            if (this->contenant == elementInst)
            {
                id3.indexElt = this->sf2->get(id, champ_sampleID).wValue;
                sprintf(str, "%.03d%s", this->sf2->get(id, champ_keyRange).rValue.byLo, \
                        this->sf2->getQstr(id3, champ_name).toStdString().c_str());
                qStr = this->sf2->getQstr(id3, champ_name).left(10);
                qStr.append("\n");
                qStr.append(this->sf2->getQstr(id3, champ_name).mid(10).left(10));
                for (int j = 1; j < nbSmplInst+1; j++)
                {
                    // note basse de la colonne, et prise en compte du nom du sample lié
                    id3.indexElt = this->sf2->get(this->table->getID(j), champ_sampleID).wValue;
                    sprintf(str2, "%.03d%s", this->sf2->get(this->table->getID(j), champ_keyRange).rValue.byLo, \
                            this->sf2->getQstr(id3, champ_name).toStdString().c_str());
                    if (strcmp(str, str2) > 0)
                        numCol++;
                }
            }
            else
            {
                id3.indexElt = this->sf2->get(id, champ_instrument).wValue;
                qStr = this->sf2->getQstr(id3, champ_name);
                for (int j = 1; j < nbSmplInst+1; j++)
                {
                    if (qStr.toLower().compare(this->table->horizontalHeaderItem(j)->text().toLower()) > 1)
                        numCol++;
                }
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
    if (this->contenant == elementInst)
    {
        for (int i = 0; i < this->table->columnCount(); i++)
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
        for (int i = 0; i < this->table->columnCount(); i++)
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
                    sprintf(T, "%s: #%d", tr("Modulateur").toStdString().c_str(), iVal+1);
                    qStr = T;
                }
                else
                    qStr = tr("Lien (invalide)");
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
                sprintf(T, "%s: #%d", tr("Modulateur").toStdString().c_str(), genValTmp.wAmount - 32767);
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

void PageTable::set(int ligne, int colonne, bool newAction)
{
    if (this->preparation) return;
    // modification d'un élément du tableau
    EltID id = this->table->getID(colonne);
    Champ champ = this->table->getChamp(ligne);
    QString texte = this->table->item(ligne, colonne)->text();
    char T[20];
    int offset;
    if (texte.isEmpty())
    {
        // Effacement d'un paramètre ?
        switch ((int)champ)
        {
        case champ_startAddrsOffset:
            if (this->sf2->isSet(id, champ_startAddrsOffset) || this->sf2->isSet(id, champ_startAddrsCoarseOffset))
            {
                // On efface la donnée
                if (newAction) this->sf2->prepareNewActions();
                id = this->table->getID(colonne);
                this->sf2->reset(id, champ_startAddrsOffset);
                this->sf2->reset(id, champ_startAddrsCoarseOffset);
                // Mise à jour fenêtre
                this->mainWindow->updateDo();
                // Redimensionnement du tableau
                this->table->resizeColumnsToContents();
            }
            break;
        case champ_endAddrsOffset:
            if (this->sf2->isSet(id, champ_endAddrsOffset) || this->sf2->isSet(id, champ_endAddrsCoarseOffset))
            {
                // On efface la donnée
                if (newAction) this->sf2->prepareNewActions();
                id = this->table->getID(colonne);
                this->sf2->reset(id, champ_endAddrsOffset);
                this->sf2->reset(id, champ_endAddrsCoarseOffset);
                // Mise à jour fenêtre
                this->mainWindow->updateDo();
                // Redimensionnement du tableau
                this->table->resizeColumnsToContents();
            }
            break;
        case champ_startloopAddrsOffset:
            if (this->sf2->isSet(id, champ_startloopAddrsOffset) || this->sf2->isSet(id, champ_startloopAddrsCoarseOffset))
            {
                // On efface la donnée
                if (newAction) this->sf2->prepareNewActions();
                id = this->table->getID(colonne);
                this->sf2->reset(id, champ_startloopAddrsOffset);
                this->sf2->reset(id, champ_startloopAddrsCoarseOffset);
                // Mise à jour fenêtre
                this->mainWindow->updateDo();
                // Redimensionnement du tableau
                this->table->resizeColumnsToContents();
            }
            break;
        case champ_endloopAddrsOffset:
            if (this->sf2->isSet(id, champ_endloopAddrsOffset) || this->sf2->isSet(id, champ_endloopAddrsCoarseOffset))
            {
                // On efface la donnée
                if (newAction) this->sf2->prepareNewActions();
                id = this->table->getID(colonne);
                this->sf2->reset(id, champ_endloopAddrsOffset);
                this->sf2->reset(id, champ_endloopAddrsCoarseOffset);
                // Mise à jour fenêtre
                this->mainWindow->updateDo();
                // Redimensionnement du tableau
                this->table->resizeColumnsToContents();
            }
            break;
        default:
            if (this->sf2->isSet(id, champ))
            {
                // On efface la donnée
                if (newAction) this->sf2->prepareNewActions();
                id = this->table->getID(colonne);
                this->sf2->reset(id, champ);
                // Mise à jour fenêtre
                this->mainWindow->updateDo();
                // Redimensionnement du tableau
                this->table->resizeColumnsToContents();
            }
        }
    }
    else
    {
        texte = texte.left(9);
        bool ok;
        genAmountType genAmount = getValue(texte, champ, ok);
        if (ok)
        {
            this->preparation = 1;
            int iVal;
            genAmountType genAmount2;
            switch ((int)champ)
            {
            case champ_startAddrsOffset:
                genAmount2 = getValue(texte, champ_startAddrsCoarseOffset, ok);
                iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ_startAddrsOffset, id);
                genAmount2.shAmount = iVal / 32768;
                genAmount.shAmount = iVal % 32768;
                if (genAmount.shAmount != this->sf2->get(id, champ_startAddrsOffset).shValue || \
                    genAmount2.shAmount != this->sf2->get(id, champ_startAddrsCoarseOffset).shValue)
                {
                    // Modification du sf2
                    if (newAction) this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ_startAddrsOffset, value);
                    value.genValue = genAmount2;
                    this->sf2->set(id, champ_startAddrsCoarseOffset, value);
                    // Mise à jour fenêtre
                    this->mainWindow->updateDo();
                    // Redimensionnement du tableau
                    this->table->resizeColumnsToContents();
                }
                // Mise à jour de la valeur dans la cellule
                offset = this->sf2->get(id, champ_startAddrsOffset).genValue.shAmount + \
                        32768 * this->sf2->get(id, champ_startAddrsCoarseOffset).genValue.shAmount;
                this->table->item(ligne, colonne)->setText(getTextValue(T, champ, offset));
                break;
            case champ_endAddrsOffset:
                genAmount2 = getValue(texte, champ_endAddrsCoarseOffset, ok);
                iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ_endAddrsOffset, id);
                genAmount2.shAmount = iVal / 32768;
                genAmount.shAmount = iVal % 32768;
                if (genAmount.shAmount != this->sf2->get(id, champ_endAddrsOffset).shValue || \
                    genAmount2.shAmount != this->sf2->get(id, champ_endAddrsCoarseOffset).shValue)
                {
                    // Modification du sf2
                    if (newAction) this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ_endAddrsOffset, value);
                    value.genValue = genAmount2;
                    this->sf2->set(id, champ_endAddrsCoarseOffset, value);
                    // Mise à jour fenêtre
                    this->mainWindow->updateDo();
                    // Redimensionnement du tableau
                    this->table->resizeColumnsToContents();
                }
                // Mise à jour de la valeur dans la cellule
                offset = this->sf2->get(id, champ_endAddrsOffset).genValue.shAmount + \
                        32768 * this->sf2->get(id, champ_endAddrsCoarseOffset).genValue.shAmount;
                this->table->item(ligne, colonne)->setText(getTextValue(T, champ, offset));
                break;
            case champ_startloopAddrsOffset:
                genAmount2 = getValue(texte, champ_startloopAddrsCoarseOffset, ok);
                iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ_startloopAddrsOffset, id);
                genAmount2.shAmount = iVal / 32768;
                genAmount.shAmount = iVal % 32768;
                if (genAmount.shAmount != this->sf2->get(id, champ_startloopAddrsOffset).shValue || \
                    genAmount2.shAmount != this->sf2->get(id, champ_startloopAddrsCoarseOffset).shValue)
                {
                    // Modification du sf2
                    if (newAction) this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ_startloopAddrsOffset, value);
                    value.genValue = genAmount2;
                    this->sf2->set(id, champ_startloopAddrsCoarseOffset, value);
                    // Mise à jour fenêtre
                    this->mainWindow->updateDo();
                    // Redimensionnement du tableau
                    this->table->resizeColumnsToContents();
                }
                // Mise à jour de la valeur dans la cellule
                offset = this->sf2->get(id, champ_startloopAddrsOffset).genValue.shAmount + \
                        32768 * this->sf2->get(id, champ_startloopAddrsCoarseOffset).genValue.shAmount;
                this->table->item(ligne, colonne)->setText(getTextValue(T, champ, offset));
                break;
            case champ_endloopAddrsOffset:
                genAmount2 = getValue(texte, champ_endloopAddrsCoarseOffset, ok);
                iVal = limit(32768 * genAmount2.shAmount + genAmount.shAmount, champ_endloopAddrsOffset, id);
                genAmount2.shAmount = iVal / 32768;
                genAmount.shAmount = iVal % 32768;
                if (genAmount.shAmount != this->sf2->get(id, champ_endloopAddrsOffset).shValue || \
                    genAmount2.shAmount != this->sf2->get(id, champ_endloopAddrsCoarseOffset).shValue)
                {
                    // Modification du sf2
                    if (newAction) this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ_endloopAddrsOffset, value);
                    value.genValue = genAmount2;
                    this->sf2->set(id, champ_endloopAddrsCoarseOffset, value);
                    // Mise à jour fenêtre
                    this->mainWindow->updateDo();
                    // Redimensionnement du tableau
                    this->table->resizeColumnsToContents();
                }
                // Mise à jour de la valeur dans la cellule
                offset = this->sf2->get(id, champ_endloopAddrsOffset).genValue.shAmount + \
                        32768 * this->sf2->get(id, champ_endloopAddrsCoarseOffset).genValue.shAmount;
                this->table->item(ligne, colonne)->setText(getTextValue(T, champ, offset));
                break;
            default:
                if (genAmount.wAmount != this->sf2->get(id, champ).wValue || !this->sf2->isSet(id, champ))
                {
                    // Modification du sf2
                    if (newAction) this->sf2->prepareNewActions();
                    id = this->table->getID(colonne);
                    Valeur value;
                    value.genValue = genAmount;
                    this->sf2->set(id, champ, value);
                    // Mise à jour fenêtre
                    this->mainWindow->updateDo();
                    // Redimensionnement du tableau
                    this->table->resizeColumnsToContents();
                }
                // Mise à jour de la valeur dans la cellule
                this->table->item(ligne, colonne)->setText(getTextValue(T, champ, genAmount));
            }
            this->preparation = 0;
        }
        else
        {
            // On restaure le texte précédent
            this->preparation = 1;
            switch (champ)
            {
            case champ_startAddrsOffset:
                if (this->sf2->isSet(id, champ_startAddrsOffset) || this->sf2->isSet(id, champ_startAddrsCoarseOffset))
                {
                    offset = this->sf2->get(id, champ_startAddrsOffset).genValue.shAmount + \
                            32768 * this->sf2->get(id, champ_startAddrsCoarseOffset).genValue.shAmount;
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ_startAddrsOffset, offset));
                }
                else this->table->item(ligne, colonne)->setText("");
                break;
            case champ_endAddrsOffset:
                if (this->sf2->isSet(id, champ_endAddrsOffset) || this->sf2->isSet(id, champ_endAddrsCoarseOffset))
                {
                    offset = this->sf2->get(id, champ_endAddrsOffset).genValue.shAmount + \
                            32768 * this->sf2->get(id, champ_endAddrsCoarseOffset).genValue.shAmount;
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ_endAddrsOffset, offset));
                }
                else this->table->item(ligne, colonne)->setText("");
                break;
            case champ_startloopAddrsOffset:
                if (this->sf2->isSet(id, champ_startloopAddrsOffset) || this->sf2->isSet(id, champ_startloopAddrsCoarseOffset))
                {
                    offset = this->sf2->get(id, champ_startloopAddrsOffset).genValue.shAmount + \
                            32768 * this->sf2->get(id, champ_startloopAddrsCoarseOffset).genValue.shAmount;
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ_startloopAddrsOffset, offset));
                }
                else this->table->item(ligne, colonne)->setText("");
                break;
            case champ_endloopAddrsOffset:
                if (this->sf2->isSet(id, champ_endloopAddrsOffset) || this->sf2->isSet(id, champ_endloopAddrsCoarseOffset))
                {
                    offset = this->sf2->get(id, champ_endloopAddrsOffset).genValue.shAmount + \
                            32768 * this->sf2->get(id, champ_endloopAddrsCoarseOffset).genValue.shAmount;
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ_endloopAddrsOffset, offset));
                }
                else this->table->item(ligne, colonne)->setText("");
                break;
            default:
                if (this->sf2->isSet(id, champ))
                    this->table->item(ligne, colonne)->setText(getTextValue(T, champ, this->sf2->get(id, champ).genValue));
                else this->table->item(ligne, colonne)->setText("");
            }
            this->preparation = 0;
        }
    }
    this->table->resizeColumnsToContents();
    // Mise à jour partie mod (car entre 2 des mod peuvent être définitivement détruits, et les index peuvent être mis à jour)
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
        val.wValue = this->sf2->get(id2, champ_sfModDestOper).wValue;
        if (val.wValue != index)
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
                        val2.wValue = 0;
                        this->sf2->set(id4, champ_sfModDestOper, val2);
                    }
                } while (id4.indexMod != -1);
                val2.sfModValue = this->sf2->get(id3, champ_sfModSrcOper).sfModValue;
                val2.sfModValue.Index = 127;
                val2.sfModValue.CC = 0;
                this->sf2->set(id3, champ_sfModSrcOper, val2);
            }
            val.wValue = index;
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
    this->tree->selectNone();
    int compte = this->table->selectedItems().count();
    int colonne;
    for (int i = 0; i < compte; i++)
    {
        colonne = this->table->selectedItems().takeAt(i)->column();
        this->tree->select(this->table->getID(colonne));
    }
    // Mise à jour des informations sur les mods
    if (compte)
    {
        this->preparation = 1;
        colonne = this->table->selectedItems().takeAt(compte-1)->column();
        this->afficheMod(this->table->getID(colonne));
        this->preparation = 0;
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
                sprintf(T, "%s: #%d", tr("Modulateur").toStdString().c_str(), id2.indexMod+1);
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
                sprintf(T, "%s: #%d", tr("Modulateur").toStdString().c_str(), id2.indexMod+1);
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


//////////////////////////////////////////// TABLE WIDGET ////////////////////////////////////////////

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{
    KeyPressCatcher * keyPressCatcher = new KeyPressCatcher(this);
    this->installEventFilter(keyPressCatcher);
    connect(keyPressCatcher, SIGNAL(set(int, int, bool)), this, SLOT(emitSet(int,int,bool)));
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
        this->setItem(i, column, new QTableWidgetItem);
    // Modification du titre
    this->setHorizontalHeaderItem(column, new QTableWidgetItem(title));
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
    EltID id;
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
    return id;
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
    EltID id = {elementUnknown, 0, 0, 0, 0};
    if (this->selectedItems().count())
        id = getID(this->selectedItems().takeAt(0)->row());
    return id;
}
