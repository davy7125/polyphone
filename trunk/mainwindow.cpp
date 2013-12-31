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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sound.h"
#include "dialog_rename.h"
#include "conversion_sfz.h"
#include "dialog_export.h"
#include "duplicator.h"
#include "import_sfz.h"
#include <QFileDialog>
#include <QInputDialog>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif

// Constructeurs, destructeurs
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent, Qt::Window | Qt::WindowCloseButtonHint |
                                                      Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint |
                                                      Qt::WindowSystemMenuHint | Qt::WindowTitleHint |
                                                      Qt::CustomizeWindowHint),
    ui(new Ui::MainWindow),
    synth(NULL),
    audioDevice(NULL),
    configuration(Config::getInstance(this)),
    help(this),
    about(this),
    dialList(this),
    keyboard(NULL),
    dialogMagneto(NULL), // doit être appelé après le premier appel au singleton Config
    actionKeyboard(NULL),
    _currentKey(-1),
    _isSustainOn(false)
{
    ui->setupUi(this);
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    ui->editSearch->setPlaceholderText(trUtf8("Rechercher..."));
#endif
    // Taille max de l'application et restauration de l'état de la fenêtre
    this->setMaximumSize(QApplication::desktop()->size());
    restoreGeometry(configuration->getWindowGeometry());
    restoreState(configuration->getWindowState());

    // Initialisation de l'objet pile sf2
    this->sf2 = new Pile_sf2(ui->arborescence, this->configuration->getRam(), this);

    // Connexion avec mise à jour table
    connect(this->sf2, SIGNAL(updateTable(int,int,int,int)), this, SLOT(updateTable(int,int,int,int)));

    // Initialisation du synthétiseur
    this->synth = new Synth(this->sf2);
    this->synth->moveToThread(&this->synthThread);
    this->synthThread.start();

    // Connexions du magnétophone avec le synthé
    this->dialogMagneto.setSynth(this->synth);
    connect(this->synth, SIGNAL(sampleRateChanged(qint32)), &dialogMagneto, SLOT(setSampleRate(qint32)));
    connect(this->synth, SIGNAL(samplesRead(int)), &dialogMagneto, SLOT(avanceSamples(int)));

    // Initialisation de la sortie audio
    this->audioDevice = new AudioDevice(this->synth);
    connect(this, SIGNAL(initAudio(int, int)), this->audioDevice, SLOT(initAudio(int, int)));
    connect(this, SIGNAL(stopAudio()), this->audioDevice, SLOT(closeConnections()), Qt::BlockingQueuedConnection);
    this->audioDevice->moveToThread(&this->audioThread);
    this->audioThread.start();

    // Priorité des thread
    this->audioThread.setPriority(QThread::HighestPriority);
    this->synthThread.setPriority(QThread::TimeCriticalPriority);

    if (this->configuration->getAudioType() == 0)
        this->setAudioEngine(configuration->getAudioIndex(), configuration->getBufferSize());
    else
        this->setAudioEngine(configuration->getAudioType(), configuration->getBufferSize());
    this->setSynthGain(this->configuration->getSynthGain());
    this->setSynthChorus(this->configuration->getSynthChoLevel(),
                         this->configuration->getSynthChoDepth(),
                         this->configuration->getSynthChoFrequency());
    this->setSynthReverb(this->configuration->getSynthRevLevel(),
                         this->configuration->getSynthRevSize(),
                         this->configuration->getSynthRevWidth(),
                         this->configuration->getSynthRevDamp());

    // Création des widgets
    page_sf2 = new Page_Sf2(this, ui->arborescence, ui->stackedWidget, this->sf2, this->synth);
    page_smpl = new Page_Smpl();
    page_inst = new Page_Inst();
    page_prst = new Page_Prst();
    ui->stackedWidget->addWidget(page_sf2);
    ui->stackedWidget->addWidget(page_smpl);
    ui->stackedWidget->addWidget(page_inst);
    ui->stackedWidget->addWidget(page_prst);

    // Initialisation arbre (passage de l'adresse de mainWindow)
    ui->arborescence->init(this, sf2);

    // Initialisation dialog liste (pointeur vers les sf2 et mainWindow)
    this->dialList.init(this, this->sf2);

    // Fichiers récents
    updateFavoriteFiles();

    // Affichage logo logiciel
    ui->stackedWidget->setCurrentWidget(ui->page_Soft);

    // Préférences d'affichage
    if (!this->configuration->getAfficheToolBar())
    {
        this->ui->actionBarre_d_outils->setChecked(0);
        ui->toolBar->setVisible(0);
    }
    if (!this->configuration->getAfficheMod())
    {
        this->ui->actionSection_modulateurs->setChecked(0);
        this->page_inst->setModVisible(0);
        this->page_prst->setModVisible(0);
    }

    // Clavier
    this->keyboard = new PianoKeybdCustom(this);
    QHBoxLayout * layout = (QHBoxLayout*)this->ui->ensembleKeyboard->layout();
    layout->insertWidget(2, this->keyboard);
    this->setKeyboardType(this->configuration->getKeyboardType());

    // Déplacement dans la barre de menu
    this->ui->toolBar->setContentsMargins(0, 0, 0, 0);
    ui->toolBar->setFixedHeight(40);
    this->ui->ensembleKeyboard->setMaximumHeight(this->ui->toolBar->height() + 0);
    this->keyboard->setMaximumHeight(this->ui->toolBar->height() + 0);
    this->ui->velocityButton->setMaximumHeight(this->ui->toolBar->height() + 0);
    actionKeyboard = this->ui->toolBar->addWidget(this->ui->ensembleKeyboard);
    this->showKeyboard(false);
    this->ui->velocityButton->setValue(this->configuration->getKeyboardVelocity());

    // Ouverture port midi et connexions
    this->keyboard->openMidiPort(this->configuration->getNumPortMidi());
    connect(this->keyboard, SIGNAL(keyChanged(int,int)), this, SLOT(noteChanged(int,int)));
    connect(this->keyboard, SIGNAL(sustainChanged(bool)), this, SLOT(setSustain(bool)));
    connect(this->keyboard, SIGNAL(volumeChanged(int)), this, SLOT(setVolume(int)));
    connect(this->keyboard, SIGNAL(noteOn(int)), this, SLOT(noteOn(int)));
    connect(this->keyboard, SIGNAL(noteOff(int)), this, SLOT(noteOff(int)));
    connect(this->keyboard, SIGNAL(mouseOver(int)), this, SLOT(noteHover(int)));
    connect(this->page_smpl, SIGNAL(noteChanged(int,int)), this, SLOT(noteChanged(int,int)));

    // Connexion changement de couleur
    connect(Config::getInstance(), SIGNAL(colorsChanged()), this->page_smpl, SLOT(updateColors()));

    // Initialisation des actions dans les configurations
    this->configuration->setListeActions(this->getListeActions());

    // Passage du mapper au clavier
    this->keyboard->setMapper(this->configuration->getMapper());

    QDate date = QDate::currentDate();
    if (date.month() == 10 && date.day() == 31)
        this->ui->label->setPixmap(QPixmap(":/logo/halloween"));
    else if (date.month() == 12 && (date.day() == 24 || date.day() == 25))
        this->ui->label->setPixmap(QPixmap(":/logo/noel"));
    else if (date.month() == 2 && date.day() == 14)
        this->ui->label->setPixmap(QPixmap(":/logo/valentin"));

    // Initialisation objet Sound
    Sound::setParent(this);

    // Bug QT: restauration de la largeur d'un QDockWidget si fenêtre maximisée
    QApplication::processEvents();
    int dockWidth = configuration->getDockWidth();
    if (ui->dockWidget->width() < dockWidth)
        ui->dockWidget->setMinimumWidth(dockWidth);
    else
        ui->dockWidget->setMaximumWidth(dockWidth);
    QTimer::singleShot(1, this, SLOT(returnToOldMaxMinSizes()));
}
MainWindow::~MainWindow()
{
    // Arrêt audio et fin du synthé
    this->stopAudio();
    this->synth->interruption();
    delete this->sf2;
    delete this->page_inst;
    delete this->page_prst;
    delete this->page_sf2;
    delete this->page_smpl;
    // Fin des threads (200 ms pour quitter)
    this->audioThread.quit();
    this->synthThread.quit();
    this->audioThread.wait(200);
    this->synthThread.wait(200);
    delete this->synth;
    delete this->audioDevice;
    delete this->keyboard;
    Config::kill();
    delete ui;
}

void MainWindow::returnToOldMaxMinSizes()
{
    ui->dockWidget->setMinimumWidth(150);
    ui->dockWidget->setMaximumWidth(300);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Sauvegarde de la géométrie
    configuration->setDockWidth(ui->dockWidget->width());
    configuration->setWindowGeometry(saveGeometry());
    configuration->setWindowState(saveState());

    // Nombre de fichiers non sauvegardés
    int nbFile = 0;
    EltID id(elementSf2, -1, 0, 0, 0);
    int nbSf2 = sf2->count(id);
    QString qStr = "";
    for (int i = 0; i < nbSf2; i++)
    {
        id.indexSf2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->isEdited(i))
            {
                if (nbFile)
                    qStr.append("<br /> - " + sf2->getQstr(id, champ_name));
                else
                    qStr = sf2->getQstr(id, champ_name);
                nbFile++;
            }
        }
    }
    if (nbFile)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(trUtf8("<b>Sauvegarder avant de quitter ?</b>"));
        if (nbFile > 1)
        {
            qStr.prepend(trUtf8("Les fichiers suivants ont été modifiés :<br/> - "));
            msgBox.setInformativeText(qStr);
        }
        else
        {
            qStr = trUtf8("Le fichier « ") + qStr + trUtf8(" » a été modifié.");
            msgBox.setInformativeText(qStr);
        }
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Enregistrer"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Annuler"));
        msgBox.button(QMessageBox::Discard)->setText(trUtf8("&Quitter sans enregistrer"));
        msgBox.button(QMessageBox::Save)->setIcon(QIcon::fromTheme("filesave"));
        msgBox.setDefaultButton(QMessageBox::Save);
        switch (msgBox.exec())
        {
        case QMessageBox::Cancel:
            event->ignore();
            return;
        case QMessageBox::Save:
            for (int i = 0; i < nbSf2; i++)
            {
                id.indexSf2 = i;
                if (!sf2->get(id, champ_hidden).bValue)
                    if (sf2->isEdited(i))
                        if (sauvegarder(i, 0))
                        {
                            event->ignore();
                            return;
                        }
            }
            break;
        }
    }
    dialogMagneto.close();
}
void MainWindow::resizeEvent(QResizeEvent *)
{
    QString titre;
    if (_title.isEmpty())
        titre = "Polyphone";
    else
    {
        QString path = _title;
        if (_title.indexOf("*") == 0)
        {
            titre = "*";
            path = path.right(path.size() - 1);
        }
        else
            titre = "";

        QFileInfo fileInfo(path);
        path = fileInfo.path();
        if (path.indexOf(QDir::homePath()) == 0)
            path = "~" + path.right(path.size() - QDir::homePath().size());
        QString titreCourt = titre + fileInfo.fileName() + " - Polyphone";
        QString titreLong = titre + fileInfo.fileName() + " (" + path + ") - Polyphone";

        QFont font = QApplication::font("QWorkspaceTitleBar");
        QFontMetrics fm(font);
        if (fm.width(titreLong) > this->width() - 100)
            titre = titreCourt;
        else
            titre = titreLong;
    }

    this->setWindowTitle(titre);
}

// Ouverture, fermeture, suppression
void MainWindow::ouvrir()
{
    // Chargement d'un fichier .sf2
    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Ouvrir une soundfont"),
                                                    Config::getInstance()->getLastDirectory(Config::typeFichierSf2),
                                                    trUtf8("Fichier .sf2 (*.sf2)"));
    if (fileName.isNull()) return;
    ouvrir(fileName);
}
void MainWindow::ouvrirFichier1() {ouvrir(this->configuration->getLastFile(Config::typeFichierSf2, 0));}
void MainWindow::ouvrirFichier2() {ouvrir(this->configuration->getLastFile(Config::typeFichierSf2, 1));}
void MainWindow::ouvrirFichier3() {ouvrir(this->configuration->getLastFile(Config::typeFichierSf2, 2));}
void MainWindow::ouvrirFichier4() {ouvrir(this->configuration->getLastFile(Config::typeFichierSf2, 3));}
void MainWindow::ouvrirFichier5() {ouvrir(this->configuration->getLastFile(Config::typeFichierSf2, 4));}
void MainWindow::ouvrir(QString fileName)
{
    // Chargement d'un fichier .sf2
    this->sf2->prepareNewActions();
    switch (this->sf2->ouvrir(fileName))
    {
    case 0:
        // le chargement s'est bien déroulé
        this->configuration->addFile(Config::typeFichierSf2, fileName);
        updateFavoriteFiles();
        updateDo();
        ui->arborescence->clearSelection();
        this->ui->arborescence->searchTree(this->ui->editSearch->text());
        updateActions();
        break;
    case 1:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Format inconnu."));
        updateDo();
        break;
    case 2:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier est déjà chargé."));
        updateDo();
        break;
    case 3:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Impossible d'ouvrir le fichier."));
        updateDo();
        break;
    case 4:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Lecture impossible."));
        updateDo();
        break;
    case 5: case 6:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier est corrompu."));
        updateDo();
        break;
    }
}
void MainWindow::nouveau()
{
    bool ok;
    QString name = QInputDialog::getText(this, " ", trUtf8("Nom de la nouvelle soundfont :"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty())
    {
        sf2->prepareNewActions();
        sf2->nouveau(name.left(20));
        updateDo();
        ui->arborescence->clearSelection();
        this->ui->arborescence->searchTree(this->ui->editSearch->text());
        updateActions();
    }
}
void MainWindow::Fermer()
{
    // Fermeture SF2
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    if (!ui->arborescence->isSelectedItemsSf2Unique()) return;
    int ret;
    EltID id = ui->arborescence->getID(0);
    id.typeElement = elementSf2;
    if (sf2->isEdited(id.indexSf2))
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        id.typeElement = elementSf2;
        QString qStr = trUtf8("<b>Sauvegarder avant de quitter ?</b>");
        msgBox.setText(qStr);
        qStr = trUtf8("Le fichier « ") + sf2->getQstr(id,champ_name) + trUtf8(" » a été modifié.");
        msgBox.setInformativeText(qStr);
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Enregistrer"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Annuler"));
        msgBox.button(QMessageBox::Discard)->setText(trUtf8("&Fermer sans enregistrer"));
        msgBox.button(QMessageBox::Save)->setIcon(QIcon::fromTheme("filesave"));
        msgBox.setDefaultButton(QMessageBox::Save);
        ret = msgBox.exec();
    }
    else ret = QMessageBox::Discard;
    switch (ret)
    {
    case QMessageBox::Cancel: return;
    case QMessageBox::Save:
        if (sauvegarder(id.indexSf2, 0)) return;
    case QMessageBox::Discard:{
        sf2->prepareNewActions();
        // Reprise id si modif
        id = ui->arborescence->getID(0);
        id.typeElement = elementSf2;
        this->ui->arborescence->selectNone();
        sf2->remove(id);
        updateActions();
        updateDo();
        this->page_inst->clearTable();
        this->page_prst->clearTable();
    }break;
    }
}
void MainWindow::supprimerElt()
{
    // Suppression d'un ou plusieurs éléments dans l'arborescence
    int nb = ui->arborescence->getSelectedItemsNumber() - 1;
    if (nb >= 0) sf2->prepareNewActions();
    int message = 1;
    for (int i = nb; i >=0; i--)
        sf2->remove(ui->arborescence->getID(i), &message);
    updateActions();
    updateDo();
}

// Sauvegarde, undo, redo
void MainWindow::sauvegarder()
{
    // Sauvegarde d'un SF2 (appel de l'interface)
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    if (!ui->arborescence->isSelectedItemsSf2Unique()) return;
    EltID id = ui->arborescence->getID(0);
    sauvegarder(id.indexSf2, 0);
}
void MainWindow::sauvegarderSous()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    if (!ui->arborescence->isSelectedItemsSf2Unique()) return;
    EltID id = ui->arborescence->getID(0);
    sauvegarder(id.indexSf2, 1);
}
int  MainWindow::sauvegarder(int indexSf2, bool saveAs)
{
    this->sf2->prepareNewActions();
    EltID id(elementSf2, indexSf2, 0, 0, 0);
    // Avertissement si enregistrement dans une résolution inférieure
    if (sf2->get(id, champ_wBpsSave).wValue < sf2->get(id, champ_wBpsInit).wValue)
    {
        int ret = QMessageBox::Save;
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        char T[20];
        sprintf(T,"%d &#8658; %d", sf2->get(id, champ_wBpsInit).wValue, sf2->get(id, champ_wBpsSave).wValue);
        msgBox.setText( trUtf8("<b>Perte de résolution ") + QString(T) + trUtf8(" bits</b>"));
        msgBox.setInformativeText(trUtf8("La qualité des samples sera abaissée suite à cette opération. Continuer ?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Oui"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Non"));
        msgBox.setDefaultButton(QMessageBox::Cancel);
        ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) return 1;
    }

    // Compte du nombre de générateurs utilisés
    int unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen;
    this->page_sf2->compte(unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen);
    if ((instGen > 65536 || prstGen > 65536) && Config::getInstance()->getActivationSaveWarning_toManyGenerators())
    {
        int ret = QMessageBox::Save;
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        QString texte;
        if (instGen > 65536 && prstGen > 65536)
            texte = trUtf8("<b>Trop de paramètres dans les instruments et les presets.</b>");
        else if (instGen > 65536)
            texte = trUtf8("<b>Trop de paramètres dans les instruments.</b>");
        else
            texte = trUtf8("<b>Trop de paramètres dans les presets.</b>");
        msgBox.setText(texte);
        msgBox.setInformativeText(trUtf8("Certains synthétiseurs ne prennent pas en compte "
                                         "les paramètres au delà du 65536ème.\n"
                                         "Diviser le fichier en plusieurs sf2 peut résoudre le problème."));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save | QMessageBox::YesAll);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Sauvegarder"));
        msgBox.button(QMessageBox::YesAll)->setText(trUtf8("Sauvegarder, &désactiver ce message"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Annuler"));
        msgBox.setDefaultButton(QMessageBox::Cancel);
        ret = msgBox.exec();
        if (ret == QMessageBox::Cancel) return 1;
        if (ret == QMessageBox::YesAll)
            Config::getInstance()->setActivationSaveWarning_toManyGenerators(false);
    }

    QString fileName;
    if (saveAs || sf2->getQstr(id, champ_filename) == "")
    {
        if (sf2->getQstr(id, champ_filename) == "")
        {
            fileName = Config::getInstance()->getLastDirectory(Config::typeFichierSf2) + "/" + sf2->getQstr(id, champ_name) + ".sf2";
            fileName = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder une soundfont"), fileName, trUtf8("Fichier .sf2 (*.sf2)"));
        }
        else
            fileName = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder une soundfont"),
                                                    sf2->getQstr(id, champ_filename), trUtf8("Fichier .sf2 (*.sf2)"));
        if (fileName.isNull()) return 1;
    }
    else fileName = sf2->getQstr(id, champ_filename);
    switch (this->sf2->sauvegarder(indexSf2, fileName))
    {
    case 0:
        // sauvegarde ok
        updateDo();
        this->configuration->addFile(Config::typeFichierSf2, fileName);
        updateFavoriteFiles();
        if (ui->stackedWidget->currentWidget() == this->page_sf2)
            this->page_sf2->afficher();
        return 0;
        break;
    case 1:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Extension inconnue."));
        break;
    case 2:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Fichier déjà ouvert, impossible de sauvegarder."));
        break;
    case 3:
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Impossible d'enregistrer le fichier."));
        break;
    }
    return 1;
}
void MainWindow::undo()
{
    this->ui->arborescence->clearPastedID();
    sf2->undo();
    updateActions();
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::redo()
{
    this->ui->arborescence->clearPastedID();
    sf2->redo();
    updateActions();
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}

// Fenetres / affichage
void MainWindow::showConfig()
{
    this->configuration->setWindowModality(Qt::ApplicationModal);
    this->configuration->show();
}
void MainWindow::showAbout()
{
    about.show();
}
void MainWindow::showHelp()
{
    this->help.show();
}
void MainWindow::AfficherBarreOutils()
{
    if (ui->actionBarre_d_outils->isChecked())
    {
        this->configuration->setAfficheToolBar(1);
        ui->toolBar->setVisible(1);
    }
    else
    {
        this->configuration->setAfficheToolBar(0);
        ui->toolBar->setVisible(0);
    }
}
void MainWindow::afficherSectionModulateurs()
{
    if (ui->actionSection_modulateurs->isChecked())
    {
        this->configuration->setAfficheMod(1);
        this->page_inst->setModVisible(1);
        this->page_prst->setModVisible(1);
    }
    else
    {
        this->configuration->setAfficheMod(0);
        this->page_inst->setModVisible(0);
        this->page_prst->setModVisible(0);
    }
}
void MainWindow::setKeyboardType0()     {this->setKeyboardType(0);}
void MainWindow::setKeyboardType1()     {this->setKeyboardType(1);}
void MainWindow::setKeyboardType2()     {this->setKeyboardType(2);}
void MainWindow::setKeyboardType3()     {this->setKeyboardType(3);}
void MainWindow::setKeyboardType(int val)
{
    this->ui->actionAucun->blockSignals(true);
    this->ui->action5_octaves->blockSignals(true);
    this->ui->action6_octaves->blockSignals(true);
    this->ui->action128_notes->blockSignals(true);
    this->ui->actionAucun->setChecked(false);
    this->ui->action5_octaves->setChecked(false);
    this->ui->action6_octaves->setChecked(false);
    this->ui->action128_notes->setChecked(false);
    switch (val)
    {
    case 1:
        // Clavier 5 octaves
        this->keyboard->setKeyboardType(PianoKeybdCustom::KEYBOARD_5_OCTAVES);
        this->ui->action5_octaves->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    case 2:
        // Clavier 6 octaves
        this->keyboard->setKeyboardType(PianoKeybdCustom::KEYBOARD_6_OCTAVES);
        this->ui->action6_octaves->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    case 3:
        // Clavier 128 notes
        this->keyboard->setKeyboardType(PianoKeybdCustom::KEYBOARD_128_NOTES);
        this->ui->action128_notes->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    default:
        // Aucun clavier
        this->ui->actionAucun->setChecked(true);
        this->showKeyboard(false);
        break;
    }
    this->ui->actionAucun->blockSignals(false);
    this->ui->action5_octaves->blockSignals(false);
    this->ui->action6_octaves->blockSignals(false);
    this->ui->action128_notes->blockSignals(false);
    // Sauvegarde du paramètre
    this->configuration->setKeyboardType(val);
}
void MainWindow::magnetophone()
{
    dialogMagneto.show();
}
QList<QAction *> MainWindow::getListeActions()
{
    QList<QAction *> listeAction;
    listeAction << ui->actionNouveau
                << ui->actionOuvrir
                << ui->actionEnregistrer
                << ui->actionEnregistrer_sous
                << ui->actionImporter
                << ui->actionExporter
                << ui->actionFermer_le_fichier
                << ui->actionNouvel_instrument
                << ui->actionNouveau_preset
                << ui->actionAnnuler
                << ui->actionR_tablir
                << ui->actionCopier
                << ui->actionColler
                << ui->action_Supprimer
                << ui->actionRenommer
                << ui->actionPr_f_rences
                << ui->actionAjuster_la_fin_de_boucle
                << ui->actionBouclage_automatique
                << ui->action_Diminuer_sifflements
                << ui->actionEnlever_blanc_au_d_part
                << ui->actionFiltre_mur_de_brique
                << ui->actionNormaliser_volume
                << ui->action_R_glage_balance
                << ui->actionTransposer
                << ui->action_Cr_ation_mutation_mixture
                << ui->actionD_saccordage_ondulant
                << ui->actionD_uplication_des_divisions
                << ui->action_laboration_release
                << ui->action_Param_trage_global
                << ui->action_R_partition_automatique
                << ui->actionSpacialisation_du_son
                << ui->action_Association_auto_samples
                << ui->action_Enlever_les_l_ments_non_utilis_s
                << ui->actionR_gler_att_nuation_minimale
                << ui->actionMagn_tophone
                << ui->actionSommaire
                << ui->action_Visualiseur
                << ui->actionExporter_en_tant_qu_sfz
                << ui->actionI_mporter_soundfont
                << ui->action_Dissocier_les_samples_st_r_o
                << ui->actionExporter_pics_de_fr_quence;
    return listeAction;
}
void MainWindow::setListeActions(QList<QAction *> listeActions)
{
    // On vide la barre d'outils
    QList<QAction *> actions = this->getListeActions();
    for (int i = 0; i < actions.size(); i++)
        this->ui->toolBar->removeAction(actions.at(i));
    this->ui->toolBar->removeAction(actionKeyboard);
    int size = actionSeparators.size();
    for (int i = 0; i < size; i++)
    {
        this->ui->toolBar->removeAction(actionSeparators.at(0));
        delete actionSeparators.takeFirst();
    }

    // Ajout des actions
    for (int i = 0; i < listeActions.size(); i++)
    {
        if (listeActions.at(i))
            this->ui->toolBar->addAction(listeActions.at(i));
        else
        {
            // Ajout d'un séparateur
            actionSeparators << this->ui->toolBar->addSeparator();
        }
    }
    this->ui->toolBar->addAction(actionKeyboard);
}

// Mise à jour
void MainWindow::updateDo()
{
    // Suppressions des listes d'actions vides
    sf2->cleanActions();
    // Affichage undo / redo et titre
    if (sf2->isUndoable()) ui->actionAnnuler->setEnabled(1);
    else ui->actionAnnuler->setEnabled(0);
    if (sf2->isRedoable()) ui->actionR_tablir->setEnabled(1);
    else ui->actionR_tablir->setEnabled(0);
    updateTitle();
}
void MainWindow::updateTitle()
{
    // Nombre d'éléments sélectionnés
    int nb = ui->arborescence->getSelectedItemsNumber();
    EltID id(elementSf2, 0, 0, 0, 0);
    bool fichierUnique = 0;
    if (nb > 0)
    {
        // Au moins un élément est sélectionné
        id.indexSf2 = ui->arborescence->getID(0).indexSf2;
        fichierUnique = ui->arborescence->isSelectedItemsSf2Unique();
    }
    if (fichierUnique)
    {
        if (sf2->isEdited(id.indexSf2))
        {
            _title = "*" + sf2->getQstr(id, champ_filename);
            ui->actionEnregistrer->setEnabled(1);
        }
        else
        {
            _title = sf2->getQstr(id, champ_filename);
            ui->actionEnregistrer->setEnabled(0);
        }
    }
    else
    {
        _title = "";
        ui->actionEnregistrer->setEnabled(0);
    }
    resizeEvent(NULL);
}

void MainWindow::updateActions()
{
    // Un élément est cliqué dans l'arborescence
    int nb;
    bool fichierUnique = 1;
    bool familleUnique = 1;
    bool typeUnique = 1;
    ElementType type;
    EltID id;
    // Modification du titre
    updateTitle();
    // Caractéristiques de la sélection
    // Nombre d'éléments sélectionnés
    nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0)
    {
        // Rien n'est sélectionné
        fichierUnique = 0;

        // Affichage page logiciel
        ui->stackedWidget->setCurrentWidget(ui->page_Soft);
    }
    else
    {
        // Au moins un élément est sélectionné
        id = ui->arborescence->getID(0);
        type = id.typeElement;
        fichierUnique = ui->arborescence->isSelectedItemsSf2Unique();
        typeUnique = ui->arborescence->isSelectedItemsTypeUnique();
        familleUnique = ui->arborescence->isSelectedItemsFamilyUnique();
        // Affichage partie droite
        if (typeUnique && fichierUnique && id.typeElement == elementSmpl)
        {
            // Affichage page Smpl
            page_smpl->afficher();
            if (this->configuration->getKeyboardType() && nb == 1)
                this->showKeyboard(true);
            else
                this->showKeyboard(false);
        }
        else if (fichierUnique && familleUnique)
        {
            if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            {
                // Affichage page Inst
                page_inst->afficher();
                if (this->configuration->getKeyboardType())
                    this->showKeyboard(true);
                else
                    this->showKeyboard(false);
            }
            else
            {
                // Affichage page Prst
                page_prst->afficher();
                if (this->configuration->getKeyboardType())
                    this->showKeyboard(true);
                else
                    this->showKeyboard(false);
            }
        }
        else
        {
            if (fichierUnique)
                page_sf2->afficher();
            else
                ui->stackedWidget->setCurrentWidget(ui->page_Soft);
            this->showKeyboard(false);
        }
    }

    // ACTIVATION, DESACTIVATION DES COMMANDES
    if (fichierUnique)
    {
        // Actions possibles : fermer, importer, coller, enregistrer sous, exporter sfz
        // Nouveau sample, instrument, preset
        ui->actionFermer_le_fichier->setEnabled(true);
        ui->actionImporter->setEnabled(true);
        ui->actionColler->setEnabled(true);
        ui->actionEnregistrer_sous->setEnabled(true);
        ui->actionExporter_en_tant_qu_sfz->setEnabled(true);
        ui->actionNouveau_preset->setEnabled(true);
        ui->actionNouvel_instrument->setEnabled(true);

        // Supprimer, copier
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique)
                           || type == elementSmpl || type == elementInst || type == elementPrst)
                && !this->page_smpl->isPlaying())
        {
            ui->action_Supprimer->setEnabled(1);
            ui->actionCopier->setEnabled(1);
        }
        else
        {
            ui->action_Supprimer->setEnabled(0);
            ui->actionCopier->setEnabled(0);
        }

        // Renommer
        if (nb == 1 && typeUnique && (type == elementSmpl || type == elementInst || type == elementPrst || type == elementSf2))
        {
            ui->actionRenommer->setText(trUtf8("&Renommer"));
            ui->actionRenommer->setEnabled(1);
        }
        else
        {
            if (nb > 1 && typeUnique && type == elementSmpl)
            {
                ui->actionRenommer->setText(trUtf8("&Renommer en masse"));
                ui->actionRenommer->setEnabled(true);
            }
            else
            {
                ui->actionRenommer->setText(trUtf8("&Renommer"));
                ui->actionRenommer->setEnabled(false);
            }
        }

        // Action exporter
        if ((typeUnique && type == elementSmpl) || (familleUnique && type == elementInstSmpl))
            ui->actionExporter->setEnabled(true);
        else
            ui->actionExporter->setEnabled(false);

        // Outils
        this->enableActionSample(typeUnique && type == elementSmpl && !this->page_smpl->isPlaying());
        this->enableActionInstrument((type == elementInst || type == elementInstSmpl) && familleUnique);
        this->enableActionPreset((type == elementPrst || type == elementPrstInst) && familleUnique);
        this->enableActionSf2(true);

        // Particularité 1 : "enlever éléments non utilisés" doit être désactivé si la lecture est en cours
        if (!this->page_smpl->isPlaying())
            ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(true);
        else
            ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(false);

        // Particularité 2 : "duplication", "paramétrage globale", "visualiseur" et "spatialiseur" sont communs aux
        // instruments et aux presets -> pas de désactivation si l'une des 2 conditions est remplie
        if (familleUnique && (type == elementInst || type == elementInstSmpl ||
                              type == elementPrst || type == elementPrstInst))
        {
            ui->actionDuplication_des_divisions->setEnabled(true);
            ui->actionD_uplication_des_divisions->setEnabled(true);
            ui->action_Param_trage_global->setEnabled(true);
            ui->action_Param_trage_global_2->setEnabled(true);
            ui->action_Visualiseur->setEnabled(true);
            ui->action_Visualiseur_2->setEnabled(true);
            ui->actionSpacialisation_du_son->setEnabled(true);
            ui->actionSpatialisation_du_son->setEnabled(true);
        }
    }
    else
    {
        // Action impossible : fermer
        ui->actionFermer_le_fichier->setEnabled(0);
        // Action impossible : copier, coller, supprimer
        ui->actionCopier->setEnabled(0);
        ui->actionColler->setEnabled(0);
        ui->action_Supprimer->setEnabled(0);
        // Actions importer, exporter
        ui->actionImporter->setEnabled(0);
        ui->actionExporter->setEnabled(0);
        // Actions enregistrer sous, exporter en sfz
        ui->actionEnregistrer_sous->setEnabled(0);
        ui->actionExporter_en_tant_qu_sfz->setEnabled(0);
        // Nouveau sample, instrument, preset
        ui->actionNouveau_preset->setEnabled(0);
        ui->actionNouvel_instrument->setEnabled(0);
        // Renommer, clavier, ...
        ui->actionRenommer->setEnabled(0);
        this->showKeyboard(false);
        // Outils
        this->enableActionSample(false);
        this->enableActionInstrument(false);
        this->enableActionPreset(false);
        this->enableActionSf2(false);
    }
}
void MainWindow::enableActionSample(bool isEnabled)
{
    ui->menuSample->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuSample->actions().size(); i++)
        ui->menuSample->actions().at(i)->setEnabled(isEnabled);
}
void MainWindow::enableActionInstrument(bool isEnabled)
{
    ui->menuInstrument->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuInstrument->actions().size(); i++)
        ui->menuInstrument->actions().at(i)->setEnabled(isEnabled);
}
void MainWindow::enableActionPreset(bool isEnabled)
{
    ui->menuPreset->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuPreset->actions().size(); i++)
        ui->menuPreset->actions().at(i)->setEnabled(isEnabled);
}
void MainWindow::enableActionSf2(bool isEnabled)
{
    ui->menuDivers->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuDivers->actions().size(); i++)
        ui->menuDivers->actions().at(i)->setEnabled(isEnabled);
}

void MainWindow::desactiveOutilsSmpl()
{
    // Appel depuis pageSmpl
    this->ui->action_Supprimer->setEnabled(0);
    this->ui->menuSample->setEnabled(0);
    this->ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(0);
    this->ui->arborescence->desactiveSuppression();
}
void MainWindow::activeOutilsSmpl()
{
    // Appel depuis pageSmpl
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb != 1)
    {
        this->updateActions();
        this->ui->arborescence->clicTree();
        return;
    }
    EltID ID = ui->arborescence->getID(0);
    ElementType type = ID.typeElement;
    if (type != elementSmpl)
    {
        this->updateActions();
        this->ui->arborescence->clicTree();
        return;
    }
    this->ui->action_Supprimer->setEnabled(1);
    this->ui->menuSample->setEnabled(1);
    this->ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(1);
    this->ui->arborescence->activeSuppression();
}
bool MainWindow::isPlaying()
{
    return this->page_smpl->isPlaying();
}
void MainWindow::updateFavoriteFiles()
{
    QAction *qAct = NULL;
    for (int i = 0; i < 5; i++)
    {
        switch (i)
        {
        case 0: qAct = ui->actionFichier_1; break;
        case 1: qAct = ui->actionFichier_2; break;
        case 2: qAct = ui->actionFichier_3; break;
        case 3: qAct = ui->actionFichier_4; break;
        case 4: qAct = ui->actionFichier_5; break;
        }
        if (!this->configuration->getLastFile(Config::typeFichierSf2, i).isEmpty())
        {
            qAct->setText(this->configuration->getLastFile(Config::typeFichierSf2, i));
            qAct->setVisible(1);
            qAct->setEnabled(1);
        }
        else
        {
            qAct->setVisible(0);
            qAct->setEnabled(0);
        }
    }
}
void MainWindow::updateTable(int type, int sf2, int elt, int elt2)
{
    // Un élément a été supprimé définitivement, mise à jour des tables
    EltID id((ElementType)type, sf2, elt, elt2, 0);
    if (this->ui->stackedWidget->currentWidget() == this->page_inst &&
            (id.typeElement == elementSf2 || id.typeElement == elementInst ||
             id.typeElement == elementInstSmpl ||
             id.typeElement == elementInstMod || id.typeElement == elementInstSmplMod))
    {
        // Mise à jour table instrument
        this->page_inst->updateId(id);
    }
    else if (this->ui->stackedWidget->currentWidget() == this->page_prst &&
             (id.typeElement == elementSf2 || id.typeElement == elementPrst ||
              id.typeElement == elementPrstInst ||
              id.typeElement == elementPrstMod || id.typeElement == elementPrstInstMod))
    {
        // Mise à jour table preset
        this->page_prst->updateId(id);
    }
    // Annulation des ID copiés dans l'arborescence
    this->ui->arborescence->clearPastedID();
}
void MainWindow::prepareNewAction()
{
    sf2->prepareNewActions();
}

void MainWindow::noteNameChanged()
{
    if (this->ui->stackedWidget->currentWidget() == this->page_smpl)
        this->page_smpl->afficher();
    else if (this->ui->stackedWidget->currentWidget() == this->page_inst)
        this->page_inst->afficher();
    else if (this->ui->stackedWidget->currentWidget() == this->page_prst)
        this->page_prst->afficher();
}

// Modifications
void MainWindow::renommer()
{
    bool ok;
    // Nb d'éléments sélectionnés
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID ID = ui->arborescence->getID(0);
    ElementType type = ID.typeElement;
    if ((nb > 1 && type == elementSmpl) || (nb == 1 && (type == elementSmpl || type == elementInst || type == elementPrst || type == elementSf2)))
    {
        if (nb > 1)
        {
            DialogRename * dial = new DialogRename(sf2->getQstr(ID, champ_name), this);
            dial->setAttribute(Qt::WA_DeleteOnClose);
            connect(dial, SIGNAL(updateNames(QString,int)), this, SLOT(renommerEnMasse(QString,int)));
            dial->show();
        }
        else
        {
            QString msg;
            if (type == elementSmpl) msg = trUtf8("Nom de l'échantillon (max 20 caractères) :");
            else if (type == elementInst) msg = trUtf8("Nom de l'instrument (max 20 caractères) :");
            else if (type == elementPrst) msg = trUtf8("Nom du preset (max 20 caractères) :");
            else if (type == elementSf2) msg = trUtf8("Nom du SF2 (max 255 caractères) :");
            QString text = QInputDialog::getText(this, trUtf8("Question"), msg, QLineEdit::Normal, sf2->getQstr(ID, champ_name), &ok);
            if (ok && !text.isEmpty())
            {
                sf2->prepareNewActions();
                ID = ui->arborescence->getID(0);
                sf2->set(ID, champ_name, text);
                updateDo();
                updateActions();
                this->ui->arborescence->searchTree(this->ui->editSearch->text());
            }
        }
    }
}
void MainWindow::renommerEnMasse(QString name, int modificationType)
{
    // Renommage de masse
    if (name.isEmpty())
        return;
    sf2->prepareNewActions();

    QList<EltID> listID;
    for (unsigned int i = 0; i < ui->arborescence->getSelectedItemsNumber(); i++)
        listID << ui->arborescence->getID(i);

    for (int i = 0; i < listID.size(); i++)
    {
        EltID ID = listID.at(i);

        // Détermination du nom
        QString newName;
        switch (modificationType)
        {
        case 0:{
            // Suffix
            QString suffix = " " + Config::getInstance()->getKeyName(sf2->get(ID, champ_byOriginalPitch).bValue);
            SFSampleLink pos = sf2->get(ID, champ_sfSampleType).sfLinkValue;
            if (pos == rightSample || pos == RomRightSample)
                suffix += 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                suffix += 'L';

            newName = name.left(20 - suffix.size()) + suffix;
        }break;
        case 1:
            // Remplacement du nom, ajout incrément
            name = name.left(17);
            char str2[20];
            sprintf(str2,"%.2d", (i+1)%100);
            newName = name + "-" + str2;
            break;
        case 2:
            // Ajout d'un préfixe
            newName = name + sf2->getQstr(ID, champ_name);
            break;
        case 3:
            // Ajout d'un suffixe
            newName = sf2->getQstr(ID, champ_name) + name;
            break;
        }
        newName = newName.left(20);

        if (strcmp(sf2->getQstr(ID, champ_name).toLower().toStdString().c_str(),
                   newName.toLower().toStdString().c_str()) != 0)
            sf2->set(ID, champ_name, newName);
    }
    updateDo();
    updateActions();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::dragAndDrop(EltID idDest, QList<EltID> idSources)
{
    // prepareNewActions() et updateDo() faits à l'extérieur
    Duplicator duplicator(this->sf2, this->sf2, this);
    for (int i = 0; i < idSources.size(); i++)
        duplicator.copy(idSources.at(i), idDest);
    updateActions();
}
void MainWindow::dragAndDrop(QString path, EltID idDest, int * replace)
{
#ifdef PA_USE_ASIO // Si windows
    if (path.left(1).compare("/") == 0)
        path = path.right(path.length() - 1);
#endif
    if (path.left(7).compare("file://") == 0)
        path = path.right(path.length() - 7);

    // prepareNewActions() et updateDo() faits à l'extérieur
    QFileInfo fileInfo(path);
    QString extension = fileInfo.suffix().toLower();
    if (extension.compare("sf2") == 0)
    {
        // Ouverture d'un fichier sf2
        this->ouvrir(path);
    }
    else if (extension.compare("sfz") == 0)
    {
        // Import sfz
        int valueTmp = -1;
        if (replace == NULL)
            replace = &valueTmp;
        ImportSfz import(this->sf2);
        import.import(path, replace);
    }
    else if (extension.compare("wav") == 0 && idDest.typeElement != elementUnknown && idDest.indexSf2 != -1)
    {
        // Chargement d'un son wav
        int valueTmp = 0;
        if (replace == NULL)
            replace = &valueTmp;
        importerSmpl(path, idDest, replace);
    }
}
void MainWindow::importerSmpl()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;

    // Autre format permis ?
    QString ext = "";
    typedef QString (*MyPrototype)();
    MyPrototype myFunction = (MyPrototype) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                             "getExtensionFilter");
    if (myFunction)
        ext = myFunction();

    // Affichage dialogue
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Importer un fichier audio"),
                                                        Config::getInstance()->getLastDirectory(Config::typeFichierSample),
                                                        trUtf8("Fichier .wav (*.wav)") + ext);
    if (strList.count() == 0)
        return;

    this->sf2->prepareNewActions();
    EltID id = this->ui->arborescence->getID(0);
    int replace = 0;
    while (!strList.isEmpty())
        this->dragAndDrop(strList.takeAt(0), id, &replace);
    updateDo();
    updateActions();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::importerSmpl(QString path, EltID id, int * replace)
{
    id.typeElement = elementSmpl;
    QString qStr, nom;

    qStr = path;
    Config::getInstance()->addFile(Config::typeFichierSample, qStr);
    QFileInfo qFileInfo = qStr;

    // Récupération des informations d'un sample
    Sound * son = new Sound(qStr);
    int nChannels = son->get(champ_wChannels);

    // Ajout d'un sample
    Valeur val;
    nom = qFileInfo.completeBaseName();

    // Remplacement ?
    int indexL = -1;
    int indexR = -1;
    QString qStr3 = "";
    if (*replace != -1)
    {
        for (int j = 0; j < this->sf2->count(id); j++)
        {
            id.indexElt = j;
            if (!this->sf2->get(id, champ_hidden).bValue)
            {
                if (nChannels == 2)
                {
                    if (this->sf2->getQstr(id, champ_name).compare(nom.left(19).append("L")) == 0)
                    {
                        indexL = j;
                        qStr3 = trUtf8("L'échantillon « ") + nom.left(19).toUtf8() +
                                trUtf8("L » existe déjà.<br />Que faire ?");
                    }
                    else if (this->sf2->getQstr(id, champ_name).compare(nom.left(19).append("R")) == 0)
                    {
                        indexR = j;
                        qStr3 = trUtf8("L'échantillon « ") + nom.left(19).toUtf8() +
                                trUtf8("R » existe déjà.<br />Que faire ?");
                    }
                }
                else
                {
                    if (this->sf2->getQstr(id, champ_name).compare(nom.left(20)) == 0)
                    {
                        indexL = j;
                        qStr3 = trUtf8("L'échantillon « ") + nom.left(20).toUtf8() +
                                trUtf8(" » existe déjà.<br />Que faire ?");
                    }
                }
            }
        }
        if (*replace != 2 && *replace != 4 && (indexL != -1 || indexR != -1))
        {
            // Remplacement ?
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(qStr3);
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save |
                                      QMessageBox::NoAll | QMessageBox::No);
            msgBox.button(QMessageBox::Yes)->setText(trUtf8("&Remplacer"));
            msgBox.button(QMessageBox::YesAll)->setText(trUtf8("R&emplacer tout"));
            msgBox.button(QMessageBox::Save)->setText(trUtf8("&Dupliquer"));
            msgBox.button(QMessageBox::SaveAll)->setText(trUtf8("D&upliquer tout"));
            msgBox.button(QMessageBox::No)->setText(trUtf8("&Ignorer"));
            msgBox.button(QMessageBox::NoAll)->setText(trUtf8("I&gnorer tout"));
            msgBox.setDefaultButton(QMessageBox::YesAll);
            switch (msgBox.exec())
            {
            case QMessageBox::NoAll: *replace = 4; break;
            case QMessageBox::No: *replace = 3; break;
            case QMessageBox::YesAll: *replace = 2; break;
            case QMessageBox::Yes: *replace = 1; break;
            case QMessageBox::Save: *replace = 0; break;
            case QMessageBox::SaveAll: *replace = -1; break;
            }
        }
    }
    for (int j = 0; j < nChannels; j++)
    {
        if (*replace < 3 || (nChannels == 2 && j == 0 && indexL == -1) ||
                (nChannels == 2 && j == 1 && indexR == -1) ||
                (nChannels == 1 && indexL == -1)) // Si pas ignorer
        {
            if (((nChannels == 2 && j == 0 && indexL != -1) ||
                 (nChannels == 2 && j == 1 && indexR != -1) ||
                 (nChannels == 1 && indexL != -1)) && (*replace == 2 || *replace == 1))
            {
                if ((nChannels == 2 && j == 0 && indexL != -1) || (nChannels == 1 && indexL != -1))
                    id.indexElt = indexL;
                else
                    id.indexElt = indexR;

                // Mise à jour des données
                Valeur valTmp;
                valTmp.wValue = j;
                son->set(champ_wChannel, valTmp);
                QByteArray data = son->getData(24);
                this->sf2->set(id, champ_sampleDataFull24, data);
            }
            else
            {
                // Ajout d'un sample
                id.indexElt = this->sf2->add(id);
                if (nChannels == 2)
                {
                    if (j == 0)
                    {
                        // Gauche
                        this->sf2->set(id, champ_name, nom.left(19).append("L"));
                        val.wValue = id.indexElt + 1;
                        this->sf2->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = leftSample;
                        this->sf2->set(id, champ_sfSampleType, val);
                    }
                    else
                    {
                        // Droite
                        this->sf2->set(id, champ_name, nom.left(19).append("R"));
                        val.wValue = id.indexElt - 1;
                        this->sf2->set(id, champ_wSampleLink, val);
                        val.sfLinkValue = rightSample;
                        this->sf2->set(id, champ_sfSampleType, val);
                    }
                }
                else
                {
                    this->sf2->set(id, champ_name, QString(nom.left(20)));
                    val.wValue = 0;
                    this->sf2->set(id, champ_wSampleLink, val);
                    val.sfLinkValue = monoSample;
                    this->sf2->set(id, champ_sfSampleType, val);
                }
                this->sf2->set(id, champ_filename, qStr);
                val.dwValue = son->get(champ_dwStart16);
                this->sf2->set(id, champ_dwStart16, val);
                val.dwValue = son->get(champ_dwStart24);
                this->sf2->set(id, champ_dwStart24, val);
            }
            // Configuration du sample
            val.wValue = j;
            this->sf2->set(id, champ_wChannel, val);
            val.dwValue = son->get(champ_dwLength);
            this->sf2->set(id, champ_dwLength, val);
            val.dwValue = son->get(champ_dwSampleRate);
            this->sf2->set(id, champ_dwSampleRate, val);
            val.dwValue = son->get(champ_dwStartLoop);
            this->sf2->set(id, champ_dwStartLoop, val);
            val.dwValue = son->get(champ_dwEndLoop);
            this->sf2->set(id, champ_dwEndLoop, val);
            val.bValue = (BYTE)son->get(champ_byOriginalPitch);
            this->sf2->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)son->get(champ_chPitchCorrection);
            this->sf2->set(id, champ_chPitchCorrection, val);
            // Retrait automatique du blanc au départ ?
            if (this->configuration->getRemoveBlank())
                this->page_smpl->enleveBlanc(id);
            // Ajustement automatique à la boucle ?
            if (this->configuration->getWavAutoLoop())
                this->page_smpl->enleveFin(id);
        }

        // Chargement dans la ram
        if (this->configuration->getRam())
        {
            val.wValue = 1;
            this->sf2->set(id, champ_ram, val);
        }
    }
    delete son;
}
void MainWindow::exporterSmpl()
{
    int nbElt = ui->arborescence->getSelectedItemsNumber();
    if (nbElt == 0) return;
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     Config::getInstance()->getLastDirectory(Config::typeFichierSample));
    if (qDir.isEmpty()) return;
    qDir.append(QDir::separator());
    QFile file;
    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementSmpl;
    // Initialisation des états d'exportation des samples
    int nbSmpl = this->sf2->count(id);
    int * status = new int[nbSmpl];
    for (int i = 0; i < nbSmpl; i++)
        status[i] = 0;
    // Exportation pour chaque sample
    int sampleID = -1;
    int sampleID2 = -1;
    QString qStr, qStr2;
    EltID id2;
    for (int i = 0; i < nbElt; i++)
    {
        id = this->ui->arborescence->getID(i);
        id2 = id;
        if (id.typeElement == elementSmpl)
            sampleID = id.indexElt;
        else if (id.typeElement == elementInstSmpl)
            sampleID = this->sf2->get(id, champ_sampleID).wValue;
        else
            sampleID = -1;
        if (sampleID != -1)
        {
            // Vérification que le sample n'a pas déjà été exporté
            if (status[sampleID] == 0)
            {
                qStr = qDir;
                // Stéréo ?
                id.typeElement = elementSmpl;
                id.indexElt = sampleID;
                if (this->sf2->get(id, champ_sfSampleType).wValue != monoSample &&
                        this->sf2->get(id, champ_sfSampleType).wValue != RomMonoSample)
                {
                    sampleID2 = this->sf2->get(id, champ_wSampleLink).wValue;
                    id2.indexElt = sampleID2;
                    id2.typeElement = elementSmpl;

                    // Nom du fichier
                    QString nom1 = sf2->getQstr(id, champ_name);
                    QString nom2 = sf2->getQstr(id2, champ_name);
                    int nb = Sound::lastLettersToRemove(nom1, nom2);
                    qStr.append(nom1.left(nom1.size() - nb));

                    if (this->sf2->get(id, champ_sfSampleType).wValue == rightSample &&
                            this->sf2->get(id, champ_sfSampleType).wValue != RomRightSample)
                    {
                        // Inversion smpl1 smpl2
                        EltID idTmp = id;
                        id = id2;
                        id2 = idTmp;
                    }

                    // Mise à jour des états d'exportations
                    status[sampleID] = 1;
                    status[sampleID2] = 1;
                }
                else
                {
                    sampleID2 = -1;
                    // Nom du fichier
                    qStr.append(this->sf2->getQstr(id, champ_name));
                    // Mise à jour des états d'exporations
                    status[sampleID] = 1;
                }
                // Nom pour l'exportation
                qStr2 = qStr;
                file.setFileName(qStr2.append(".wav"));
                if (file.exists())
                {
                    // Modification du nom
                    int indice = 0;
                    do
                    {
                        indice++;
                        qStr2 = qStr;
                        file.setFileName(qStr2.append(QString("-%1.wav").arg(indice)));
                    } while (file.exists());
                }
                // Exportation
                if (sampleID2 == -1)
                    Sound::exporter(qStr2, this->sf2->getSon(id));
                else
                    Sound::exporter(qStr2, this->sf2->getSon(id), this->sf2->getSon(id2));
                Config::getInstance()->addFile(Config::typeFichierSample, qStr2);
            }
        }
    }
    delete [] status;
}
void MainWindow::exporter()
{
    int nbElt = ui->arborescence->getSelectedItemsNumber();
    if (nbElt == 0)
        return;

    DialogExport * dial = new DialogExport(sf2, ui->arborescence->getID(0), this);
    connect(dial, SIGNAL(accepted(QList<EltID>,QString,int,bool,bool,bool)),
            this, SLOT(exporter(QList<EltID>,QString,int,bool,bool,bool)));
    dial->show();
}
void MainWindow::importer()
{
    // Affichage dialogue
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Importer une soundfont"),
                                                        Config::getInstance()->getLastDirectory(Config::typeFichierImport),
                                                        trUtf8("Fichier .sfz (*.sfz)"));
    if (strList.isEmpty())
        return;

    Config::getInstance()->addFile(Config::typeFichierImport, strList.first());

    this->sf2->prepareNewActions();
    int numSf2 = -1;
    while (!strList.isEmpty())
        this->dragAndDrop(strList.takeFirst(), EltID(elementUnknown, -1, -1, -1, -1), &numSf2);
    updateDo();
    updateActions();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::exporter(QList<EltID> listID, QString dir, int format, bool presetPrefix, bool bankDir, bool gmSort)
{
    if (dir.isEmpty() || !QDir(dir).exists() || listID.isEmpty())
        return;

    switch (format)
    {
    case 0:{
        // Export sf2, création d'un nouvel sf2 indépendant
        Pile_sf2 newSf2(NULL, false, NULL);
        EltID idDest(elementSf2, 0, 0, 0, 0);
        idDest.indexSf2 = newSf2.add(idDest);

        // Copie des infos
        EltID idSf2Source = listID.at(0);
        idSf2Source.typeElement = elementSf2;
        QString name = sf2->getQstr(idSf2Source, champ_name);
        newSf2.set(idDest, champ_name, name);
        newSf2.set(idDest, champ_ISNG, sf2->getQstr(idSf2Source, champ_ISNG));
        newSf2.set(idDest, champ_IROM, sf2->getQstr(idSf2Source, champ_IROM));
        newSf2.set(idDest, champ_ICRD, sf2->getQstr(idSf2Source, champ_ICRD));
        newSf2.set(idDest, champ_IENG, sf2->getQstr(idSf2Source, champ_IENG));
        newSf2.set(idDest, champ_IPRD, sf2->getQstr(idSf2Source, champ_IPRD));
        newSf2.set(idDest, champ_ICOP, sf2->getQstr(idSf2Source, champ_ICOP));
        newSf2.set(idDest, champ_ICMT, sf2->getQstr(idSf2Source, champ_ICMT));
        newSf2.set(idDest, champ_ISFT, sf2->getQstr(idSf2Source, champ_ISFT));

        // Ajout des presets
        Duplicator duplicator(this->sf2, &newSf2, this);
        for (int i = 0; i < listID.size(); i++)
            duplicator.copy(listID.at(i), idDest);

        // Détermination du nom de fichier
        QFile fichier(dir + "/" + name + ".sf2");
        if (fichier.exists())
        {
            int i = 1;
            while (QFile(dir + "/" + name + "-" + QString::number(i) + ".sf2").exists())
                i++;
            name += "-" + QString::number(i);
        }
        name = dir + "/" + name + ".sf2";

        // Sauvegarde
        newSf2.sauvegarder(idDest.indexSf2, name);
        }break;
    case 1:
        // Export sfz
        ConversionSfz(sf2).convert(dir, listID, presetPrefix, bankDir, gmSort);
        break;
    default:
        break;
    }
}
void MainWindow::nouvelInstrument()
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->arborescence->getID(0);
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromUtf8(" "), trUtf8("Nom du nouvel instrument :"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty())
    {
        sf2->prepareNewActions();
        // Reprise ID si modif
        id = ui->arborescence->getID(0);
        id.typeElement = elementInst;
        id.indexElt = this->sf2->add(id);
        this->sf2->set(id, champ_name, name.left(20));
        updateDo();
        ui->arborescence->clearSelection();
        this->ui->arborescence->searchTree(this->ui->editSearch->text());
        updateActions();
    }
}
void MainWindow::nouveauPreset()
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->arborescence->getID(0);
    // Vérification qu'un preset est disponible
    int nPreset = -1;
    int nBank = -1;
    sf2->firstAvailablePresetBank(id, nBank, nPreset);
    if (nPreset == -1)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Aucun preset n'est disponible."));
        return;
    }
    bool ok;
    QString text = "";
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
    {
        id.typeElement = elementInst;
        text = this->sf2->getQstr(id, champ_name);
    }
    QString name = QInputDialog::getText(this, " ", trUtf8("Nom du nouveau preset :"), QLineEdit::Normal, text, &ok);
    if (ok && !name.isEmpty())
    {
        Valeur val;
        sf2->prepareNewActions();
        // Reprise de l'identificateur si modification
        id = ui->arborescence->getID(0);
        id.typeElement = elementPrst;
        id.indexElt = this->sf2->add(id);
        this->sf2->set(id, champ_name, name.left(20));
        val.wValue = nPreset;
        this->sf2->set(id, champ_wPreset, val);
        val.wValue = nBank;
        this->sf2->set(id, champ_wBank, val);
        updateDo();
        this->ui->arborescence->searchTree(this->ui->editSearch->text());
        updateActions();
    }
}
void MainWindow::associer()
{
    if (!ui->arborescence->getSelectedItemsNumber())
        return;
    EltID id = ui->arborescence->getID(0);
    this->sf2->prepareNewActions();
    this->dialList.showDialog(id, DialogList::MODE_ASSOCIATION);
}
void MainWindow::associer(EltID idDest)
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0 || (idDest.typeElement != elementInst && idDest.typeElement != elementPrst))
    {
        updateDo();
        return;
    }
    Champ champ;
    if (idDest.typeElement == elementInst)
    {
        idDest.typeElement = elementInstSmpl;
        champ = champ_sampleID;
    }
    else
    {
        idDest.typeElement = elementPrstInst;
        champ = champ_instrument;
    }
    sf2->prepareNewActions();
    Valeur val;
    EltID idSrc;
    // Liste des éléments sources
    QList<EltID> listeSrc;
    for (int i = 0; i < nb; i++)
        listeSrc << this->ui->arborescence->getID(i);
    for (int i = 0; i < nb; i++)
    {
        idSrc = listeSrc.at(i);
        // Création élément lié
        idDest.indexElt2 = this->sf2->add(idDest);
        // Association de idSrc vers idDest
        val.wValue = idSrc.indexElt;
        this->sf2->set(idDest, champ, val);
        if (champ == champ_sampleID)
        {
            // Balance
            if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample ||
                    this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
            {
                val.shValue = 500;
                this->sf2->set(idDest, champ_pan, val);
            }
            else if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample ||
                     this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == RomLeftSample)
            {
                val.shValue = -500;
                this->sf2->set(idDest, champ_pan, val);
            }
            else
            {
                val.shValue = 0;
                this->sf2->set(idDest, champ_pan, val);
            }
        }
        else
        {
            // Key range
            int keyMin = 127;
            int keyMax = 0;
            EltID idLinked = idSrc;
            idLinked.typeElement = elementInstSmpl;
            for (int i = 0; i < this->sf2->count(idLinked); i++)
            {
                idLinked.indexElt2 = i;
                if (!this->sf2->get(idLinked, champ_hidden).bValue)
                {
                    if (this->sf2->isSet(idLinked, champ_keyRange))
                    {
                        keyMin = qMin(keyMin, (int)this->sf2->get(idLinked, champ_keyRange).rValue.byLo);
                        keyMax = qMax(keyMax, (int)this->sf2->get(idLinked, champ_keyRange).rValue.byHi);
                    }
                }
            }
            Valeur value;
            if (keyMin < keyMax)
            {
                value.rValue.byLo = keyMin;
                value.rValue.byHi = keyMax;
            }
            else
            {
                value.rValue.byLo = 0;
                value.rValue.byHi = 127;
            }
            this->sf2->set(idDest, champ_keyRange, value);
        }
    }
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
    updateActions();
}
void MainWindow::remplacer()
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb != 1) return;
    EltID id = ui->arborescence->getID(0);
    this->sf2->prepareNewActions();
    this->dialList.showDialog(id, DialogList::MODE_REMPLACEMENT);
}
void MainWindow::remplacer(EltID idSrc)
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb != 1 || (idSrc.typeElement != elementSmpl && idSrc.typeElement != elementInst))
    {
        updateDo();
        return;
    }
    EltID idDest = this->ui->arborescence->getID(0);
    if (idDest.typeElement != elementInstSmpl && idDest.typeElement != elementPrstInst)
        return;
    sf2->prepareNewActions();
    Champ champ;
    if (idSrc.typeElement == elementSmpl)
        champ = champ_sampleID;
    else
        champ = champ_instrument;
    // Remplacement d'un sample ou instrument lié
    Valeur val;
    val.wValue = idSrc.indexElt;
    this->sf2->set(idDest, champ, val);
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
    updateActions();
}

// Envoi de signaux
void MainWindow::copier()
{
    // émission d'un signal "copier"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier));
    }
}
void MainWindow::coller()
{
    // émission d'un signal "coller"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier));
    }
}
void MainWindow::supprimer()
{
    // émission d'un signal "supprimer"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused,
                                new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, 0));
        QApplication::postEvent(focused,
                                new QKeyEvent(QEvent::KeyRelease, Qt::Key_Delete, 0));
    }
}

// Outils
void MainWindow::enleveBlanc()      {this->page_smpl->enleveBlanc();}
void MainWindow::enleveFin()        {this->page_smpl->enleveFin();}
void MainWindow::normalisation()    {this->page_smpl->normalisation();}
void MainWindow::bouclage()         {this->page_smpl->bouclage();}
void MainWindow::filtreMur()        {this->page_smpl->filtreMur();}
void MainWindow::reglerBalance()    {this->page_smpl->reglerBalance();}
void MainWindow::transposer()       {this->page_smpl->transposer();}
void MainWindow::sifflements()      {this->page_smpl->sifflements();}
void MainWindow::desaccorder()      {this->page_inst->desaccorder();}
void MainWindow::duplication()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->arborescence->getID(0).typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->duplication();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->duplication();
}
void MainWindow::paramGlobal()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->arborescence->getID(0).typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->paramGlobal();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->paramGlobal();
}
void MainWindow::spatialisation()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->arborescence->getID(0).typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->spatialisation();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->spatialisation();
}
void MainWindow::visualize()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->arborescence->getID(0).typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->visualize();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->visualize();
}
void MainWindow::repartitionAuto()  {this->page_inst->repartitionAuto();}
void MainWindow::mixture()          {this->page_inst->mixture();}
void MainWindow::release()          {this->page_inst->release();}
void MainWindow::purger()
{
    // Suppression des éléments non utilisés
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    if (!ui->arborescence->isSelectedItemsSf2Unique()) return;
    sf2->prepareNewActions();
    EltID id = ui->arborescence->getID(0);
    // Nombre de samples et instruments non utilisés
    int unusedSmpl = 0;
    int unusedInst = 0;
    id.typeElement = elementSmpl;
    int nbSmpl = sf2->count(id);
    id.typeElement = elementInst;
    int nbInst = sf2->count(id);
    id.typeElement = elementPrst;
    int nbPrst = sf2->count(id);
    bool smplUsed, instUsed;
    int nbPrstInst, nbInstSmpl;
    // Suppression des instruments
    for (int i = 0; i < nbInst; i++)
    {
        // l'instrument ne doit pas être masqué
        id.typeElement = elementInst;
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            instUsed = false;
            // on regarde chaque preset
            for (int j = 0; j < nbPrst; j++)
            {
                // le preset ne doit pas être masqué
                id.typeElement = elementPrst;
                id.indexElt = j;
                if (!sf2->get(id, champ_hidden).bValue)
                {
                    // composé d'instruments
                    id.typeElement = elementPrstInst;
                    nbPrstInst = sf2->count(id);
                    for (int k = 0; k < nbPrstInst; k++)
                    {
                        id.indexElt2 = k;
                        if (!sf2->get(id, champ_hidden).bValue)
                            if (sf2->get(id, champ_instrument).wValue == i) instUsed = true;
                    }
                }
            }
            if (!instUsed)
            {
                // suppression de l'instrument
                unusedInst++;
                id.typeElement = elementInst;
                id.indexElt = i;
                int message;
                sf2->remove(id, &message);
            }
        }
    }
    // Suppression des samples
    // pour chaque sample
    for (int i = 0; i < nbSmpl; i++)
    {
        // le sample ne doit pas être masqué
        id.typeElement = elementSmpl;
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            smplUsed = false;
            // on regarde chaque instrument
            for (int j = 0; j < nbInst; j++)
            {
                // l'instrument ne doit pas être masqué
                id.typeElement = elementInst;
                id.indexElt = j;
                if (!sf2->get(id, champ_hidden).bValue)
                {
                    // composé de samples
                    id.typeElement = elementInstSmpl;
                    nbInstSmpl = sf2->count(id);
                    for (int k = 0; k < nbInstSmpl; k++)
                    {
                        id.indexElt2 = k;
                        if (!sf2->get(id, champ_hidden).bValue)
                            if (sf2->get(id, champ_sampleID).wValue == i) smplUsed = true;
                    }
                }
            }
            if (!smplUsed)
            {
                // suppression du sample
                unusedSmpl++;
                id.typeElement = elementSmpl;
                id.indexElt = i;
                int message;
                sf2->remove(id, &message);
            }
        }
    }
    // Bilan
    QString qStr;
    if (unusedSmpl < 2)
        qStr = QString::number(unusedSmpl) + trUtf8(" échantillon et ");
    else
        qStr = QString::number(unusedSmpl) + trUtf8(" échantillons et ");
    if (unusedInst < 2)
        qStr += QString::number(unusedInst) + trUtf8(" instrument ont été supprimés.");
    else
        qStr += QString::number(unusedInst) + trUtf8(" instruments ont été supprimés.");

    QMessageBox::information(this, "", qStr);
    updateDo();
    updateActions();
}
void MainWindow::attenuationMini()
{
    // Atténuation minimale souhaitée
    bool ok;
    Config * conf = Config::getInstance();
    double rep = QInputDialog::getDouble(this, trUtf8("Question"),
                                         trUtf8("Atténuation minimale (dB) :"),
                                         conf->getTools_2_attenuation_dB(), 0, 200, 2,
                                         &ok);
    if (!ok) return;
    conf->setTools_2_attenuation_dB(rep);
    // Calcul de l'atténuation minimale actuelle
    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementInst;
    EltID id2 = id;
    id2.typeElement = elementInstSmpl;
    qint32 attenuationMini = -1;
    // Pour chaque instrument
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            // Pour chaque sample lié
            for (int j = 0; j < this->sf2->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (this->sf2->isSet(id2, champ_initialAttenuation))
                    {
                        if (attenuationMini == -1 || this->sf2->get(id2, champ_initialAttenuation).shValue < attenuationMini)
                            attenuationMini = this->sf2->get(id2, champ_initialAttenuation).shValue;
                    }
                    else
                        attenuationMini = 0;
                }
            }
        }
    }
    // Décalage à effectuer
    double decalage = 10.0 * rep - attenuationMini;
    if (decalage == 0) return;
    // Application du décalage
    Valeur val;
    this->sf2->prepareNewActions();
    // Reprise des identificateurs si modification
    id = this->ui->arborescence->getID(0);
    id.typeElement = elementInst;
    id2 = id;
    id2.typeElement = elementInstSmpl;
    // Pour chaque instrument
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            id2.indexElt = i;
            // Pour chaque sample lié
            for (int j = 0; j < this->sf2->count(id2); j++)
            {
                id2.indexElt2 = j;
                if (!this->sf2->get(id2, champ_hidden).bValue)
                {
                    if (this->sf2->isSet(id2, champ_initialAttenuation))
                        val.shValue = this->sf2->get(id2, champ_initialAttenuation).shValue + decalage;
                    else
                        val.shValue = decalage;
                    this->sf2->set(id2, champ_initialAttenuation, val);
                }
            }
        }
    }
    // Mise à jour
    this->updateDo();
    if (ui->stackedWidget->currentWidget() == this->page_inst)
        this->page_inst->afficher();
}
void MainWindow::associationAutoSmpl()
{
    // Association automatique des samples
    // Condition : même nom sauf pour la dernière lettre (R ou L)
    this->sf2->prepareNewActions();
    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementSmpl;
    // Constitution listes de noms et indices
    QList<EltID> listID;
    QStringList  listNom;
    QList<bool>  listSens;
    QString currentStr;
    for (int i = 0; i < this->sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!this->sf2->get(id, champ_hidden).bValue)
        {
            currentStr = this->sf2->getQstr(id, champ_name);
            if (currentStr.right(1).toUpper() == "R")
            {
                listID.append(id);
                listNom.append(currentStr.left(currentStr.size() - 1));
                listSens.append(false);
            }
            else if (currentStr.right(1).toUpper() == "L")
            {
                listID.append(id);
                listNom.append(currentStr.left(currentStr.size() - 1));
                listSens.append(true);
            }
        }
    }
    // Assemblage
    EltID currentID, idBis;
    bool currentSens;
    bool isFound;
    int indice;
    Valeur value;
    while (listID.size())
    {
        currentID = listID.takeLast();
        currentStr = listNom.takeLast();
        currentSens = listSens.takeLast();
        // Recherche d'une association
        indice = 0;
        isFound = false;
        while (indice < listID.size() && !isFound)
        {
            if (listNom.at(indice) == currentStr && listSens.at(indice) != currentSens)
            {
                idBis = listID.takeAt(indice);
                listNom.takeAt(indice);
                listSens.takeAt(indice);
                // Association idBis avec currentID
                value.wValue = idBis.indexElt;
                this->sf2->set(currentID, champ_wSampleLink, value);
                value.wValue = currentID.indexElt;
                this->sf2->set(idBis, champ_wSampleLink, value);
                if (currentSens)
                {
                    value.sfLinkValue = leftSample;
                    this->sf2->set(currentID, champ_sfSampleType, value);
                    value.sfLinkValue = rightSample;
                    this->sf2->set(idBis, champ_sfSampleType, value);
                }
                else
                {
                    value.sfLinkValue = rightSample;
                    this->sf2->set(currentID, champ_sfSampleType, value);
                    value.sfLinkValue = leftSample;
                    this->sf2->set(idBis, champ_sfSampleType, value);
                }
                isFound = true;
            }
            else
                indice++;
        }
    }
    // Mise à jour
    this->updateDo();
    if (ui->stackedWidget->currentWidget() == this->page_smpl)
        this->page_smpl->afficher();
}
void MainWindow::on_action_Dissocier_les_samples_st_r_o_triggered()
{
    this->sf2->prepareNewActions();
    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementSmpl;
    for (int i = 0; i < sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            SFSampleLink type = sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (type != monoSample && type != RomMonoSample)
            {
                Valeur value;
                if (type == leftSample || type == rightSample || type == linkedSample)
                {
                    value.sfLinkValue = monoSample;
                    sf2->set(id, champ_sfSampleType, value);
                }
                else if (type == RomLeftSample || type == RomRightSample || type == RomLinkedSample)
                {
                    value.sfLinkValue = RomMonoSample;
                    sf2->set(id, champ_sfSampleType, value);
                }
                value.wValue = 0;
                sf2->set(id, champ_wSampleLink, value);
            }
        }
    }

    // Mise à jour
    this->updateDo();
    if (ui->stackedWidget->currentWidget() == this->page_smpl)
        this->page_smpl->afficher();
}
void MainWindow::on_actionExporter_pics_de_fr_quence_triggered()
{
    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementSf2;
    QString defaultFile = configuration->getLastDirectory(Config::typeFichierFrequences) + "/" +
            sf2->getQstr(id, champ_name).replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Exporter les pics de fréquence"),
                                                    defaultFile, trUtf8("Fichier .csv (*.csv)"));
    if (!fileName.isEmpty())
    {
        configuration->addFile(Config::typeFichierFrequences, fileName);
        exporterFrequences(fileName);
    }
}
void MainWindow::exporterFrequences(QString fileName)
{
    // Création fichier csv
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QString sep = trUtf8(";");
    QTextStream stream(&file);
    stream << "\"" << trUtf8("Echantillon") << "\"" << sep << "\"" << trUtf8("Numéro de pic") << "\"" << sep << "\""
           << trUtf8("Facteur") << "\"" << sep << "\"" << trUtf8("Fréquence") << "\"" << sep << "\""
           << trUtf8("Note") << "\"" << sep << "\"" << trUtf8("Correction") << "\"";

    EltID id = this->ui->arborescence->getID(0);
    id.typeElement = elementSmpl;
    QString nomSmpl;
    QList<double> listeFrequences, listeFacteurs;
    QList<int> listeNotes, listeCorrections;
    for (int i = 0; i < sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            nomSmpl = sf2->getQstr(id, champ_name).replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_");

            // Ecriture valeurs pour l'échantillon
            page_smpl->getPeakFrequencies(id, listeFrequences, listeFacteurs, listeNotes, listeCorrections);

            for (int j = 0; j < listeFrequences.size(); j++)
            {
                stream << endl;
                stream << "\"" << nomSmpl << "\"" << sep;
                stream << j << sep;
                stream << QString::number(listeFacteurs.at(j)).replace(".", trUtf8(",")) << sep;
                stream << QString::number(listeFrequences.at(j)).replace(".", trUtf8(",")) << sep;
                stream << Config::getInstance()->getKeyName(listeNotes.at(j)) << sep;
                stream << listeCorrections.at(j);
            }
        }
    }
    file.close();
}

// Gestion du clavier virtuel / du son
void MainWindow::setAudioEngine(int audioEngine, int bufferSize)
{
    emit(initAudio(audioEngine, bufferSize));
}
void MainWindow::showKeyboard(bool val)
{
    this->keyboard->setVisible(val);
    this->ui->velocityButton->setVisible(val);
    this->ui->labelNote->setVisible(val);
    this->ui->labelVelocite->setVisible(val);
    this->ui->label_2->setVisible(val);
    this->ui->label_3->setVisible(val);
}
void MainWindow::setVelocity(int val)
{
    this->configuration->setKeyboardVelocity(val);
    if (val > 0)
        this->ui->labelVelocite->setText(QString("%1").arg(val));
    else
        this->ui->labelVelocite->setText("-");
}
QStringList MainWindow::getListMidi()
{
    return this->keyboard->getPortNames();
}
void MainWindow::openMidiPort(int val)
{
    this->keyboard->openMidiPort(val);
}
void MainWindow::noteOn(int key)
{
    noteChanged(key, this->configuration->getKeyboardVelocity());
}
void MainWindow::noteOff(int key)   { noteChanged(key, 0); }
void MainWindow::noteHover(int key)
{
    if (_currentKey == -1)
    {
        if (key != -1)
            this->ui->labelNote->setText(configuration->getKeyName(key));
        else
            this->ui->labelNote->setText("-");
    }
}
void MainWindow::setSustain(bool isOn)
{
    _isSustainOn = isOn;
    if (!isOn)
    {
        // On relâche les notes maintenues
        while (_listKeysToRelease.size())
        {
            noteChanged(_listKeysToRelease.takeFirst(), 0);
        }
    }
}
void MainWindow::setVolume(int vol)
{
    vol = (double)vol / 127. * 101. - 50.5;
    configuration->setVolume(vol);
    setSynthGain(vol);
}
void MainWindow::noteChanged(int key, int vel)
{
    if (vel == 0)
         _currentKey = -1;
    else
        _currentKey = key;

    // Cas particulier : arrêt de la lecture d'un sample
    if (key == -1 && vel == 0)
    {
        this->synth->play(0, 0, 0, -1, 0);
        return;
    }

    // Mise en évidence de la ou des éléments liés étant en train de jouer
    this->page_inst->enlightColumn(key, vel != 0);
    this->page_prst->enlightColumn(key, vel != 0);

    if (vel)
    {
        if (_listKeysToRelease.contains(key))
            noteChanged(key, 0);
    }
    else
    {
        if (_isSustainOn)
        {
            if (!_listKeysToRelease.contains(key))
                _listKeysToRelease << key;
            return;
        }
    }

    int defaultVelocity = this->configuration->getKeyboardVelocity();
    if (defaultVelocity > 0 && vel > 0)
        vel = defaultVelocity;
    if (vel > 0 && key != -1)
    {
        this->ui->labelNote->setText(configuration->getKeyName(key));
        this->ui->labelVelocite->setText(QString::number(vel));
    }
    else
    {
        this->ui->labelNote->setText("-");
        if (defaultVelocity == 0)
            this->ui->labelVelocite->setText("-");
    }

    // Lecture ?
    if (this->ui->arborescence->getSelectedItemsNumber())
    {
        EltID id = this->ui->arborescence->getID(0);
        if (this->ui->arborescence->isSelectedItemsSf2Unique() && !this->sf2->get(id, champ_hidden).bValue)
        {
            if (id.typeElement == elementSmpl && this->ui->arborescence->getSelectedItemsNumber() == 1)
                this->synth->play(0, id.indexSf2, id.indexElt, key, vel);
            else if (this->ui->arborescence->isSelectedItemsFamilyUnique())
            {
                if ((id.typeElement == elementInst || id.typeElement == elementInstSmpl) &&
                        this->ui->arborescence->isSelectedItemsFamilyUnique())
                    this->synth->play(1, id.indexSf2, id.indexElt, key, vel);
                else if ((id.typeElement == elementPrst || id.typeElement == elementPrstInst) &&
                         this->ui->arborescence->isSelectedItemsFamilyUnique())
                    this->synth->play(2, id.indexSf2, id.indexElt, key, vel);
            }
        }
    }
}
void MainWindow::setSynthGain(int val)
{
    this->synth->setGain(val);
}
void MainWindow::setSynthReverb(int level, int size, int width, int damping)
{
    this->synth->setReverb(level, size, width, damping);
}
void MainWindow::setSynthChorus(int level, int depth, int frequency)
{
    this->synth->setChorus(level, depth, frequency);
}
