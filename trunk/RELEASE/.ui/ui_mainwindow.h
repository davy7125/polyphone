/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "pianokeybdcustom.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOuvrir;
    QAction *actionQuitter;
    QAction *actionSommaire;
    QAction *actionA_propos;
    QAction *actionEnregistrer;
    QAction *actionEnregistrer_sous;
    QAction *actionNouveau;
    QAction *actionAnnuler;
    QAction *actionR_tablir;
    QAction *actionPr_f_rences;
    QAction *actionBarre_d_outils;
    QAction *actionFermer_le_fichier;
    QAction *actionCopier;
    QAction *actionColler;
    QAction *action_Supprimer;
    QAction *action_Enlever_les_l_ments_non_utilis_s;
    QAction *actionRenommer;
    QAction *actionFichier_1;
    QAction *actionFichier_2;
    QAction *actionFichier_3;
    QAction *actionFichier_4;
    QAction *actionFichier_5;
    QAction *actionSection_modulateurs;
    QAction *actionImporter;
    QAction *actionExporter;
    QAction *actionNouvel_instrument;
    QAction *actionNouveau_preset;
    QAction *actionEnlever_blanc_au_d_part;
    QAction *actionAjuster_la_fin_de_boucle;
    QAction *actionNormaliser_volume;
    QAction *actionBouclage_automatique;
    QAction *actionFiltre_mur_de_brique;
    QAction *actionTransposer;
    QAction *actionR_gler_att_nuation_minimale;
    QAction *actionD_saccordage_ondulant;
    QAction *actionSpacialisation_du_son;
    QAction *action_R_partition_automatique;
    QAction *actionD_uplication_des_divisions;
    QAction *action_Param_trage_global;
    QAction *action_R_glage_balance;
    QAction *action_Cr_ation_mutation_mixture;
    QAction *action_Diminuer_sifflements;
    QAction *action_Diminuer_sifflements_2;
    QAction *action_Clavier;
    QAction *action5_octaves;
    QAction *action6_octaves;
    QAction *action128_notes;
    QAction *actionAucun;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QWidget *ensembleKeyboard;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QLabel *labelNote;
    QLabel *labelVelocite;
    PianoKeybdCustom *keyboard;
    QDial *velocityButton;
    QStackedWidget *stackedWidget;
    QWidget *page_Soft;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QMenuBar *Menubar;
    QMenu *menuFichier;
    QMenu *menu;
    QMenu *menuEdition;
    QMenu *menuOutils;
    QMenu *menuSample;
    QMenu *menuDivers;
    QMenu *menuInstrument;
    QMenu *menuAffichage;
    QMenu *menuClavier;
    QToolBar *toolBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_3;
    Tree *arborescence;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *push_enrouler;
    QPushButton *push_derouler;
    QLineEdit *editSearch;
    QPushButton *push_nosearch;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(943, 431);
        MainWindow->setContextMenuPolicy(Qt::CustomContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        MainWindow->setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionOuvrir = new QAction(MainWindow);
        actionOuvrir->setObjectName(QString::fromUtf8("actionOuvrir"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icones/open"), QSize(), QIcon::Normal, QIcon::Off);
        actionOuvrir->setIcon(icon1);
        actionQuitter = new QAction(MainWindow);
        actionQuitter->setObjectName(QString::fromUtf8("actionQuitter"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icones/exit"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuitter->setIcon(icon2);
        actionSommaire = new QAction(MainWindow);
        actionSommaire->setObjectName(QString::fromUtf8("actionSommaire"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icones/help"), QSize(), QIcon::Normal, QIcon::Off);
        actionSommaire->setIcon(icon3);
        actionA_propos = new QAction(MainWindow);
        actionA_propos->setObjectName(QString::fromUtf8("actionA_propos"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icones/about"), QSize(), QIcon::Normal, QIcon::Off);
        actionA_propos->setIcon(icon4);
        actionEnregistrer = new QAction(MainWindow);
        actionEnregistrer->setObjectName(QString::fromUtf8("actionEnregistrer"));
        actionEnregistrer->setEnabled(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icones/save"), QSize(), QIcon::Normal, QIcon::Off);
        actionEnregistrer->setIcon(icon5);
        actionEnregistrer_sous = new QAction(MainWindow);
        actionEnregistrer_sous->setObjectName(QString::fromUtf8("actionEnregistrer_sous"));
        actionEnregistrer_sous->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icones/save_as"), QSize(), QIcon::Normal, QIcon::Off);
        actionEnregistrer_sous->setIcon(icon6);
        actionNouveau = new QAction(MainWindow);
        actionNouveau->setObjectName(QString::fromUtf8("actionNouveau"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icones/new"), QSize(), QIcon::Normal, QIcon::Off);
        actionNouveau->setIcon(icon7);
        actionAnnuler = new QAction(MainWindow);
        actionAnnuler->setObjectName(QString::fromUtf8("actionAnnuler"));
        actionAnnuler->setEnabled(false);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icones/undo"), QSize(), QIcon::Normal, QIcon::Off);
        actionAnnuler->setIcon(icon8);
        actionR_tablir = new QAction(MainWindow);
        actionR_tablir->setObjectName(QString::fromUtf8("actionR_tablir"));
        actionR_tablir->setEnabled(false);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icones/redo"), QSize(), QIcon::Normal, QIcon::Off);
        actionR_tablir->setIcon(icon9);
        actionPr_f_rences = new QAction(MainWindow);
        actionPr_f_rences->setObjectName(QString::fromUtf8("actionPr_f_rences"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icones/settings"), QSize(), QIcon::Normal, QIcon::Off);
        actionPr_f_rences->setIcon(icon10);
        actionBarre_d_outils = new QAction(MainWindow);
        actionBarre_d_outils->setObjectName(QString::fromUtf8("actionBarre_d_outils"));
        actionBarre_d_outils->setCheckable(true);
        actionBarre_d_outils->setChecked(true);
        actionFermer_le_fichier = new QAction(MainWindow);
        actionFermer_le_fichier->setObjectName(QString::fromUtf8("actionFermer_le_fichier"));
        actionFermer_le_fichier->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icones/close"), QSize(), QIcon::Normal, QIcon::Off);
        actionFermer_le_fichier->setIcon(icon11);
        actionCopier = new QAction(MainWindow);
        actionCopier->setObjectName(QString::fromUtf8("actionCopier"));
        actionCopier->setEnabled(false);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icones/copy"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopier->setIcon(icon12);
        actionCopier->setShortcutContext(Qt::WidgetShortcut);
        actionColler = new QAction(MainWindow);
        actionColler->setObjectName(QString::fromUtf8("actionColler"));
        actionColler->setEnabled(false);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icones/paste"), QSize(), QIcon::Normal, QIcon::Off);
        actionColler->setIcon(icon13);
        actionColler->setShortcutContext(Qt::WidgetShortcut);
        action_Supprimer = new QAction(MainWindow);
        action_Supprimer->setObjectName(QString::fromUtf8("action_Supprimer"));
        action_Supprimer->setEnabled(false);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/icones/delete"), QSize(), QIcon::Normal, QIcon::Off);
        action_Supprimer->setIcon(icon14);
        action_Supprimer->setShortcutContext(Qt::WidgetShortcut);
        action_Enlever_les_l_ments_non_utilis_s = new QAction(MainWindow);
        action_Enlever_les_l_ments_non_utilis_s->setObjectName(QString::fromUtf8("action_Enlever_les_l_ments_non_utilis_s"));
        action_Enlever_les_l_ments_non_utilis_s->setEnabled(true);
        actionRenommer = new QAction(MainWindow);
        actionRenommer->setObjectName(QString::fromUtf8("actionRenommer"));
        actionRenommer->setEnabled(false);
        actionFichier_1 = new QAction(MainWindow);
        actionFichier_1->setObjectName(QString::fromUtf8("actionFichier_1"));
        actionFichier_1->setEnabled(false);
        actionFichier_1->setVisible(false);
        actionFichier_2 = new QAction(MainWindow);
        actionFichier_2->setObjectName(QString::fromUtf8("actionFichier_2"));
        actionFichier_2->setEnabled(false);
        actionFichier_2->setVisible(false);
        actionFichier_3 = new QAction(MainWindow);
        actionFichier_3->setObjectName(QString::fromUtf8("actionFichier_3"));
        actionFichier_3->setEnabled(false);
        actionFichier_3->setVisible(false);
        actionFichier_4 = new QAction(MainWindow);
        actionFichier_4->setObjectName(QString::fromUtf8("actionFichier_4"));
        actionFichier_4->setEnabled(false);
        actionFichier_4->setVisible(false);
        actionFichier_5 = new QAction(MainWindow);
        actionFichier_5->setObjectName(QString::fromUtf8("actionFichier_5"));
        actionFichier_5->setEnabled(false);
        actionFichier_5->setVisible(false);
        actionSection_modulateurs = new QAction(MainWindow);
        actionSection_modulateurs->setObjectName(QString::fromUtf8("actionSection_modulateurs"));
        actionSection_modulateurs->setCheckable(true);
        actionSection_modulateurs->setChecked(true);
        actionImporter = new QAction(MainWindow);
        actionImporter->setObjectName(QString::fromUtf8("actionImporter"));
        actionImporter->setEnabled(false);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/icones/wave"), QSize(), QIcon::Normal, QIcon::Off);
        actionImporter->setIcon(icon15);
        actionExporter = new QAction(MainWindow);
        actionExporter->setObjectName(QString::fromUtf8("actionExporter"));
        actionExporter->setEnabled(false);
        actionNouvel_instrument = new QAction(MainWindow);
        actionNouvel_instrument->setObjectName(QString::fromUtf8("actionNouvel_instrument"));
        actionNouvel_instrument->setEnabled(false);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/icones/sound"), QSize(), QIcon::Normal, QIcon::Off);
        actionNouvel_instrument->setIcon(icon16);
        actionNouveau_preset = new QAction(MainWindow);
        actionNouveau_preset->setObjectName(QString::fromUtf8("actionNouveau_preset"));
        actionNouveau_preset->setEnabled(false);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/icones/music"), QSize(), QIcon::Normal, QIcon::Off);
        actionNouveau_preset->setIcon(icon17);
        actionEnlever_blanc_au_d_part = new QAction(MainWindow);
        actionEnlever_blanc_au_d_part->setObjectName(QString::fromUtf8("actionEnlever_blanc_au_d_part"));
        actionAjuster_la_fin_de_boucle = new QAction(MainWindow);
        actionAjuster_la_fin_de_boucle->setObjectName(QString::fromUtf8("actionAjuster_la_fin_de_boucle"));
        actionAjuster_la_fin_de_boucle->setEnabled(true);
        actionNormaliser_volume = new QAction(MainWindow);
        actionNormaliser_volume->setObjectName(QString::fromUtf8("actionNormaliser_volume"));
        actionBouclage_automatique = new QAction(MainWindow);
        actionBouclage_automatique->setObjectName(QString::fromUtf8("actionBouclage_automatique"));
        actionFiltre_mur_de_brique = new QAction(MainWindow);
        actionFiltre_mur_de_brique->setObjectName(QString::fromUtf8("actionFiltre_mur_de_brique"));
        actionTransposer = new QAction(MainWindow);
        actionTransposer->setObjectName(QString::fromUtf8("actionTransposer"));
        actionR_gler_att_nuation_minimale = new QAction(MainWindow);
        actionR_gler_att_nuation_minimale->setObjectName(QString::fromUtf8("actionR_gler_att_nuation_minimale"));
        actionD_saccordage_ondulant = new QAction(MainWindow);
        actionD_saccordage_ondulant->setObjectName(QString::fromUtf8("actionD_saccordage_ondulant"));
        actionD_saccordage_ondulant->setEnabled(true);
        actionSpacialisation_du_son = new QAction(MainWindow);
        actionSpacialisation_du_son->setObjectName(QString::fromUtf8("actionSpacialisation_du_son"));
        action_R_partition_automatique = new QAction(MainWindow);
        action_R_partition_automatique->setObjectName(QString::fromUtf8("action_R_partition_automatique"));
        actionD_uplication_des_divisions = new QAction(MainWindow);
        actionD_uplication_des_divisions->setObjectName(QString::fromUtf8("actionD_uplication_des_divisions"));
        action_Param_trage_global = new QAction(MainWindow);
        action_Param_trage_global->setObjectName(QString::fromUtf8("action_Param_trage_global"));
        action_R_glage_balance = new QAction(MainWindow);
        action_R_glage_balance->setObjectName(QString::fromUtf8("action_R_glage_balance"));
        action_Cr_ation_mutation_mixture = new QAction(MainWindow);
        action_Cr_ation_mutation_mixture->setObjectName(QString::fromUtf8("action_Cr_ation_mutation_mixture"));
        action_Diminuer_sifflements = new QAction(MainWindow);
        action_Diminuer_sifflements->setObjectName(QString::fromUtf8("action_Diminuer_sifflements"));
        action_Diminuer_sifflements->setEnabled(true);
        action_Diminuer_sifflements_2 = new QAction(MainWindow);
        action_Diminuer_sifflements_2->setObjectName(QString::fromUtf8("action_Diminuer_sifflements_2"));
        action_Clavier = new QAction(MainWindow);
        action_Clavier->setObjectName(QString::fromUtf8("action_Clavier"));
        action_Clavier->setCheckable(true);
        action_Clavier->setChecked(true);
        action5_octaves = new QAction(MainWindow);
        action5_octaves->setObjectName(QString::fromUtf8("action5_octaves"));
        action5_octaves->setCheckable(true);
        action5_octaves->setChecked(true);
        action6_octaves = new QAction(MainWindow);
        action6_octaves->setObjectName(QString::fromUtf8("action6_octaves"));
        action6_octaves->setCheckable(true);
        action128_notes = new QAction(MainWindow);
        action128_notes->setObjectName(QString::fromUtf8("action128_notes"));
        action128_notes->setCheckable(true);
        actionAucun = new QAction(MainWindow);
        actionAucun->setObjectName(QString::fromUtf8("actionAucun"));
        actionAucun->setCheckable(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        ensembleKeyboard = new QWidget(centralWidget);
        ensembleKeyboard->setObjectName(QString::fromUtf8("ensembleKeyboard"));
        horizontalLayout_4 = new QHBoxLayout(ensembleKeyboard);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, 0, -1);
        labelNote = new QLabel(ensembleKeyboard);
        labelNote->setObjectName(QString::fromUtf8("labelNote"));
        labelNote->setMinimumSize(QSize(25, 0));
        labelNote->setMaximumSize(QSize(25, 16777215));
        QFont font;
        font.setPointSize(9);
        labelNote->setFont(font);
        labelNote->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelNote);

        labelVelocite = new QLabel(ensembleKeyboard);
        labelVelocite->setObjectName(QString::fromUtf8("labelVelocite"));
        labelVelocite->setMinimumSize(QSize(25, 0));
        labelVelocite->setMaximumSize(QSize(25, 16777215));
        labelVelocite->setFont(font);
        labelVelocite->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelVelocite);


        horizontalLayout_4->addLayout(verticalLayout);

        keyboard = new PianoKeybdCustom(ensembleKeyboard);
        keyboard->setObjectName(QString::fromUtf8("keyboard"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(keyboard->sizePolicy().hasHeightForWidth());
        keyboard->setSizePolicy(sizePolicy);
        keyboard->setMinimumSize(QSize(0, 35));
        keyboard->setMaximumSize(QSize(650, 40));
        keyboard->setSizeIncrement(QSize(21, 0));

        horizontalLayout_4->addWidget(keyboard);

        velocityButton = new QDial(ensembleKeyboard);
        velocityButton->setObjectName(QString::fromUtf8("velocityButton"));
        velocityButton->setMinimumSize(QSize(45, 35));
        velocityButton->setMaximumSize(QSize(45, 40));
        velocityButton->setMaximum(127);

        horizontalLayout_4->addWidget(velocityButton);


        gridLayout->addWidget(ensembleKeyboard, 0, 0, 1, 1);

        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page_Soft = new QWidget();
        page_Soft->setObjectName(QString::fromUtf8("page_Soft"));
        gridLayout_2 = new QGridLayout(page_Soft);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(page_Soft);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(50);
        sizePolicy1.setVerticalStretch(50);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setPixmap(QPixmap(QString::fromUtf8(":/icones/logo")));

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        stackedWidget->addWidget(page_Soft);

        gridLayout->addWidget(stackedWidget, 2, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        Menubar = new QMenuBar(MainWindow);
        Menubar->setObjectName(QString::fromUtf8("Menubar"));
        Menubar->setGeometry(QRect(0, 0, 943, 21));
        menuFichier = new QMenu(Menubar);
        menuFichier->setObjectName(QString::fromUtf8("menuFichier"));
        menu = new QMenu(Menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menuEdition = new QMenu(Menubar);
        menuEdition->setObjectName(QString::fromUtf8("menuEdition"));
        menuOutils = new QMenu(Menubar);
        menuOutils->setObjectName(QString::fromUtf8("menuOutils"));
        menuSample = new QMenu(menuOutils);
        menuSample->setObjectName(QString::fromUtf8("menuSample"));
        menuSample->setEnabled(false);
        menuDivers = new QMenu(menuOutils);
        menuDivers->setObjectName(QString::fromUtf8("menuDivers"));
        menuDivers->setEnabled(false);
        menuInstrument = new QMenu(menuOutils);
        menuInstrument->setObjectName(QString::fromUtf8("menuInstrument"));
        menuInstrument->setEnabled(false);
        menuAffichage = new QMenu(Menubar);
        menuAffichage->setObjectName(QString::fromUtf8("menuAffichage"));
        menuClavier = new QMenu(menuAffichage);
        menuClavier->setObjectName(QString::fromUtf8("menuClavier"));
        MainWindow->setMenuBar(Menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setAutoFillBackground(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setMinimumSize(QSize(220, 367));
        dockWidget->setMaximumSize(QSize(300, 524287));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        dockWidget->setFont(font1);
        dockWidget->setStyleSheet(QString::fromUtf8("QDockWidget::title\n"
"{\n"
"	border: 0px;\n"
"	padding: 3px;\n"
"}"));
        dockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QString::fromUtf8("dockWidgetContents_2"));
        horizontalLayout_3 = new QHBoxLayout(dockWidgetContents_2);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(-1, 0, -1, -1);
        arborescence = new Tree(dockWidgetContents_2);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        arborescence->setHeaderItem(__qtreewidgetitem);
        arborescence->setObjectName(QString::fromUtf8("arborescence"));
        QFont font2;
        font2.setBold(false);
        font2.setWeight(50);
        arborescence->setFont(font2);
        arborescence->setMouseTracking(false);
        arborescence->setContextMenuPolicy(Qt::CustomContextMenu);
        arborescence->setAcceptDrops(false);
        arborescence->setFrameShape(QFrame::Box);
        arborescence->setEditTriggers(QAbstractItemView::NoEditTriggers);
        arborescence->setProperty("showDropIndicator", QVariant(false));
        arborescence->setDragEnabled(true);
        arborescence->setDragDropMode(QAbstractItemView::DragDrop);
        arborescence->setDefaultDropAction(Qt::IgnoreAction);
        arborescence->setSelectionMode(QAbstractItemView::ExtendedSelection);
        arborescence->setIndentation(17);
        arborescence->setAnimated(true);
        arborescence->setColumnCount(1);
        arborescence->header()->setVisible(false);

        verticalLayout_3->addWidget(arborescence);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setContentsMargins(2, 2, 2, 2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        push_enrouler = new QPushButton(dockWidgetContents_2);
        push_enrouler->setObjectName(QString::fromUtf8("push_enrouler"));
        push_enrouler->setMaximumSize(QSize(30, 16777215));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/icones/collapse"), QSize(), QIcon::Normal, QIcon::Off);
        push_enrouler->setIcon(icon18);
        push_enrouler->setCheckable(false);
        push_enrouler->setFlat(true);

        horizontalLayout_2->addWidget(push_enrouler);

        push_derouler = new QPushButton(dockWidgetContents_2);
        push_derouler->setObjectName(QString::fromUtf8("push_derouler"));
        push_derouler->setMaximumSize(QSize(30, 16777215));
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/icones/expand"), QSize(), QIcon::Normal, QIcon::Off);
        push_derouler->setIcon(icon19);
        push_derouler->setFlat(true);

        horizontalLayout_2->addWidget(push_derouler);

        editSearch = new QLineEdit(dockWidgetContents_2);
        editSearch->setObjectName(QString::fromUtf8("editSearch"));
        editSearch->setFont(font2);

        horizontalLayout_2->addWidget(editSearch);

        push_nosearch = new QPushButton(dockWidgetContents_2);
        push_nosearch->setObjectName(QString::fromUtf8("push_nosearch"));
        push_nosearch->setMaximumSize(QSize(30, 16777215));
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/icones/erase"), QSize(), QIcon::Normal, QIcon::Off);
        push_nosearch->setIcon(icon20);
        push_nosearch->setFlat(true);

        horizontalLayout_2->addWidget(push_nosearch);


        verticalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_3);

        dockWidget->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);

        Menubar->addAction(menuFichier->menuAction());
        Menubar->addAction(menuEdition->menuAction());
        Menubar->addAction(menuOutils->menuAction());
        Menubar->addAction(menuAffichage->menuAction());
        Menubar->addAction(menu->menuAction());
        menuFichier->addAction(actionNouveau);
        menuFichier->addAction(actionOuvrir);
        menuFichier->addSeparator();
        menuFichier->addAction(actionEnregistrer);
        menuFichier->addAction(actionEnregistrer_sous);
        menuFichier->addSeparator();
        menuFichier->addAction(actionImporter);
        menuFichier->addAction(actionExporter);
        menuFichier->addSeparator();
        menuFichier->addAction(actionFichier_1);
        menuFichier->addAction(actionFichier_2);
        menuFichier->addAction(actionFichier_3);
        menuFichier->addAction(actionFichier_4);
        menuFichier->addAction(actionFichier_5);
        menuFichier->addSeparator();
        menuFichier->addAction(actionFermer_le_fichier);
        menuFichier->addAction(actionQuitter);
        menu->addAction(actionSommaire);
        menu->addSeparator();
        menu->addAction(actionA_propos);
        menuEdition->addAction(actionAnnuler);
        menuEdition->addAction(actionR_tablir);
        menuEdition->addSeparator();
        menuEdition->addAction(actionCopier);
        menuEdition->addAction(actionColler);
        menuEdition->addAction(action_Supprimer);
        menuEdition->addSeparator();
        menuEdition->addAction(actionRenommer);
        menuEdition->addSeparator();
        menuEdition->addAction(actionPr_f_rences);
        menuOutils->addAction(menuSample->menuAction());
        menuOutils->addAction(menuInstrument->menuAction());
        menuOutils->addAction(menuDivers->menuAction());
        menuSample->addAction(actionAjuster_la_fin_de_boucle);
        menuSample->addAction(actionBouclage_automatique);
        menuSample->addAction(action_Diminuer_sifflements);
        menuSample->addAction(actionEnlever_blanc_au_d_part);
        menuSample->addAction(actionFiltre_mur_de_brique);
        menuSample->addAction(actionNormaliser_volume);
        menuSample->addAction(action_R_glage_balance);
        menuSample->addAction(actionTransposer);
        menuDivers->addAction(action_Enlever_les_l_ments_non_utilis_s);
        menuDivers->addAction(actionR_gler_att_nuation_minimale);
        menuInstrument->addAction(action_Cr_ation_mutation_mixture);
        menuInstrument->addAction(actionD_saccordage_ondulant);
        menuInstrument->addAction(actionD_uplication_des_divisions);
        menuInstrument->addAction(action_Param_trage_global);
        menuInstrument->addAction(action_R_partition_automatique);
        menuInstrument->addAction(actionSpacialisation_du_son);
        menuAffichage->addAction(actionBarre_d_outils);
        menuAffichage->addAction(actionSection_modulateurs);
        menuAffichage->addAction(menuClavier->menuAction());
        menuClavier->addAction(actionAucun);
        menuClavier->addAction(action5_octaves);
        menuClavier->addAction(action6_octaves);
        menuClavier->addAction(action128_notes);
        toolBar->addAction(actionNouveau);
        toolBar->addAction(actionOuvrir);
        toolBar->addAction(actionEnregistrer);
        toolBar->addSeparator();
        toolBar->addAction(actionAnnuler);
        toolBar->addAction(actionR_tablir);
        toolBar->addSeparator();
        toolBar->addAction(actionImporter);
        toolBar->addAction(actionNouvel_instrument);
        toolBar->addAction(actionNouveau_preset);
        toolBar->addSeparator();
        toolBar->addAction(action_Supprimer);

        retranslateUi(MainWindow);
        QObject::connect(actionOuvrir, SIGNAL(triggered()), MainWindow, SLOT(ouvrir()));
        QObject::connect(actionQuitter, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(push_derouler, SIGNAL(clicked()), arborescence, SLOT(expandAll()));
        QObject::connect(push_enrouler, SIGNAL(clicked()), arborescence, SLOT(collapse()));
        QObject::connect(push_nosearch, SIGNAL(clicked()), editSearch, SLOT(clear()));
        QObject::connect(actionBarre_d_outils, SIGNAL(triggered()), MainWindow, SLOT(AfficherBarreOutils()));
        QObject::connect(editSearch, SIGNAL(textChanged(QString)), arborescence, SLOT(searchTree(QString)));
        QObject::connect(actionFermer_le_fichier, SIGNAL(triggered()), MainWindow, SLOT(Fermer()));
        QObject::connect(arborescence, SIGNAL(customContextMenuRequested(QPoint)), arborescence, SLOT(clicTreeRight()));
        QObject::connect(action_Supprimer, SIGNAL(triggered()), MainWindow, SLOT(supprimer()));
        QObject::connect(actionNouveau, SIGNAL(triggered()), MainWindow, SLOT(nouveau()));
        QObject::connect(actionAnnuler, SIGNAL(triggered()), MainWindow, SLOT(undo()));
        QObject::connect(actionR_tablir, SIGNAL(triggered()), MainWindow, SLOT(redo()));
        QObject::connect(actionRenommer, SIGNAL(triggered()), MainWindow, SLOT(renommer()));
        QObject::connect(actionEnregistrer, SIGNAL(triggered()), MainWindow, SLOT(sauvegarder()));
        QObject::connect(actionEnregistrer_sous, SIGNAL(triggered()), MainWindow, SLOT(sauvegarderSous()));
        QObject::connect(action_Enlever_les_l_ments_non_utilis_s, SIGNAL(triggered()), MainWindow, SLOT(purger()));
        QObject::connect(actionFichier_1, SIGNAL(triggered()), MainWindow, SLOT(ouvrirFichier1()));
        QObject::connect(actionFichier_2, SIGNAL(triggered()), MainWindow, SLOT(ouvrirFichier2()));
        QObject::connect(actionFichier_3, SIGNAL(triggered()), MainWindow, SLOT(ouvrirFichier3()));
        QObject::connect(actionFichier_4, SIGNAL(triggered()), MainWindow, SLOT(ouvrirFichier4()));
        QObject::connect(actionFichier_5, SIGNAL(triggered()), MainWindow, SLOT(ouvrirFichier5()));
        QObject::connect(arborescence, SIGNAL(itemSelectionChanged()), arborescence, SLOT(clicTree()));
        QObject::connect(actionPr_f_rences, SIGNAL(triggered()), MainWindow, SLOT(showConfig()));
        QObject::connect(actionA_propos, SIGNAL(triggered()), MainWindow, SLOT(showAbout()));
        QObject::connect(actionSection_modulateurs, SIGNAL(triggered()), MainWindow, SLOT(afficherSectionModulateurs()));
        QObject::connect(arborescence, SIGNAL(dropped(EltID,EltID,int,int*,QByteArray*,QByteArray*)), MainWindow, SLOT(dragAndDrop(EltID,EltID,int,int*,QByteArray*,QByteArray*)));
        QObject::connect(actionColler, SIGNAL(triggered()), MainWindow, SLOT(coller()));
        QObject::connect(actionCopier, SIGNAL(triggered()), MainWindow, SLOT(copier()));
        QObject::connect(actionImporter, SIGNAL(triggered()), MainWindow, SLOT(importerSmpl()));
        QObject::connect(actionExporter, SIGNAL(triggered()), MainWindow, SLOT(exporterSmpl()));
        QObject::connect(actionNouvel_instrument, SIGNAL(triggered()), MainWindow, SLOT(nouvelInstrument()));
        QObject::connect(actionNouveau_preset, SIGNAL(triggered()), MainWindow, SLOT(nouveauPreset()));
        QObject::connect(actionEnlever_blanc_au_d_part, SIGNAL(triggered()), MainWindow, SLOT(enleveBlanc()));
        QObject::connect(actionAjuster_la_fin_de_boucle, SIGNAL(triggered()), MainWindow, SLOT(enleveFin()));
        QObject::connect(actionNormaliser_volume, SIGNAL(triggered()), MainWindow, SLOT(normalisation()));
        QObject::connect(actionBouclage_automatique, SIGNAL(triggered()), MainWindow, SLOT(bouclage()));
        QObject::connect(actionFiltre_mur_de_brique, SIGNAL(triggered()), MainWindow, SLOT(filtreMur()));
        QObject::connect(actionTransposer, SIGNAL(triggered()), MainWindow, SLOT(transposer()));
        QObject::connect(actionD_saccordage_ondulant, SIGNAL(triggered()), MainWindow, SLOT(desaccorder()));
        QObject::connect(actionSpacialisation_du_son, SIGNAL(triggered()), MainWindow, SLOT(spatialisation()));
        QObject::connect(actionD_uplication_des_divisions, SIGNAL(triggered()), MainWindow, SLOT(duplication()));
        QObject::connect(action_Param_trage_global, SIGNAL(triggered()), MainWindow, SLOT(paramGlobal()));
        QObject::connect(action_R_partition_automatique, SIGNAL(triggered()), MainWindow, SLOT(repartitionAuto()));
        QObject::connect(actionR_gler_att_nuation_minimale, SIGNAL(triggered()), MainWindow, SLOT(attenuationMini()));
        QObject::connect(action_R_glage_balance, SIGNAL(triggered()), MainWindow, SLOT(reglerBalance()));
        QObject::connect(actionSommaire, SIGNAL(triggered()), MainWindow, SLOT(showHelp()));
        QObject::connect(action_Cr_ation_mutation_mixture, SIGNAL(triggered()), MainWindow, SLOT(mixture()));
        QObject::connect(action_Diminuer_sifflements, SIGNAL(triggered()), MainWindow, SLOT(sifflements()));
        QObject::connect(actionAucun, SIGNAL(triggered()), MainWindow, SLOT(setKeyboardType0()));
        QObject::connect(action5_octaves, SIGNAL(triggered()), MainWindow, SLOT(setKeyboardType1()));
        QObject::connect(action6_octaves, SIGNAL(triggered()), MainWindow, SLOT(setKeyboardType2()));
        QObject::connect(action128_notes, SIGNAL(triggered()), MainWindow, SLOT(setKeyboardType3()));
        QObject::connect(velocityButton, SIGNAL(valueChanged(int)), MainWindow, SLOT(setVelocity(int)));

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Polyphone", 0, QApplication::UnicodeUTF8));
        actionOuvrir->setText(QApplication::translate("MainWindow", "&Ouvrir...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOuvrir->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOuvrir->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionQuitter->setText(QApplication::translate("MainWindow", "&Quitter", 0, QApplication::UnicodeUTF8));
        actionQuitter->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionSommaire->setText(QApplication::translate("MainWindow", "&Manuel", 0, QApplication::UnicodeUTF8));
        actionSommaire->setShortcut(QApplication::translate("MainWindow", "F1", 0, QApplication::UnicodeUTF8));
        actionA_propos->setText(QApplication::translate("MainWindow", "&A propos", 0, QApplication::UnicodeUTF8));
        actionEnregistrer->setText(QApplication::translate("MainWindow", "&Enregistrer", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionEnregistrer->setToolTip(QApplication::translate("MainWindow", "Enregistre le fichier actuel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionEnregistrer->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionEnregistrer_sous->setText(QApplication::translate("MainWindow", "Enregistrer &sous...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionEnregistrer_sous->setToolTip(QApplication::translate("MainWindow", "Enregistre sous un autre fichier", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionEnregistrer_sous->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+S", 0, QApplication::UnicodeUTF8));
        actionNouveau->setText(QApplication::translate("MainWindow", "&Nouveau...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNouveau->setToolTip(QApplication::translate("MainWindow", "Cr\303\251e un nouveau fichier", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNouveau->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actionAnnuler->setText(QApplication::translate("MainWindow", "&Annuler", "1", QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAnnuler->setToolTip(QApplication::translate("MainWindow", "Annule la derni\303\250re action", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionAnnuler->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", 0, QApplication::UnicodeUTF8));
        actionR_tablir->setText(QApplication::translate("MainWindow", "R\303\251&tablir", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionR_tablir->setToolTip(QApplication::translate("MainWindow", "Refait la derni\303\250re action annul\303\251e", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionR_tablir->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+Z", 0, QApplication::UnicodeUTF8));
        actionPr_f_rences->setText(QApplication::translate("MainWindow", "&Pr\303\251f\303\251rences", 0, QApplication::UnicodeUTF8));
        actionBarre_d_outils->setText(QApplication::translate("MainWindow", "&Barre d'outils", 0, QApplication::UnicodeUTF8));
        actionFermer_le_fichier->setText(QApplication::translate("MainWindow", "&Fermer le fichier", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFermer_le_fichier->setToolTip(QApplication::translate("MainWindow", "Ferme le fichier", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionFermer_le_fichier->setShortcut(QApplication::translate("MainWindow", "Ctrl+W", 0, QApplication::UnicodeUTF8));
        actionCopier->setText(QApplication::translate("MainWindow", "&Copier", 0, QApplication::UnicodeUTF8));
        actionCopier->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        actionColler->setText(QApplication::translate("MainWindow", "C&oller", 0, QApplication::UnicodeUTF8));
        actionColler->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", 0, QApplication::UnicodeUTF8));
        action_Supprimer->setText(QApplication::translate("MainWindow", "&Supprimer", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Supprimer->setToolTip(QApplication::translate("MainWindow", "Supprime un \303\251l\303\251ment", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Supprimer->setShortcut(QApplication::translate("MainWindow", "Del", 0, QApplication::UnicodeUTF8));
        action_Enlever_les_l_ments_non_utilis_s->setText(QApplication::translate("MainWindow", "&Enlever les \303\251l\303\251ments non utilis\303\251s", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Enlever_les_l_ments_non_utilis_s->setToolTip(QApplication::translate("MainWindow", "Enleve les \303\251l\303\251ments non utilis\303\251s", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionRenommer->setText(QApplication::translate("MainWindow", "&Renommer", 0, QApplication::UnicodeUTF8));
        actionRenommer->setShortcut(QApplication::translate("MainWindow", "F2", 0, QApplication::UnicodeUTF8));
        actionFichier_1->setText(QApplication::translate("MainWindow", "fichier 1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFichier_1->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier r\303\251cent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionFichier_2->setText(QApplication::translate("MainWindow", "fichier 2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFichier_2->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier r\303\251cent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionFichier_3->setText(QApplication::translate("MainWindow", "fichier 3", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFichier_3->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier r\303\251cent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionFichier_4->setText(QApplication::translate("MainWindow", "fichier 4", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFichier_4->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier r\303\251cent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionFichier_5->setText(QApplication::translate("MainWindow", "fichier 5", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFichier_5->setToolTip(QApplication::translate("MainWindow", "Ouvre un fichier r\303\251cent", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSection_modulateurs->setText(QApplication::translate("MainWindow", "&Section modulateurs", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSection_modulateurs->setToolTip(QApplication::translate("MainWindow", "Affiche la section modulateurs", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionImporter->setText(QApplication::translate("MainWindow", "&Importer ...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionImporter->setToolTip(QApplication::translate("MainWindow", "Importe un fichier audio", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExporter->setText(QApplication::translate("MainWindow", "E&xporter ...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExporter->setToolTip(QApplication::translate("MainWindow", "Exporte au format wav", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNouvel_instrument->setText(QApplication::translate("MainWindow", "Nouvel instrument", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNouvel_instrument->setToolTip(QApplication::translate("MainWindow", "Cr\303\251e un nouvel instrument", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionNouveau_preset->setText(QApplication::translate("MainWindow", "Nouveau preset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionNouveau_preset->setToolTip(QApplication::translate("MainWindow", "Cr\303\251e un nouveau preset", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionEnlever_blanc_au_d_part->setText(QApplication::translate("MainWindow", "&Enlever blanc au d\303\251part", 0, QApplication::UnicodeUTF8));
        actionAjuster_la_fin_de_boucle->setText(QApplication::translate("MainWindow", "&Ajuster \303\240 la fin de boucle", 0, QApplication::UnicodeUTF8));
        actionNormaliser_volume->setText(QApplication::translate("MainWindow", "&Normaliser volume", 0, QApplication::UnicodeUTF8));
        actionBouclage_automatique->setText(QApplication::translate("MainWindow", "&Bouclage automatique", 0, QApplication::UnicodeUTF8));
        actionFiltre_mur_de_brique->setText(QApplication::translate("MainWindow", "&Filtre \"mur de brique\"...", 0, QApplication::UnicodeUTF8));
        actionTransposer->setText(QApplication::translate("MainWindow", "&Transposer...", 0, QApplication::UnicodeUTF8));
        actionR_gler_att_nuation_minimale->setText(QApplication::translate("MainWindow", "&R\303\251gler att\303\251nuation minimale...", 0, QApplication::UnicodeUTF8));
        actionD_saccordage_ondulant->setText(QApplication::translate("MainWindow", "&D\303\251saccordage ondulant...", 0, QApplication::UnicodeUTF8));
        actionSpacialisation_du_son->setText(QApplication::translate("MainWindow", "&Spatialisation du son...", 0, QApplication::UnicodeUTF8));
        action_R_partition_automatique->setText(QApplication::translate("MainWindow", "&R\303\251partition automatique", 0, QApplication::UnicodeUTF8));
        actionD_uplication_des_divisions->setText(QApplication::translate("MainWindow", "D&uplication des divisions", 0, QApplication::UnicodeUTF8));
        action_Param_trage_global->setText(QApplication::translate("MainWindow", "&Param\303\251trage global...", 0, QApplication::UnicodeUTF8));
        action_R_glage_balance->setText(QApplication::translate("MainWindow", "&R\303\251glage balance", 0, QApplication::UnicodeUTF8));
        action_Cr_ation_mutation_mixture->setText(QApplication::translate("MainWindow", "&Cr\303\251ation mixture...", 0, QApplication::UnicodeUTF8));
        action_Diminuer_sifflements->setText(QApplication::translate("MainWindow", "&Diminuer sifflements...", 0, QApplication::UnicodeUTF8));
        action_Diminuer_sifflements_2->setText(QApplication::translate("MainWindow", "&Diminuer sifflements...", 0, QApplication::UnicodeUTF8));
        action_Clavier->setText(QApplication::translate("MainWindow", "&Clavier", 0, QApplication::UnicodeUTF8));
        action5_octaves->setText(QApplication::translate("MainWindow", "&5 octaves", 0, QApplication::UnicodeUTF8));
        action6_octaves->setText(QApplication::translate("MainWindow", "&6 octaves", 0, QApplication::UnicodeUTF8));
        action128_notes->setText(QApplication::translate("MainWindow", "&128 notes", 0, QApplication::UnicodeUTF8));
        actionAucun->setText(QApplication::translate("MainWindow", "&Aucun", 0, QApplication::UnicodeUTF8));
        labelNote->setText(QApplication::translate("MainWindow", "-", 0, QApplication::UnicodeUTF8));
        labelVelocite->setText(QApplication::translate("MainWindow", "-", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
#ifndef QT_NO_ACCESSIBILITY
        Menubar->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
        menuFichier->setTitle(QApplication::translate("MainWindow", "&Fichier", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindow", "&Aide", 0, QApplication::UnicodeUTF8));
        menuEdition->setTitle(QApplication::translate("MainWindow", "&\303\211dition", 0, QApplication::UnicodeUTF8));
        menuOutils->setTitle(QApplication::translate("MainWindow", "&Outils", 0, QApplication::UnicodeUTF8));
        menuSample->setTitle(QApplication::translate("MainWindow", "&Sample", 0, QApplication::UnicodeUTF8));
        menuDivers->setTitle(QApplication::translate("MainWindow", "&Sf2", 0, QApplication::UnicodeUTF8));
        menuInstrument->setTitle(QApplication::translate("MainWindow", "&Instrument", 0, QApplication::UnicodeUTF8));
        menuAffichage->setTitle(QApplication::translate("MainWindow", "A&ffichage", 0, QApplication::UnicodeUTF8));
        menuClavier->setTitle(QApplication::translate("MainWindow", "Clavier", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "Barre d'outils", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        toolBar->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Arborescence", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        push_enrouler->setToolTip(QApplication::translate("MainWindow", "Enroule l'arborescence", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        push_enrouler->setText(QString());
#ifndef QT_NO_TOOLTIP
        push_derouler->setToolTip(QApplication::translate("MainWindow", "D\303\251roule l'arborescence", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        push_derouler->setText(QString());
        editSearch->setInputMask(QString());
        editSearch->setText(QString());
        editSearch->setPlaceholderText(QApplication::translate("MainWindow", "Rechercher...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        push_nosearch->setToolTip(QApplication::translate("MainWindow", "Efface la recherche", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        push_nosearch->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
