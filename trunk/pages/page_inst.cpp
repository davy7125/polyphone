/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#include "page_inst.h"
#include "mainwindow.h"
#include "ui_page_inst.h"
#include "dialog_mixture.h"
#include "dialog_release.h"
#include "dialog_celeste.h"
#include <QProgressDialog>
#include <QInputDialog>


// Constructeur, destructeur
Page_Inst::Page_Inst(QWidget *parent) :
    PageTable(PAGE_INST, parent), ui(new Ui::Page_Inst)
{
    ui->setupUi(this);
    this->contenant = elementInst;
    this->contenantGen = elementInstGen;
    this->contenantMod = elementInstMod;
    this->contenu = elementSmpl;
    this->lien = elementInstSmpl;
    this->lienGen = elementInstSmplGen;
    this->lienMod = elementInstSmplMod;
    this->table = this->ui->tableInst;
    this->tableMod = this->ui->tableMod;
    this->spinAmount = this->ui->spinSource2;
    this->checkAbs = this->ui->checkAbs;
    this->pushNouveauMod = this->ui->pushNouveauMod;
    this->pushSupprimerMod = this->ui->pushSupprimerMod;
    this->comboSource1Courbe = this->ui->comboCourbeSource1;
    this->comboSource2Courbe = this->ui->comboCourbeSource2;
    this->comboSource1 = this->ui->comboSource1;
    this->comboSource2 = this->ui->comboSource2;
    this->comboDestination = this->ui->comboDestination;

    // Remplissage de comboDestination
    for (int i = 0; i < 48; i++)
        this->comboDestination->addItem(getGenName(this->getDestNumber(i)));
    this->comboDestination->setLimite(48);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

#ifdef Q_OS_MAC
    this->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = this->table->font();
    font.setPixelSize(10);
    this->table->setFont(font);
#endif
}
Page_Inst::~Page_Inst()
{
    delete ui;
}
void Page_Inst::setModVisible(bool visible)
{
    this->ui->frameModulator->setVisible(visible);
}
void Page_Inst::afficher()
{
    PageTable::afficher();
    EltID id = this->tree->getID(0);
    id.typeElement = elementInst;
    // Liste des presets qui utilisent l'instrument
    int nbPrst = 0;
    bool isFound;
    int j;
    QString qStr = "";
    EltID id2 = id;
    id2.typeElement = elementPrst;
    EltID id3 = id;
    id3.typeElement = elementPrstInst;
    // Parcours de tous les presets
    for (int i = 0; i < this->sf2->count(id2); i++)
    {
        id2.indexElt = i;
        id3.indexElt = i;
        // Parcours de tous les instruments liés au preset
        isFound = false;
        j = 0;
        while(j < this->sf2->count(id3) && !isFound)
        {
            id3.indexElt2 = j;
            if (!this->sf2->get(id3, champ_hidden).bValue)
            {
                if (this->sf2->get(id3, champ_instrument).wValue == id.indexElt)
                {
                    // Ajout d'un preset
                    if (nbPrst)
                        qStr.append(", ");
                    qStr.append(this->sf2->getQstr(id2, champ_name).toStdString().c_str());
                    nbPrst++;
                    isFound = true;
                }
            }
            j++;
        }
    }
    if (nbPrst == 0)
        qStr = trUtf8("<b>Instrument lié à aucun preset.</b>");
    else if (nbPrst == 1)
        qStr.prepend(trUtf8("<b>Instrument lié au preset : </b>"));
    else
        qStr.prepend(trUtf8("<b>Instrument lié aux presets : </b>"));
    this->ui->labelPrst->setText(qStr);
}

// Outils instrument
void Page_Inst::desaccorder()
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    if (this->sf2->count(id, false) == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        return;
    }
    DialogCeleste * dialogCeleste = new DialogCeleste(this);
    dialogCeleste->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogCeleste, SIGNAL(accepted(double,double)),
                  SLOT(desaccorder(double, double)));
    dialogCeleste->show();
}
void Page_Inst::desaccorder(double doHerz, double division)
{
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    EltID id = this->tree->getID(0);
    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié
            EltID id2 = id;
            id2.typeElement = elementSmpl;
            id2.indexElt = this->sf2->get(id, champ_sampleID).genValue.wAmount;
            // Numéro de la note du sample
            int numNoteSmpl = this->sf2->get(id2, champ_byOriginalPitch).bValue;
            // Note de base sur le clavier correspondant à numNoteSmpl;
            int numBase = numNoteSmpl;
            if (this->sf2->isSet(id, champ_overridingRootKey))
                numBase = this->sf2->get(id, champ_overridingRootKey).wValue;
            // Etendue du sample sur le clavier
            int numBas = 0;
            int numHaut = 0;
            // Intervalle joué par le sample
            if (this->sf2->isSet(id, champ_keynum))
            {
                numBas = numNoteSmpl + this->sf2->get(id, champ_keynum).wValue - numBase;
                numHaut = numBas;
            }
            else
            {
                numBas = numNoteSmpl + this->sf2->get(id, champ_keyRange).rValue.byLo - numBase;
                numHaut = numNoteSmpl + this->sf2->get(id, champ_keyRange).rValue.byHi - numBase;
            }
            // Fréquence des battements
            double keyMoy = (double)(this->sf2->get(id, champ_keyRange).rValue.byHi +
                                     this->sf2->get(id, champ_keyRange).rValue.byLo) / 2.;
            double bps = doHerz * qPow(division, (60. - keyMoy) / 12.);
            if (bps < -30)
                bps = -30;
            else if (bps > 30)
                bps = 30;
            // Note moyenne
            double noteMoy = (double)(numBas + numHaut) / 2;
            // Calcul du désaccordage, passage en frequence
            double freqMoy = qPow(2., (noteMoy + 36.3763) / 12);
            // Ajout du désaccordage
            // - octave ondulante : division par 2 de bps
            // - diminution désaccordage vers les graves
            // - accentuation désaccordage vers les aigus
            double freqMod = freqMoy + 1.2 * qPow(2., (noteMoy - 60)/30) * bps / 2;
            // Retour en pitch
            double noteMod = 12 * qLn(freqMod) / 0.69314718056 - 36.3763;
            // Décalage
            int decalage = ceil(100*(noteMod - noteMoy)-0.5);
            if (bps > 0)
            {
                if (decalage < 1) decalage = 1;
                else if (decalage > 50) decalage = 50;
            }
            else if (bps < 0)
            {
                if (decalage < -50) decalage = -50;
                else if (decalage > -1) decalage = -1;
            }
            // Modification instSmpl
            if (this->sf2->get(id, champ_fineTune).shValue != decalage)
            {
                Valeur val;
                val.shValue = decalage;
                this->sf2->set(id, champ_fineTune, val);
            }
        }
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}
void Page_Inst::repartitionAuto()
{
    EltID id = this->tree->getID(0);
    id.typeElement = elementInstSmpl;
    if (this->sf2->count(id, false) == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        return;
    }
    // Répartition automatique des notes sur le clavier
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    id = this->tree->getID(0);
    // Liste des samples liés avec note de base
    QList<EltID> listID;
    QList<int> listNote;
    int notePrecedente, note, noteSuivante;
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            if (this->sf2->isSet(id, champ_overridingRootKey))
                note = this->sf2->get(id, champ_overridingRootKey).wValue;
            else
            {
                EltID id2 = id;
                id2.typeElement = elementSmpl;
                id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
                note = this->sf2->get(id2, champ_byOriginalPitch).bValue;
            }
            // Insertion triée
            int pos = 0;
            for (int j = 0; j < listID.size(); j++)
                if (note > listNote.at(j)) pos++;
            listID.insert(pos, id);
            listNote.insert(pos, note);
        }
    }

    // Répartition
    Valeur val;
    noteSuivante = -1;
    notePrecedente = -1;
    for (int i = 0; i < listID.size(); i++)
    {
        // Note actuelle
        note = listNote.at(i);
        if (note != notePrecedente)
        {
            // Recherche de la note suivante
            noteSuivante = -1;
            for (int j = i+1; j < listID.size(); j++)
            {
                if (noteSuivante == -1 && listNote.at(i) != listNote.at(j))
                    noteSuivante = listNote.at(j);
            }

            // borne inférieure de la division
            if (notePrecedente == -1)
                val.rValue.byLo = 0;
            else
                val.rValue.byLo = val.rValue.byHi + 1;
            if (val.rValue.byLo > 127)
                val.rValue.byLo = 127;

            // borne supérieure de la division
            if (noteSuivante == -1)
                val.rValue.byHi = 127;
            else
            {
                val.rValue.byHi = qMin(note + (127 - note) / 20,
                                       (int)floor((double)(2 * note + noteSuivante) / 3));
                if (val.rValue.byLo > val.rValue.byHi)
                    val.rValue.byHi = val.rValue.byLo;
            }
            notePrecedente = note;
        }
        // Modification
        if (this->sf2->get(listID.at(i), champ_keyRange).rValue.byLo != val.rValue.byLo ||
            this->sf2->get(listID.at(i), champ_keyRange).rValue.byHi != val.rValue.byHi)
            this->sf2->set(listID.at(i), champ_keyRange, val);
    }
    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}

void Page_Inst::mixture()
{
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInstSmpl;
    if (this->sf2->count(idInst, false) == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        return;
    }
    DialogMixture * dialogMixture = new DialogMixture(this);
    dialogMixture->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogMixture, SIGNAL(accepted(QList<QList<int> >, QString, bool, int, bool)),
                  SLOT(mixture(QList<QList<int> >, QString, bool, int, bool)));
    dialogMixture->show();
}
void Page_Inst::mixture(QList<QList<int> > listeParam, QString nomInst, bool bouclage, int freq, bool stereo)
{
    // Création d'une mixture
    this->sf2->prepareNewActions();

    // Nombre d'étapes
    int nbEtapes = 0;
    for (int i = 0; i < listeParam.length(); i++)
    {
        int nbNotes = (qAbs(listeParam.at(i).at(0) - listeParam.at(i).at(1))) / freq + 1;
        nbEtapes += nbNotes * (listeParam.at(i).length() + 2 * bouclage);
    }
    if (!stereo)
        nbEtapes /= 2;

    // Ouverture d'une barre de progression
    QString textProgress = trUtf8("Création ");
    QProgressDialog progress("", trUtf8("Annuler"), 0, nbEtapes, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedWidth(350);
    progress.show();
    double dureeSmpl = 7;
    qint32 fEch = 48000;
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInst;
    EltID idSmpl;
    QByteArray baData;

    // Création d'un nouvel instrument
    EltID idNewInst = idInst;
    idNewInst.indexElt = this->sf2->add(idNewInst);

    // Configuration instrument
    if (nomInst.isEmpty())
        nomInst = trUtf8("sans nom");
    this->sf2->set(idNewInst, champ_name, nomInst.left(20));
    if (bouclage)
    {
        Valeur value;
        value.wValue = 1;
        this->sf2->set(idNewInst, champ_sampleModes, value);
    }
    EltID idInstSmpl = idNewInst;
    idInstSmpl.typeElement = elementInstSmpl;

    // Création de samples et ajout dans l'instrument
    for (int numDiv = 0; numDiv < listeParam.length(); numDiv++)
    {
        QList<int> listRangs = listeParam.at(numDiv);

        // Etendue de note
        int noteStart2 = qMin(listRangs[0], listRangs[1]);
        int noteEnd = qMax(listRangs[0], listRangs[1]);
        int noteStart = noteStart2 + (noteEnd - noteStart2) % freq;
        listRangs.takeAt(0);
        listRangs.takeAt(0);

        // Pour chaque note
        for (int note = noteStart; note <= noteEnd; note += freq)
        {
            // Pour chaque côté
            for (int cote = 0; cote < 1 + stereo; cote++)
            {
                QString name;
                if (stereo)
                    name = nomInst.left(15);
                else
                    name = nomInst.left(16);
                char str2[20];
                sprintf(str2,"%.3d", note);
                if (stereo)
                {
                    if (cote == 0)
                        name = name + ' ' + str2 + 'R';
                    else
                        name = name + ' ' + str2 + 'L';
                }
                else
                    name = name + ' ' + str2;
                progress.setLabelText(textProgress + name);
                QApplication::processEvents();
                baData.resize(dureeSmpl*fEch*4);
                baData.fill(0);

                // Calcul de l'atténuation mini de tous les rangs
                double attMini = 1000000;
                for (int numRang = 0; numRang < listRangs.length()/2; numRang++)
                {
                    double noteTmp = (double)note + getOffset(listRangs.at(2*numRang), listRangs.at(2*numRang+1));
                    double ecart;
                    EltID idInstSmplTmp;
                    idSmpl = closestSample(idInst, noteTmp, ecart, cote, idInstSmplTmp);
                    double attenuation = 0;
                    if (sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        attenuation = (double)sf2->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0;
                    if (attenuation < attMini)
                        attMini = attenuation;
                }

                // Pour chaque rang
                for (int numRang = 0; numRang < listRangs.length()/2; numRang++)
                {
                    if (!progress.wasCanceled())
                        progress.setValue(progress.value() + 1);
                    else
                    {
                        // Actualisation et retour
                        this->mainWindow->updateDo();
                        return;
                    }

                    // Calcul de la note à ajouter à la mixture
                    double noteTmp = (double)note + getOffset(listRangs.at(2*numRang), listRangs.at(2*numRang+1));
                    if (noteTmp <= 120)
                    {
                        // Sample le plus proche et écart associé
                        double ecart;
                        EltID idInstSmplTmp;
                        idSmpl = closestSample(idInst, noteTmp, ecart, cote, idInstSmplTmp);
//                        printf("touche %d, note cherchee %.2f, sample %s, instsmpl %d-%d\n",
//                               note, noteTmp, sf2->getQstr(idSmpl, champ_name).toStdString().c_str(),
//                               sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo,
//                               sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi);
                        // Fréquence d'échantillonnage initiale fictive (pour accordage)
                        double fEchInit = (double)this->sf2->get(idSmpl, champ_dwSampleRate).dwValue * pow(2, ecart/12.0);
                        // Récupération du son
                        QByteArray baDataTmp = getSampleData(idSmpl, dureeSmpl * fEchInit);
                        // Prise en compte atténuation en dB
                        double attenuation = 1;
                        if (sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        {
                            attenuation = (double)sf2->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0 - attMini;
                            attenuation = pow(10, -attenuation / 20.0);
                        }
                        // Rééchantillonnage
                        baDataTmp = Sound::resampleMono(baDataTmp, fEchInit, fEch, 32);
                        // Ajout du son
                        baData = addSampleData(baData, baDataTmp, attenuation);
                    }
                }
                if (!progress.wasCanceled())
                    progress.setValue(progress.value() + 1);
                else
                {
                    // Actualisation et retour
                    this->mainWindow->updateDo();
                    return;
                }
                qint32 loopStart = 0;
                qint32 loopEnd = 0;

                // Bouclage du sample
                if (bouclage)
                {
                    QByteArray baData2 = Sound::bouclage(baData, fEch, loopStart, loopEnd, 32);
                    if (!baData2.isEmpty())
                        baData = baData2;
                    if (!progress.wasCanceled())
                        progress.setValue(progress.value() + 1);
                    else
                    {
                        // Actualisation et retour
                        this->mainWindow->updateDo();
                        return;
                    }
                }

                // Création d'un nouveau sample
                idSmpl.indexElt = this->sf2->add(idSmpl);

                // Ajout des données
                this->sf2->set(idSmpl, champ_sampleData16, Sound::bpsConversion(baData, 32, 16));
                EltID idSf2 = idSmpl;
                idSf2.typeElement = elementSf2;
                if (this->sf2->get(idSf2, champ_wBpsSave).wValue == 24)
                    this->sf2->set(idSmpl, champ_sampleData24, Sound::bpsConversion(baData, 32, 824));

                // Configuration
                Valeur value;
                value.dwValue = baData.length() / 4;
                this->sf2->set(idSmpl, champ_dwLength, value);
                value.dwValue = fEch;
                this->sf2->set(idSmpl, champ_dwSampleRate, value);
                value.wValue = note;
                this->sf2->set(idSmpl, champ_byOriginalPitch, value);
                value.cValue = 0;
                this->sf2->set(idSmpl, champ_chPitchCorrection, value);
                value.dwValue = loopStart;
                this->sf2->set(idSmpl, champ_dwStartLoop, value);
                value.dwValue = loopEnd;
                this->sf2->set(idSmpl, champ_dwEndLoop, value);
                this->sf2->set(idSmpl, champ_name, name);
                // Lien
                if (stereo)
                {
                    if (cote == 0)
                        value.sfLinkValue = rightSample;
                    else
                        value.sfLinkValue = leftSample;
                }
                else
                    value.sfLinkValue = monoSample;
                this->sf2->set(idSmpl, champ_sfSampleType, value);
                if (cote == 1)
                {
                    EltID idLink = idSmpl;
                    idLink.indexElt = idSmpl.indexElt - 1;
                    value.wValue = idLink.indexElt;
                    this->sf2->set(idSmpl, champ_wSampleLink, value);
                    value.wValue = idSmpl.indexElt;
                    this->sf2->set(idLink, champ_wSampleLink, value);
                }

                // Ajout du sample dans l'instrument
                idInstSmpl.indexElt2 = this->sf2->add(idInstSmpl);

                // Configuration
                value.wValue = idSmpl.indexElt;
                this->sf2->set(idInstSmpl, champ_sampleID, value);
                value.rValue.byLo = qMax(noteStart2, note-freq+1);
                value.rValue.byHi = note;
                this->sf2->set(idInstSmpl, champ_keyRange, value);
                if (stereo)
                {
                    if (cote == 0)
                        value.shValue = 500;
                    else
                        value.shValue = -500;
                }
                else
                    value.shValue = 0;
                this->sf2->set(idInstSmpl, champ_pan, value);
                value.wValue = attMini * 10;
                this->sf2->set(idInstSmpl, champ_initialAttenuation, value);
                if (progress.wasCanceled())
                {
                    // Actualisation et retour
                    this->mainWindow->updateDo();
                    return;
                }
            }
        }
    }

    // Actualisation
    this->mainWindow->updateDo();
}
void Page_Inst::release()
{
    EltID idInst = this->tree->getID(0);
    idInst.typeElement = elementInstSmpl;
    if (this->sf2->count(idInst, false) == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        return;
    }
    DialogRelease * dialogRelease = new DialogRelease(this);
    dialogRelease->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogRelease, SIGNAL(accepted(double, double, double)),
                  SLOT(release(double, double, double)));
    dialogRelease->show();
}
void Page_Inst::release(double duree36, double division, double deTune)
{
    this->sf2->prepareNewActions();
    // Reprise de l'identificateur si modification
    EltID id = this->tree->getID(0);
    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            // Note moyenne
            double noteMoy = (double)(this->sf2->get(id, champ_keyRange).rValue.byHi +
                    this->sf2->get(id, champ_keyRange).rValue.byLo) / 2;
            // Calcul durée release
            double release = pow(division, ((36. - noteMoy) / 12.)) * duree36;
            if (release < 0.001) release = 0.001;
            else if (release > 101.594) release = 101.594;
            // Valeur correspondante
            short val = 1200 * qLn(release) / 0.69314718056;
            // Modification instSmpl
            Valeur valeur;
            if (this->sf2->get(id, champ_releaseVolEnv).shValue != val)
            {
                valeur.shValue = val;
                this->sf2->set(id, champ_releaseVolEnv, valeur);
            }
            if (deTune != 0)
            {
                // Release de l'enveloppe de modulation
                if (this->sf2->get(id, champ_releaseModEnv).shValue != val)
                {
                    valeur.shValue = val;
                    this->sf2->set(id, champ_releaseModEnv, valeur);
                }
                if (this->sf2->get(id, champ_modEnvToPitch).shValue != -(int)100*deTune)
                {
                    valeur.shValue = -(int)100*deTune;
                    this->sf2->set(id, champ_modEnvToPitch, valeur);
                }
                // Hauteur de l'effet
                int tuningCoarse = floor(deTune);
                int tuningFine = 100*(deTune - tuningCoarse);
                valeur.shValue = this->sf2->get(id, champ_coarseTune).shValue + tuningCoarse;
                if (valeur.shValue != 0)
                    this->sf2->set(id, champ_coarseTune, valeur);
                else
                    this->sf2->reset(id, champ_coarseTune);
                valeur.shValue = this->sf2->get(id, champ_fineTune).shValue + tuningFine;
                if (valeur.shValue != 0)
                    this->sf2->set(id, champ_fineTune, valeur);
                else
                    this->sf2->reset(id, champ_fineTune);
            }
        }
    }

    // Actualisation
    this->mainWindow->updateDo();
    this->afficher();
}

double Page_Inst::getOffset(int type1, int type2)
{
    // Calcul du multiple de la fréquence fondamentale
    double multiple = (double)(2 * type1 + 1) * pow(2.0f, type2 - 2);

    // Renvoi du nombre de demi-tons à ajouter à la fondamentale pour obtenir l'harmonique
    return 12. * qLn(multiple) / 0.69314718056;
}

EltID Page_Inst::closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl)
{
    // Recherche du sample le plus proche de pitch dans l'instrument idInst
    double ecart_min_abs = 1000;
    EltID idInstSmplTmp = idInst;
    idInstSmplTmp.typeElement = elementInstSmpl;
    EltID idSmpl = idInst;
    idSmpl.indexElt = -1;
    idSmpl.typeElement = elementSmpl;
    EltID idSmplRet = idSmpl;
    for (int i = 0; i < sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            // Hauteur du sample
            idSmpl.indexElt = sf2->get(idInstSmplTmp, champ_sampleID).wValue;
            double pitchSmpl = sf2->get(idSmpl, champ_byOriginalPitch).bValue
                - (double)sf2->get(idSmpl, champ_chPitchCorrection).cValue / 100.0;
            // Mesure de l'écart
            double ecartTmp = pitchSmpl - pitch;
            double absEcart;
            if (ecartTmp < 0) absEcart = -3 * ecartTmp;
            else absEcart = ecartTmp;
            if (absEcart < ecart_min_abs)
            {
                ecart_min_abs = absEcart;
                ecart = -ecartTmp;
                idSmplRet = idSmpl;
                idInstSmpl = idInstSmplTmp;
            }
        }
    }
    // Type de sample
    int indexEltBase = idSmplRet.indexElt;
    SFSampleLink type = sf2->get(idSmplRet, champ_sfSampleType).sfLinkValue;
    if (!(type == RomMonoSample || type == monoSample ||
       ((type == RomRightSample || type == rightSample || type == RomLinkedSample || type == linkedSample) && cote == 0) ||
       ((type == RomLeftSample || type == leftSample) && cote == 1)))
        idSmplRet.indexElt = sf2->get(idSmplRet, champ_wSampleLink).wValue;
    double ecartMin = 1000;
    double ecartTmp;
    int rootKeySmpl = sf2->get(idSmplRet, champ_byOriginalPitch).bValue;
    // Recherche de l'instSmpl le plus proche de pitch, ayant comme sample_ID idSmplRet
    for (int i = 0; i < sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            if (sf2->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
            {
                // Notes min et max pour lesquels le sample est joué
                int noteMin = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                int noteMax = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
                // Ajustement
                int rootKeyInstSmpl = rootKeySmpl;
                if (sf2->isSet(idInstSmplTmp, champ_overridingRootKey))
                    rootKeyInstSmpl = sf2->get(idInstSmplTmp, champ_overridingRootKey).wValue;
                noteMin += rootKeySmpl - rootKeyInstSmpl;
                noteMax += rootKeySmpl - rootKeyInstSmpl;
                // Mesure de l'écart
                if (pitch < noteMin)
                    ecartTmp = noteMin - pitch;
                else if (pitch > noteMax)
                    ecartTmp = pitch - noteMax;
                else
                    ecartTmp = 0;
                if (ecartTmp < ecartMin)
                {
                    ecartMin = ecartTmp;
                    idInstSmpl = idInstSmplTmp;
                }
            }
        }
    }
    if (ecartMin > 900 && idSmplRet.indexElt != indexEltBase)
    {
        // Le sample associé n'a pas été trouvé, retour sur le sample de base
        idSmplRet.indexElt = indexEltBase;
        rootKeySmpl = sf2->get(idSmplRet, champ_byOriginalPitch).bValue;
        for (int i = 0; i < sf2->count(idInstSmplTmp); i++)
        {
            idInstSmplTmp.indexElt2 = i;
            if (!sf2->get(idInstSmplTmp, champ_hidden).bValue)
            {
                if (sf2->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
                {
                    // Notes min et max pour lesquels le sample est joué
                    int noteMin = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                    int noteMax = sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
                    // Ajustement
                    int rootKeyInstSmpl = rootKeySmpl;
                    if (sf2->isSet(idInstSmplTmp, champ_overridingRootKey))
                        rootKeyInstSmpl = sf2->get(idInstSmplTmp, champ_overridingRootKey).wValue;
                    noteMin += rootKeySmpl - rootKeyInstSmpl;
                    noteMax += rootKeySmpl - rootKeyInstSmpl;
                    // Mesure de l'écart
                    if (pitch < noteMin)
                        ecartTmp = noteMin - pitch;
                    else if (pitch > noteMax)
                        ecartTmp = pitch - noteMax;
                    else
                        ecartTmp = 0;
                    if (ecartTmp < ecartMin)
                    {
                        ecartMin = ecartTmp;
                        idInstSmpl = idInstSmplTmp;
                    }
                }
            }
        }
    }
    return idSmplRet;
}
QByteArray Page_Inst::getSampleData(EltID idSmpl, qint32 nbRead)
{
    // Récupération de données provenant d'un sample, en prenant en compte la boucle
    QByteArray baData = sf2->getData(idSmpl, champ_sampleData32);
    qint64 loopStart = sf2->get(idSmpl, champ_dwStartLoop).dwValue;
    qint64 loopEnd = sf2->get(idSmpl, champ_dwEndLoop).dwValue;
    QByteArray baDataRet;
    baDataRet.resize(nbRead * 4);
    qint64 posInit = 0;
    const char * data = baData.constData();
    char * dataRet = baDataRet.data();
    if (loopStart != loopEnd)
    {
        // Boucle
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(loopEnd - posInit, nbRead - total);
            memcpy(dataRet + 4 * total, data + 4 * posInit, 4 * chunk);
            posInit += chunk;
            if (posInit >= loopEnd) posInit = loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (baData.size()/4 < nbRead)
        {
            baDataRet.fill(0);
            memcpy(dataRet, data, baData.size());
        }
        else
            memcpy(dataRet, data, 4 * nbRead);
    }
    return baDataRet;
}
QByteArray Page_Inst::addSampleData(QByteArray baData1, QByteArray baData2, double mult)
{
    // Ajout de baData2 multiplié par mult dans baData1
    qint32 * data1 = (qint32 *)baData1.data();
    qint32 * data2 = (qint32 *)baData2.data();
    for (int i = 0; i < qMin(baData1.size(), baData2.size())/4; i++)
        data1[i] += mult * data2[i];
    return baData1;
}

// TableWidgetInst
TableWidgetInst::TableWidgetInst(QWidget *parent) : TableWidget(parent)
{
}
TableWidgetInst::~TableWidgetInst()
{
}

int TableWidgetInst::getRow(WORD champ)
{
    int row = -1;
    switch (champ)
    {
    case champ_keyRange: row = 4; break;
    case champ_velRange: row = 5; break;
    case champ_initialAttenuation: row = 6; break;
    case champ_pan: row = 7; break;
    case champ_sampleModes: row = 8; break;
    case champ_overridingRootKey: row = 9; break;
    case champ_coarseTune: row = 10; break;
    case champ_fineTune: row = 11; break;
    case champ_scaleTuning: row = 12; break;
    case champ_initialFilterFc: row = 13; break;
    case champ_initialFilterQ: row = 14; break;
    case champ_delayVolEnv: row = 15; break;
    case champ_attackVolEnv: row = 16; break;
    case champ_holdVolEnv: row = 17; break;
    case champ_decayVolEnv: row = 18; break;
    case champ_sustainVolEnv: row = 19; break;
    case champ_releaseVolEnv: row = 20; break;
    case champ_keynumToVolEnvHold: row = 21; break;
    case champ_keynumToVolEnvDecay: row = 22; break;
    case champ_delayModEnv: row = 23; break;
    case champ_attackModEnv: row = 24; break;
    case champ_holdModEnv: row = 25; break;
    case champ_decayModEnv: row = 26; break;
    case champ_sustainModEnv: row = 27; break;
    case champ_releaseModEnv: row = 28; break;
    case champ_modEnvToPitch: row = 29; break;
    case champ_modEnvToFilterFc: row = 30; break;
    case champ_keynumToModEnvHold: row = 31; break;
    case champ_keynumToModEnvDecay: row = 32; break;
    case champ_delayModLFO: row = 33; break;
    case champ_freqModLFO: row = 34; break;
    case champ_modLfoToPitch: row = 35; break;
    case champ_modLfoToFilterFc: row = 36; break;
    case champ_modLfoToVolume: row = 37; break;
    case champ_delayVibLFO: row = 38; break;
    case champ_freqVibLFO: row = 39; break;
    case champ_vibLfoToPitch: row = 40; break;
    case champ_exclusiveClass: row = 41; break;
    case champ_chorusEffectsSend: row = 42; break;
    case champ_reverbEffectsSend: row = 43; break;
    case champ_keynum: row = 44; break;
    case champ_velocity: row = 45; break;
    case champ_startAddrsOffset: row = 46; break;
    case champ_endAddrsOffset: row = 47; break;
    case champ_startloopAddrsOffset: row = 48; break;
    case champ_endloopAddrsOffset: row = 49; break;
    }
    return row;
}
Champ TableWidgetInst::getChamp(int row)
{
    Champ champ = champ_unknown;
    switch (row)
    {
    case 4: champ = champ_keyRange; break;
    case 5: champ = champ_velRange; break;
    case 6: champ = champ_initialAttenuation; break;
    case 7: champ = champ_pan; break;
    case 8: champ = champ_sampleModes; break;
    case 9: champ = champ_overridingRootKey; break;
    case 10: champ = champ_coarseTune; break;
    case 11: champ = champ_fineTune; break;
    case 12: champ = champ_scaleTuning; break;
    case 13: champ = champ_initialFilterFc; break;
    case 14: champ = champ_initialFilterQ; break;
    case 15: champ = champ_delayVolEnv; break;
    case 16: champ = champ_attackVolEnv; break;
    case 17: champ = champ_holdVolEnv; break;
    case 18: champ = champ_decayVolEnv; break;
    case 19: champ = champ_sustainVolEnv; break;
    case 20: champ = champ_releaseVolEnv; break;
    case 21: champ = champ_keynumToVolEnvHold; break;
    case 22: champ = champ_keynumToVolEnvDecay; break;
    case 23: champ = champ_delayModEnv; break;
    case 24: champ = champ_attackModEnv; break;
    case 25: champ = champ_holdModEnv; break;
    case 26: champ = champ_decayModEnv; break;
    case 27: champ = champ_sustainModEnv; break;
    case 28: champ = champ_releaseModEnv; break;
    case 29: champ = champ_modEnvToPitch; break;
    case 30: champ = champ_modEnvToFilterFc; break;
    case 31: champ = champ_keynumToModEnvHold; break;
    case 32: champ = champ_keynumToModEnvDecay; break;
    case 33: champ = champ_delayModLFO; break;
    case 34: champ = champ_freqModLFO; break;
    case 35: champ = champ_modLfoToPitch; break;
    case 36: champ = champ_modLfoToFilterFc; break;
    case 37: champ = champ_modLfoToVolume; break;
    case 38: champ = champ_delayVibLFO; break;
    case 39: champ = champ_freqVibLFO; break;
    case 40: champ = champ_vibLfoToPitch; break;
    case 41: champ = champ_exclusiveClass; break;
    case 42: champ = champ_chorusEffectsSend; break;
    case 43: champ = champ_reverbEffectsSend; break;
    case 44: champ = champ_keynum; break;
    case 45: champ = champ_velocity; break;
    case 46: champ = champ_startAddrsOffset; break;
    case 47: champ = champ_endAddrsOffset; break;
    case 48: champ = champ_startloopAddrsOffset; break;
    case 49: champ = champ_endloopAddrsOffset; break;
    default: champ = champ_unknown;
    }
    return champ;
}
