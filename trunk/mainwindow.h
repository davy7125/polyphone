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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QDir>
#include <stdlib.h>
#include <stdio.h>
#include "pile_sf2.h"
#include "config.h"
#include "dialog_list.h"
#include "dialog_help.h"
#include "dialog_about.h"
#include "page_sf2.h"
#include "page_smpl.h"
#include "page_inst.h"
#include "page_prst.h"
#include "audiodevice.h"
#include "synth.h"
#include "pianokeybdcustom.h"
#include "dialog_magnetophone.h"

namespace Ui
{
    class MainWindow;
    class Tree;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    // Méthodes publiques
    void prepareNewAction();
    void updateDo();
    void updateActions();
    void enableActionSample(bool isEnabled);
    void enableActionInstrument(bool isEnabled);
    void enableActionPreset(bool isEnabled);
    void enableActionSf2(bool isEnabled);
    void associer(EltID idDest);
    void remplacer(EltID idSrc);
    void desactiveOutilsSmpl();
    void activeOutilsSmpl();
    bool isPlaying();
    void ouvrir(QString fileName);
    QStringList getListMidi();
    void openMidiPort(int val);
    void setAudioEngine(int audioEngine, int bufferSize);
    void setSynthGain(int val);
    void setSynthReverb(int level, int size, int width, int damping);
    void setSynthChorus(int level, int depth, int frequency);
    void setListeActions(QList<QAction *> listeActions);
    void dragAndDrop(EltID idDest, QList<EltID> idSources);

public slots:
    void returnToOldMaxMinSizes();
    void supprimerElt();        // Suppression des éléments sélectionnés dans l'arbre
    void copier();              // Envoi du signal "copier"
    void coller();              // Envoi du signal "coller"

private slots:
    void updateTable(int type, int sf2, int elt, int elt2);  // Mise à jour tables si suppression définitive d'un élément masqué

    void nouveau();             // Clic sur l'action "nouveau"
    void ouvrir();              // Clic sur l'action "ouvrir"
    void ouvrirFichier1();      // Clic sur l'action "ouvrir fichier 1"
    void ouvrirFichier2();      // Clic sur l'action "ouvrir fichier 2"
    void ouvrirFichier3();      // Clic sur l'action "ouvrir fichier 3"
    void ouvrirFichier4();      // Clic sur l'action "ouvrir fichier 4"
    void ouvrirFichier5();      // Clic sur l'action "ouvrir fichier 5"
    void Fermer();              // Clic sur l'action "fermer"

    void renommer();            // Renommer un ou plusieurs éléments dans l'arborescence
    void renommerEnMasse(QString name, int modificationType);
    void importerSmpl();        // Import d'un sample
    void exporterSmpl();        // Export d'un sample
    void exporter();            // Export d'un ou plusieurs preset(s)
    void exporter(QList<EltID> listID, QString dir, int format);
    void nouvelInstrument();    // Création d'un instrument
    void nouveauPreset();       // Création d'un preset
    void associer();            // Association InstSmpl et PrstInst
    void remplacer();           // Remplacement InstSmpl et PrstInst
    void supprimer();           // Envoi du signal "supprimer"

    void showConfig();          // Affichage fenêtre configuration
    void showAbout();           // Affichage fenêtre à propos
    void showHelp();            // Affichage fenêtre d'aide
    void AfficherBarreOutils(); // Clic sur l'action "barre d'outils" du menu "afficher"
    void afficherSectionModulateurs();
    void setKeyboardType0();    // Clic sur clavier -> aucun
    void setKeyboardType1();    // Clic sur clavier -> 5 octaves
    void setKeyboardType2();    // Clic sur clavier -> 6 octaves
    void setKeyboardType3();    // Clic sur clavier -> 128 notes
    void setVelocity(int val);  // Rotation du bouton vélocité
    void undo();                // Clic sur l'action "undo"
    void redo();                // Clic sur l'action "redo"
    void sauvegarder();         // Clic sur l'action "sauvegarder"
    void sauvegarderSous();     // Clic sur l'action "sauvegarder sous"

    void enleveBlanc();         // outil sample, enlever blanc au départ
    void enleveFin();           // outil sample, ajuster à la fin de boucle
    void normalisation();       // outil sample, normalisation
    void bouclage();            // outil sample, bouclage
    void filtreMur();           // outil sample, filtre "mur de brique"
    void reglerBalance();       // outil sample, réglage de la balance (samples liés)
    void transposer();          // outil sample, transposition
    void sifflements();         // outil sample, suppression des sifflements
    void desaccorder();         // outil instrument, désaccordage ondulant
    void repartitionAuto();     // outil instrument, répartition automatique keyrange
    void spatialisation();      // outil instrument, spatialisation du son
    void mixture();             // outil instrument, création mixture
    void release();             // outil instrument, élaboration de releases
    void duplication();         // outil instrument et preset, duplication des divisions
    void paramGlobal();         // outil instrument et preset, modification globale d'un paramètre
    void visualize();           // outil instrument et preset, visualise les paramètres utilisés dans un graphique
    void attenuationMini();     // outil sf2, mise à jour de toutes les atténuations
    void purger();              // outil sf2, suppression des éléments non utilisés
    void associationAutoSmpl(); // outil sf2, association auto gauche-droite des samples
    void magnetophone();        // affichage du magnétophone

    void noteOn(int key);
    void noteOff(int key);
    void setSustain(bool isOn);
    void setVolume(int vol);
    void noteChanged(int key, int vel);

signals:
    void initAudio(int numDevice, int bufferSize);
    void stopAudio();
    void play(int type, int idSf2, int idElt, int note, int velocity);

private:
    Page_Sf2 * page_sf2;
    Page_Smpl * page_smpl;
    Page_Inst * page_inst;
    Page_Prst * page_prst;
    Pile_sf2 * sf2;
    Synth * synth;
    AudioDevice * audioDevice;
    QThread audioThread;
    QThread synthThread;
    Config * configuration;
    DialogHelp help;
    DialogAbout about;
    DialogList dialList;
    PianoKeybdCustom * keyboard;
    DialogMagnetophone dialogMagneto;
    QAction * actionKeyboard;
    QList<QAction *> actionSeparators;

    // Gestion sustain pedal
    QList<int> _listKeysToRelease;
    bool _isSustainOn;

    // Méthodes privées
    void updateTitle();
    int sauvegarder(int indexSf2, bool saveAs);
    void updateFavoriteFiles();
    void setKeyboardType(int val);
    void showKeyboard(bool val);
    QList<QAction *> getListeActions();

protected:
    void closeEvent(QCloseEvent *event);
};


#endif // MAINWINDOW_H
