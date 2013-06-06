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
#include <QFileDialog>
#include <QInputDialog>

// Constructeurs, destructeurs
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent, Qt::Window | Qt::WindowCloseButtonHint |
                                                      Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint |
                                                      Qt::WindowSystemMenuHint | Qt::WindowTitleHint |
                                                      Qt::CustomizeWindowHint),
    ui(new Ui::MainWindow),
    configuration(this), help(this)
{
    ui->setupUi(this);
    // Taille max de l'application
    this->setMaximumSize(QApplication::desktop()->size());
    // Initialisation de l'objet pile sf2
    this->sf2 = new Pile_sf2(ui->arborescence, this->configuration.getRam());
    // Connexion avec mise à jour table
    connect(this->sf2, SIGNAL(updateTable(int,int,int,int)), this,
            SLOT(updateTable(int,int,int,int)));
    // Initialisation du synthétiseur
    this->synth = new Synth(this->sf2);
    this->synth->moveToThread(&this->synthThread);
    this->synthThread.start();
    // Initialisation de la sortie audio
    this->audioDevice = new AudioDevice(this->synth);
    connect(this, SIGNAL(initAudio(int)), this->audioDevice, SLOT(initAudio(int)));
    connect(this, SIGNAL(stopAudio()), this->audioDevice, SLOT(closeConnections()),
            Qt::BlockingQueuedConnection);
    this->audioDevice->moveToThread(&this->audioThread);
    this->audioThread.start();
    // Priorité des thread
    this->audioThread.setPriority(QThread::HighestPriority);
    this->synthThread.setPriority(QThread::TimeCriticalPriority);

    if (this->configuration.getAudioType() == 0)
        this->setAudioEngine(this->configuration.getAudioIndex());
    else
        this->setAudioEngine(this->configuration.getAudioType());
    this->setSynthGain(this->configuration.getSynthGain());
    this->setSynthChorus(this->configuration.getSynthChoLevel(),
                         this->configuration.getSynthChoDepth(),
                         this->configuration.getSynthChoFrequency());
    this->setSynthReverb(this->configuration.getSynthRevLevel(),
                         this->configuration.getSynthRevSize(),
                         this->configuration.getSynthRevWidth(),
                         this->configuration.getSynthRevDamp());
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
    ui->arborescence->init(this);
    // Initialisation dialog liste (pointeur vers les sf2 et mainWindow)
    this->dialList.init(this, this->sf2);
    // Initialisation répertoire de départ
    fileName = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    //fileName =  QStandardPaths::displayName(QStandardPaths::DesktopLocation); // QT 5
    // Fichiers récents
    updateFavoriteFiles();
    // Affichage logo logiciel
    ui->stackedWidget->setCurrentWidget(ui->page_Soft);
    // Préférences d'affichage
    if (!this->configuration.getAfficheToolBar())
    {
        this->ui->actionBarre_d_outils->setChecked(0);
        ui->toolBar->setVisible(0);
    }
    if (!this->configuration.getAfficheMod())
    {
        this->ui->actionSection_modulateurs->setChecked(0);
        this->page_inst->setModVisible(0);
        this->page_prst->setModVisible(0);
    }
    // Clavier
    this->keyboard = new PianoKeybdCustom(this);
    QHBoxLayout * layout = (QHBoxLayout*)this->ui->ensembleKeyboard->layout();
    layout->insertWidget(2, this->keyboard);
    this->setKeyboardType(this->configuration.getKeyboardType());
    // Déplacement dans la barre de menu
    this->ui->toolBar->setContentsMargins(0, 0, 0, 0);
    this->ui->ensembleKeyboard->setMaximumHeight(this->ui->toolBar->height()+5);
    this->keyboard->setMaximumHeight(this->ui->toolBar->height()+5);
    this->ui->velocityButton->setMaximumHeight(this->ui->toolBar->height()+5);
    this->ui->toolBar->addWidget(this->ui->ensembleKeyboard);
    this->showKeyboard(false);
    this->ui->velocityButton->setValue(this->configuration.getKeyboardVelocity());
    // Ouverture port midi et connexions
    this->keyboard->openMidiPort(this->configuration.getNumPortMidi());
    connect(this->keyboard, SIGNAL(keyChanged(int,int)), this, SLOT(noteChanged(int,int)));
    connect(this->keyboard, SIGNAL(noteOn(int)), this, SLOT(noteOn(int)));
    connect(this->keyboard, SIGNAL(noteOff(int)), this, SLOT(noteOff(int)));
    connect(this->page_smpl, SIGNAL(noteChanged(int,int)), this, SLOT(noteChanged(int,int)));
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
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Nombre de fichiers non sauvegardés
    int nbFile = 0;
    EltID id = {elementSf2, -1, 0, 0, 0};
    int nbSf2 = sf2->count(id);
    QString qStr = "";
    for (int i = 0; i < nbSf2; i++)
    {
        id.indexSf2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            if (sf2->isEdited(i))
            {
                if (nbFile) qStr.append("<br /> - " + sf2->getQstr(id, champ_name));
                else qStr = sf2->getQstr(id, champ_name);
                nbFile ++;
            }
        }
    }
    if (nbFile)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        if (nbFile > 1)
        {
            qStr.prepend(tr("<b>Les fichiers suivants ont été modifiés depuis leur dernier enregistrement :<br/> - "));
            qStr.append("</b>");
            msgBox.setText(QString::fromUtf8(qStr.toStdString().c_str()));
        }
        else
        {
            qStr = tr("<b>Voulez-vous enregistrer les modifications du fichier «&#160;") + qStr + tr("&#160;» avant de quitter ?</b>");
            msgBox.setText(QString::fromUtf8(qStr.toStdString().c_str()));
        }
        qStr = tr("Si vous n'enregistrez pas, les modifications effectuées depuis la dernière sauvegarde seront définitivement perdues.");
        msgBox.setInformativeText(QString::fromUtf8(qStr.toStdString().c_str()));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(tr("&Enregistrer"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("&Annuler"));
        msgBox.button(QMessageBox::Discard)->setText(tr("&Quitter sans enregistrer"));
        msgBox.button(QMessageBox::Save)->setIcon(QIcon::fromTheme("filesave"));
        msgBox.setDefaultButton(QMessageBox::Save);
        switch (msgBox.exec())
        {
        case QMessageBox::Cancel: event->ignore();
            break;
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
}

// Ouverture, fermeture, suppression
void MainWindow::ouvrir()
{
    // Chargement d'un fichier .sf2
    QFileInfo fileInfo(fileName);
    QString fileNameTmp = fileInfo.completeBaseName() + ".sf2";
    fileNameTmp = QFileDialog::getOpenFileName(this, tr("Ouvrir une soundfont"),
                                               fileNameTmp, tr("Fichier .sf2 (*.sf2)"));
    if (fileNameTmp.isNull()) return;
    fileName = fileNameTmp;
    ouvrir(fileName);
}
void MainWindow::ouvrirFichier1() {fileName = this->configuration.getFile(0); ouvrir(this->configuration.getFile(0));}
void MainWindow::ouvrirFichier2() {fileName = this->configuration.getFile(1); ouvrir(this->configuration.getFile(1));}
void MainWindow::ouvrirFichier3() {fileName = this->configuration.getFile(2); ouvrir(this->configuration.getFile(2));}
void MainWindow::ouvrirFichier4() {fileName = this->configuration.getFile(3); ouvrir(this->configuration.getFile(3));}
void MainWindow::ouvrirFichier5() {fileName = this->configuration.getFile(4); ouvrir(this->configuration.getFile(4));}
void MainWindow::ouvrir(QString fileName)
{
    // Chargement d'un fichier .sf2
    this->sf2->prepareNewActions();
    switch (this->sf2->ouvrir(fileName))
    {
    case 0:
        // le chargement s'est bien déroulé
        this->configuration.addFavorite(fileName);
        updateFavoriteFiles();
        updateDo();
        ui->arborescence->clearSelection();
        this->ui->arborescence->searchTree(this->ui->editSearch->text());
        updateActions();
        break;
    case 1:
        QMessageBox::warning(this, tr("Attention"), "Format inconnu.");
        break;
    case 2:
        QMessageBox::warning(this, tr("Attention"), QString::fromUtf8(tr("Le fichier est déjà chargé.").toStdString().c_str()));
        break;
    case 3:
        QMessageBox::warning(this, tr("Attention"), QString::fromUtf8(tr("Impossible d'ouvrir le fichier.").toStdString().c_str()));
        break;
    case 4:
        QMessageBox::warning(this, tr("Attention"), tr("Lecture impossible."));
        break;
    case 5:
        QMessageBox::warning(this, tr("Attention"), tr("Le fichier est corrompu."));
        break;
    case 6:
        QMessageBox::warning(this, tr("Attention"), tr("La somme de la taille des blocs ne donne pas la taille totale du fichier."));
        break;
    }
}
void MainWindow::nouveau()
{
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromUtf8(" "), tr("Nom de la nouvelle soundfont :"), QLineEdit::Normal, "", &ok);
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
        QString qStr = tr("<b>Voulez-vous enregistrer les modifications du fichier «&#160;") + sf2->getQstr(id,champ_name) + \
                tr("&#160;» avant de le fermer ?</b>");
        msgBox.setText(QString::fromUtf8(qStr.toStdString().c_str()));
        qStr = tr("Si vous n'enregistrez pas, les modifications effectuées depuis la dernière sauvegarde seront définitivement perdues.");
        msgBox.setInformativeText(QString::fromUtf8(qStr.toStdString().c_str()));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.button(QMessageBox::Save)->setText(tr("&Enregistrer"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("&Annuler"));
        msgBox.button(QMessageBox::Discard)->setText(tr("&Fermer sans enregistrer"));
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
    EltID id = {elementSf2, indexSf2, 0, 0, 0};
    // Avertissement si enregistrement dans une résolution inférieure
    int ret = QMessageBox::Save;
    if (sf2->get(id, champ_wBpsSave).wValue < sf2->get(id, champ_wBpsInit).wValue)
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        char T[20];
        sprintf(T,"%d &#8658; %d", sf2->get(id, champ_wBpsInit).wValue, sf2->get(id, champ_wBpsSave).wValue);
        QString qStr = tr("<b>Perte de résolution ") + QString(T) + tr(" bits</b>");
        msgBox.setText(QString::fromUtf8(qStr.toStdString().c_str()));
        qStr = tr("La qualité des samples sera abaissée suite à cette opération. Continuer ?");
        msgBox.setInformativeText(QString::fromUtf8(qStr.toStdString().c_str()));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save);
        msgBox.button(QMessageBox::Save)->setText(tr("&Oui"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("&Non"));
        msgBox.setDefaultButton(QMessageBox::Cancel);
        ret = msgBox.exec();
    }
    if (ret == QMessageBox::Cancel) return 1;
    if (saveAs || sf2->getQstr(id, champ_filename) == "")
    {
        QString fileNameTmp = fileName;
        if (sf2->getQstr(id, champ_filename) == "")
        {
            QFileInfo fileInfo = fileNameTmp;
            fileNameTmp = fileInfo.filePath() + QDir::separator() + sf2->getQstr(id, champ_name) + ".sf2";
            fileNameTmp = QFileDialog::getSaveFileName(this, tr("Sauvegarder une soundfont"), fileNameTmp, tr("Fichier .sf2 (*.sf2)"));
        }
        else
            fileNameTmp = QFileDialog::getSaveFileName(this, tr("Sauvegarder une soundfont"), \
                                                       sf2->getQstr(id, champ_filename), tr("Fichier .sf2 (*.sf2)"));
        if (fileNameTmp.isNull()) return 1;
        else fileName = fileNameTmp;
    }
    else fileName = sf2->getQstr(id, champ_filename);
    switch (this->sf2->sauvegarder(indexSf2, fileName))
    {
    case 0:
        // sauvegarde ok
        updateDo();
        this->configuration.addFavorite(fileName);
        updateFavoriteFiles();
        if (ui->stackedWidget->currentWidget() == this->page_sf2)
            this->page_sf2->afficher();
        return 0;
        break;
    case 1:
        QMessageBox::warning(NULL, tr("Attention"), tr("Extension inconnue."));
        break;
    case 2:
        QMessageBox::warning(NULL, tr("Attention"), QString::fromUtf8(tr("Fichier déjà ouvert, impossible de sauvegarder.").toStdString().c_str()));
        break;
    case 3:
        QMessageBox::warning(NULL, tr("Attention"), QString::fromUtf8(tr("Impossible d'enregistrer le fichier.").toStdString().c_str()));
        break;
    }
    return 1;
}
void MainWindow::undo()
{
    sf2->undo();
    updateActions();
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::redo()
{
    sf2->redo();
    updateActions();
    updateDo();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}

// Fenetres / affichage
void MainWindow::showConfig()
{
    this->configuration.setWindowModality(Qt::ApplicationModal);
    this->configuration.show();
}
void MainWindow::showAbout()
{
    QMessageBox::about(this, "Polyphone",
        trUtf8("<b>Polyphone</b> © 2013<br/>" \
        "Version : ") + VERSION  + trUtf8("<br/>" \
        "Auteur : Davy Triponney<br/>" \
        "Site web : <a href=\"http://www.polyphone.fr\">www.polyphone.fr</a><br/>" \
        "Support : <a href=\"mailto:info@polyphone.fr\">info@polyphone.fr</a>").toStdString().c_str());
}
void MainWindow::showHelp()
{
    this->help.show();
}
void MainWindow::AfficherBarreOutils()
{
    if (ui->actionBarre_d_outils->isChecked())
    {
        this->configuration.setAfficheToolBar(1);
        ui->toolBar->setVisible(1);
    }
    else
    {
        this->configuration.setAfficheToolBar(0);
        ui->toolBar->setVisible(0);
    }
}
void MainWindow::afficherSectionModulateurs()
{
    if (ui->actionSection_modulateurs->isChecked())
    {
        this->configuration.setAfficheMod(1);
        this->page_inst->setModVisible(1);
        this->page_prst->setModVisible(1);
    }
    else
    {
        this->configuration.setAfficheMod(0);
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
    this->configuration.setKeyboardType(val);
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
    EltID id = {elementSf2, 0, 0, 0, 0};
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
            setWindowTitle("*" + sf2->getQstr(id, champ_filename) + " - Polyphone");
            ui->actionEnregistrer->setEnabled(1);
        }
        else
        {
            setWindowTitle(sf2->getQstr(id, champ_filename) + " - Polyphone");
            ui->actionEnregistrer->setEnabled(0);
        }
    }
    else
    {
        setWindowTitle("Polyphone");
        ui->actionEnregistrer->setEnabled(0);
    }
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
        if (typeUnique && fichierUnique && familleUnique)
        {
            if (id.typeElement == elementSmpl)
            {
                page_smpl->afficher();
                if (this->configuration.getKeyboardType())
                    this->showKeyboard(true);
                else
                    this->showKeyboard(false);
            }
            else if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            {
                page_inst->afficher();
                if (this->configuration.getKeyboardType())
                    this->showKeyboard(true);
                else
                    this->showKeyboard(false);
            }
            else if (id.typeElement == elementPrst || id.typeElement == elementPrstInst)
            {
                page_prst->afficher();
                if (this->configuration.getKeyboardType())
                    this->showKeyboard(true);
                else
                    this->showKeyboard(false);
            }
            else
            {
                page_sf2->afficher();
                this->showKeyboard(false);
            }
        }
        else
        {
            ui->stackedWidget->setCurrentWidget(ui->page_Soft);
            this->showKeyboard(false);
        }
    }

    // ACTIVATION, DESACTIVATION DES COMMANDES
    if (fichierUnique)
    {
        // Action possible : fermer
        ui->actionFermer_le_fichier->setEnabled(1);
        // Action possible : importer
        ui->actionImporter->setEnabled(1);
        // Action possible : coller
        ui->actionColler->setEnabled(1);
        // Supprimer, copier
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique) \
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
            ui->actionRenommer->setText(tr("&Renommer"));
            ui->actionRenommer->setEnabled(1);
        }
        else
        {
            if (nb > 1 && typeUnique && type == elementSmpl)
            {
                ui->actionRenommer->setText(tr("&Renommer en masse"));
                ui->actionRenommer->setEnabled(1);
            }
            else
            {
                ui->actionRenommer->setText(tr("&Renommer"));
                ui->actionRenommer->setEnabled(0);
            }
        }
        // Actions enregistrer sous
        ui->actionEnregistrer_sous->setEnabled(1);
        // Action exporter
        if ((typeUnique && type == elementSmpl) || (familleUnique && type == elementInstSmpl))
            ui->actionExporter->setEnabled(1);
        else
            ui->actionExporter->setEnabled(0);
        // Nouveau sample, instrument, preset
        ui->actionNouveau_preset->setEnabled(1);
        ui->actionNouvel_instrument->setEnabled(1);
        // Outils
        if (!this->page_smpl->isPlaying())
            ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(1);
        else
            ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(0);
        if (typeUnique && type == elementSmpl && !this->page_smpl->isPlaying())
            ui->menuSample->setEnabled(1);
        else
            ui->menuSample->setEnabled(0);
        if ((type == elementInst || type == elementInstSmpl) && familleUnique)
            ui->menuInstrument->setEnabled(1);
        else
            ui->menuInstrument->setEnabled(0);
        if ((type == elementPrst || type == elementPrstInst) && familleUnique)
            ui->menuPreset->setEnabled(1);
        else
            ui->menuPreset->setEnabled(0);
        ui->menuDivers->setEnabled(1);
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
        // Actions enregistrer sous
        ui->actionEnregistrer_sous->setEnabled(0);
        // Nouveau sample, instrument, preset
        ui->actionNouveau_preset->setEnabled(0);
        ui->actionNouvel_instrument->setEnabled(0);
        // Outils
        ui->menuSample->setEnabled(0);
        ui->menuInstrument->setEnabled(0);
        ui->menuDivers->setEnabled(0);
        ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(0);
    }
}
void MainWindow::desactiveOutilsSmpl()
{
    this->ui->action_Supprimer->setEnabled(0);
    this->ui->menuSample->setEnabled(0);
    this->ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(0);
    this->ui->arborescence->desactiveSuppression();
}
void MainWindow::activeOutilsSmpl()
{
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
        if (this->configuration.getFile(i)[0] != '\0')
        {
            qAct->setText(this->configuration.getFile(i));
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
    EltID id = {(ElementType)type, sf2, elt, elt2, 0};
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
void MainWindow::anticipateNewAction()
{
    this->sf2->prepareNewActions();
    this->updateDo();
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
            if (type == elementSmpl) msg = QString::fromUtf8(tr("Nom du sample (max 20 caractères) :").toStdString().c_str());
            else if (type == elementInst) msg = QString::fromUtf8(tr("Nom de l'instrument (max 20 caractères) :").toStdString().c_str());
            else if (type == elementPrst) msg = QString::fromUtf8(tr("Nom du preset (max 20 caractères) :").toStdString().c_str());
            else if (type == elementSf2) msg = QString::fromUtf8(tr("Nom du SF2 (max 255 caractères) :").toStdString().c_str());
            QString text = QInputDialog::getText(this, tr("Question"), msg, QLineEdit::Normal, sf2->getQstr(ID, champ_name), &ok);
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
            // Remplacement du nom, ajout note de base et indication L/R
            name = name.left(15);
            // note du sample
            int note = sf2->get(ID, champ_byOriginalPitch).bValue;
            char str2[20];
            sprintf(str2,"%.3hu", note);
            // position du sample
            SFSampleLink pos = sf2->get(ID, champ_sfSampleType).sfLinkValue;
            // Concaténation
            if (pos == rightSample || pos == RomRightSample)
                newName = name + ' ' + str2 + 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                newName = name + ' ' + str2 + 'L';
            else
                newName = name + ' ' + str2;
            }break;
        case 1:
            // Remplacement du nom, ajout incrément
            name = name.left(17);
            char str2[20];
            sprintf(str2,"%.2hu", (i+1)%100);
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
void MainWindow::dragAndDrop(EltID idDest, EltID idSrc, int temps, int *msg, QByteArray *ba1, QByteArray *ba2)
{
    if (idDest.typeElement != elementSf2 && idDest.typeElement != elementInst && \
            idDest.typeElement != elementInstSmpl && idDest.typeElement != elementRootInst && \
            idDest.typeElement != elementPrst && idDest.typeElement != elementPrstInst && \
            idDest.typeElement != elementRootPrst && idDest.typeElement != elementRootSmpl) return;
    // Copie / lien, de idSrc vers idDest
    // temps = -1 : nouvelle série d'actions
    // temps =  0 : série en cours
    // temps =  1 : fin de la série d'actions
    // temps =  2 : début et fin de la série (1 seule action)
    if (idDest.indexSf2 == idSrc.indexSf2)
    {
        // Déplacement / lien dans le même sf2
        switch (idSrc.typeElement)
        {
        case elementSmpl:
            if (idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl)
            {
                // Ajout d'un sample dans un instrument
                EltID id = idDest;
                Valeur val;
                id.typeElement = elementInstSmpl;
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                id.indexElt2 = this->sf2->add(id);
                val.wValue = idSrc.indexElt;
                this->sf2->set(id, champ_sampleID, val);
                // Balance
                if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample || \
                        this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
                {
                    val.shValue = 500;
                    this->sf2->set(id, champ_pan, val);
                }
                else if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample || \
                         this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == RomLeftSample)
                {
                    val.shValue = -500;
                    this->sf2->set(id, champ_pan, val);
                }
                else
                {
                    val.shValue = 0;
                    this->sf2->set(id, champ_pan, val);
                }
            }
            break;
        case elementInst:
            if (idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst)
            {
                // Ajout d'un instrument dans un preset
                EltID id = idDest;
                Valeur val;
                id.typeElement = elementPrstInst;
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                id.indexElt2 = this->sf2->add(id);
                val.wValue = idSrc.indexElt;
                this->sf2->set(id, champ_instrument, val);
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
                this->sf2->set(id, champ_keyRange, value);
            }
            else if ((idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl) && \
                     idSrc.indexElt != idDest.indexElt)
            {
                // Copie de tous les samples de l'instrument dans un autre
                EltID id = idSrc;
                id.typeElement = elementInstSmpl;
                int nbInstSmpl = this->sf2->count(id);
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                for (int i = 0; i < nbInstSmpl; i++)
                {
                    id.indexElt2 = i;
                    this->dragAndDrop(idDest, id, 0, msg, ba1, ba2);
                }
            }
            break;
        case elementInstSmpl:
            if ((idDest.typeElement == elementInst || idDest.typeElement == elementInstSmpl) && \
                    idDest.indexElt != idSrc.indexElt)
            {
                // Copie d'un sample lié à un instrument
                Valeur val;
                idDest.typeElement = elementInstSmpl;
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                // Création InstSmpl
                idDest.indexElt2 = this->sf2->add(idDest);
                // Copie des gens
                idSrc.typeElement = elementInstSmplGen;
                for (int i = 0; i < this->sf2->count(idSrc); i++)
                {
                    idSrc.indexMod = i;
                    val.genValue = this->sf2->get(idSrc, champ_sfGenAmount).genValue;
                    this->sf2->set(idDest, this->sf2->get(idSrc, champ_sfGenOper).sfGenValue, val);
                }
                // Copie des mods
                idSrc.typeElement = elementInstSmplMod;
                idDest.typeElement = elementInstSmplMod;
                for (int i = 0; i < this->sf2->count(idSrc); i++)
                {
                    idSrc.indexMod = i;
                    idDest.indexMod = this->sf2->add(idDest);
                    this->sf2->set(idDest, champ_modAmount, this->sf2->get(idSrc, champ_modAmount));
                    this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(idSrc, champ_sfModSrcOper));
                    this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(idSrc, champ_sfModAmtSrcOper));
                    this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(idSrc, champ_sfModDestOper));
                    this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(idSrc, champ_sfModTransOper));
                }
                idSrc.typeElement = elementInstSmpl;
            }
            break;
        case elementPrst:
            if ((idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst) && \
                                 idSrc.indexElt != idDest.indexElt)
            {
                // Copie de tous les instruments du preset dans un autre
                EltID id = idSrc;
                id.typeElement = elementPrstInst;
                int nbPrstInst = this->sf2->count(id);
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                for (int i = 0; i < nbPrstInst; i++)
                {
                    id.indexElt2 = i;
                    this->dragAndDrop(idDest, id, 0, msg, ba1, ba2);
                }
            }
            break;
        case elementPrstInst:
            if ((idDest.typeElement == elementPrst || idDest.typeElement == elementPrstInst) && \
                    idDest.indexElt != idSrc.indexElt)
            {
                // Copie d'un instrument lié à un preset
                Valeur val;
                idDest.typeElement = elementPrstInst;
                if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
                // Création PrstInst
                idDest.indexElt2 = this->sf2->add(idDest);
                // Copie des gens
                idSrc.typeElement = elementPrstInstGen;
                for (int i = 0; i < this->sf2->count(idSrc); i++)
                {
                    idSrc.indexMod = i;
                    val.genValue = this->sf2->get(idSrc, champ_sfGenAmount).genValue;
                    this->sf2->set(idDest, this->sf2->get(idSrc, champ_sfGenOper).sfGenValue, val);
                }
                // Copie des mods
                idSrc.typeElement = elementPrstInstMod;
                idDest.typeElement = elementPrstInstMod;
                for (int i = 0; i < this->sf2->count(idSrc); i++)
                {
                    idSrc.indexMod = i;
                    idDest.indexMod = this->sf2->add(idDest);
                    this->sf2->set(idDest, champ_modAmount, this->sf2->get(idSrc, champ_modAmount));
                    this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(idSrc, champ_sfModSrcOper));
                    this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(idSrc, champ_sfModAmtSrcOper));
                    this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(idSrc, champ_sfModDestOper));
                    this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(idSrc, champ_sfModTransOper));
                }
                idSrc.typeElement = elementPrstInst;
            }
            break;
        default: return;
        }
    }
    else
    {
        // Copie entre 2 sf2
        if (ba1->isEmpty())
        {
            // Préparation des samples
            EltID id = idSrc;
            id.typeElement = elementSmpl;
            ba1->resize(this->sf2->count(id)*4);
            qint32 * baba1 = (qint32 *)ba1->data();
            for (int i = 0; i < this->sf2->count(id); i++)
                baba1[i] = -1;
        }
        if (ba2->isEmpty())
        {
            // Préparation des instruments
            EltID id = idSrc;
            id.typeElement = elementInst;
            ba2->resize(this->sf2->count(id)*4);
            qint32 * baba2 = (qint32 *)ba2->data();
            for (int i = 0; i < this->sf2->count(id); i++)
                baba2[i] = -1;
        }
        switch (idSrc.typeElement)
        {
        case elementSmpl:{
            if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
            // Copie d'un sample
            idDest.typeElement = elementSmpl;
            // Nom du sample à copier
            QString nom = this->sf2->getQstr(idSrc, champ_name);
            // Recherche si un sample du même nom existe déjà
            int index = -1;
            if (msg[0] != -1)
            {
                for (int j = 0; j < this->sf2->count(idDest); j++)
                {
                    idDest.indexElt = j;
                    if (!this->sf2->get(idDest, champ_hidden).bValue)
                    {
                        if (this->sf2->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                            index = j;
                    }
                }
                if (msg[0] != 2 && msg[0] != 4 && (index != -1))
                {
                    // Remplacement ?
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Warning);
                    QString qStr3 = tr("Le sample «&#160;") + nom.left(20).toUtf8() + \
                            tr("&#160;» existe déjà.<br />Que faire ?");
                    msgBox.setText(QString::fromUtf8(qStr3.toStdString().c_str()));
                    msgBox.setInformativeText("");
                    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save | \
                                              QMessageBox::NoAll | QMessageBox::No);
                    msgBox.button(QMessageBox::Yes)->setText(tr("&Remplacer"));
                    msgBox.button(QMessageBox::YesAll)->setText(tr("R&emplacer tout"));
                    msgBox.button(QMessageBox::Save)->setText(tr("&Dupliquer"));
                    msgBox.button(QMessageBox::SaveAll)->setText(tr("D&upliquer tout"));
                    msgBox.button(QMessageBox::No)->setText(tr("&Ignorer"));
                    msgBox.button(QMessageBox::NoAll)->setText(tr("I&gnorer tout"));
                    msgBox.setDefaultButton(QMessageBox::YesAll);
                    switch (msgBox.exec())
                    {
                    case QMessageBox::NoAll: msg[0] = 4; break;
                    case QMessageBox::No: msg[0] = 3; break;
                    case QMessageBox::YesAll: msg[0] = 2; break;
                    case QMessageBox::Yes: msg[0] = 1; break;
                    case QMessageBox::Save: msg[0] = 0; break;
                    case QMessageBox::SaveAll: msg[0] = -1; break;
                    }
                }
            }
            if (index != -1 && msg[0] > 0)
            {
                // utilisation de l'index d'un sample existant
                idDest.indexElt = index;
            }
            else
            {
                // création d'un nouveau smpl
                idDest.indexElt = this->sf2->add(idDest);
            }
            qint32 * baba1 = (qint32 *)ba1->data();
            if (index == -1 || msg[0] < 3)
            {
                // Configuration du sample
                this->sf2->set(idDest, champ_sampleData16, this->sf2->getData(idSrc, champ_sampleData16));
                EltID id3 = idDest;
                id3.typeElement = elementSf2;
                if (this->sf2->get(id3, champ_wBpsSave).wValue == 24)
                    this->sf2->set(idDest, champ_sampleData24, this->sf2->getData(idSrc, champ_sampleData24));
                this->sf2->set(idDest, champ_dwLength, this->sf2->get(idSrc, champ_dwLength));
                this->sf2->set(idDest, champ_dwSampleRate, this->sf2->get(idSrc, champ_dwSampleRate));
                this->sf2->set(idDest, champ_dwStartLoop, this->sf2->get(idSrc, champ_dwStartLoop));
                this->sf2->set(idDest, champ_dwEndLoop, this->sf2->get(idSrc, champ_dwEndLoop));
                this->sf2->set(idDest, champ_sfSampleType, this->sf2->get(idSrc, champ_sfSampleType));
                this->sf2->set(idDest, champ_byOriginalPitch, this->sf2->get(idSrc, champ_byOriginalPitch));
                this->sf2->set(idDest, champ_chPitchCorrection, this->sf2->get(idSrc, champ_chPitchCorrection));
                this->sf2->set(idDest, champ_name, this->sf2->getQstr(idSrc, champ_name).left(20));
                // Lien
                if (this->sf2->get(idSrc, champ_sfSampleType).wValue != RomMonoSample &&
                        this->sf2->get(idSrc, champ_sfSampleType).wValue != monoSample)
                {
                    // Possible ?
                    WORD indexLink = this->sf2->get(idSrc, champ_wSampleLink).wValue;
                    if (baba1[indexLink] != -1)
                    {
                        // Association
                        Valeur val;
                        EltID idLink = idDest;
                        idLink.indexElt = baba1[indexLink];
                        val.wValue = idLink.indexElt;
                        this->sf2->set(idDest, champ_wSampleLink, val);
                        val.wValue = idDest.indexElt;
                        this->sf2->set(idLink, champ_wSampleLink, val);
                    }
                }
            }
            // Enregistrement de l'index
            baba1[idSrc.indexElt] = idDest.indexElt;
        }break;
        case elementInst:{
            if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
            // Copie des InstSmpl associés
            EltID id = idSrc;
            EltID id2 = idSrc;
            id.typeElement = elementInstSmpl;
            id2.typeElement = elementSmpl;
            qint32 * baba1 = (qint32 *)ba1->data();
            for (int i = 0; i < this->sf2->count(id); i++)
            {
                id.indexElt2 = i;
                if (!this->sf2->get(id, champ_hidden).bValue)
                {
                    id2.indexElt = this->sf2->get(id, champ_sampleID).wValue;
                    if (baba1[id2.indexElt] == -1) // Ne pas copier un smpl déjà copié
                        this->dragAndDrop(idDest, id2, 0, msg, ba1, ba2);
                }
            }
            // Copie d'un instrument
            idDest.typeElement = elementInst;
            // Nom de l'instrument à copier
            QString nom = this->sf2->getQstr(idSrc, champ_name);
            // Recherche si un instrument du même nom existe déjà
            int index = -1;
            if (msg[1] != -1)
            {
                for (int j = 0; j < this->sf2->count(idDest); j++)
                {
                    idDest.indexElt = j;
                    if (!this->sf2->get(idDest, champ_hidden).bValue)
                    {
                        if (this->sf2->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                            index = j;
                    }
                }
                if (msg[1] != 2 && msg[1] != 4 && (index != -1))
                {
                    // Remplacement ?
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Warning);
                    QString qStr3 = tr("L'instrument «&#160;") + nom.left(20).toUtf8() + \
                            tr("&#160;» existe déjà.<br />Souhaitez-vous le remplacer ?");
                    msgBox.setText(QString::fromUtf8(qStr3.toStdString().c_str()));
                    msgBox.setInformativeText("");
                    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save | \
                                              QMessageBox::NoAll | QMessageBox::No);
                    msgBox.button(QMessageBox::Yes)->setText(tr("&Remplacer"));
                    msgBox.button(QMessageBox::YesAll)->setText(tr("R&emplacer tout"));
                    msgBox.button(QMessageBox::Save)->setText(tr("&Dupliquer"));
                    msgBox.button(QMessageBox::SaveAll)->setText(tr("D&upliquer tout"));
                    msgBox.button(QMessageBox::No)->setText(tr("&Ignorer"));
                    msgBox.button(QMessageBox::NoAll)->setText(tr("I&gnorer tout"));
                    msgBox.setDefaultButton(QMessageBox::YesAll);
                    switch (msgBox.exec())
                    {
                    case QMessageBox::NoAll: msg[1] = 4; break;
                    case QMessageBox::No: msg[1] = 3; break;
                    case QMessageBox::YesAll: msg[1] = 2; break;
                    case QMessageBox::Yes: msg[1] = 1; break;
                    case QMessageBox::Save: msg[1] = 0; break;
                    case QMessageBox::SaveAll: msg[1] = -1; break;
                    }
                }
            }
            if (index != -1 && msg[1] > 0)
            {
                // utilisation de l'index d'un instrument déjà existant
                idDest.indexElt = index;
                if (msg[1] < 3)
                {
                    // effacement des mods globaux, gens globaux et des InstSmpl
                    id = idDest;
                    id.typeElement = elementInstGen;
                    id.indexMod = 0;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexMod = i;
                        this->sf2->reset(idDest, this->sf2->get(id, champ_sfGenOper).sfGenValue);
                    }
                    id.typeElement = elementInstMod;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexMod = i;
                        if (!this->sf2->get(id, champ_hidden).bValue)
                            this->sf2->remove(id);
                    }
                    id.typeElement = elementInstSmpl;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexElt2 = i;
                        if (!this->sf2->get(id, champ_hidden).bValue)
                            this->sf2->remove(id);
                    }
                }
            }
            else
            {
                // création d'un nouvel instrument
                idDest.indexElt = this->sf2->add(idDest);
            }
            qint32 * baba2 = (qint32 *)ba2->data();
            // Modification de l'instrument
            if (index == -1 || msg[1] < 3)
            {
                // Info
                this->sf2->set(idDest, champ_name, nom.left(20));
                // Gen globaux
                id = idSrc;
                id.typeElement = elementInstGen;
                Valeur val;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    val = this->sf2->get(id, champ_sfGenAmount);
                    this->sf2->set(idDest, this->sf2->get(id, champ_sfGenOper).sfGenValue, val);
                }
                // Mod globaux
                id.typeElement = elementInstMod;
                idDest.typeElement = elementInstMod;
                // Correspondance des index et initialisation
                int *indMod = new int[this->sf2->count(id)];
                for (int i = 0; i < this->sf2->count(id); i++)
                    indMod[i] = -1;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        // Création nouveau mod
                        idDest.indexMod = this->sf2->add(idDest);
                        indMod[i] = idDest.indexMod;
                        this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(id, champ_sfModSrcOper));
                        this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(id, champ_sfModAmtSrcOper));
                        this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(id, champ_sfModTransOper));
                        this->sf2->set(idDest, champ_modAmount, this->sf2->get(id, champ_modAmount));
                        if (this->sf2->get(id, champ_sfModDestOper).wValue < 32768) // pas de lien
                            this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(id, champ_sfModDestOper));
                    }
                }
                // Mise en place des liens
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        if (this->sf2->get(id, champ_sfModDestOper).wValue > 32767)
                        {
                            idDest.indexMod = indMod[i];
                            val.wValue = 32768 + indMod[this->sf2->get(id, champ_sfModDestOper).wValue - 32768];
                            this->sf2->set(idDest, champ_sfModDestOper, val);
                        }
                    }
                }
                delete indMod;
                // InstSmpl
                id.typeElement = elementInstSmpl;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexElt2 = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        // Création d'un lien avec sample
                        idDest.typeElement = elementInstSmpl;
                        idDest.indexElt2 = this->sf2->add(idDest);
                        id2 = id;
                        // Copie des gens
                        id2.typeElement = elementInstSmplGen;
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (this->sf2->get(id2, champ_sfGenOper).sfGenValue == champ_sampleID)
                                val.wValue = baba1[this->sf2->get(id2, champ_sfGenAmount).wValue];
                            else
                                val = this->sf2->get(id2, champ_sfGenAmount);
                            this->sf2->set(idDest, this->sf2->get(id2, champ_sfGenOper).sfGenValue, val);
                        }
                        // Copie des mods
                        idDest.typeElement = elementInstSmplMod;
                        id2.typeElement = elementInstSmplMod;
                        // Correspondance des index et initialisation
                        int *indMod = new int[this->sf2->count(id2)];
                        for (int j = 0; j < this->sf2->count(id2); j++)
                            indMod[j] = -1;
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (!this->sf2->get(id2, champ_hidden).bValue)
                            {
                                idDest.indexMod = this->sf2->add(idDest);
                                indMod[j] = idDest.indexMod;
                                this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(id2, champ_sfModSrcOper));
                                this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(id2, champ_sfModAmtSrcOper));
                                this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(id2, champ_sfModTransOper));
                                this->sf2->set(idDest, champ_modAmount, this->sf2->get(id2, champ_modAmount));
                                if (this->sf2->get(id2, champ_sfModDestOper).wValue < 32768) // pas de lien
                                    this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(id2, champ_sfModDestOper));
                            }
                        }
                        // Mise en place des liens
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (!this->sf2->get(id2, champ_hidden).bValue)
                            {
                                if (this->sf2->get(id2, champ_sfModDestOper).wValue > 32767)
                                {
                                    idDest.indexMod = indMod[j];
                                    val.wValue = 32768 + indMod[this->sf2->get(id2, champ_sfModDestOper).wValue - 32768];
                                    this->sf2->set(idDest, champ_sfModDestOper, val);
                                }
                            }
                        }
                        delete indMod;
                    }
                }
            }
            // Enregistrement de l'index
            baba2[idSrc.indexElt] = idDest.indexElt;
        }break;
        case elementPrst:
        {
            if (temps == -1 || temps == 2) this->sf2->prepareNewActions();
            // Copie des PrstInst associés
            EltID id = idSrc;
            EltID id2 = idSrc;
            id.typeElement = elementPrstInst;
            id2.typeElement = elementInst;
            qint32 * baba2 = (qint32 *)ba2->data();
            for (int i = 0; i < this->sf2->count(id); i++)
            {
                id.indexElt2 = i;
                if (!this->sf2->get(id, champ_hidden).bValue)
                {
                    id2.indexElt = this->sf2->get(id, champ_instrument).wValue;
                    if (baba2[id2.indexElt] == -1) // Ne pas copier un inst déjà copié
                        this->dragAndDrop(idDest, id2, 0, msg, ba1, ba2);
                }
            }
            // Copie d'un preset
            idDest.typeElement = elementPrst;
            // Nom du preset à copier
            QString nom = this->sf2->getQstr(idSrc, champ_name);
            // Recherche si un preset du même nom existe déjà
            int index = -1;
            if (msg[2] != -1)
            {
                for (int j = 0; j < this->sf2->count(idDest); j++)
                {
                    idDest.indexElt = j;
                    if (!this->sf2->get(idDest, champ_hidden).bValue)
                    {
                        if (this->sf2->getQstr(idDest, champ_name).compare(nom.left(20)) == 0)
                            index = j;
                    }
                }
                if (msg[2] != 2 && msg[2] != 4 && (index != -1))
                {
                    // Remplacement ?
                    QMessageBox msgBox(this);
                    msgBox.setIcon(QMessageBox::Warning);
                    QString qStr3 = tr("Le preset «&#160;") + nom.left(20).toUtf8() + \
                            tr("&#160;» existe déjà.<br />Souhaitez-vous le remplacer ?");
                    msgBox.setText(QString::fromUtf8(qStr3.toStdString().c_str()));
                    msgBox.setInformativeText("");
                    msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save | \
                                              QMessageBox::NoAll | QMessageBox::No);
                    msgBox.button(QMessageBox::Yes)->setText(tr("&Remplacer"));
                    msgBox.button(QMessageBox::YesAll)->setText(tr("R&emplacer tout"));
                    msgBox.button(QMessageBox::Save)->setText(tr("&Dupliquer"));
                    msgBox.button(QMessageBox::SaveAll)->setText(tr("D&upliquer tout"));
                    msgBox.button(QMessageBox::No)->setText(tr("&Ignorer"));
                    msgBox.button(QMessageBox::NoAll)->setText(tr("I&gnorer tout"));
                    msgBox.setDefaultButton(QMessageBox::YesAll);
                    switch (msgBox.exec())
                    {
                    case QMessageBox::NoAll: msg[2] = 4; break;
                    case QMessageBox::No: msg[2] = 3; break;
                    case QMessageBox::YesAll: msg[2] = 2; break;
                    case QMessageBox::Yes: msg[2] = 1; break;
                    case QMessageBox::Save: msg[2] = 0; break;
                    case QMessageBox::SaveAll: msg[2] = -1; break;
                    }
                }
            }
            if (index != -1 && msg[2] > 0)
            {
                // utilisation de l'index d'un preset existant
                idDest.indexElt = index;
                if (msg[2] < 3)
                {
                    // effacement des mods globaux, gens globaux et des PrstInst
                    id = idDest;
                    id.typeElement = elementPrstGen;
                    id.indexMod = 0;
                    Champ champ;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexMod = i;
                        champ = this->sf2->get(id, champ_sfGenOper).sfGenValue;
                        if (champ != champ_wBank && champ != champ_wPreset) // on garde bank et preset
                            this->sf2->reset(idDest, champ);
                    }
                    id.typeElement = elementPrstMod;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexMod = i;
                        if (!this->sf2->get(id, champ_hidden).bValue)
                            this->sf2->remove(id);
                    }
                    id.typeElement = elementPrstInst;
                    for (int i = 0; i < this->sf2->count(id); i++)
                    {
                        id.indexElt2 = i;
                        if (!this->sf2->get(id, champ_hidden).bValue)
                            this->sf2->remove(id);
                    }
                }
            }
            else
            {
                // Vérification qu'un emplacement dans la banque est disponible
                int numPreset;
                int numBank;
                this->page_prst->firstAvailablePresetBank(idDest, numBank, numPreset);
                if (numPreset == -1)
                {
                    // Aucun preset disponible
                    if (msg[3] == 0)
                    {
                        QMessageBox::warning(this, tr("Attention"), QString::fromUtf8(tr("Aucun preset n'est disponible.").toStdString().c_str()));
                        msg[3] = 1;
                    }
                    idDest.indexElt = -1;
                }
                else
                {
                    // création d'un nouveau preset
                    idDest.indexElt = this->sf2->add(idDest);
                    Valeur val;
                    val.wValue = numBank;
                    this->sf2->set(idDest, champ_wBank, val);
                    val.wValue = numPreset;
                    this->sf2->set(idDest, champ_wPreset, val);
                }
            }
            // Modification du preset
            if (idDest.indexElt != -1 && (index == -1 || msg[2] < 3))
            {
                // Info
                this->sf2->set(idDest, champ_name, nom.left(20));
                // Gen globaux
                id = idSrc;
                id.typeElement = elementPrstGen;
                Valeur val;
                Champ champ;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    val = this->sf2->get(id, champ_sfGenAmount);
                    champ = this->sf2->get(id, champ_sfGenOper).sfGenValue;
                    if (champ != champ_wBank && champ != champ_wPreset)
                        this->sf2->set(idDest, champ, val);
                }
                // Mod globaux
                id.typeElement = elementPrstMod;
                idDest.typeElement = elementPrstMod;
                // Correspondance des index et initialisation
                int *indMod = new int[this->sf2->count(id)];
                for (int i = 0; i < this->sf2->count(id); i++)
                    indMod[i] = -1;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        // Création nouveau mod
                        idDest.indexMod = this->sf2->add(idDest);
                        indMod[i] = idDest.indexMod;
                        this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(id, champ_sfModSrcOper));
                        this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(id, champ_sfModAmtSrcOper));
                        this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(id, champ_sfModTransOper));
                        this->sf2->set(idDest, champ_modAmount, this->sf2->get(id, champ_modAmount));
                        if (this->sf2->get(id, champ_sfModDestOper).wValue < 32768) // pas de lien
                            this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(id, champ_sfModDestOper));
                    }
                }
                // Mise en place des liens
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexMod = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        if (this->sf2->get(id, champ_sfModDestOper).wValue > 32767)
                        {
                            idDest.indexMod = indMod[i];
                            val.wValue = 32768 + indMod[this->sf2->get(id, champ_sfModDestOper).wValue - 32768];
                            this->sf2->set(idDest, champ_sfModDestOper, val);
                        }
                    }
                }
                delete indMod;
                // PrstInst
                id.typeElement = elementPrstInst;
                for (int i = 0; i < this->sf2->count(id); i++)
                {
                    id.indexElt2 = i;
                    if (!this->sf2->get(id, champ_hidden).bValue)
                    {
                        // Création d'un lien avec instrument
                        idDest.typeElement = elementPrstInst;
                        idDest.indexElt2 = this->sf2->add(idDest);
                        id2 = id;
                        // Copie des gens
                        id2.typeElement = elementPrstInstGen;
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (this->sf2->get(id2, champ_sfGenOper).sfGenValue == champ_instrument)
                                val.wValue = baba2[this->sf2->get(id2, champ_sfGenAmount).wValue];
                            else
                                val = this->sf2->get(id2, champ_sfGenAmount);
                            this->sf2->set(idDest, this->sf2->get(id2, champ_sfGenOper).sfGenValue, val);
                        }
                        // Copie des mods
                        idDest.typeElement = elementPrstInstMod;
                        id2.typeElement = elementPrstInstMod;
                        // Correspondance des index et initialisation
                        int *indMod = new int[this->sf2->count(id2)];
                        for (int j = 0; j < this->sf2->count(id2); j++)
                            indMod[j] = -1;
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (!this->sf2->get(id2, champ_hidden).bValue)
                            {
                                idDest.indexMod = this->sf2->add(idDest);
                                indMod[j] = idDest.indexMod;
                                this->sf2->set(idDest, champ_sfModSrcOper, this->sf2->get(id2, champ_sfModSrcOper));
                                this->sf2->set(idDest, champ_sfModAmtSrcOper, this->sf2->get(id2, champ_sfModAmtSrcOper));
                                this->sf2->set(idDest, champ_sfModTransOper, this->sf2->get(id2, champ_sfModTransOper));
                                this->sf2->set(idDest, champ_modAmount, this->sf2->get(id2, champ_modAmount));
                                if (this->sf2->get(id2, champ_sfModDestOper).wValue < 32768) // pas de lien
                                    this->sf2->set(idDest, champ_sfModDestOper, this->sf2->get(id2, champ_sfModDestOper));
                            }
                        }
                        // Mise en place des liens
                        for (int j = 0; j < this->sf2->count(id2); j++)
                        {
                            id2.indexMod = j;
                            if (!this->sf2->get(id2, champ_hidden).bValue)
                            {
                                if (this->sf2->get(id2, champ_sfModDestOper).wValue > 32767)
                                {
                                    idDest.indexMod = indMod[j];
                                    val.wValue = 32768 + indMod[this->sf2->get(id2, champ_sfModDestOper).wValue - 32768];
                                    this->sf2->set(idDest, champ_sfModDestOper, val);
                                }
                            }
                        }
                        delete indMod;
                    }
                }
            }
        }
        break;
        default: return;
        }
    }
    if (temps > 0)
    {
        delete [] msg;
        delete ba1;
        delete ba2;
        // Mise à jour de l'affichage
        this->updateDo();
        this->updateActions();
    }
}
void MainWindow::importerSmpl()
{
    if (ui->arborescence->getSelectedItemsNumber() == 0) return;
    QFileInfo qFileInfo = fileName;
    // Affichage dialogue
    QStringList strList = QFileDialog::getOpenFileNames(this, tr("Importer un fichier audio"), \
        qFileInfo.absolutePath(), tr("Fichier .wav (*.wav)"));
    if (strList.count() == 0) return;
    this->sf2->prepareNewActions();
    EltID id = {elementSmpl, this->ui->arborescence->getID(0).indexSf2, 0, 0, 0};
    QString qStr, nom;
    int replace = 0;
    int nbElt = strList.count();
    for (int i = 0; i < nbElt; i++)
    {
        qStr = strList.takeAt(0);
        if (i == 0) fileName = qStr;
        qFileInfo = qStr;
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
        if (replace != -1)
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
                            qStr3 = tr("Le sample «&#160;") + nom.left(19).toUtf8() + \
                                                tr("L&#160;» existe déjà.<br />Que faire ?");
                        }
                        else if (this->sf2->getQstr(id, champ_name).compare(nom.left(19).append("R")) == 0)
                        {
                            indexR = j;
                            qStr3 = tr("Le sample «&#160;") + nom.left(19).toUtf8() + \
                                                tr("R&#160;» existe déjà.<br />Que faire ?");
                        }
                    }
                    else
                    {
                        if (this->sf2->getQstr(id, champ_name).compare(nom.left(20)) == 0)
                        {
                            indexL = j;
                            qStr3 = tr("Le sample «&#160;") + nom.left(20).toUtf8() + \
                                                tr("&#160;» existe déjà.<br />Que faire ?");
                        }
                    }
                }
            }
            if (replace != 2 && replace != 4 && (indexL != -1 || indexR != -1))
            {
                // Remplacement ?
                QMessageBox msgBox(this);
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText(QString::fromUtf8(qStr3.toStdString().c_str()));
                msgBox.setInformativeText("");
                msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::SaveAll | QMessageBox::Save | \
                                          QMessageBox::NoAll | QMessageBox::No);
                msgBox.button(QMessageBox::Yes)->setText(tr("&Remplacer"));
                msgBox.button(QMessageBox::YesAll)->setText(tr("R&emplacer tout"));
                msgBox.button(QMessageBox::Save)->setText(tr("&Dupliquer"));
                msgBox.button(QMessageBox::SaveAll)->setText(tr("D&upliquer tout"));
                msgBox.button(QMessageBox::No)->setText(tr("&Ignorer"));
                msgBox.button(QMessageBox::NoAll)->setText(tr("I&gnorer tout"));
                msgBox.setDefaultButton(QMessageBox::YesAll);
                switch (msgBox.exec())
                {
                case QMessageBox::NoAll: replace = 4; break;
                case QMessageBox::No: replace = 3; break;
                case QMessageBox::YesAll: replace = 2; break;
                case QMessageBox::Yes: replace = 1; break;
                case QMessageBox::Save: replace = 0; break;
                case QMessageBox::SaveAll: replace = -1; break;
                }
            }
        }
        for (int j = 0; j < nChannels; j++)
        {
            if (replace < 3 || (nChannels == 2 && j == 0 && indexL == -1) || \
                    (nChannels == 2 && j == 1 && indexR == -1) || \
                    (nChannels == 1 && indexL == -1)) // Si pas ignorer
            {
                if (((nChannels == 2 && j == 0 && indexL != -1) || \
                    (nChannels == 2 && j == 1 && indexR != -1) || \
                     (nChannels == 1 && indexL != -1)) && (replace == 2 || replace == 1))
                {
                    if ((nChannels == 2 && j == 0 && indexL != -1) || (nChannels == 1 && indexL != -1))
                        id.indexElt = indexL;
                    else
                        id.indexElt = indexR;
                    // Mise à jour des données
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
                if (this->configuration.getRemoveBlank())
                    this->page_smpl->enleveBlanc(id);
                // Ajustement automatique à la boucle ?
                if (this->configuration.getWavAutoLoop())
                    this->page_smpl->enleveFin(id);
            }

            // Chargement dans la ram
            if (this->configuration.getRam())
            {
                val.wValue = 1;
                this->sf2->set(id, champ_ram, val);
            }
        }
        delete son;
    }
    updateDo();
    updateActions();
    this->ui->arborescence->searchTree(this->ui->editSearch->text());
}
void MainWindow::exporterSmpl()
{
    int nbElt = ui->arborescence->getSelectedItemsNumber();
    if (nbElt == 0) return;
    EltID id;
    QFileInfo qFileInfo = fileName;
    QString qDir = QFileDialog::getExistingDirectory(this, QString::fromUtf8(tr("Choisir un répertoire de destination").toStdString().c_str()), \
                                                     qFileInfo.absolutePath());
    if (qDir.isEmpty()) return;
    qDir.append(QDir::separator());
    QFile file;
    EltID id2;
    id = this->ui->arborescence->getID(0);
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
                if (this->sf2->get(id, champ_sfSampleType).wValue != monoSample && \
                        this->sf2->get(id, champ_sfSampleType).wValue != RomMonoSample)
                {
                    sampleID2 = this->sf2->get(id, champ_wSampleLink).wValue;
                    id2.indexElt = sampleID2;
                    id2.typeElement = elementSmpl;
                    // Nom du fichier
                    if (this->sf2->getQstr(id, champ_name).size() == this->sf2->getQstr(id2, champ_name).size())
                    {
                        QString nom1 = this->sf2->getQstr(id, champ_name);
                        QString nom2 = this->sf2->getQstr(id2, champ_name);
                        int size = nom1.size();
                        if (nom1.compare(nom2) == 0)
                            qStr = qStr.append(nom1);
                        else
                        {
                            // Les noms sont différents
                            QString nom1bis = nom1;
                            QString nom2bis = nom2;
                            if (nom1bis.left(size-1).compare(nom2bis.left(size-1)) == 0)
                            {
                                // Seule la dernière lettre change
                                const char c1 = this->sf2->getQstr(id, champ_name).right(1).toLower().toStdString()[0];
                                const char c2 = this->sf2->getQstr(id2, champ_name).right(1).toLower().toStdString()[0];
                                if ((c1 == 'l' || c1 == 'r') && (c2 == 'l' || c2 == 'r'))
                                    qStr = qStr.append(nom1.left(size-1));
                                else
                                    qStr = qStr.append(nom1);
                            }
                            else
                                qStr = qStr.append(nom1);
                        }
                    }
                    else
                        qStr.append(this->sf2->getQstr(id, champ_name));
                    // Mise à jour des états d'exporations
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
            }
        }
    }
    delete [] status;
}
void MainWindow::nouvelInstrument()
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->arborescence->getID(0);
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromUtf8(" "), tr("Nom du nouvel instrument :"), QLineEdit::Normal, "", &ok);
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
    int nPreset;
    int nBank;
    this->page_prst->firstAvailablePresetBank(id, nBank, nPreset);
    if (nPreset == -1)
    {
        QMessageBox::warning(this, tr("Attention"), QString::fromUtf8(tr("Aucun preset n'est disponible.").toStdString().c_str()));
        return;
    }
    bool ok;
    QString text = "";
    if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
    {
        id.typeElement = elementInst;
        text = this->sf2->getQstr(id, champ_name);
    }
    QString name = QInputDialog::getText(this, QString::fromUtf8(" "), tr("Nom du nouveau preset :"), QLineEdit::Normal, text, &ok);
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
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->arborescence->getID(0);
    this->anticipateNewAction(); // Les ID ne changeront pas lors du prochain prepareNewAction
    this->dialList.showDialog(id, DialogList::MODE_ASSOCIATION);
}
void MainWindow::associer(EltID idDest)
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb == 0 || (idDest.typeElement != elementInst && idDest.typeElement != elementPrst)) return;
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
    for (int i = 0; i < nb; i++)
    {
        idSrc = this->ui->arborescence->getID(i);
        // Création élément lié
        idDest.indexElt2 = this->sf2->add(idDest);
        // Association de idSrc vers idDest
        val.wValue = idSrc.indexElt;
        this->sf2->set(idDest, champ, val);
        if (champ == champ_sampleID)
        {
            // Balance
            if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == rightSample || \
                    this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == RomRightSample)
            {
                val.shValue = 500;
                this->sf2->set(idDest, champ_pan, val);
            }
            else if (this->sf2->get(idSrc, champ_sfSampleType).sfLinkValue == leftSample || \
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
    this->anticipateNewAction(); // Les ID ne changeront pas lors du prochain prepareNewAction
    this->dialList.showDialog(id, DialogList::MODE_REMPLACEMENT);
}
void MainWindow::remplacer(EltID idSrc)
{
    int nb = ui->arborescence->getSelectedItemsNumber();
    if (nb != 1 || (idSrc.typeElement != elementSmpl && idSrc.typeElement != elementInst)) return;
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
        QApplication::postEvent(focused,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
        QApplication::postEvent(focused,
            new QKeyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier));
    }
}
void MainWindow::coller()
{
    // émission d'un signal "coller"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
        QApplication::postEvent(focused,
            new QKeyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier));
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
void MainWindow::duplication()      {this->page_inst->duplication();}
void MainWindow::paramGlobal()      {this->page_inst->paramGlobal(&this->configuration);}
void MainWindow::repartitionAuto()  {this->page_inst->repartitionAuto();}
void MainWindow::spatialisation()   {this->page_inst->spatialisation();}
void MainWindow::mixture()          {this->page_inst->mixture();}
void MainWindow::release()          {this->page_inst->release();}
void MainWindow::duplicationPrst()  {this->page_prst->duplication();}
void MainWindow::paramGlobal2()     {this->page_prst->paramGlobal(&this->configuration);}
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
    char c1[20];
    if (unusedSmpl < 2) sprintf(c1, tr("%d sample et ").toStdString().c_str(), unusedSmpl);
    else sprintf(c1, tr("%d samples et ").toStdString().c_str(), unusedSmpl);
    char c2[40];
    if (unusedInst < 2) sprintf(c2, tr("%d instrument ont été supprimés.").toStdString().c_str(), unusedInst);
    else sprintf(c2, tr("%d instruments ont été supprimés.").toStdString().c_str(), unusedInst);
    QString qStr = c1;
    qStr.append(c2);
    QMessageBox::information(this, "", QString::fromUtf8(qStr.toStdString().c_str()));
    updateDo();
    updateActions();
}
void MainWindow::attenuationMini()
{
    // Atténuation minimale souhaitée
    bool ok;
    double rep = QInputDialog::getDouble(this, tr("Question"),
                                         QString::fromUtf8(tr("Atténuation minimale (dB) :").toStdString().c_str()),
                                         5, 0, 200, 2,
                                         &ok);
    if (!ok) return;
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


// Gestion du clavier virtuel / du son
void MainWindow::setAudioEngine(int audioEngine)
{
    emit(initAudio(audioEngine));
}
void MainWindow::showKeyboard(bool val)
{
    this->keyboard->setVisible(val);
    this->ui->velocityButton->setVisible(val);
    this->ui->labelNote->setVisible(val);
    this->ui->labelVelocite->setVisible(val);
}
void MainWindow::setVelocity(int val)
{
    this->configuration.setKeyboardVelocity(val);
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
void MainWindow::noteOn(int key)    {this->noteChanged(key, this->configuration.getKeyboardVelocity());}
void MainWindow::noteOff(int key)   {this->noteChanged(key, 0);}
void MainWindow::noteChanged(int key, int vel)
{
    // Mise en évidence de la ou des éléments liés étant en train de jouer
    this->page_inst->enlightColumn(key, vel != 0);
    this->page_prst->enlightColumn(key, vel != 0);

    // Cas particulier : arrêt de la lecture d'un sample
    if (key == -1 && vel == 0)
    {
        this->synth->play(0, 0, 0, -1, 0);
        return;
    }
    int defaultVelocity = this->configuration.getKeyboardVelocity();
    if (defaultVelocity > 0 && vel > 0)
        vel = defaultVelocity;
    if (vel > 0 && key != -1)
    {
        this->ui->labelNote->setText(QString("#%1").arg(key));
        this->ui->labelVelocite->setText(QString("%1").arg(vel));
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
        if (this->ui->arborescence->isSelectedItemsSf2Unique() &&
            this->ui->arborescence->isSelectedItemsTypeUnique())
        {
            EltID id = this->ui->arborescence->getID(0);
            if (id.typeElement == elementSmpl && this->ui->arborescence->getSelectedItemsNumber() == 1)
                this->synth->play(0, id.indexSf2, id.indexElt, key, vel);
            else if ((id.typeElement == elementInst || id.typeElement == elementInstSmpl) &&
                     this->ui->arborescence->isSelectedItemsFamilyUnique())
                this->synth->play(1, id.indexSf2, id.indexElt, key, vel);
            else if ((id.typeElement == elementPrst || id.typeElement == elementPrstInst) &&
                     this->ui->arborescence->isSelectedItemsFamilyUnique())
                this->synth->play(2, id.indexSf2, id.indexElt, key, vel);
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
