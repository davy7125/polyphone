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

#define VERSION "0.4"

#include <QMainWindow>
#include <QDir>
#include <stdlib.h>
#include <stdio.h>
#include "pile_sf2.h"
#include "config.h"
#include "dialog_list.h"
#include "dialog_help.h"
#include "page_sf2.h"
#include "page_smpl.h"
#include "page_inst.h"
#include "page_prst.h"
#include "audiodevice.h"
#include "synth.h"
#include "pianokeybdcustom.h"

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
    void anticipateNewAction();
    void updateDo();
    void updateActions();
    void associer(EltID idDest);
    void remplacer(EltID idSrc);
    void desactiveOutilsSmpl();
    void activeOutilsSmpl();
    bool isPlaying();
    void ouvrir(QString fileName);
    QStringList getListMidi();
    void openMidiPort(int val);
    void setAudioEngine(int audioEngine);
    void setSynthGain(int val);
public slots:
    void supprimerElt();        // Suppression des éléments sélectionnés dans l'arbre
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
    void dragAndDrop(EltID idDest, EltID idSrc, int temps, int *msg, QByteArray *ba1, QByteArray *ba2);
    void importerSmpl();        // Import d'un sample
    void exporterSmpl();        // Export d'un sample
    void nouvelInstrument();    // Création d'un instrument
    void nouveauPreset();       // Création d'un preset
    void associer();            // Association InstSmpl et PrstInst
    void remplacer();           // Remplacement InstSmpl et PrstInst
    void copier();              // Envoi du signal "copier"
    void coller();              // Envoi du signal "coller"
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
    void desaccorder();         // outil instrument, désaccordage ondulant
    void duplication();         // outil instrument, duplication des divisions
    void paramGlobal();         // outil instrument, modification globale d'un paramètre
    void repartitionAuto();     // outil instrument, répartition automatique keyrange
    void spatialisation();      // outil instrument, spatialisation du son
    void mixture();             // outil instrument, création mixture
    void release();             // outil instrument, élaboration de releases
    void duplicationPrst();     // outil preset, duplication des divisions
    void paramGlobal2();        // outil preset, modification globale d'un paramètre
    void attenuationMini();     // outil sf2, mise à jour de toutes les atténuations
    void purger();              // outil sf2, suppression des éléments non utilisés
    void sifflements();         // outil sf2, suppression des sifflements

    void noteOn(int key);
    void noteOff(int key);
    void noteChanged(int key, int vel);

signals:
    void initAudio(int numDevice);
    void play(int type, int idSf2, int idElt, int note, int velocity);

private:
    Page_Sf2 * page_sf2;
    Page_Smpl * page_smpl;
    Page_Inst * page_inst;
    Page_Prst * page_prst;
    Pile_sf2 * sf2;
    Synth * synth;
    AudioDevice * audioDevice;
    QThreadEx audioThread;
    QThreadEx synthThread;
    QString fileName;
    Config configuration;
    DialogHelp help;
    DialogList dialList;
    PianoKeybdCustom * keyboard;

    // Méthodes privées
    void updateTitle();
    int sauvegarder(int indexSf2, bool saveAs);
    void updateFavoriteFiles();
    void setKeyboardType(int val);
    void showKeyboard(bool val);

protected:
    void closeEvent(QCloseEvent *event);
};


#endif // MAINWINDOW_H
