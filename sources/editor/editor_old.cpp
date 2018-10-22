#include "editor_old.h"
#include "ui_editor_old.h"
#include "sound.h"
#include "dialog_rename.h"
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
    about(this),
    dialList(this),
    actionKeyboard(NULL),
    _progressDialog(NULL)
{
    ui->setupUi(this);
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    ui->editSearch->setPlaceholderText(trUtf8("Rechercher..."));
#endif
#ifdef Q_OS_MAC
    ui->verticalLayout_3->setSpacing(4);
#endif

    ui->actionPlein_cran->setChecked(this->windowState() & Qt::WindowFullScreen);

    // Initialisation de l'objet pile sf2
    this->sf2 = SoundfontManager::getInstance();

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
    SoundfontManager::kill();
    delete this->page_inst;
    delete this->page_prst;
    delete this->page_sf2;
    delete this->page_smpl;
    delete _pageOverviewSmpl;
    delete _pageOverviewInst;
    delete _pageOverviewPrst;
    delete ui;
}

void MainWindowOld::spaceKeyPressedInTree()
{
    if (ui->stackedWidget->currentWidget() == page_smpl)
        page_smpl->pushPlayPause();
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

void MainWindowOld::onPleinEcranTriggered()
{
    this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
}

// Modifications
void MainWindowOld::dragAndDrop(EltID idDest, QList<EltID> idSources)
{
    // prepareNewActions() et updateDo() faits à l'extérieur
    Duplicator duplicator(this->sf2, this->sf2, this);
    for (int i = 0; i < idSources.size(); i++)
        duplicator.copy(idSources.at(i), idDest);
    //updateActions();
}

void MainWindowOld::exporterSmpl()
{
    QList<EltID> listIDs;
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
    int nbElt;
    if (nbElt == 0)
        return;

    // List of sf2
    QList<EltID> selectedElements;// = ui->tree->getAllIDs();
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
                    AttributeValue value;
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
            //updateFavoriteFiles();
            //updateActions();
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
    }

    _progressDialog->deleteLater();
    _progressDialog = NULL;
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

void MainWindowOld::duplication()
{
    //if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type;// = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->duplication();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->duplication();
}

void MainWindowOld::spatialisation()
{
    //if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type;// = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->spatialisation();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->spatialisation();
}

void MainWindowOld::visualize()
{
    //if (ui->tree->getSelectedItemsNumber() == 0) return;
    ElementType type;// = ui->tree->getFirstID().typeElement;
    if (type == elementInst || type == elementInstSmpl)
        this->page_inst->visualize();
    else if (type == elementPrst || type == elementPrstInst)
        this->page_prst->visualize();
}

void MainWindowOld::exportPresetList()
{
    //if (ui->tree->getSelectedItemsNumber() == 0) return;
    EltID id;// = ui->tree->getFirstID();
    DialogExportList * dial = new DialogExportList(sf2, id, this);
    dial->setAttribute(Qt::WA_DeleteOnClose);
    dial->show();
}
void MainWindowOld::on_actionExporter_pics_de_fr_quence_triggered()
{
    EltID id;// = ui->tree->getFirstID();
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

    EltID id;// = ui->tree->getFirstID();
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
