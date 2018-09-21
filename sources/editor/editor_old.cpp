#include "editor_old.h"
#include "ui_editor_old.h"
#include "sound.h"
#include "dialog_rename.h"
#include "dialog_attenuation.h"
#include "dialog_exportlist.h"
#include "sfz/conversion_sfz.h"
#include "dialog_export.h"
#include "duplicator.h"
#include "sfz/import_sfz.h"
#include "dialogchangelog.h"
#include "contextmanager.h"
#include "modalprogressdialog.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QUrl>
#include <QDate>
#include <QLibrary>
#include <QDesktopWidget>
#include <QDesktopServices>

// Constructeurs, destructeurs
MainWindowOld::MainWindowOld(QWidget *parent) : QMainWindow(parent, Qt::Window | Qt::WindowCloseButtonHint |
                                                            Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint |
                                                            Qt::WindowSystemMenuHint | Qt::WindowTitleHint |
                                                            Qt::CustomizeWindowHint
                                                            #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                                                            | Qt::WindowFullscreenButtonHint
                                                            #endif
                                                            ),
    ui(new Ui::MainWindowOld),
    synth(NULL),
    about(this),
    dialList(this),
    dialogMagneto(NULL),
    actionKeyboard(NULL),
    _currentKey(-1),
    _dialKeyboard(this, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint),
    _progressDialog(NULL),
    _isSustainOn(false)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    audioDevice = ContextManager::audio();
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    ui->editSearch->setPlaceholderText(trUtf8("Rechercher..."));
#endif
#ifdef Q_OS_MAC
    ui->verticalLayout_3->setSpacing(4);
#endif

    ui->actionPlein_cran->setChecked(this->windowState() & Qt::WindowFullScreen);

    // Initialisation de l'objet pile sf2
    this->sf2 = SoundfontManager::getInstance();

    // Connexion avec mise à jour table
    connect(this->sf2, SIGNAL(updateTable(int,int,int,int)), this, SLOT(updateTable(int,int,int,int)));

    // Initialisation du synthétiseur
    //this->synth = new Synth(this->sf2);

    // Connexions du magnétophone avec le synthé
    connect(this->synth, SIGNAL(sampleRateChanged(qint32)), &dialogMagneto, SLOT(setSampleRate(qint32)));
    connect(this->synth, SIGNAL(samplesRead(int)), &dialogMagneto, SLOT(avanceSamples(int)));

    // Initialisation de la sortie audio²²
    connect(this, SIGNAL(stopAudio()), this->audioDevice, SLOT(closeConnections()));
    connect(this->audioDevice, SIGNAL(connectionDone()), this, SLOT(onAudioConnectionDone()));

    connect(ContextManager::configuration(), SIGNAL(soundEngineConfigurationChanged()), this->synth, SLOT(updateConfiguration()));
    connect(ContextManager::configuration(), SIGNAL(audioServerConfigurationChanged()), this->audioDevice, SLOT(initAudio()));
    connect(ContextManager::configuration(), SIGNAL(interfaceChanged()), this, SLOT(noteNameChanged()));
    this->synth->updateConfiguration();
    this->audioDevice->initAudio();

    // Création des pages
    page_sf2 = new PageSf2();
    page_smpl = new PageSmpl();
    page_inst = new PageInst();
    page_prst = new PagePrst();
    _pageOverviewSmpl = new PageOverviewSmpl();
    _pageOverviewInst = new PageOverviewInst();
    _pageOverviewPrst = new PageOverviewPrst();
    ui->stackedWidget->addWidget(page_sf2);
    ui->stackedWidget->addWidget(page_smpl);
    ui->stackedWidget->addWidget(page_inst);
    ui->stackedWidget->addWidget(page_prst);
    ui->stackedWidget->addWidget(_pageOverviewSmpl);
    ui->stackedWidget->addWidget(_pageOverviewInst);
    ui->stackedWidget->addWidget(_pageOverviewPrst);

    // Initialisation arbre (passage de l'adresse de mainWindow et du combobox)
    ui->tree->init(this, sf2, ui->comboSf2);

    // Initialisation dialog liste (pointeur vers les sf2 et mainWindow)
    this->dialList.init(this, this->sf2);

    // Fichiers récents
    updateFavoriteFiles();

    // Affichage logo logiciel
    ui->stackedWidget->setCurrentWidget(ui->page_Soft);

    // Préférences d'affichage
    if (!ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "tool_bar", true).toBool())
    {
        ui->actionBarre_d_outils->setChecked(false);
        ui->toolBar->setVisible(false);
    }
    if (!ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "section_modulateur", true).toBool())
    {
        ui->actionSection_modulateurs->setChecked(false);
        this->page_inst->setModVisible(false);
        this->page_prst->setModVisible(false);
    }

    // Clavier
    _dialKeyboard.setWindowTitle(trUtf8("Clavier virtuel"));
    connect(&_dialKeyboard, SIGNAL(finished(int)), ui->actionDans_la_barre_d_outils, SLOT(trigger()));
    QVBoxLayout * layoutKeyboard = new QVBoxLayout();
    layoutKeyboard->setContentsMargins(0, 0, 0, 0);
    _dialKeyboard.setLayout(layoutKeyboard);
    _geometryDialKeyboard = ContextManager::configuration()->getValue(
                ConfManager::SECTION_DISPLAY, "keyboardGeometry", QByteArray()).toByteArray();
    if (_geometryDialKeyboard.isEmpty())
        _dialKeyboard.resize(600, 75);
    else
        _dialKeyboard.restoreGeometry(_geometryDialKeyboard);
    ui->toolBar->setFixedHeight(40);
    ui->toolBar->setContentsMargins(0, 0, 0, 0);
    actionKeyboard = ui->toolBar->addWidget(ui->ensembleKeyboard);
    this->showKeyboard(false);
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "keyboard_in_toolbar", true).toBool())
        this->on_actionDans_la_barre_d_outils_triggered();
    else
        this->on_action_Flottant_triggered();

    // Ouverture port midi et connexions
    ContextManager::midi()->openMidiPort(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "index_port", -1).toInt());
    connect(ui->widgetKeyboard, SIGNAL(noteOn(int,int)), this, SLOT(noteChanged(int,int)));
    connect(ui->widgetKeyboard, SIGNAL(sustainChanged(bool)), this, SLOT(setSustain(bool)));
    connect(ui->widgetKeyboard, SIGNAL(volumeChanged(int)), this, SLOT(setVolume(int)));
    connect(ui->widgetKeyboard, SIGNAL(noteOff(int)), this, SLOT(noteOff(int)));
    connect(ui->widgetKeyboard, SIGNAL(mouseOver(int, int)), this, SLOT(noteHover(int, int)));
    connect(this->page_smpl, SIGNAL(noteChanged(int,int)), this, SLOT(noteChanged(int,int)));

    // Initialisation objet Sound
    Sound::setParent(this);

    // Bug QT: restauration de la largeur d'un QDockWidget si fenêtre maximisée
    int dockWidth = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "dock_width", 150).toInt();
    if (ui->dockWidget->width() < dockWidth)
        ui->dockWidget->setMinimumWidth(dockWidth);
    else
        ui->dockWidget->setMaximumWidth(dockWidth);

    ui->actionA_propos->setMenuRole(QAction::AboutRole);
    ui->actionPr_f_rences->setMenuRole(QAction::PreferencesRole);

    connect(&_futureWatcher, SIGNAL (finished()), this, SLOT (futureFinished()));
}

MainWindowOld::~MainWindowOld()
{
    // Arrêt audio et fin du synthé
    this->stopAudio();
    SoundfontManager::kill();
    delete this->page_inst;
    delete this->page_prst;
    delete this->page_sf2;
    delete this->page_smpl;
    delete _pageOverviewSmpl;
    delete _pageOverviewInst;
    delete _pageOverviewPrst;
    delete this->synth;
    delete this->audioDevice;
    delete ui;
}

void MainWindowOld::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_K)
    {
        if (_dialKeyboard.isVisible())
            _dialKeyboard.activateWindow();
        ui->widgetKeyboard->setFocus();
        ui->widgetKeyboard->triggerGlowEffect();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindowOld::spaceKeyPressedInTree()
{
    if (ui->stackedWidget->currentWidget() == page_smpl)
        page_smpl->pushPlayPause();
}

void MainWindowOld::supprimerElt()
{
    // Suppression d'un ou plusieurs éléments dans l'arborescence
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb <= 0)
        return;

    QList<EltID> listID = ui->tree->getAllIDs();

    // 1er élément à supprimer
    EltID elementToSelect = ui->tree->getElementToSelectAfterDeletion();

    int message = 1;
    foreach (EltID id, listID)
        sf2->remove(id, &message);

    if (message % 2 == 0)
        QMessageBox::warning(this, trUtf8("Attention"),
                             trUtf8("Impossible de supprimer un échantillon s'il est utilisé par un instrument."));
    if (message % 3 == 0)
        QMessageBox::warning(this, trUtf8("Attention"),
                             trUtf8("Impossible de supprimer un instrument s'il est utilisé par un preset."));

    if (message == 1 && elementToSelect.typeElement != elementUnknown)
        ui->tree->select(elementToSelect, true);

    updateActions();
    updateDo();
}

// Sauvegarde, undo, redo
void MainWindowOld::sauvegarder()
{
    // Sauvegarde d'un SF2 (appel de l'interface)
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    if (!ui->tree->isSelectedItemsSf2Unique()) return;
    EltID id = ui->tree->getFirstID();
    sauvegarder(id.indexSf2, false);
}

void MainWindowOld::sauvegarderSous()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    if (!ui->tree->isSelectedItemsSf2Unique()) return;
    EltID id = ui->tree->getFirstID();
    sauvegarder(id.indexSf2, true);
}

int MainWindowOld::sauvegarder(int indexSf2, bool saveAs)
{
    // Remove the focus from the interface
    this->setFocus();
    if (!sf2->isEdited(indexSf2) && !saveAs)
        return 0;

    EltID id(elementSf2, indexSf2, 0, 0, 0);
    // Avertissement si enregistrement dans une résolution inférieure
    if (sf2->get(id, champ_wBpsSave).wValue < sf2->get(id, champ_wBpsInit).wValue)
    {
        int ret = QMessageBox::Save;
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(trUtf8("<b>Perte de résolution ") +
                       QString::number(sf2->get(id, champ_wBpsInit).wValue) + " &#8658; " +
                       QString::number(sf2->get(id, champ_wBpsSave).wValue) +
                       trUtf8(" bits</b>"));
        msgBox.setInformativeText(trUtf8("La qualité des samples sera abaissée suite à cette opération. Continuer ?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save);
        msgBox.button(QMessageBox::Save)->setText(trUtf8("&Oui"));
        msgBox.button(QMessageBox::Cancel)->setText(trUtf8("&Non"));
        msgBox.setDefaultButton(QMessageBox::Cancel);
        ret = msgBox.exec();
        if (ret == QMessageBox::Cancel)
            return 1;
    }

    // Compte du nombre de générateurs utilisés
    int unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen;
    //this->page_sf2->countUnused(unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen);
    if ((instGen > 65536 || prstGen > 65536) && ContextManager::configuration()->getValue(ConfManager::SECTION_WARNINGS,
                                                                                          "to_many_generators", true).toBool())
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
        if (ret == QMessageBox::Cancel)
            return 1;
        if (ret == QMessageBox::YesAll)
            ContextManager::configuration()->setValue(ConfManager::SECTION_WARNINGS, "to_many_generators", false);
    }

    /*QString fileName;
    if (saveAs || sf2->getQstr(id, champ_filename) == "")
    {
        if (sf2->getQstr(id, champ_filename) == "")
        {
            fileName = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SF2) +
                    "/" + sf2->getQstr(id, champ_name) + ".sf2";
            fileName = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder une soundfont"), fileName, trUtf8("Fichier .sf2 (*.sf2)"));
        }
        else
            fileName = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder une soundfont"),
                                                    sf2->getQstr(id, champ_filename), trUtf8("Fichier .sf2 (*.sf2)"));
        if (fileName.isNull())
            return 1;
    }
    else
        fileName = sf2->getQstr(id, champ_filename);
    switch (this->sf2->save(indexSf2, fileName))
    {
    case 0:
        // sauvegarde ok
        updateDo();
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SF2, fileName);
        updateFavoriteFiles();
        //        if (ui->stackedWidget->currentWidget() == this->page_sf2)
        //            this->page_sf2->preparePage();
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
    }*/
    return 1;
}

void MainWindowOld::undo()
{
    ui->tree->clearPastedID();
    //sf2->undo();
    updateActions();
    updateDo();
}

void MainWindowOld::redo()
{
    ui->tree->clearPastedID();
    //sf2->redo();
    updateActions();
    updateDo();
}

// Fenetres / affichage
void MainWindowOld::showAbout()
{
    about.show();
}

void MainWindowOld::AfficherBarreOutils()
{
    bool display = ui->actionBarre_d_outils->isChecked();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "tool_bar", display);
    ui->toolBar->setVisible(display);
}

void MainWindowOld::afficherSectionModulateurs()
{
    bool display = ui->actionSection_modulateurs->isChecked();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "section_modulateur", display);
    this->page_inst->setModVisible(display);
    this->page_prst->setModVisible(display);
}

void MainWindowOld::setKeyboardType0() {this->setKeyboardType(0);}
void MainWindowOld::setKeyboardType1() {this->setKeyboardType(1);}
void MainWindowOld::setKeyboardType2() {this->setKeyboardType(2);}
void MainWindowOld::setKeyboardType3() {this->setKeyboardType(3);}
void MainWindowOld::on_action88_notes_triggered() {this->setKeyboardType(4);}
void MainWindowOld::setKeyboardType(int val)
{
    ui->actionAucun->blockSignals(true);
    ui->action5_octaves->blockSignals(true);
    ui->action6_octaves->blockSignals(true);
    ui->action128_notes->blockSignals(true);
    ui->action88_notes->blockSignals(true);
    ui->actionAucun->setChecked(false);
    ui->action5_octaves->setChecked(false);
    ui->action6_octaves->setChecked(false);
    ui->action128_notes->setChecked(false);
    ui->action88_notes->setChecked(false);
    switch (val)
    {
    case 1:
        // Clavier 5 octaves
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 36);
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 61);
        ui->action5_octaves->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    case 2:
        // Clavier 6 octaves
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 36);
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 73);
        ui->action6_octaves->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    case 3:
        // Clavier 128 notes
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 0);
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 128);
        ui->action128_notes->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    case 4:
        // Clavier 88 notes
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 21);
        ui->widgetKeyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 88);
        ui->action88_notes->setChecked(true);
        if (ui->stackedWidget->currentWidget() == this->page_inst ||
                ui->stackedWidget->currentWidget() == this->page_prst)
            this->showKeyboard(true);
        break;
    default:
        // Aucun clavier
        ui->actionAucun->setChecked(true);
        this->showKeyboard(false);
        break;
    }

    // Redimensionnement
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "keyboard_in_toolbar", true).toBool())
    {
        int widthKeyboard = (int)((double)(ui->ensembleKeyboard->height()) * .87 * ui->widgetKeyboard->ratio()) + 12;
        ui->widgetKeyboard->setFixedWidth(widthKeyboard);
    }

    ui->actionAucun->blockSignals(false);
    ui->action5_octaves->blockSignals(false);
    ui->action6_octaves->blockSignals(false);
    ui->action128_notes->blockSignals(false);
    ui->action88_notes->blockSignals(false);

    // Save the parameter
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "type", val);
}
void MainWindowOld::on_actionDans_la_barre_d_outils_triggered()
{
    ui->actionDans_la_barre_d_outils->blockSignals(true);
    ui->actionDans_la_barre_d_outils->setChecked(true);
    ui->actionDans_la_barre_d_outils->blockSignals(false);
    ui->action_Flottant->blockSignals(true);
    ui->action_Flottant->setChecked(false);
    ui->action_Flottant->blockSignals(false);

    // Save the parameter
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "keyboard_in_toolbar", true);

    QGridLayout * layout = (QGridLayout*)ui->ensembleKeyboard->layout();
    layout->addWidget(ui->widgetKeyboard, 0, 3, 2, 1);
    updateActions();
}
void MainWindowOld::on_action_Flottant_triggered()
{
    ui->action_Flottant->blockSignals(true);
    ui->action_Flottant->setChecked(true);
    ui->action_Flottant->blockSignals(false);
    ui->actionDans_la_barre_d_outils->blockSignals(true);
    ui->actionDans_la_barre_d_outils->setChecked(false);
    ui->actionDans_la_barre_d_outils->blockSignals(false);

    // Save the parameter
    ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD, "keyboard_in_toolbar", false);

    _dialKeyboard.layout()->addWidget(ui->widgetKeyboard);
    updateActions();
}

void MainWindowOld::showKeyboard(bool val)
{
    ui->widgetKeyboard->setVisible(val);
    ui->labelNote->setVisible(val);
    ui->labelVelocite->setVisible(val);
    ui->label_2->setVisible(val);
    ui->label_3->setVisible(val);
    if (val && !ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "keyboard_in_toolbar", true).toBool())
    {
        if (!_dialKeyboard.isVisible())
        {
            ui->widgetKeyboard->setMinimumWidth(0);
            ui->widgetKeyboard->setMaximumWidth(100000);
            _dialKeyboard.restoreGeometry(_geometryDialKeyboard);
            _dialKeyboard.show();
        }
    }
    else
    {
        _geometryDialKeyboard = _dialKeyboard.saveGeometry();
        int widthKeyboard = (int)((double)(ui->ensembleKeyboard->height()) * .87 * ui->widgetKeyboard->ratio()) + 12;
        ui->widgetKeyboard->setFixedWidth(widthKeyboard);
        _dialKeyboard.hide();
    }
}

void MainWindowOld::onPleinEcranTriggered()
{
    this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
}

void MainWindowOld::magnetophone()
{
    dialogMagneto.show();
}

QList<QAction *> MainWindowOld::getListeActions()
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
                << ui->actionEnlever_blanc_au_d_part
                << ui->actionFiltrer
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
                << ui->action_Dissocier_les_samples_st_r_o
                << ui->actionExporter_pics_de_fr_quence
                << ui->action_Transposer
                << ui->actionEnlever_tous_les_modulateurs
                << ui->action_Commande
                << ui->actionExporter_la_liste_des_presets;
    return listeAction;
}

// Mise à jour
void MainWindowOld::updateDo()
{
    // Affichage undo / redo et titre
    ui->actionAnnuler->setEnabled(sf2->isUndoable(-1));
    ui->actionR_tablir->setEnabled(sf2->isRedoable(-1));
}

void MainWindowOld::updateActions()
{
    // Un élément est cliqué dans l'arborescence
    int nb;
    bool fichierUnique = true;
    bool familleUnique = true;
    bool typeUnique = true;
    ElementType type = elementUnknown;
    EltID id;

    // Caractéristiques de la sélection
    // Nombre d'éléments sélectionnés
    nb = ui->tree->getSelectedItemsNumber();
    if (nb == 0)
    {
        // Rien n'est sélectionné
        fichierUnique = false;

        // Affichage page logiciel
        ui->stackedWidget->setCurrentWidget(ui->page_Soft);

        // Export soundfont
        ui->actionExporter_en_tant_qu_sfz->setEnabled(false);
    }
    else
    {
        // Au moins un élément est sélectionné
        id = ui->tree->getFirstID();
        type = id.typeElement;
        fichierUnique = ui->tree->isSelectedItemsSf2Unique();
        typeUnique = ui->tree->isSelectedItemsTypeUnique();
        familleUnique = ui->tree->isSelectedItemsFamilyUnique();

        // Affichage partie droite
        if (typeUnique && fichierUnique && id.typeElement == elementSmpl)
        {
            // Affichage page Smpl
            //page_smpl->preparePage();
            if (ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "type", 1).toInt() > 0 && nb == 1)
                this->showKeyboard(true);
            else
                this->showKeyboard(false);
        }
        else if (fichierUnique && (familleUnique ||
                                   (typeUnique && (id.typeElement == elementInst || id.typeElement == elementPrst))))
        {
            if (id.typeElement == elementInst || id.typeElement == elementInstSmpl)
            {
                // Affichage page Inst
                //page_inst->afficher();
                if (ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "type", 1).toInt() > 0)
                    this->showKeyboard(familleUnique);
                else
                    this->showKeyboard(false);
            }
            else
            {
                // Affichage page Prst
                //page_prst->afficher();
                if (ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "type", 1).toInt() > 0)
                    this->showKeyboard(familleUnique);
                else
                    this->showKeyboard(false);
            }
        }
        else
        {
            if (fichierUnique)
            {
                //                if (typeUnique)
                //                {
                //                    switch (id.typeElement)
                //                    {
                //                    case elementSf2:
                //                        page_sf2->afficher();
                //                        break;
                //                    case elementRootSmpl:
                //                        _pageOverviewSmpl->afficher();
                //                        break;
                //                    case elementRootInst:
                //                        _pageOverviewInst->afficher();
                //                        break;
                //                    case elementRootPrst:
                //                        _pageOverviewPrst->afficher();
                //                        break;
                //                    default:
                //                        ui->stackedWidget->setCurrentWidget(ui->page_Soft);
                //                        break;
                //                    }
                //                }
                //                else
                //                    ui->stackedWidget->setCurrentWidget(ui->page_Soft);
            }
            else
                ui->stackedWidget->setCurrentWidget(ui->page_Soft);
            this->showKeyboard(false);
        }

        // Export soundfonts and samples
        ui->actionExporter_en_tant_qu_sfz->setEnabled(true);
        ui->actionExporter->setEnabled(typeUnique &&
                                       (id.typeElement == elementSmpl || id.typeElement == elementInstSmpl));
    }

    // ACTIVATION, DESACTIVATION DES COMMANDES
    if (fichierUnique)
    {
        // Actions possibles : fermer, importer, coller, enregistrer (sous)
        // Nouveau sample, instrument, preset
        ui->actionFermer_le_fichier->setEnabled(true);
        ui->actionImporter->setEnabled(true);
        ui->actionColler->setEnabled(true);
        ui->actionEnregistrer_sous->setEnabled(true);
        ui->actionEnregistrer->setEnabled(true);
        ui->actionNouveau_preset->setEnabled(true);
        ui->actionNouvel_instrument->setEnabled(true);

        // Supprimer, copier
        if (typeUnique && (((type == elementInstSmpl || type == elementPrstInst) && familleUnique) ||
                           type == elementSmpl || type == elementInst || type == elementPrst) &&
                !this->page_smpl->isPlaying())
        {
            ui->action_Supprimer->setEnabled(true);
            ui->actionCopier->setEnabled(true);
        }
        else
        {
            ui->action_Supprimer->setEnabled(false);
            ui->actionCopier->setEnabled(false);
        }

        // Renommer
        if (!typeUnique || (type != elementSmpl && type != elementInst && type != elementPrst && type != elementSf2))
        {
            ui->actionRenommer->setText(trUtf8("&Renommer"));
            ui->actionRenommer->setEnabled(false);
        }
        else
        {
            if (nb == 1)
            {
                ui->actionRenommer->setText(trUtf8("&Renommer"));
                ui->actionRenommer->setEnabled(true);
            }
            else
            {
                ui->actionRenommer->setText(trUtf8("&Renommer en masse"));
                ui->actionRenommer->setEnabled(true);
            }
        }

        // Outils
        this->enableActionSample(typeUnique && type == elementSmpl && !this->page_smpl->isPlaying());
        this->enableActionInstrument((typeUnique && type == elementInst) ||
                                     (familleUnique && (type == elementInst || type == elementInstSmpl)));
        this->enableActionPreset((typeUnique && type == elementPrst) ||
                                 (familleUnique && (type == elementPrst || type == elementPrstInst)));
        this->enableActionSf2(true);

        // Particularité 1 : "enlever éléments non utilisés" doit être désactivé si la lecture est en cours
        ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(!this->page_smpl->isPlaying());

        // Particularité 2: visualiseurs et mixtures désactivés si plusieurs instruments / presets sont sélectionnés
        ui->action_Visualiseur->setEnabled(typeUnique && familleUnique);
        ui->action_Visualiseur_2->setEnabled(typeUnique && familleUnique);
        ui->action_Cr_ation_mutation_mixture->setEnabled(typeUnique && familleUnique);

        // Particularité 3: paramétrage global, répartition automatique et duplication des divisions sont communs pour inst et prst
        ui->action_Param_trage_global->setEnabled((typeUnique && (type == elementInst || type == elementPrst)) ||
                                                  (familleUnique && (type == elementInst || type == elementInstSmpl ||
                                                                     type == elementPrst || type == elementPrstInst)));
        ui->action_R_partition_automatique->setEnabled((typeUnique && (type == elementInst || type == elementPrst)) ||
                                                       (familleUnique && (type == elementInst || type == elementInstSmpl ||
                                                                          type == elementPrst || type == elementPrstInst)));
        ui->actionD_uplication_des_divisions->setEnabled((typeUnique && (type == elementInst || type == elementPrst)) ||
                                                         (familleUnique && (type == elementInst || type == elementInstSmpl ||
                                                                            type == elementPrst || type == elementPrstInst)));
    }
    else
    {
        // Action impossible : fermer
        ui->actionFermer_le_fichier->setEnabled(false);

        // Action impossible : copier, coller, supprimer
        ui->actionCopier->setEnabled(false);
        ui->actionColler->setEnabled(false);
        ui->action_Supprimer->setEnabled(false);

        // Actions importer des échantillons
        ui->actionImporter->setEnabled(false);

        // Actions enregistrer (sous)
        ui->actionEnregistrer_sous->setEnabled(false);
        ui->actionEnregistrer->setEnabled(false);

        // Nouveau sample, instrument, preset
        ui->actionNouveau_preset->setEnabled(false);
        ui->actionNouvel_instrument->setEnabled(false);

        // Renommer, clavier, ...
        ui->actionRenommer->setEnabled(false);
        this->showKeyboard(false);

        // Outils
        this->enableActionSample(false);
        this->enableActionInstrument(false);
        this->enableActionPreset(false);
        this->enableActionSf2(false);
    }
}
void MainWindowOld::enableActionSample(bool isEnabled)
{
    ui->menuSample->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuSample->actions().size(); i++)
        ui->menuSample->actions().at(i)->setEnabled(isEnabled);
}
void MainWindowOld::enableActionInstrument(bool isEnabled)
{
    ui->menuInstrument->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuInstrument->actions().size(); i++)
        ui->menuInstrument->actions().at(i)->setEnabled(isEnabled);
}
void MainWindowOld::enableActionPreset(bool isEnabled)
{
    ui->menuPreset->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuPreset->actions().size(); i++)
        ui->menuPreset->actions().at(i)->setEnabled(isEnabled);
}
void MainWindowOld::enableActionSf2(bool isEnabled)
{
    ui->menuDivers->setEnabled(isEnabled);
    for (int i = 0; i < ui->menuDivers->actions().size(); i++)
        ui->menuDivers->actions().at(i)->setEnabled(isEnabled);
}

void MainWindowOld::desactiveOutilsSmpl()
{
    // Appel depuis pageSmpl
    ui->action_Supprimer->setEnabled(0);
    ui->menuSample->setEnabled(0);
    ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(0);
    ui->tree->desactiveSuppression();
}
void MainWindowOld::activeOutilsSmpl()
{
    // Appel depuis pageSmpl
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb != 1)
    {
        this->updateActions();
        ui->tree->clicTree();
        return;
    }
    EltID ID = ui->tree->getFirstID();
    ElementType type = ID.typeElement;
    if (type != elementSmpl)
    {
        this->updateActions();
        ui->tree->clicTree();
        return;
    }
    ui->action_Supprimer->setEnabled(1);
    ui->menuSample->setEnabled(1);
    ui->action_Enlever_les_l_ments_non_utilis_s->setEnabled(1);
    ui->tree->activeSuppression();
}
bool MainWindowOld::isPlaying()
{
    return this->page_smpl->isPlaying();
}
void MainWindowOld::updateFavoriteFiles()
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
        if (!ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_SF2, i).isEmpty())
        {
            qAct->setText(ContextManager::recentFile()->getLastFile(RecentFileManager::FILE_TYPE_SF2, i));
            qAct->setVisible(true);
            qAct->setEnabled(true);
        }
        else
        {
            qAct->setVisible(false);
            qAct->setEnabled(false);
        }
    }
}

void MainWindowOld::noteNameChanged()
{
    QWidget * widget = ui->stackedWidget->currentWidget();
    if (widget == this->page_smpl || widget == this->page_inst || widget == this->page_prst ||
            widget == _pageOverviewSmpl || widget == _pageOverviewInst || widget == _pageOverviewPrst)
    {
        Page * page = (Page *)ui->stackedWidget->currentWidget();
        //        page->afficher();
    }
}

// Modifications
void MainWindowOld::renommer()
{
    bool ok;
    // Nb d'éléments sélectionnés
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID ID = ui->tree->getFirstID();
    ElementType type = ID.typeElement;

    if (type != elementSf2 && type != elementSmpl && type != elementInst && type != elementPrst)
        return;

    if (nb > 1)
    {
        DialogRename * dial = new DialogRename(type == elementSmpl, this);
        dial->setAttribute(Qt::WA_DeleteOnClose);
        connect(dial, SIGNAL(updateNames(int, QString, QString, int, int)),
                this, SLOT(renommerEnMasse(int, QString, QString, int, int)));
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
            ID = ui->tree->getFirstID();
            sf2->set(ID, champ_name, text);
            updateDo();
            updateActions();
        }
    }
}
void MainWindowOld::renommerEnMasse(int renameType, QString text1, QString text2, int val1, int val2)
{
    if (renameType == 4)
    {
        if (val1 == val2)
            return;
    }
    else
    {
        if (text1.isEmpty())
            return;
    }

    QList<EltID> listID = ui->tree->getAllIDs();

    for (int i = 0; i < listID.size(); i++)
    {
        EltID ID = listID.at(i);

        // Détermination du nom
        QString newName = sf2->getQstr(ID, champ_name);
        switch (renameType)
        {
        case 0:{
            // Remplacement avec note en suffixe
            QString suffix = " " + ContextManager::keyName()->getKeyName(sf2->get(ID, champ_byOriginalPitch).bValue, false, true);
            SFSampleLink pos = sf2->get(ID, champ_sfSampleType).sfLinkValue;
            if (pos == rightSample || pos == RomRightSample)
                suffix += 'R';
            else if (pos == leftSample || pos == RomLeftSample)
                suffix += 'L';

            newName = text1.left(20 - suffix.size()) + suffix;
        }break;
        case 1:
            // Remplacement avec index en suffixe
            if ((i+1) % 100 < 10)
                newName = text1.left(17) + "-0" + QString::number((i+1) % 100);
            else
                newName = text1.left(17) + "-" + QString::number((i+1) % 100);
            break;
        case 2:
            // Remplacement d'une chaîne de caractère
            newName.replace(text1, text2, Qt::CaseInsensitive);
            break;
        case 3:
            // Insertion d'une chaîne de caractère
            if (val1 > newName.size())
                val1 = newName.size();
            newName.insert(val1, text1);
            break;
        case 4:
            // Suppression d'une étendue
            if (val2 > val1)
                newName.remove(val1, val2 - val1);
            else
                newName.remove(val2, val1 - val2);
            break;
        }

        newName = newName.left(20);

        if (sf2->getQstr(ID, champ_name).compare(newName, Qt::CaseInsensitive))
            sf2->set(ID, champ_name, newName);
    }

    updateDo();
    updateActions();
}

void MainWindowOld::dragAndDrop(EltID idDest, QList<EltID> idSources)
{
    // prepareNewActions() et updateDo() faits à l'extérieur
    Duplicator duplicator(this->sf2, this->sf2, this);
    for (int i = 0; i < idSources.size(); i++)
        duplicator.copy(idSources.at(i), idDest);
    updateActions();
}

void MainWindowOld::importerSmpl()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;

    // Other allowed format?
    QString ext = "";
    typedef QString (*MyPrototype)();
    MyPrototype myFunction = (MyPrototype) QLibrary::resolve(QCoreApplication::applicationDirPath() + "/extension_lecture",
                                                             "getExtensionFilter");
    if (myFunction)
        ext = myFunction();

    // Display dialog
    QStringList strList = QFileDialog::getOpenFileNames(this, trUtf8("Importer un fichier audio"),
                                                        ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE),
                                                        trUtf8("Fichier .wav") + " (*.wav)" + ext);

    if (strList.count() == 0)
        return;

    EltID id = ui->tree->getFirstID();
    int replace = 0;
    while (!strList.isEmpty())
    {
        ui->tree->clearSelection();
        //this->dragAndDrop(strList.takeAt(0), id, &replace);
    }
    updateDo();
    updateActions();
}

void MainWindowOld::importerSmpl(QString path, EltID id, int * replace)
{
    id.typeElement = elementSmpl;
    QString qStr, nom;

    qStr = path;
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, qStr);
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
        foreach (int j, this->sf2->getSiblings(id))
        {
            id.indexElt = j;
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
            case QMessageBox::NoAll:    *replace =  4; break;
            case QMessageBox::No:       *replace =  3; break;
            case QMessageBox::YesAll:   *replace =  2; break;
            case QMessageBox::Yes:      *replace =  1; break;
            case QMessageBox::Save:     *replace =  0; break;
            case QMessageBox::SaveAll:  *replace = -1; break;
            }
        }
    }

    // Ajustement du nom, s'il est déjà utilisé
    if (*replace == 0 || *replace == -1)
    {
        QStringList listSampleName;
        foreach (int j, this->sf2->getSiblings(id))
        {
            id.indexElt = j;
            listSampleName << this->sf2->getQstr(id, champ_name);
        }

        int suffixNumber = 0;
        if (nChannels == 1)
        {
            while (listSampleName.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 20, suffixNumber);
        }
        else
        {
            while ((listSampleName.contains(getName(nom, 19, suffixNumber) + "L", Qt::CaseInsensitive) ||
                    listSampleName.contains(getName(nom, 19, suffixNumber) + "R", Qt::CaseInsensitive)) &&
                   suffixNumber < 100)
            {
                suffixNumber++;
            }
            nom = getName(nom, 19, suffixNumber);
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
                this->sf2->set(id, champ_filenameForData, qStr);
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
            val.bValue = (quint8)son->get(champ_byOriginalPitch);
            this->sf2->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)son->get(champ_chPitchCorrection);
            this->sf2->set(id, champ_chPitchCorrection, val);

            // Automatically remove leading blank?
//            if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool())
//                this->page_smpl->enleveBlanc(id);

//            // Automatically trim to loop?
//            if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool())
//                this->page_smpl->enleveFin(id);

            ui->tree->select(id, true);
        }
    }
    delete son;
}

QString MainWindowOld::getName(QString name, int maxCharacters, int suffixNumber)
{
    if (suffixNumber == 0)
        return name.left(maxCharacters);
    QString suffix = QString::number(suffixNumber);
    int suffixSize = suffix.length();
    if (suffixSize > 3 || maxCharacters < suffixSize + 1)
        return name.left(maxCharacters);
    return name.left(maxCharacters - suffixSize - 1) + "-" + suffix;
}

void MainWindowOld::exporterSmpl()
{
    QList<EltID> listIDs = ui->tree->getAllIDs();
    if (listIDs.isEmpty()) return;
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE));
    if (qDir.isEmpty()) return;
    qDir += "/";

    // Mémorisation des samples exportés
    QList<EltID> exportedSamples;

    // Exportation pour chaque sample
    int sampleID = -1;
    int sampleID2 = -1;
    QString qStr;
    EltID id2;
    foreach (EltID id, listIDs)
    {
        // Find EltID of the sample
        id2 = id;
        if (id.typeElement == elementSmpl)
            sampleID = id.indexElt;
        else if (id.typeElement == elementInstSmpl)
            sampleID = this->sf2->get(id, champ_sampleID).wValue;
        else
            sampleID = -1;
        id.typeElement = elementSmpl;
        id.indexElt = sampleID;

        if (sampleID != -1)
        {
            // Vérification que le sample n'a pas déjà été exporté
            if (!exportedSamples.contains(id))
            {
                qStr = qDir;

                // Stéréo ?
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
                    qStr.append(nom1.left(nom1.size() - nb).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_"));

                    if (this->sf2->get(id, champ_sfSampleType).wValue == rightSample &&
                            this->sf2->get(id, champ_sfSampleType).wValue != RomRightSample)
                    {
                        // Inversion smpl1 smpl2
                        EltID idTmp = id;
                        id = id2;
                        id2 = idTmp;
                    }

                    // Mémorisation de l'export
                    exportedSamples << id << id2;
                }
                else
                {
                    sampleID2 = -1;

                    // Nom du fichier
                    qStr.append(this->sf2->getQstr(id, champ_name).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_"));

                    // Mémorisation de l'export
                    exportedSamples << id;
                }

                // Nom pour l'exportation
                if (QFile::exists(qStr + ".wav"))
                {
                    // Modification du nom
                    int indice = 1;
                    while (QFile::exists(qStr + "-" + QString::number(indice) + ".wav"))
                        indice++;
                    qStr += "-" + QString::number(indice);
                }

                // Exportation
                if (sampleID2 == -1)
                    Sound::exporter(qStr + ".wav", this->sf2->getSon(id));
                else
                    Sound::exporter(qStr + ".wav", this->sf2->getSon(id), this->sf2->getSon(id2));
                ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, qStr + ".wav");
            }
        }
    }
}

void MainWindowOld::exporter()
{
    int nbElt = ui->tree->getSelectedItemsNumber();
    if (nbElt == 0)
        return;

    // List of sf2
    QList<EltID> selectedElements = ui->tree->getAllIDs();
    QList<EltID> listSf2;
    for (int i = 0; i < selectedElements.size(); i++)
    {
        int present = false;
        for (int j = 0; j < listSf2.size(); j++)
            if (listSf2[j].indexSf2 == selectedElements[i].indexSf2)
                present = true;
        if (!present)
            listSf2 << selectedElements[i];
    }

    DialogExport * dial = new DialogExport(sf2, listSf2, this);
    connect(dial, SIGNAL(accepted(QList<QList<EltID> >,QString,int,bool,bool,bool,int)),
            this, SLOT(exporter(QList<QList<EltID> >,QString,int,bool,bool,bool,int)));
    dial->show();
}

void MainWindowOld::exporter(QList<QList<EltID> > listID, QString dir, int format, bool presetPrefix, bool bankDir, bool gmSort, int quality)
{
    int flags = presetPrefix + bankDir * 0x02 + gmSort * 0x04;

    _progressDialog = new ModalProgressDialog(trUtf8("Opération en cours..."), trUtf8("Annuler"), 0, 0, this);
    _progressDialog->show();
    QApplication::processEvents();

    QFuture<int> future = QtConcurrent::run(this, &MainWindowOld::exporter2, listID, dir, format, flags, quality);
    _futureWatcher.setFuture(future);
}

int MainWindowOld::exporter2(QList<QList<EltID> > listID, QString dir, int format, int flags, int quality)
{
    // Flags
    bool presetPrefix = ((flags & 0x01) != 0);
    bool bankDir = ((flags & 0x02) != 0);
    bool gmSort = ((flags & 0x04) != 0);

    switch (format)
    {
    case 0: case 1: {
        // Export sf2 ou sf3, création d'un nouvel sf2 indépendant
        SoundfontManager * newSf2 = SoundfontManager::getOtherInstance(); // Not linked with tree
        EltID idDest(elementSf2, 0, 0, 0, 0);
        idDest.indexSf2 = newSf2->add(idDest);

        // Infos du nouvel sf2
        QString name, comment;
        if (listID.size() == 1)
        {
            EltID idSf2Source = listID.first().first();
            idSf2Source.typeElement = elementSf2;
            name = sf2->getQstr(idSf2Source, champ_name);
            comment = sf2->getQstr(idSf2Source, champ_ICMT);
            newSf2->set(idDest, champ_ISNG, sf2->getQstr(idSf2Source, champ_ISNG));
            newSf2->set(idDest, champ_IROM, sf2->getQstr(idSf2Source, champ_IROM));
            newSf2->set(idDest, champ_ICRD, sf2->getQstr(idSf2Source, champ_ICRD));
            newSf2->set(idDest, champ_IENG, sf2->getQstr(idSf2Source, champ_IENG));
            newSf2->set(idDest, champ_IPRD, sf2->getQstr(idSf2Source, champ_IPRD));
            newSf2->set(idDest, champ_ICOP, sf2->getQstr(idSf2Source, champ_ICOP));
            newSf2->set(idDest, champ_ISFT, sf2->getQstr(idSf2Source, champ_ISFT));
        }
        else
        {
            name = "soundfont";
            comment = trUtf8("Fusion des soundfonts :");
            foreach (QList<EltID> subList, listID)
            {
                EltID idSf2Source = subList.first();
                idSf2Source.typeElement = elementSf2;
                comment += "\n - " + sf2->getQstr(idSf2Source, champ_name);
            }
        }
        newSf2->set(idDest, champ_name, name);
        newSf2->set(idDest, champ_ICMT, comment);

        // Ajout des presets
        Duplicator duplicator(this->sf2, newSf2, this);
        for (int nbBank = 0; nbBank < listID.size(); nbBank++)
        {
            QList<EltID> subList = listID[nbBank];
            for (int nbPreset = 0; nbPreset < subList.size(); nbPreset++)
            {
                EltID id = subList[nbPreset];

                if (listID.size() == 1)
                {
                    duplicator.copy(id, idDest);
                }
                else
                {
                    int originalBank = sf2->get(id, champ_wBank).wValue;
                    int originalPreset = sf2->get(id, champ_wPreset).wValue;
                    Valeur value;
                    value.wValue = nbBank;
                    sf2->set(id, champ_wBank, value);
                    value.wValue = nbPreset;
                    sf2->set(id, champ_wPreset, value);

                    duplicator.copy(id, idDest);

                    value.wValue = originalBank;
                    sf2->set(id, champ_wBank, value);
                    value.wValue = originalPreset;
                    sf2->set(id, champ_wPreset, value);
                }
            }
        }
        sf2->clearNewEditing();

        // Détermination du nom de fichier
        name = name.replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");

        QString extension = (format == 0 ? ".sf2" : ".sf3");
        QFile fichier(dir + "/" + name + extension);
        if (fichier.exists())
        {
            int i = 1;
            while (QFile(dir + "/" + name + "-" + QString::number(i) + extension).exists())
                i++;
            name += "-" + QString::number(i);
        }
        name = dir + "/" + name + extension;

        // Sauvegarde
        newSf2->save(idDest.indexSf2, name, quality);
        delete newSf2;
    }break;
    case 2:
        // Export sfz
        foreach (QList<EltID> sublist, listID)
            ConversionSfz(sf2).convert(dir, sublist, presetPrefix, bankDir, gmSort);
        break;
    default:
        break;
    }

    return -10; // For QFuture
}

void MainWindowOld::futureFinished()
{
    int result = _futureWatcher.result();
    if (result > -10)
    {
        // An sf2 has been just loaded
        switch (result)
        {
        case -1: // Warning and continue with 0
            QMessageBox::warning(this, QObject::trUtf8("Attention"),
                                 trUtf8("Fichier corrompu : utilisation des échantillons en qualité 16 bits."));
        case 0:
            // Loading ok
            updateFavoriteFiles();
            updateActions();
            break;
        case 1:
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Format inconnu."));
            break;
        case 2:
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier est déjà chargé."));
            break;
        case 3:
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Impossible d'ouvrir le fichier."));
            break;
        case 4:
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Lecture impossible."));
            break;
        case 5: case 6:
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier est corrompu."));
            break;
        }

        updateDo();
    }

    _progressDialog->deleteLater();
    _progressDialog = NULL;
}

void MainWindowOld::nouvelInstrument()
{
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->tree->getFirstID();
    bool ok;
    QString name = QInputDialog::getText(this, trUtf8("Créer un nouvel instrument"), trUtf8("Nom du nouvel instrument :"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty())
    {
        // Reprise ID si modif
        id = ui->tree->getFirstID();
        id.typeElement = elementInst;
        id.indexElt = this->sf2->add(id);
        this->sf2->set(id, champ_name, name.left(20));
        updateDo();
        ui->tree->clearSelection();
        ui->tree->select(id, true);
    }
}
void MainWindowOld::nouveauPreset()
{
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb == 0) return;
    EltID id = ui->tree->getFirstID();
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
    QString name = QInputDialog::getText(this, trUtf8("Créer un nouveau preset"), trUtf8("Nom du nouveau preset :"), QLineEdit::Normal, text, &ok);
    if (ok && !name.isEmpty())
    {
        Valeur val;
        // Reprise de l'identificateur si modification
        id = ui->tree->getFirstID();
        id.typeElement = elementPrst;
        id.indexElt = this->sf2->add(id);
        this->sf2->set(id, champ_name, name.left(20));
        val.wValue = nPreset;
        this->sf2->set(id, champ_wPreset, val);
        val.wValue = nBank;
        this->sf2->set(id, champ_wBank, val);
        updateDo();
        ui->tree->clearSelection();
        ui->tree->select(id, true);
    }
}
void MainWindowOld::associer()
{
    if (!ui->tree->getSelectedItemsNumber())
        return;
    EltID id = ui->tree->getFirstID();
    this->dialList.showDialog(id, DialogList::MODE_ASSOCIATION);
}
void MainWindowOld::associer(EltID idDest)
{
    int nb = ui->tree->getSelectedItemsNumber();
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
    Valeur val;

    // Liste des éléments sources
    QList<EltID> listeSrc = ui->tree->getAllIDs();
    foreach (EltID idSrc, listeSrc)
    {
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
            foreach (int i, this->sf2->getSiblings(idLinked))
            {
                idLinked.indexElt2 = i;
                if (this->sf2->isSet(idLinked, champ_keyRange))
                {
                    keyMin = qMin(keyMin, (int)this->sf2->get(idLinked, champ_keyRange).rValue.byLo);
                    keyMax = qMax(keyMax, (int)this->sf2->get(idLinked, champ_keyRange).rValue.byHi);
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
    updateActions();
}
void MainWindowOld::remplacer()
{
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb != 1) return;
    EltID id = ui->tree->getFirstID();
    this->dialList.showDialog(id, DialogList::MODE_REMPLACEMENT);
}
void MainWindowOld::remplacer(EltID idSrc)
{
    int nb = ui->tree->getSelectedItemsNumber();
    if (nb != 1 || (idSrc.typeElement != elementSmpl && idSrc.typeElement != elementInst))
    {
        updateDo();
        return;
    }
    EltID idDest = ui->tree->getFirstID();
    if (idDest.typeElement != elementInstSmpl && idDest.typeElement != elementPrstInst)
        return;
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
    updateActions();
}

// Envoi de signaux
void MainWindowOld::copier()
{
    // émission d'un signal "copier"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier));
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier));
    }
}
void MainWindowOld::coller()
{
    // émission d'un signal "coller"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier));
    }
}
void MainWindowOld::supprimer()
{
    // émission d'un signal "supprimer"
    QWidget* focused = QApplication::focusWidget();
    if( focused != 0 )
    {
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, 0));
        QApplication::postEvent(focused, new QKeyEvent(QEvent::KeyRelease, Qt::Key_Delete, 0));
    }
}

// Outils
void MainWindowOld::desaccorder()      {this->page_inst->desaccorder();}
void MainWindowOld::duplication()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->duplication();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->duplication();
}

void MainWindowOld::paramGlobal()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->paramGlobal();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->paramGlobal();
}

void MainWindowOld::spatialisation()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->spatialisation();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->spatialisation();
}

void MainWindowOld::on_action_Transposer_triggered()
{
    this->page_inst->transposer();
}

void MainWindowOld::visualize()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->visualize();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->visualize();
}
void MainWindowOld::purger()
{
    // Suppression des éléments non utilisés
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    if (!ui->tree->isSelectedItemsSf2Unique()) return;
    EltID id = ui->tree->getFirstID();

    // Nombre de samples et instruments non utilisés
    int unusedSmpl = 0;
    int unusedInst = 0;
    id.typeElement = elementSmpl;
    QList<int> nbSmpl = sf2->getSiblings(id);
    id.typeElement = elementInst;
    QList<int> nbInst = sf2->getSiblings(id);
    id.typeElement = elementPrst;
    QList<int> nbPrst = sf2->getSiblings(id);
    bool smplUsed, instUsed;

    // Suppression des instruments
    foreach (int i, nbInst)
    {
        instUsed = false;

        // on regarde chaque preset
        foreach (int j, nbPrst)
        {
            // composé d'instruments
            id.indexElt = j;
            id.typeElement = elementPrstInst;
            foreach (int k, sf2->getSiblings(id))
            {
                id.indexElt2 = k;
                if (sf2->get(id, champ_instrument).wValue == i)
                    instUsed = true;
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

    // Suppression des samples
    // pour chaque sample
    foreach (int i, nbSmpl)
    {
        smplUsed = false;

        // on regarde chaque instrument
        foreach (int j, nbInst)
        {
            // composé de samples
            id.indexElt = j;
            id.typeElement = elementInstSmpl;
            foreach (int k, sf2->getSiblings(id))
            {
                id.indexElt2 = k;
                if (sf2->get(id, champ_sampleID).wValue == i)
                    smplUsed = true;
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

void MainWindowOld::attenuationMini()
{
    EltID id = ui->tree->getFirstID();
    EltID id2 = id;

    // Compute minimal and maximal attenuations for instruments
    id.typeElement = elementInst;
    id2.typeElement = elementInstSmpl;
    int minAttenuation = 1440, maxAttenuation = 0;
    bool instrumentFound = false;
    foreach (int i, this->sf2->getSiblings(id))
    {
        id.indexElt = i;
        instrumentFound = true;
        int defaultAttenuation = 0;
        if (this->sf2->isSet(id, champ_initialAttenuation))
            defaultAttenuation = this->sf2->get(id, champ_initialAttenuation).shValue;

        // For each sample linked
        id2.indexElt = i;
        foreach (int j, this->sf2->getSiblings(id2))
        {
            id2.indexElt2 = j;
            int value = defaultAttenuation;
            if (this->sf2->isSet(id2, champ_initialAttenuation))
                value = this->sf2->get(id2, champ_initialAttenuation).shValue;

            if (value < minAttenuation)
                minAttenuation = value;
            if (value > maxAttenuation)
                maxAttenuation = value;
        }
    }

    if (!instrumentFound) {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier doit contenir au moins un instrument."));
        return;
    }

    // Compute minimal and maximal attenuations for presets
    id.typeElement = elementPrst;
    id2.typeElement = elementPrstInst;
    int minAttenuationPrst = 1440, maxAttenuationPrst = -1440;
    foreach (int i, this->sf2->getSiblings(id))
    {
        id.indexElt = i;
        int defaultAttenuation = 0;
        if (this->sf2->isSet(id, champ_initialAttenuation))
            defaultAttenuation = this->sf2->get(id, champ_initialAttenuation).shValue;

        // For each instrument linked
        id2.indexElt = i;
        foreach (int j, this->sf2->getSiblings(id2))
        {
            id2.indexElt2 = j;
            int value = defaultAttenuation;
            if (this->sf2->isSet(id2, champ_initialAttenuation))
                value = this->sf2->get(id2, champ_initialAttenuation).shValue;

            if (value < minAttenuationPrst)
                minAttenuationPrst = value;
            if (value > maxAttenuationPrst)
                maxAttenuationPrst = value;
        }
    }

    DialogAttenuation * dial = new DialogAttenuation(0.1 * minAttenuation, 0.1 * maxAttenuation,
                                                     0.1 * minAttenuationPrst, 0.1 * maxAttenuationPrst, this);
    dial->setAttribute(Qt::WA_DeleteOnClose);
    connect(dial, SIGNAL(accepted(double, double)),
            this, SLOT(attenuationMini(double, double)));
    dial->show();
}

void MainWindowOld::attenuationMini(double value, double valuePrst)
{
    // Offsets
    int offset = round(10.0 * value);
    int offset2 = round(10.0 * valuePrst);
    if (offset == 0 && offset2 == 0)
        return;

    // Apply the offset
    Valeur val;

    EltID id = ui->tree->getFirstID();
    EltID id2 = id;

    // For each instrument
    if (offset != 0)
    {
        id.typeElement = elementInst;
        id2.typeElement = elementInstSmpl;
        foreach (int i, this->sf2->getSiblings(id))
        {
            id.indexElt = i;
            bool defaultDefined = this->sf2->isSet(id, champ_initialAttenuation);
            int defaultAttenuation = defaultDefined ? this->sf2->get(id, champ_initialAttenuation).shValue : 0;
            if (defaultDefined || defaultAttenuation + offset != 0)
            {
                val.shValue = defaultAttenuation + offset;
                this->sf2->set(id, champ_initialAttenuation, val);
            }

            // For each sample linked
            id2.indexElt = i;
            foreach (int j, this->sf2->getSiblings(id2))
            {
                id2.indexElt2 = j;
                if (this->sf2->isSet(id2, champ_initialAttenuation))
                {
                    val.shValue = this->sf2->get(id2, champ_initialAttenuation).shValue + offset;
                    this->sf2->set(id2, champ_initialAttenuation, val);
                }
            }
        }
    }

    // For each preset
    if (offset2 != 0)
    {
        id.typeElement = elementPrst;
        id2.typeElement = elementPrstInst;
        foreach (int i, this->sf2->getSiblings(id))
        {
            id.indexElt = i;
            bool defaultDefined = this->sf2->isSet(id, champ_initialAttenuation);
            int defaultAttenuation = defaultDefined ? this->sf2->get(id, champ_initialAttenuation).shValue : 0;
            if (defaultDefined || defaultAttenuation + offset2 != 0)
            {
                val.shValue = defaultAttenuation + offset2;
                this->sf2->set(id, champ_initialAttenuation, val);
            }

            // For each instrument linked
            id2.indexElt = i;
            foreach (int j, this->sf2->getSiblings(id2))
            {
                id2.indexElt2 = j;
                if (this->sf2->isSet(id2, champ_initialAttenuation))
                {
                    val.shValue = this->sf2->get(id2, champ_initialAttenuation).shValue + offset2;
                    this->sf2->set(id2, champ_initialAttenuation, val);
                }
            }
        }
    }

    // Update interface
    this->updateDo();
    //    if (ui->stackedWidget->currentWidget() == this->page_inst)
    //        this->page_inst->afficher();
    //    else if (ui->stackedWidget->currentWidget() == this->page_prst)
    //        this->page_prst->afficher();
}

void MainWindowOld::associationAutoSmpl()
{
    // Association automatique des samples
    // Condition : même nom sauf pour la dernière lettre (R ou L)
    EltID id = ui->tree->getFirstID();
    id.typeElement = elementSmpl;

    // Constitution listes de noms et indices
    QList<EltID> listID;
    QStringList  listNom;
    QList<bool>  listSens;
    QString currentStr;
    foreach (int i, this->sf2->getSiblings(id))
    {
        id.indexElt = i;
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
    //    this->updateDo();
    //    if (ui->stackedWidget->currentWidget() == this->page_smpl)
    //        this->page_smpl->afficher();
}
void MainWindowOld::exportPresetList()
{
    if (ui->tree->getSelectedItemsNumber() == 0) return;
    EltID id = ui->tree->getFirstID();
    DialogExportList * dial = new DialogExportList(sf2, id, this);
    dial->setAttribute(Qt::WA_DeleteOnClose);
    dial->show();
}
void MainWindowOld::on_action_Dissocier_les_samples_st_r_o_triggered()
{
    EltID id = ui->tree->getFirstID();
    id.typeElement = elementSmpl;
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
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

    // Mise à jour
    //    this->updateDo();
    //    if (ui->stackedWidget->currentWidget() == this->page_smpl)
    //        this->page_smpl->afficher();
}
void MainWindowOld::on_actionExporter_pics_de_fr_quence_triggered()
{
    EltID id = ui->tree->getFirstID();
    id.typeElement = elementSf2;
    QString defaultFile = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_FREQUENCIES) + "/" +
            sf2->getQstr(id, champ_name).replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".csv";
    QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Exporter les pics de fréquence"),
                                                    defaultFile, trUtf8("Fichier .csv (*.csv)"));
    if (!fileName.isEmpty())
    {
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_FREQUENCIES, fileName);
        exporterFrequences(fileName);
    }
}

void MainWindowOld::exporterFrequences(QString fileName)
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

    EltID id = ui->tree->getFirstID();
    id.typeElement = elementSmpl;
    QString nomSmpl;
    QList<double> listeFrequences, listeFacteurs;
    QList<int> listeNotes, listeCorrections;
    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
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
            stream << ContextManager::keyName()->getKeyName(listeNotes.at(j)) << sep;
            stream << listeCorrections.at(j);
        }
    }
    file.close();
}

void MainWindowOld::on_actionEnlever_tous_les_modulateurs_triggered()
{
    EltID id = ui->tree->getFirstID();

    int count = 0;

    // Suppression des modulateurs liés aux instruments
    id.typeElement = elementInst;
    count += deleteMods(id);

    // Suppression des modulateurs liés aux presets
    id.typeElement = elementPrst;
    count += deleteMods(id);

    if (count == 0)
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le fichier ne contient aucun modulateur."));
    else if (count == 1)
        QMessageBox::information(this, trUtf8("Information"), trUtf8("1 modulateur a été supprimé."));
    else
        QMessageBox::information(this, trUtf8("Information"), QString::number(count) + " " +
                                 trUtf8("modulateurs ont été supprimés."));
    updateDo();
    updateActions();
}

int MainWindowOld::deleteMods(EltID id)
{
    int count = 0;

    foreach (int i, sf2->getSiblings(id))
    {
        id.indexElt = i;
        // Modulateurs globaux
        EltID idMod = id;
        if (id.typeElement == elementInst)
            idMod.typeElement = elementInstMod;
        else
            idMod.typeElement = elementPrstMod;
        foreach (int j, sf2->getSiblings(idMod))
        {
            idMod.indexMod = j;
            sf2->remove(idMod);
            count++;
        }

        // Modulateurs de chaque division
        EltID idSub = id;
        if (id.typeElement == elementInst)
            idSub.typeElement = elementInstSmpl;
        else
            idSub.typeElement = elementPrstInst;
        foreach (int j, sf2->getSiblings(idSub))
        {
            idSub.indexElt2 = j;
            idMod = idSub;
            if (id.typeElement == elementInst)
                idMod.typeElement = elementInstSmplMod;
            else
                idMod.typeElement = elementPrstInstMod;
            foreach (int k, sf2->getSiblings(idMod))
            {
                idMod.indexMod = k;
                sf2->remove(idMod);
                count++;
            }
        }
    }

    return count;
}


// Gestion du clavier virtuel / du son
void MainWindowOld::onAudioConnectionDone()
{
    //Config::getInstance()->storeAudioConfig();
}

void MainWindowOld::noteOff(int key)
{
    noteChanged(key, 0);
}

void MainWindowOld::noteHover(int key, int vel)
{
    if (_currentKey == -1)
    {
        if (key != -1)
        {
            ui->labelNote->setText(ContextManager::keyName()->getKeyName(key));
            ui->labelVelocite->setText(QString::number(vel));
        }
        else
        {
            ui->labelNote->setText("-");
            ui->labelVelocite->setText("-");
        }
    }
}
void MainWindowOld::setSustain(bool isOn)
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

void MainWindowOld::setVolume(int vol)
{
    vol = (double)vol / 127. * 101. - 50.5;
    ContextManager::configuration()->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", vol);
}

void MainWindowOld::noteChanged(int key, int vel)
{
    // Note hover
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

    if (key != -1)
    {
        // Mise en évidence de la ou des éléments liés étant en train de jouer
        this->page_inst->keyPlayed(key, vel);
        this->page_prst->keyPlayed(key, vel);
    }

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

    if (vel > 0 && key != -1)
    {
        ui->labelNote->setText(ContextManager::keyName()->getKeyName(key));
        ui->labelVelocite->setText(QString::number(vel));
    }
    else
    {
        ui->labelNote->setText("-");
        ui->labelVelocite->setText("-");
    }

    // Lecture ?
    if (ui->tree->getSelectedItemsNumber())
    {
        EltID id = ui->tree->getFirstID();
        if (ui->tree->isSelectedItemsSf2Unique())
        {
            if (id.typeElement == elementSmpl && ui->tree->getSelectedItemsNumber() == 1)
                this->synth->play(0, id.indexSf2, id.indexElt, key, vel);
            else if (ui->tree->isSelectedItemsFamilyUnique())
            {
                if ((id.typeElement == elementInst || id.typeElement == elementInstSmpl) &&
                        ui->tree->isSelectedItemsFamilyUnique())
                {
                    // Mise en évidence des étendues concernées
                    if (vel)
                    {
                        EltID idInst = id;
                        idInst.typeElement = elementInst;
                        rangesType defaultKeyRange, defaultVelRange;
                        if (sf2->isSet(idInst, champ_keyRange))
                            defaultKeyRange = sf2->get(idInst, champ_keyRange).rValue;
                        else
                        {
                            defaultKeyRange.byLo = 0;
                            defaultKeyRange.byHi = 127;
                        }
                        if (sf2->isSet(idInst, champ_velRange))
                            defaultVelRange = sf2->get(idInst, champ_velRange).rValue;
                        else
                        {
                            defaultVelRange.byLo = 0;
                            defaultVelRange.byHi = 127;
                        }

                        EltID idInstSmpl = id;
                        idInstSmpl.typeElement = elementInstSmpl;
                        foreach (int i, sf2->getSiblings(idInstSmpl))
                        {
                            idInstSmpl.indexElt2 = i;
                            int keyMin, keyMax, velMin, velMax;
                            if (sf2->isSet(idInstSmpl, champ_keyRange))
                            {
                                rangesType rangeTmp = sf2->get(idInstSmpl, champ_keyRange).rValue;
                                keyMin = rangeTmp.byLo;
                                keyMax = rangeTmp.byHi;
                            }
                            else
                            {
                                keyMin = defaultKeyRange.byLo;
                                keyMax = defaultKeyRange.byHi;
                            }
                            if (sf2->isSet(idInstSmpl, champ_velRange))
                            {
                                rangesType rangeTmp = sf2->get(idInstSmpl, champ_velRange).rValue;
                                velMin = rangeTmp.byLo;
                                velMax = rangeTmp.byHi;
                            }
                            else
                            {
                                velMin = defaultVelRange.byLo;
                                velMax = defaultVelRange.byHi;
                            }
                            if (keyMin <= key && keyMax >= key && velMin <= vel && velMax >= vel)
                                ui->widgetKeyboard->setCurrentRange(key, keyMin, keyMax);
                        }
                    }

                    this->synth->play(1, id.indexSf2, id.indexElt, key, vel);
                }
                else if ((id.typeElement == elementPrst || id.typeElement == elementPrstInst) &&
                         ui->tree->isSelectedItemsFamilyUnique())
                    this->synth->play(2, id.indexSf2, id.indexElt, key, vel);
            }
        }
    }
}

void MainWindowOld::setRangeAndRootKey(int rootKey, int noteMin, int noteMax)
{
    ui->widgetKeyboard->setRangeAndRootKey(rootKey, noteMin, noteMax);
}

void MainWindowOld::clearKeyboardCustomisation()
{
    ui->widgetKeyboard->clearCustomization();
}
