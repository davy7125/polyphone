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
#include "configpanel.h"
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
#include "dialogrecorder.h"
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

    void associer(EltID idDest);
    void remplacer(EltID idSrc);
    void dragAndDrop(EltID idDest, QList<EltID> idSources);
    void spaceKeyPressedInTree();

public slots:
    void supprimerElt();        // Suppression des éléments sélectionnés dans l'arbre
    void copier();              // Envoi du signal "copier"
    void coller();              // Envoi du signal "coller"

private slots:
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

    void spatialisation();      // outil instrument, spatialisation du son
    void duplication();         // outil instrument et preset, duplication des divisions
    void visualize();           // outil instrument et preset, visualise les paramètres utilisés dans un graphique
    void on_action_Transposer_triggered();
    void purger();              // global tool, suppression des éléments non utilisés
    void exportPresetList();    // global tool
    void on_actionExporter_pics_de_fr_quence_triggered();
    void onPleinEcranTriggered();
    void futureFinished();

signals:
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
    DialogAbout about;
    DialogList dialList;
    QAction * actionKeyboard;
    QList<QAction *> actionSeparators;
    QString _title;
    QFutureWatcher<int> _futureWatcher;
    ModalProgressDialog * _progressDialog;

    // Méthodes privées
    void importerSmpl(QString path, EltID id, int *replace);
    void exporterFrequences(QString fileName);
    int sauvegarder(int indexSf2, bool saveAs);
    static QString getName(QString name, int maxCharacters, int suffixNumber);
};

#endif // MAINWINDOW_H
