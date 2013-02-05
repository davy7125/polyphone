/********************************************************************************
** Form generated from reading UI file 'page_prst.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_PRST_H
#define UI_PAGE_PRST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "page.h"
#include "page_prst.h"

QT_BEGIN_NAMESPACE

class Ui_Page_Prst
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    SpinBox *spinBank;
    QLabel *label_3;
    SpinBox *spinPreset;
    QSpacerItem *horizontalSpacer;
    TableWidgetPrst *tablePrst;
    QFrame *dockWidget;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QPushButton *pushNouveauMod;
    QPushButton *pushSupprimerMod;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_7;
    TableWidgetMod *tableMod;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_9;
    ComboBox *comboSource1;
    ComboBox *comboSource2;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_10;
    TableComboBox *comboCourbeSource1;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_9;
    TableComboBox *comboCourbeSource2;
    QSpinBox *spinSource2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_11;
    QHBoxLayout *horizontalLayout_10;
    ComboBox *comboDestination;
    QCheckBox *checkAbs;

    void setupUi(QWidget *Page_Prst)
    {
        if (Page_Prst->objectName().isEmpty())
            Page_Prst->setObjectName(QString::fromUtf8("Page_Prst"));
        Page_Prst->resize(622, 428);
        verticalLayout = new QVBoxLayout(Page_Prst);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(6, 0, 6, 6);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 0, -1, 6);
        label_2 = new QLabel(Page_Prst);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(0, 20));
        label_2->setMaximumSize(QSize(16777215, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_2);

        spinBank = new SpinBox(Page_Prst);
        spinBank->setObjectName(QString::fromUtf8("spinBank"));
        spinBank->setMinimumSize(QSize(0, 20));
        spinBank->setMaximumSize(QSize(60, 20));
        spinBank->setAccelerated(true);
        spinBank->setMaximum(127);

        horizontalLayout->addWidget(spinBank);

        label_3 = new QLabel(Page_Prst);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(60, 20));
        label_3->setMaximumSize(QSize(100, 20));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_3);

        spinPreset = new SpinBox(Page_Prst);
        spinPreset->setObjectName(QString::fromUtf8("spinPreset"));
        spinPreset->setMinimumSize(QSize(0, 20));
        spinPreset->setMaximumSize(QSize(60, 20));
        spinPreset->setAccelerated(true);
        spinPreset->setMaximum(127);

        horizontalLayout->addWidget(spinPreset);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        tablePrst = new TableWidgetPrst(Page_Prst);
        if (tablePrst->columnCount() < 1)
            tablePrst->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tablePrst->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (tablePrst->rowCount() < 41)
            tablePrst->setRowCount(41);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(5, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(6, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(7, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(8, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(9, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(10, __qtablewidgetitem11);
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        __qtablewidgetitem12->setFont(font1);
        tablePrst->setVerticalHeaderItem(11, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(12, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(13, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(14, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(15, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(16, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(17, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(18, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(19, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(20, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(21, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(22, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(23, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(24, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(25, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(26, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(27, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(28, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(29, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(30, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(31, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(32, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(33, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(34, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(35, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(36, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(37, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(38, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(39, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        tablePrst->setVerticalHeaderItem(40, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        tablePrst->setItem(13, 0, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        tablePrst->setItem(14, 0, __qtablewidgetitem43);
        tablePrst->setObjectName(QString::fromUtf8("tablePrst"));
        QFont font2;
        font2.setPointSize(8);
        tablePrst->setFont(font2);
        tablePrst->setFrameShape(QFrame::Box);
        tablePrst->setFrameShadow(QFrame::Sunken);
        tablePrst->setAutoScrollMargin(16);
        tablePrst->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
        tablePrst->setTextElideMode(Qt::ElideRight);
        tablePrst->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tablePrst->setRowCount(41);
        tablePrst->setColumnCount(1);
        tablePrst->horizontalHeader()->setDefaultSectionSize(63);
        tablePrst->horizontalHeader()->setMinimumSectionSize(18);
        tablePrst->verticalHeader()->setDefaultSectionSize(19);
        tablePrst->verticalHeader()->setMinimumSectionSize(19);

        verticalLayout->addWidget(tablePrst);

        dockWidget = new QFrame(Page_Prst);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setMaximumSize(QSize(16777215, 150));
        dockWidget->setFrameShape(QFrame::NoFrame);
        dockWidget->setFrameShadow(QFrame::Raised);
        verticalLayout_7 = new QVBoxLayout(dockWidget);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 6, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 0, -1, -1);
        label = new QLabel(dockWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(16777215, 30));
        label->setFont(font);

        horizontalLayout_6->addWidget(label);

        pushNouveauMod = new QPushButton(dockWidget);
        pushNouveauMod->setObjectName(QString::fromUtf8("pushNouveauMod"));
        pushNouveauMod->setMaximumSize(QSize(24, 24));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/add"), QSize(), QIcon::Normal, QIcon::Off);
        pushNouveauMod->setIcon(icon);

        horizontalLayout_6->addWidget(pushNouveauMod);

        pushSupprimerMod = new QPushButton(dockWidget);
        pushSupprimerMod->setObjectName(QString::fromUtf8("pushSupprimerMod"));
        pushSupprimerMod->setMaximumSize(QSize(24, 24));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icones/remove"), QSize(), QIcon::Normal, QIcon::Off);
        pushSupprimerMod->setIcon(icon1);

        horizontalLayout_6->addWidget(pushSupprimerMod);


        verticalLayout_7->addLayout(horizontalLayout_6);

        frame_3 = new QFrame(dockWidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMaximumSize(QSize(16777215, 160));
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(frame_3);
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        tableMod = new TableWidgetMod(frame_3);
        if (tableMod->columnCount() < 9)
            tableMod->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(0, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(1, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(2, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(3, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(4, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        __qtablewidgetitem49->setFont(font);
        tableMod->setHorizontalHeaderItem(5, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        __qtablewidgetitem50->setFont(font);
        tableMod->setHorizontalHeaderItem(6, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        __qtablewidgetitem51->setFont(font);
        tableMod->setHorizontalHeaderItem(7, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        __qtablewidgetitem52->setFont(font);
        tableMod->setHorizontalHeaderItem(8, __qtablewidgetitem52);
        tableMod->setObjectName(QString::fromUtf8("tableMod"));
        tableMod->setMaximumSize(QSize(800, 16777215));
        QFont font3;
        font3.setPointSize(8);
        font3.setBold(false);
        font3.setWeight(50);
        tableMod->setFont(font3);
        tableMod->setFrameShape(QFrame::Box);
        tableMod->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableMod->setTabKeyNavigation(false);
        tableMod->setProperty("showDropIndicator", QVariant(false));
        tableMod->setDragDropOverwriteMode(false);
        tableMod->setSelectionMode(QAbstractItemView::SingleSelection);
        tableMod->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableMod->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
        tableMod->setColumnCount(9);
        tableMod->horizontalHeader()->setDefaultSectionSize(130);
        tableMod->horizontalHeader()->setHighlightSections(false);
        tableMod->horizontalHeader()->setMinimumSectionSize(18);
        tableMod->horizontalHeader()->setStretchLastSection(true);
        tableMod->verticalHeader()->setVisible(false);
        tableMod->verticalHeader()->setDefaultSectionSize(19);

        horizontalLayout_7->addWidget(tableMod);

        widget_3 = new QWidget(frame_3);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        verticalLayout_8 = new QVBoxLayout(widget_3);
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        groupBox_4 = new QGroupBox(widget_3);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        groupBox_4->setMinimumSize(QSize(0, 75));
        horizontalLayout_8 = new QHBoxLayout(groupBox_4);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 3, 0, 0);
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        comboSource1 = new ComboBox(groupBox_4);
        comboSource1->setObjectName(QString::fromUtf8("comboSource1"));
        comboSource1->setFont(font1);
        comboSource1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        verticalLayout_9->addWidget(comboSource1);

        comboSource2 = new ComboBox(groupBox_4);
        comboSource2->setObjectName(QString::fromUtf8("comboSource2"));
        comboSource2->setFont(font1);

        verticalLayout_9->addWidget(comboSource2);


        horizontalLayout_8->addLayout(verticalLayout_9);

        frame_4 = new QFrame(groupBox_4);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setMinimumSize(QSize(120, 0));
        frame_4->setMaximumSize(QSize(120, 16777215));
        frame_4->setFrameShape(QFrame::NoFrame);
        frame_4->setFrameShadow(QFrame::Raised);
        verticalLayout_10 = new QVBoxLayout(frame_4);
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        comboCourbeSource1 = new TableComboBox(frame_4);
        comboCourbeSource1->setObjectName(QString::fromUtf8("comboCourbeSource1"));
        comboCourbeSource1->setMinimumSize(QSize(0, 24));
        comboCourbeSource1->setMaximumSize(QSize(50, 24));
        comboCourbeSource1->setFont(font1);
        comboCourbeSource1->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        comboCourbeSource1->setModelColumn(0);

        verticalLayout_10->addWidget(comboCourbeSource1);

        frame_5 = new QFrame(frame_4);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setMinimumSize(QSize(0, 24));
        frame_5->setFrameShape(QFrame::NoFrame);
        frame_5->setFrameShadow(QFrame::Raised);
        horizontalLayout_9 = new QHBoxLayout(frame_5);
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        comboCourbeSource2 = new TableComboBox(frame_5);
        comboCourbeSource2->setObjectName(QString::fromUtf8("comboCourbeSource2"));
        comboCourbeSource2->setMinimumSize(QSize(0, 24));
        comboCourbeSource2->setMaximumSize(QSize(50, 24));
        comboCourbeSource2->setFont(font1);
        comboCourbeSource2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout_9->addWidget(comboCourbeSource2);

        spinSource2 = new QSpinBox(frame_5);
        spinSource2->setObjectName(QString::fromUtf8("spinSource2"));
        spinSource2->setMaximumSize(QSize(63, 16777215));
        spinSource2->setMinimum(-32768);
        spinSource2->setMaximum(32767);

        horizontalLayout_9->addWidget(spinSource2);


        verticalLayout_10->addWidget(frame_5);


        horizontalLayout_8->addWidget(frame_4);


        verticalLayout_8->addWidget(groupBox_4);

        groupBox_2 = new QGroupBox(widget_3);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_11 = new QVBoxLayout(groupBox_2);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(0, 3, 0, 0);
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        comboDestination = new ComboBox(groupBox_2);
        comboDestination->setObjectName(QString::fromUtf8("comboDestination"));
        comboDestination->setFont(font1);

        horizontalLayout_10->addWidget(comboDestination);

        checkAbs = new QCheckBox(groupBox_2);
        checkAbs->setObjectName(QString::fromUtf8("checkAbs"));
        checkAbs->setMinimumSize(QSize(119, 0));
        checkAbs->setMaximumSize(QSize(118, 16777215));

        horizontalLayout_10->addWidget(checkAbs);


        verticalLayout_11->addLayout(horizontalLayout_10);


        verticalLayout_8->addWidget(groupBox_2);


        horizontalLayout_7->addWidget(widget_3);


        verticalLayout_7->addWidget(frame_3);


        verticalLayout->addWidget(dockWidget);


        retranslateUi(Page_Prst);
        QObject::connect(tablePrst, SIGNAL(cellChanged(int,int)), Page_Prst, SLOT(set(int,int)));
        QObject::connect(tablePrst, SIGNAL(itemSelectionChanged()), Page_Prst, SLOT(selected()));
        QObject::connect(tableMod, SIGNAL(itemSelectionChanged()), Page_Prst, SLOT(afficheEditMod()));
        QObject::connect(spinSource2, SIGNAL(editingFinished()), Page_Prst, SLOT(setAmount()));
        QObject::connect(comboSource2, SIGNAL(clicked(int)), Page_Prst, SLOT(setSource2(int)));
        QObject::connect(comboSource1, SIGNAL(clicked(int)), Page_Prst, SLOT(setSource(int)));
        QObject::connect(comboDestination, SIGNAL(clicked(int)), Page_Prst, SLOT(setDest(int)));
        QObject::connect(comboCourbeSource2, SIGNAL(clicked(int,int)), Page_Prst, SLOT(setSourceAmountType(int,int)));
        QObject::connect(comboCourbeSource1, SIGNAL(clicked(int,int)), Page_Prst, SLOT(setSourceType(int,int)));
        QObject::connect(checkAbs, SIGNAL(clicked()), Page_Prst, SLOT(setAbs()));
        QObject::connect(pushNouveauMod, SIGNAL(clicked()), Page_Prst, SLOT(nouveauMod()));
        QObject::connect(pushSupprimerMod, SIGNAL(clicked()), Page_Prst, SLOT(supprimerMod()));
        QObject::connect(spinBank, SIGNAL(editingFinished()), Page_Prst, SLOT(setBank()));
        QObject::connect(spinPreset, SIGNAL(editingFinished()), Page_Prst, SLOT(setPreset()));

        QMetaObject::connectSlotsByName(Page_Prst);
    } // setupUi

    void retranslateUi(QWidget *Page_Prst)
    {
        Page_Prst->setWindowTitle(QApplication::translate("Page_Prst", "Form", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Page_Prst", "Banque", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Page_Prst", "Preset", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tablePrst->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("Page_Prst", "Global", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tablePrst->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("Page_Prst", "typeElement", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tablePrst->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("Page_Prst", "indexSf2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tablePrst->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("Page_Prst", "indexElt", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tablePrst->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("Page_Prst", "indexElt2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tablePrst->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("Page_Prst", "Etendue note", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tablePrst->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("Page_Prst", "Etendue v\303\251locit\303\251", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tablePrst->verticalHeaderItem(6);
        ___qtablewidgetitem7->setText(QApplication::translate("Page_Prst", "Att\303\251nuation (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tablePrst->verticalHeaderItem(7);
        ___qtablewidgetitem8->setText(QApplication::translate("Page_Prst", "Balance [-50;50]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tablePrst->verticalHeaderItem(8);
        ___qtablewidgetitem9->setText(QApplication::translate("Page_Prst", "Accordage (demi-tons)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tablePrst->verticalHeaderItem(9);
        ___qtablewidgetitem10->setText(QApplication::translate("Page_Prst", "Accordage (centi\303\250mes)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tablePrst->verticalHeaderItem(10);
        ___qtablewidgetitem11->setText(QApplication::translate("Page_Prst", "Accordage (scale)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tablePrst->verticalHeaderItem(11);
        ___qtablewidgetitem12->setText(QApplication::translate("Page_Prst", "Filtre, fr\303\251quence (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tablePrst->verticalHeaderItem(12);
        ___qtablewidgetitem13->setText(QApplication::translate("Page_Prst", "Filtre, r\303\251sonance (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tablePrst->verticalHeaderItem(13);
        ___qtablewidgetitem14->setText(QApplication::translate("Page_Prst", "Vol env delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tablePrst->verticalHeaderItem(14);
        ___qtablewidgetitem15->setText(QApplication::translate("Page_Prst", "Vol env attack (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tablePrst->verticalHeaderItem(15);
        ___qtablewidgetitem16->setText(QApplication::translate("Page_Prst", "Vol env hold (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tablePrst->verticalHeaderItem(16);
        ___qtablewidgetitem17->setText(QApplication::translate("Page_Prst", "Vol env decay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tablePrst->verticalHeaderItem(17);
        ___qtablewidgetitem18->setText(QApplication::translate("Page_Prst", "Vol env sustain (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = tablePrst->verticalHeaderItem(18);
        ___qtablewidgetitem19->setText(QApplication::translate("Page_Prst", "Vol env release (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = tablePrst->verticalHeaderItem(19);
        ___qtablewidgetitem20->setText(QApplication::translate("Page_Prst", "Note \342\206\222 Vol env hold (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = tablePrst->verticalHeaderItem(20);
        ___qtablewidgetitem21->setText(QApplication::translate("Page_Prst", "Note \342\206\222 Vol env decay (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = tablePrst->verticalHeaderItem(21);
        ___qtablewidgetitem22->setText(QApplication::translate("Page_Prst", "Mod env delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = tablePrst->verticalHeaderItem(22);
        ___qtablewidgetitem23->setText(QApplication::translate("Page_Prst", "Mod env attack (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = tablePrst->verticalHeaderItem(23);
        ___qtablewidgetitem24->setText(QApplication::translate("Page_Prst", "Mod env hold (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = tablePrst->verticalHeaderItem(24);
        ___qtablewidgetitem25->setText(QApplication::translate("Page_Prst", "Mod env decay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = tablePrst->verticalHeaderItem(25);
        ___qtablewidgetitem26->setText(QApplication::translate("Page_Prst", "Mod env sustain (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = tablePrst->verticalHeaderItem(26);
        ___qtablewidgetitem27->setText(QApplication::translate("Page_Prst", "Mod env release (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = tablePrst->verticalHeaderItem(27);
        ___qtablewidgetitem28->setText(QApplication::translate("Page_Prst", "Mod env \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = tablePrst->verticalHeaderItem(28);
        ___qtablewidgetitem29->setText(QApplication::translate("Page_Prst", "Mod env \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = tablePrst->verticalHeaderItem(29);
        ___qtablewidgetitem30->setText(QApplication::translate("Page_Prst", "Note \342\206\222 Mod env hold (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = tablePrst->verticalHeaderItem(30);
        ___qtablewidgetitem31->setText(QApplication::translate("Page_Prst", "Note \342\206\222 Mod env decay (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = tablePrst->verticalHeaderItem(31);
        ___qtablewidgetitem32->setText(QApplication::translate("Page_Prst", "Mod LFO delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = tablePrst->verticalHeaderItem(32);
        ___qtablewidgetitem33->setText(QApplication::translate("Page_Prst", "Mod LFO freq (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = tablePrst->verticalHeaderItem(33);
        ___qtablewidgetitem34->setText(QApplication::translate("Page_Prst", "Mod LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem35 = tablePrst->verticalHeaderItem(34);
        ___qtablewidgetitem35->setText(QApplication::translate("Page_Prst", "Mod LFO \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem36 = tablePrst->verticalHeaderItem(35);
        ___qtablewidgetitem36->setText(QApplication::translate("Page_Prst", "Mod LFO \342\206\222 volume (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem37 = tablePrst->verticalHeaderItem(36);
        ___qtablewidgetitem37->setText(QApplication::translate("Page_Prst", "Vib LFO delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem38 = tablePrst->verticalHeaderItem(37);
        ___qtablewidgetitem38->setText(QApplication::translate("Page_Prst", "Vib LFO freq (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem39 = tablePrst->verticalHeaderItem(38);
        ___qtablewidgetitem39->setText(QApplication::translate("Page_Prst", "Vib LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem40 = tablePrst->verticalHeaderItem(39);
        ___qtablewidgetitem40->setText(QApplication::translate("Page_Prst", "Chorus (%)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem41 = tablePrst->verticalHeaderItem(40);
        ___qtablewidgetitem41->setText(QApplication::translate("Page_Prst", "R\303\251verb\303\251ration (%)", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = tablePrst->isSortingEnabled();
        tablePrst->setSortingEnabled(false);
        tablePrst->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("Page_Prst", "Modulateurs", 0, QApplication::UnicodeUTF8));
        pushNouveauMod->setText(QString());
        pushSupprimerMod->setText(QString());
        QTableWidgetItem *___qtablewidgetitem42 = tableMod->horizontalHeaderItem(0);
        ___qtablewidgetitem42->setText(QApplication::translate("Page_Prst", "typeElement", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem43 = tableMod->horizontalHeaderItem(1);
        ___qtablewidgetitem43->setText(QApplication::translate("Page_Prst", "indexSf2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem44 = tableMod->horizontalHeaderItem(2);
        ___qtablewidgetitem44->setText(QApplication::translate("Page_Prst", "indexElt", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem45 = tableMod->horizontalHeaderItem(3);
        ___qtablewidgetitem45->setText(QApplication::translate("Page_Prst", "indexElt2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem46 = tableMod->horizontalHeaderItem(4);
        ___qtablewidgetitem46->setText(QApplication::translate("Page_Prst", "indexMod", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem47 = tableMod->horizontalHeaderItem(5);
        ___qtablewidgetitem47->setText(QApplication::translate("Page_Prst", "#", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem48 = tableMod->horizontalHeaderItem(6);
        ___qtablewidgetitem48->setText(QApplication::translate("Page_Prst", "Source", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem49 = tableMod->horizontalHeaderItem(7);
        ___qtablewidgetitem49->setText(QApplication::translate("Page_Prst", "Quantit\303\251", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem50 = tableMod->horizontalHeaderItem(8);
        ___qtablewidgetitem50->setText(QApplication::translate("Page_Prst", "Destination", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("Page_Prst", "Source / Quantit\303\251", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Page_Prst", "Destination", 0, QApplication::UnicodeUTF8));
        checkAbs->setText(QApplication::translate("Page_Prst", "Valeur abs.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Page_Prst: public Ui_Page_Prst {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_PRST_H
