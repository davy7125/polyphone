/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MAINWINDOWOLD_H
#define MAINWINDOWOLD_H


#include <QMainWindow>
#include <QDockWidget>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtConcurrent/QtConcurrent>
#else
#include <QFutureWatcher>
#include <QtConcurrentRun>
#endif

#include <QProgressDialog>
#include <QDir>
#include <QEvent>
#include <stdlib.h>
#include <stdio.h>
#include "soundfontmanager.h"
#include "config.h"
#include "dialog_list.h"
#include "dialog_about.h"
#include "pagesf2.h"
#include "pagesmpl.h"
#include "pageinst.h"
#include "pageprst.h"
#include "pageoverviewsmpl.h"
#include "pageoverviewinst.h"
#include "pageoverviewprst.h"
#include "audiodevice.h"
#include "synth.h"
#include "dialog_magnetophone.h"
class ModalProgressDialog;

namespace Ui
{
    class MainWindowOld;
    class Tree;
}

class MainWindowOld : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindowOld *ui;
    explicit MainWindowOld(QWidget *parent = 0);
    ~MainWindowOld();

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
    void dragAndDrop(EltID idDest, QList<EltID> idSources);
    void setRangeAndRootKey(int rootKey, int noteMin, int noteMax);
    void clearKeyboardCustomisation();
    void spaceKeyPressedInTree();

public slots:
    void supprimerElt();        // Suppression des éléments sélectionnés dans l'arbre
    void copier();              // Envoi du signal "copier"
    void coller();              // Envoi du signal "coller"

private slots:
    void noteNameChanged();

    void renommer();            // Renommer un ou plusieurs éléments dans l'arborescence
    void renommerEnMasse(int renameType, QString text1, QString text2, int val1, int val2);
    void importerSmpl();        // Import d'un sample
    void exporterSmpl();        // Export d'un sample
    void exporter();            // Export d'un ou plusieurs preset(s)
    void exporter(QList<QList<EltID> > listID, QString dir, int format,
                  bool presetPrefix, bool bankDir, bool gmSort, int quality);
    int exporter2(QList<QList<EltID> > listID, QString dir, int format, int flags, int quality);
    void nouvelInstrument();    // Création d'un instrument
    void nouveauPreset();       // Création d'un preset
    void associer();            // Association InstSmpl et PrstInst
    void remplacer();           // Remplacement InstSmpl et PrstInst
    void supprimer();           // Envoi du signal "supprimer"

    void showAbout();           // Affichage fenêtre à propos
    void AfficherBarreOutils(); // Clic sur l'action "barre d'outils" du menu "afficher"
    void afficherSectionModulateurs();
    void setKeyboardType0();    // Clic sur clavier -> aucun
    void setKeyboardType1();    // Clic sur clavier -> 5 octaves
    void setKeyboardType2();    // Clic sur clavier -> 6 octaves
    void setKeyboardType3();    // Clic sur clavier -> 128 notes
    void on_action88_notes_triggered();
    void on_actionDans_la_barre_d_outils_triggered();
    void on_action_Flottant_triggered();
    void undo();                // Clic sur l'action "undo"
    void redo();                // Clic sur l'action "redo"
    void sauvegarder();         // Clic sur l'action "sauvegarder"
    void sauvegarderSous();     // Clic sur l'action "sauvegarder sous"

    void changeVolume();        // outil sample, change volume
    void filtre();              // outil sample, filtre "mur de brique"
    void reglerBalance();       // outil sample, réglage de la balance (samples liés)
    void transposer();          // outil sample, transposition
    void desaccorder();         // outil instrument, désaccordage ondulant
    void spatialisation();      // outil instrument, spatialisation du son
    void duplication();         // outil instrument et preset, duplication des divisions
    void paramGlobal();         // outil instrument et preset, modification globale d'un paramètre
    void visualize();           // outil instrument et preset, visualise les paramètres utilisés dans un graphique
    void on_action_Transposer_triggered();
    void attenuationMini();     // global tool, mise à jour de toutes les atténuations
    void attenuationMini(double value, double valuePrst);
    void purger();              // global tool, suppression des éléments non utilisés
    void associationAutoSmpl(); // global tool, association auto gauche-droite des samples
    void exportPresetList();    // global tool
    void magnetophone();        // affichage du magnétophone
    void on_action_Dissocier_les_samples_st_r_o_triggered();
    void on_actionExporter_pics_de_fr_quence_triggered();
    void onPleinEcranTriggered();

    void noteOff(int key);
    void noteHover(int key, int vel);
    void setSustain(bool isOn);
    void setVolume(int vol);
    void noteChanged(int key, int vel);

    void on_actionEnlever_tous_les_modulateurs_triggered();
    void onAudioConnectionDone();
    void futureFinished();

signals:
    void stopAudio();
    void play(int type, int idSf2, int idElt, int note, int velocity);

private:
    PageSf2 * page_sf2;
    PageSmpl * page_smpl;
    PageInst * page_inst;
    PagePrst * page_prst;
    PageOverviewSmpl * _pageOverviewSmpl;
    PageOverviewInst * _pageOverviewInst;
    PageOverviewPrst * _pageOverviewPrst;
    SoundfontManager * sf2;
    Synth * synth;
    AudioDevice * audioDevice;
    DialogAbout about;
    DialogList dialList;
    DialogMagnetophone dialogMagneto;
    QAction * actionKeyboard;
    QList<QAction *> actionSeparators;
    QString _title;
    int _currentKey;
    QDialog _dialKeyboard;
    QByteArray _geometryDialKeyboard;
    QFutureWatcher<int> _futureWatcher;
    ModalProgressDialog * _progressDialog;

    // Gestion sustain pedal
    QList<int> _listKeysToRelease;
    bool _isSustainOn;

    // Méthodes privées
    void importerSmpl(QString path, EltID id, int *replace);
    void exporterFrequences(QString fileName);
    int sauvegarder(int indexSf2, bool saveAs);
    void updateFavoriteFiles();
    void setKeyboardType(int val);
    void showKeyboard(bool val);
    QList<QAction *> getListeActions();
    static QString getName(QString name, int maxCharacters, int suffixNumber);
    int deleteMods(EltID id);

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
