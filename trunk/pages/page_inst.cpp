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

#include "page_inst.h"
#include "mainwindow.h"
#include "ui_page_inst.h"
#include "dialog_mixture.h"
#include "dialog_release.h"
#include "dialog_celeste.h"
#include "dialog_transposition.h"
#include <QProgressDialog>
#include <QInputDialog>
#include <QMenu>
#include <qmath.h>

// Constructeur, destructeur
Page_Inst::Page_Inst(QWidget *parent) :
    PageTable(PAGE_INST, parent),
    ui(new Ui::Page_Inst)
{
    ui->setupUi(this);
    this->contenant = elementInst;
    this->contenantGen = elementInstGen;
    this->contenantMod = elementInstMod;
    this->contenu = elementSmpl;
    this->lien = elementInstSmpl;
    this->lienGen = elementInstSmplGen;
    this->lienMod = elementInstSmplMod;
    this->table = ui->tableInst;
    this->tableMod = ui->tableMod;
    this->spinAmount = ui->spinSource2;
    this->checkAbs = ui->checkAbs;
    this->pushNouveauMod = ui->pushNouveauMod;
    this->pushSupprimerMod = ui->pushSupprimerMod;
    this->comboSource1Courbe = ui->comboCourbeSource1;
    this->comboSource2Courbe = ui->comboCourbeSource2;
    this->comboSource1 = ui->comboSource1;
    this->comboSource2 = ui->comboSource2;
    this->comboDestination = ui->comboDestination;
    this->_pushCopyMod = ui->pushCopyMod;
    this->_pushRangeMode = ui->pushRangeMode;
    this->_rangeEditor = ui->rangeEditor;

    // Remplissage de comboDestination
    for (int i = 0; i < 48; i++)
        this->comboDestination->addItem(getGenName(this->getDestNumber(i)));
    this->comboDestination->setLimite(48);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

    // Initialisation menu de copie de modulateurs
    _menu = new QMenu();
    _menu->addAction("", this, SLOT(duplicateMod()));
    _menu->addAction("", this, SLOT(copyMod()));

    // Initialisation édition étendues
    ui->rangeEditor->init(_sf2);

#ifdef Q_OS_MAC
    this->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = this->table->font();
    font.setPixelSize(10);
    this->table->setFont(font);
    ui->horizontalLayout_2->setSpacing(15);
#endif
    ui->tableInst->verticalHeader()->setDefaultSectionSize(QFontMetrics(ui->tableInst->font()).height() + 8);

    connect(this->table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(ui->rangeEditor, SIGNAL(updateKeyboard()), this, SLOT(updateKeyboard()));
    connect(ui->rangeEditor, SIGNAL(divisionUpdated()), this, SLOT(updateMainwindow()));
    connect(ui->rangeEditor, SIGNAL(keyTriggered(int,int)), this, SLOT(playKey(int, int)));
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(QList<EltID>)), this, SLOT(selectInTree(QList<EltID>)));
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

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, false, false);
    if (ids.count() > 1)
    {
        ui->pushRangeMode->setChecked(false);
        ui->pushRangeMode->setEnabled(false);
        ui->stackedWidget->setCurrentIndex(0);

        this->ui->labelPrst->setText("");
    }
    else
    {
        ui->pushRangeMode->setEnabled(true);

        // Liste des presets qui utilisent l'instrument
        EltID id = this->_tree->getFirstID();
        id.typeElement = elementInst;
        int nbPrst = 0;
        bool isFound;
        int j;
        QString qStr = "";
        EltID id2 = id;
        id2.typeElement = elementPrst;
        EltID id3 = id;
        id3.typeElement = elementPrstInst;

        // Parcours de tous les presets
        for (int i = 0; i < this->_sf2->count(id2); i++)
        {
            id2.indexElt = i;
            id3.indexElt = i;

            // Parcours de tous les instruments liés au preset
            isFound = false;
            j = 0;
            while(j < this->_sf2->count(id3) && !isFound)
            {
                id3.indexElt2 = j;
                if (!this->_sf2->get(id3, champ_hidden).bValue)
                {
                    if (this->_sf2->get(id3, champ_instrument).wValue == id.indexElt)
                    {
                        // Ajout d'un preset
                        if (nbPrst)
                            qStr.append(", ");
                        qStr.append(this->_sf2->getQstr(id2, champ_name).toStdString().c_str());
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
}

// Outils instrument
void Page_Inst::desaccorder()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    DialogCeleste * dialogCeleste = new DialogCeleste(this);
    dialogCeleste->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogCeleste, SIGNAL(accepted(double,double)),
                  SLOT(desaccorder(double, double)));
    dialogCeleste->show();
}

void Page_Inst::desaccorder(double doHerz, double division)
{
    this->_sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    foreach (EltID id, ids)
        desaccorder(id, doHerz, division);

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void Page_Inst::desaccorder(EltID id, double doHerz, double division)
{
    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->_sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            // Sample lié
            EltID id2 = id;
            id2.typeElement = elementSmpl;
            id2.indexElt = this->_sf2->get(id, champ_sampleID).genValue.wAmount;
            // Numéro de la note du sample
            int numNoteSmpl = this->_sf2->get(id2, champ_byOriginalPitch).bValue;
            // Note de base sur le clavier correspondant à numNoteSmpl;
            int numBase = numNoteSmpl;
            if (this->_sf2->isSet(id, champ_overridingRootKey))
                numBase = this->_sf2->get(id, champ_overridingRootKey).wValue;
            // Etendue du sample sur le clavier
            int numBas = 0;
            int numHaut = 0;
            // Intervalle joué par le sample
            if (this->_sf2->isSet(id, champ_keynum))
            {
                numBas = numNoteSmpl + this->_sf2->get(id, champ_keynum).wValue - numBase;
                numHaut = numBas;
            }
            else
            {
                numBas = numNoteSmpl + this->_sf2->get(id, champ_keyRange).rValue.byLo - numBase;
                numHaut = numNoteSmpl + this->_sf2->get(id, champ_keyRange).rValue.byHi - numBase;
            }
            // Fréquence des battements
            double keyMoy = (double)(this->_sf2->get(id, champ_keyRange).rValue.byHi +
                                     this->_sf2->get(id, champ_keyRange).rValue.byLo) / 2.;
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
            if (this->_sf2->get(id, champ_fineTune).shValue != decalage)
            {
                Valeur val;
                val.shValue = decalage;
                this->_sf2->set(id, champ_fineTune, val);
            }
        }
    }
}

void Page_Inst::repartitionAuto()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, false);
    if (ids.isEmpty() || error)
        return;

    this->_sf2->prepareNewActions();
    ids = this->getUniqueInstOrPrst(error, true, false);
    foreach (EltID id, ids)
        repartitionAuto(id);

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void Page_Inst::repartitionAuto(EltID id)
{
    // Liste des samples liés avec note de base
    QList<EltID> listID;
    QList<int> listNote;
    int notePrecedente, note, noteSuivante;
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->_sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            if (this->_sf2->isSet(id, champ_overridingRootKey))
                note = this->_sf2->get(id, champ_overridingRootKey).wValue;
            else
            {
                EltID id2 = id;
                id2.typeElement = elementSmpl;
                id2.indexElt = this->_sf2->get(id, champ_sampleID).wValue;
                note = this->_sf2->get(id2, champ_byOriginalPitch).bValue;
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
        if (this->_sf2->get(listID.at(i), champ_keyRange).rValue.byLo != val.rValue.byLo ||
            this->_sf2->get(listID.at(i), champ_keyRange).rValue.byHi != val.rValue.byHi)
            this->_sf2->set(listID.at(i), champ_keyRange, val);
    }
}

void Page_Inst::mixture()
{
    EltID idInst = this->_tree->getFirstID();
    idInst.typeElement = elementInstSmpl;
    if (this->_sf2->count(idInst, false) == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("L'instrument doit contenir des sons."));
        return;
    }
    DialogMixture * dialogMixture = new DialogMixture(this);
    this->connect(dialogMixture, SIGNAL(accepted(QList<QList<int> >, QString, bool, int, bool)),
                  SLOT(mixture(QList<QList<int> >, QString, bool, int, bool)));
    dialogMixture->show();
}

void Page_Inst::mixture(QList<QList<int> > listeParam, QString nomInst, bool bouclage, int freq, bool stereo)
{
    // Création d'une mixture
    this->_sf2->prepareNewActions();

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
    EltID idInst = this->_tree->getFirstID();
    idInst.typeElement = elementInst;
    EltID idSmpl;
    QByteArray baData;

    // Création d'un nouvel instrument
    EltID idNewInst = idInst;
    idNewInst.indexElt = this->_sf2->add(idNewInst);

    // Configuration instrument
    if (nomInst.isEmpty())
        nomInst = trUtf8("sans nom");
    this->_sf2->set(idNewInst, champ_name, nomInst.left(20));
    if (bouclage)
    {
        Valeur value;
        value.wValue = 1;
        this->_sf2->set(idNewInst, champ_sampleModes, value);
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
                QString str2 = QString("%1").arg(note, 3, 10, QChar('0'));
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
                for (int numRang = 0; numRang < listRangs.length() / 2; numRang++)
                {
                    double noteTmp = (double)note + getOffset(listRangs.at(2*numRang), listRangs.at(2*numRang+1));
                    double ecart;
                    EltID idInstSmplTmp;
                    idSmpl = closestSample(idInst, noteTmp, ecart, cote, idInstSmplTmp);
                    double attenuation = 0;
                    if (_sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        attenuation = (double)_sf2->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0;
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
                        this->_mainWindow->updateDo();
                        return;
                    }

                    // Calcul de la note à ajouter à la mixture
                    double noteTmp = (double)note + getOffset(listRangs.at(2 * numRang), listRangs.at(2 * numRang + 1));
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
                        double fEchInit = (double)this->_sf2->get(idSmpl, champ_dwSampleRate).dwValue * pow(2, ecart/12.0);
                        // Récupération du son
                        QByteArray baDataTmp = getSampleData(idSmpl, dureeSmpl * fEchInit);
                        // Prise en compte atténuation en dB
                        double attenuation = 1;
                        if (_sf2->isSet(idInstSmplTmp, champ_initialAttenuation))
                        {
                            attenuation = (double)_sf2->get(idInstSmplTmp, champ_initialAttenuation).shValue / 10.0 - attMini;
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
                    this->_mainWindow->updateDo();
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
                        this->_mainWindow->updateDo();
                        return;
                    }
                }

                // Création d'un nouveau sample
                idSmpl.indexElt = this->_sf2->add(idSmpl);

                // Ajout des données
                this->_sf2->set(idSmpl, champ_sampleData16, Sound::bpsConversion(baData, 32, 16));
                EltID idSf2 = idSmpl;
                idSf2.typeElement = elementSf2;
                if (this->_sf2->get(idSf2, champ_wBpsSave).wValue == 24)
                    this->_sf2->set(idSmpl, champ_sampleData24, Sound::bpsConversion(baData, 32, 824));

                // Configuration
                Valeur value;
                value.dwValue = baData.length() / 4;
                this->_sf2->set(idSmpl, champ_dwLength, value);
                value.dwValue = fEch;
                this->_sf2->set(idSmpl, champ_dwSampleRate, value);
                value.wValue = note;
                this->_sf2->set(idSmpl, champ_byOriginalPitch, value);
                value.cValue = 0;
                this->_sf2->set(idSmpl, champ_chPitchCorrection, value);
                value.dwValue = loopStart;
                this->_sf2->set(idSmpl, champ_dwStartLoop, value);
                value.dwValue = loopEnd;
                this->_sf2->set(idSmpl, champ_dwEndLoop, value);
                this->_sf2->set(idSmpl, champ_name, name);
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
                this->_sf2->set(idSmpl, champ_sfSampleType, value);
                if (cote == 1)
                {
                    EltID idLink = idSmpl;
                    idLink.indexElt = idSmpl.indexElt - 1;
                    value.wValue = idLink.indexElt;
                    this->_sf2->set(idSmpl, champ_wSampleLink, value);
                    value.wValue = idSmpl.indexElt;
                    this->_sf2->set(idLink, champ_wSampleLink, value);
                }

                // Ajout du sample dans l'instrument
                idInstSmpl.indexElt2 = this->_sf2->add(idInstSmpl);

                // Configuration
                value.wValue = idSmpl.indexElt;
                this->_sf2->set(idInstSmpl, champ_sampleID, value);
                value.rValue.byLo = qMax(noteStart2, note-freq+1);
                value.rValue.byHi = note;
                this->_sf2->set(idInstSmpl, champ_keyRange, value);
                if (stereo)
                {
                    if (cote == 0)
                        value.shValue = 500;
                    else
                        value.shValue = -500;
                }
                else
                    value.shValue = 0;
                this->_sf2->set(idInstSmpl, champ_pan, value);
                value.wValue = attMini * 10;
                this->_sf2->set(idInstSmpl, champ_initialAttenuation, value);
                if (progress.wasCanceled())
                {
                    // Actualisation et retour
                    this->_mainWindow->updateDo();
                    return;
                }
            }
        }
    }

    // Actualisation
    this->_mainWindow->updateDo();
}

void Page_Inst::release()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    DialogRelease * dialogRelease = new DialogRelease(this);
    dialogRelease->setAttribute(Qt::WA_DeleteOnClose, true);
    this->connect(dialogRelease, SIGNAL(accepted(double, double, double)),
                  SLOT(release(double, double, double)));
    dialogRelease->show();
}

void Page_Inst::release(double duree36, double division, double deTune)
{
    this->_sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    foreach (EltID id, ids)
        release(id, duree36, division, deTune);

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void Page_Inst::release(EltID id, double duree36, double division, double deTune)
{
    // Modification pour chaque sample lié
    id.typeElement = elementInstSmpl;
    for (int i = 0; i < this->_sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!this->_sf2->get(id, champ_hidden).bValue)
        {
            // Note moyenne
            double noteMoy = (double)(this->_sf2->get(id, champ_keyRange).rValue.byHi +
                    this->_sf2->get(id, champ_keyRange).rValue.byLo) / 2;
            // Calcul durée release
            double release = pow(division, ((36. - noteMoy) / 12.)) * duree36;
            if (release < 0.001) release = 0.001;
            else if (release > 101.594) release = 101.594;
            // Valeur correspondante
            short val = 1200 * qLn(release) / 0.69314718056;
            // Modification instSmpl
            Valeur valeur;
            if (this->_sf2->get(id, champ_releaseVolEnv).shValue != val)
            {
                valeur.shValue = val;
                this->_sf2->set(id, champ_releaseVolEnv, valeur);
            }
            if (deTune != 0)
            {
                // Release de l'enveloppe de modulation
                if (this->_sf2->get(id, champ_releaseModEnv).shValue != val)
                {
                    valeur.shValue = val;
                    this->_sf2->set(id, champ_releaseModEnv, valeur);
                }
                if (this->_sf2->get(id, champ_modEnvToPitch).shValue != -(int)100*deTune)
                {
                    valeur.shValue = -(int)100*deTune;
                    this->_sf2->set(id, champ_modEnvToPitch, valeur);
                }
                // Hauteur de l'effet
                int tuningCoarse = floor(deTune);
                int tuningFine = 100*(deTune - tuningCoarse);
                valeur.shValue = this->_sf2->get(id, champ_coarseTune).shValue + tuningCoarse;
                if (valeur.shValue != 0)
                    this->_sf2->set(id, champ_coarseTune, valeur);
                else
                    this->_sf2->reset(id, champ_coarseTune);
                valeur.shValue = this->_sf2->get(id, champ_fineTune).shValue + tuningFine;
                if (valeur.shValue != 0)
                    this->_sf2->set(id, champ_fineTune, valeur);
                else
                    this->_sf2->reset(id, champ_fineTune);
            }
        }
    }

    // Simplification
    this->_sf2->simplify(id, champ_fineTune);
    this->_sf2->simplify(id, champ_coarseTune);
    this->_sf2->simplify(id, champ_modEnvToPitch);
    this->_sf2->simplify(id, champ_releaseModEnv);
    this->_sf2->simplify(id, champ_releaseVolEnv);
}

void Page_Inst::transposer()
{
    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    DialogTransposition * dialog = new DialogTransposition(this);
    this->connect(dialog, SIGNAL(accepted(double, bool)), SLOT(transposer(double, bool)));
    dialog->show();
}

void Page_Inst::transposer(double ton, bool adaptKeyRange)
{
    _sf2->prepareNewActions();

    bool error;
    QList<EltID> ids = this->getUniqueInstOrPrst(error, true, true);
    if (ids.isEmpty() || error)
        return;

    foreach (EltID id, ids)
        transposer(id, ton, adaptKeyRange);

    // Actualisation
    this->_mainWindow->updateDo();
    this->_mainWindow->updateActions();
}

void Page_Inst::transposer(EltID idInstSmpl, double ton, bool adaptKeyRange)
{
    EltID idInst = idInstSmpl;
    idInstSmpl.typeElement = elementInstSmpl;
    idInst.typeElement = elementInst;

    // Nombre de tons
    int nbTons = qRound(ton);

    // Correction
    int correction = qRound(100. * (ton - nbTons));

    // Modification pour chaque sample lié
    for (int i = 0; i < this->_sf2->count(idInstSmpl); i++)
    {
        idInstSmpl.indexElt2 = i;
        if (!this->_sf2->get(idInstSmpl, champ_hidden).bValue)
        {
            Valeur valeur;

            // Etendue
            if (adaptKeyRange)
            {
                int noteInf = this->_sf2->get(idInstSmpl, champ_keyRange).rValue.byLo;
                int noteSup = this->_sf2->get(idInstSmpl, champ_keyRange).rValue.byHi;

                // Déplacement de l'étendue
                noteInf -= nbTons;
                noteSup -= nbTons;

                // Ajustement
                if (noteInf < 0)
                    noteInf = 0;
                else if (noteInf > 127)
                    noteInf = 127;
                if (noteSup < 0)
                    noteSup = 0;
                else if (noteSup > 127)
                    noteSup = 127;

                // Enregistrement de la nouvelle étendue
                valeur.rValue.byLo = noteInf;
                valeur.rValue.byHi = noteSup;
                this->_sf2->set(idInstSmpl, champ_keyRange, valeur);
            }

            // Note de base
            EltID idSmpl = idInstSmpl;
            idSmpl.typeElement = elementSmpl;
            idSmpl.indexElt = this->_sf2->get(idInstSmpl, champ_sampleID).wValue;
            int rootKey = this->_sf2->get(idSmpl, champ_byOriginalPitch).bValue;
            if (this->_sf2->isSet(idInst, champ_overridingRootKey))
                rootKey = this->_sf2->get(idInst, champ_overridingRootKey).wValue;
            if (this->_sf2->isSet(idInstSmpl, champ_overridingRootKey))
                rootKey = this->_sf2->get(idInstSmpl, champ_overridingRootKey).wValue;

            // Modification rootkey et enregistrement
            rootKey -= nbTons;
            if (rootKey < 0)
                rootKey = 0;
            else if (rootKey > 127)
                rootKey = 127;
            valeur.wValue = rootKey;
            this->_sf2->set(idInstSmpl, champ_overridingRootKey, valeur);

            // Correction
            int fineTune = 0;
            if (this->_sf2->isSet(idInst, champ_fineTune))
                fineTune = this->_sf2->get(idInst, champ_fineTune).wValue;
            if (this->_sf2->isSet(idInstSmpl, champ_fineTune))
                fineTune = this->_sf2->get(idInstSmpl, champ_fineTune).wValue;
            int coarseTune = 0;
            if (this->_sf2->isSet(idInst, champ_coarseTune))
                coarseTune = this->_sf2->get(idInst, champ_coarseTune).wValue;
            if (this->_sf2->isSet(idInstSmpl, champ_coarseTune))
                coarseTune = this->_sf2->get(idInstSmpl, champ_coarseTune).wValue;

            // Modification de la correction
            fineTune += correction;
            if (fineTune >= 100)
            {
                fineTune -= 100;
                coarseTune += 1;
            }
            else if (fineTune <= -100)
            {
                fineTune += 100;
                coarseTune -= 1;
            }

            // Enregistrement de la nouvelle correction
            valeur.shValue = fineTune;
            this->_sf2->set(idInstSmpl, champ_fineTune, valeur);
            valeur.shValue = coarseTune;
            this->_sf2->set(idInstSmpl, champ_coarseTune, valeur);
        }
    }

    // Simplification
    this->_sf2->simplify(idInstSmpl, champ_fineTune);
    this->_sf2->simplify(idInstSmpl, champ_coarseTune);
}

double Page_Inst::getOffset(int type1, int type2)
{
    // Calcul du multiple de la fréquence fondamentale
    double multiple = (double)(2 * type1 + 1) * pow(2.0f, type2 - 3);

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
    for (int i = 0; i < _sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!_sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            // Hauteur du sample
            idSmpl.indexElt = _sf2->get(idInstSmplTmp, champ_sampleID).wValue;
            double pitchSmpl = _sf2->get(idSmpl, champ_byOriginalPitch).bValue
                - (double)_sf2->get(idSmpl, champ_chPitchCorrection).cValue / 100.0;
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
    SFSampleLink type = _sf2->get(idSmplRet, champ_sfSampleType).sfLinkValue;
    if (!(type == RomMonoSample || type == monoSample ||
       ((type == RomRightSample || type == rightSample || type == RomLinkedSample || type == linkedSample) && cote == 0) ||
       ((type == RomLeftSample || type == leftSample) && cote == 1)))
        idSmplRet.indexElt = _sf2->get(idSmplRet, champ_wSampleLink).wValue;
    double ecartMin = 1000;
    double ecartTmp;
    int rootKeySmpl = _sf2->get(idSmplRet, champ_byOriginalPitch).bValue;
    // Recherche de l'instSmpl le plus proche de pitch, ayant comme sample_ID idSmplRet
    for (int i = 0; i < _sf2->count(idInstSmplTmp); i++)
    {
        idInstSmplTmp.indexElt2 = i;
        if (!_sf2->get(idInstSmplTmp, champ_hidden).bValue)
        {
            if (_sf2->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
            {
                // Notes min et max pour lesquels le sample est joué
                int noteMin = _sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                int noteMax = _sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
                // Ajustement
                int rootKeyInstSmpl = rootKeySmpl;
                if (_sf2->isSet(idInstSmplTmp, champ_overridingRootKey))
                    rootKeyInstSmpl = _sf2->get(idInstSmplTmp, champ_overridingRootKey).wValue;
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
        rootKeySmpl = _sf2->get(idSmplRet, champ_byOriginalPitch).bValue;
        for (int i = 0; i < _sf2->count(idInstSmplTmp); i++)
        {
            idInstSmplTmp.indexElt2 = i;
            if (!_sf2->get(idInstSmplTmp, champ_hidden).bValue)
            {
                if (_sf2->get(idInstSmplTmp, champ_sampleID).wValue == idSmplRet.indexElt)
                {
                    // Notes min et max pour lesquels le sample est joué
                    int noteMin = _sf2->get(idInstSmplTmp, champ_keyRange).rValue.byLo;
                    int noteMax = _sf2->get(idInstSmplTmp, champ_keyRange).rValue.byHi;
                    // Ajustement
                    int rootKeyInstSmpl = rootKeySmpl;
                    if (_sf2->isSet(idInstSmplTmp, champ_overridingRootKey))
                        rootKeyInstSmpl = _sf2->get(idInstSmplTmp, champ_overridingRootKey).wValue;
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
    QByteArray baData = _sf2->getData(idSmpl, champ_sampleData32);
    qint64 loopStart = _sf2->get(idSmpl, champ_dwStartLoop).dwValue;
    qint64 loopEnd = _sf2->get(idSmpl, champ_dwEndLoop).dwValue;
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

void Page_Inst::on_pushRangeMode_clicked()
{
    if (ui->pushRangeMode->isChecked())
        ui->stackedWidget->setCurrentIndex(1);
    else
        ui->stackedWidget->setCurrentIndex(0);
    PageTable::afficher();
}

// TableWidgetInst
TableWidgetInst::TableWidgetInst(QWidget *parent) : TableWidget(parent) {}

TableWidgetInst::~TableWidgetInst() {}

int TableWidgetInst::getRow(quint16 champ)
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
