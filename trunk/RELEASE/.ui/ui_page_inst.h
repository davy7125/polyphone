/********************************************************************************
** Form generated from reading UI file 'page_inst.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_INST_H
#define UI_PAGE_INST_H

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
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "page.h"
#include "page_inst.h"

QT_BEGIN_NAMESPACE

class Ui_Page_Inst
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout;
    QLabel *labelPrst;
    TableWidgetInst *tableInst;
    QFrame *dockWidget;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QPushButton *pushNouveauMod;
    QPushButton *pushSupprimerMod;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_5;
    TableWidgetMod *tableMod;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    ComboBox *comboSource1;
    ComboBox *comboSource2;
    QFrame *frame;
    QVBoxLayout *verticalLayout_4;
    TableComboBox *comboCourbeSource1;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_4;
    TableComboBox *comboCourbeSource2;
    QSpinBox *spinSource2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    ComboBox *comboDestination;
    QCheckBox *checkAbs;

    void setupUi(QWidget *Page_Inst)
    {
        if (Page_Inst->objectName().isEmpty())
            Page_Inst->setObjectName(QString::fromUtf8("Page_Inst"));
        Page_Inst->resize(700, 335);
        verticalLayout = new QVBoxLayout(Page_Inst);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(6, 0, 6, 6);
        frame_5 = new QFrame(Page_Inst);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_5->sizePolicy().hasHeightForWidth());
        frame_5->setSizePolicy(sizePolicy);
        frame_5->setFrameShape(QFrame::NoFrame);
        frame_5->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 6);
        labelPrst = new QLabel(frame_5);
        labelPrst->setObjectName(QString::fromUtf8("labelPrst"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelPrst->sizePolicy().hasHeightForWidth());
        labelPrst->setSizePolicy(sizePolicy1);
        labelPrst->setMinimumSize(QSize(0, 20));
        labelPrst->setMaximumSize(QSize(16777215, 20));
        labelPrst->setTextInteractionFlags(Qt::NoTextInteraction);

        horizontalLayout->addWidget(labelPrst);


        verticalLayout->addWidget(frame_5);

        tableInst = new TableWidgetInst(Page_Inst);
        if (tableInst->columnCount() < 1)
            tableInst->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableInst->setHorizontalHeaderItem(0, __qtablewidgetitem);
        if (tableInst->rowCount() < 50)
            tableInst->setRowCount(50);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(3, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(4, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(5, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(6, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(7, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(8, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(9, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(10, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(11, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(12, __qtablewidgetitem13);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        __qtablewidgetitem14->setFont(font);
        tableInst->setVerticalHeaderItem(13, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(14, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(15, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(16, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(17, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(18, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(19, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(20, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(21, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(22, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(23, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(24, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(25, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(26, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(27, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(28, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(29, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(30, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(31, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(32, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(33, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(34, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(35, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(36, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(37, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(38, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(39, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(40, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(41, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(42, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(43, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(44, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(45, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(46, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(47, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(48, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        tableInst->setVerticalHeaderItem(49, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        tableInst->setItem(15, 0, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        tableInst->setItem(16, 0, __qtablewidgetitem52);
        tableInst->setObjectName(QString::fromUtf8("tableInst"));
        QFont font1;
        font1.setPointSize(8);
        tableInst->setFont(font1);
        tableInst->setFrameShape(QFrame::Box);
        tableInst->setFrameShadow(QFrame::Sunken);
        tableInst->setAutoScroll(true);
        tableInst->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
        tableInst->setProperty("showDropIndicator", QVariant(false));
        tableInst->setDragDropOverwriteMode(false);
        tableInst->setTextElideMode(Qt::ElideRight);
        tableInst->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableInst->setRowCount(50);
        tableInst->setColumnCount(1);
        tableInst->horizontalHeader()->setDefaultSectionSize(63);
        tableInst->horizontalHeader()->setMinimumSectionSize(18);
        tableInst->verticalHeader()->setDefaultSectionSize(20);
        tableInst->verticalHeader()->setMinimumSectionSize(19);

        verticalLayout->addWidget(tableInst);

        dockWidget = new QFrame(Page_Inst);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidget->setMaximumSize(QSize(16777215, 150));
        dockWidget->setFrameShape(QFrame::NoFrame);
        dockWidget->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(dockWidget);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 6, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 0, -1, -1);
        label = new QLabel(dockWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(16777215, 30));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        label->setFont(font2);

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


        verticalLayout_6->addLayout(horizontalLayout_6);

        frame_3 = new QFrame(dockWidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMaximumSize(QSize(16777215, 160));
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_3);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        tableMod = new TableWidgetMod(frame_3);
        if (tableMod->columnCount() < 9)
            tableMod->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(0, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(1, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(2, __qtablewidgetitem55);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(3, __qtablewidgetitem56);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        tableMod->setHorizontalHeaderItem(4, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        __qtablewidgetitem58->setFont(font2);
        tableMod->setHorizontalHeaderItem(5, __qtablewidgetitem58);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        __qtablewidgetitem59->setFont(font2);
        tableMod->setHorizontalHeaderItem(6, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        __qtablewidgetitem60->setFont(font2);
        tableMod->setHorizontalHeaderItem(7, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        __qtablewidgetitem61->setFont(font2);
        tableMod->setHorizontalHeaderItem(8, __qtablewidgetitem61);
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

        horizontalLayout_5->addWidget(tableMod);

        widget_2 = new QWidget(frame_3);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        verticalLayout_2 = new QVBoxLayout(widget_2);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox_3 = new QGroupBox(widget_2);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy2);
        groupBox_3->setMinimumSize(QSize(0, 75));
        horizontalLayout_2 = new QHBoxLayout(groupBox_3);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 3, 0, 0);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        comboSource1 = new ComboBox(groupBox_3);
        comboSource1->setObjectName(QString::fromUtf8("comboSource1"));
        comboSource1->setFont(font);
        comboSource1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        verticalLayout_3->addWidget(comboSource1);

        comboSource2 = new ComboBox(groupBox_3);
        comboSource2->setObjectName(QString::fromUtf8("comboSource2"));
        comboSource2->setFont(font);

        verticalLayout_3->addWidget(comboSource2);


        horizontalLayout_2->addLayout(verticalLayout_3);

        frame = new QFrame(groupBox_3);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(120, 0));
        frame->setMaximumSize(QSize(120, 16777215));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frame);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        comboCourbeSource1 = new TableComboBox(frame);
        comboCourbeSource1->setObjectName(QString::fromUtf8("comboCourbeSource1"));
        comboCourbeSource1->setMinimumSize(QSize(0, 24));
        comboCourbeSource1->setMaximumSize(QSize(50, 24));
        comboCourbeSource1->setFont(font);
        comboCourbeSource1->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        comboCourbeSource1->setModelColumn(0);

        verticalLayout_4->addWidget(comboCourbeSource1);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMinimumSize(QSize(0, 24));
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_2);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        comboCourbeSource2 = new TableComboBox(frame_2);
        comboCourbeSource2->setObjectName(QString::fromUtf8("comboCourbeSource2"));
        comboCourbeSource2->setMinimumSize(QSize(0, 24));
        comboCourbeSource2->setMaximumSize(QSize(50, 24));
        comboCourbeSource2->setFont(font);
        comboCourbeSource2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout_4->addWidget(comboCourbeSource2);

        spinSource2 = new QSpinBox(frame_2);
        spinSource2->setObjectName(QString::fromUtf8("spinSource2"));
        spinSource2->setMaximumSize(QSize(63, 16777215));
        spinSource2->setMinimum(-32768);
        spinSource2->setMaximum(32767);

        horizontalLayout_4->addWidget(spinSource2);


        verticalLayout_4->addWidget(frame_2);


        horizontalLayout_2->addWidget(frame);


        verticalLayout_2->addWidget(groupBox_3);

        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_5 = new QVBoxLayout(groupBox);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 3, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        comboDestination = new ComboBox(groupBox);
        comboDestination->setObjectName(QString::fromUtf8("comboDestination"));
        comboDestination->setFont(font);

        horizontalLayout_3->addWidget(comboDestination);

        checkAbs = new QCheckBox(groupBox);
        checkAbs->setObjectName(QString::fromUtf8("checkAbs"));
        checkAbs->setMinimumSize(QSize(119, 0));
        checkAbs->setMaximumSize(QSize(118, 16777215));

        horizontalLayout_3->addWidget(checkAbs);


        verticalLayout_5->addLayout(horizontalLayout_3);


        verticalLayout_2->addWidget(groupBox);


        horizontalLayout_5->addWidget(widget_2);


        verticalLayout_6->addWidget(frame_3);


        verticalLayout->addWidget(dockWidget);


        retranslateUi(Page_Inst);
        QObject::connect(tableInst, SIGNAL(cellChanged(int,int)), Page_Inst, SLOT(set(int,int)));
        QObject::connect(spinSource2, SIGNAL(editingFinished()), Page_Inst, SLOT(setAmount()));
        QObject::connect(tableMod, SIGNAL(itemSelectionChanged()), Page_Inst, SLOT(afficheEditMod()));
        QObject::connect(checkAbs, SIGNAL(clicked()), Page_Inst, SLOT(setAbs()));
        QObject::connect(comboCourbeSource1, SIGNAL(clicked(int,int)), Page_Inst, SLOT(setSourceType(int,int)));
        QObject::connect(comboCourbeSource2, SIGNAL(clicked(int,int)), Page_Inst, SLOT(setSourceAmountType(int,int)));
        QObject::connect(comboDestination, SIGNAL(clicked(int)), Page_Inst, SLOT(setDest(int)));
        QObject::connect(comboSource1, SIGNAL(clicked(int)), Page_Inst, SLOT(setSource(int)));
        QObject::connect(comboSource2, SIGNAL(clicked(int)), Page_Inst, SLOT(setSource2(int)));
        QObject::connect(pushNouveauMod, SIGNAL(clicked()), Page_Inst, SLOT(nouveauMod()));
        QObject::connect(pushSupprimerMod, SIGNAL(clicked()), Page_Inst, SLOT(supprimerMod()));
        QObject::connect(tableInst, SIGNAL(itemSelectionChanged()), Page_Inst, SLOT(selected()));

        QMetaObject::connectSlotsByName(Page_Inst);
    } // setupUi

    void retranslateUi(QWidget *Page_Inst)
    {
        Page_Inst->setWindowTitle(QApplication::translate("Page_Inst", "Form", 0, QApplication::UnicodeUTF8));
        labelPrst->setText(QApplication::translate("Page_Inst", "TextLabel", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableInst->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("Page_Inst", "Global", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableInst->verticalHeaderItem(0);
        ___qtablewidgetitem1->setText(QApplication::translate("Page_Inst", "typeElement", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableInst->verticalHeaderItem(1);
        ___qtablewidgetitem2->setText(QApplication::translate("Page_Inst", "indexSf2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableInst->verticalHeaderItem(2);
        ___qtablewidgetitem3->setText(QApplication::translate("Page_Inst", "indexElt", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableInst->verticalHeaderItem(3);
        ___qtablewidgetitem4->setText(QApplication::translate("Page_Inst", "indexElt2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableInst->verticalHeaderItem(4);
        ___qtablewidgetitem5->setText(QApplication::translate("Page_Inst", "Etendue note", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableInst->verticalHeaderItem(5);
        ___qtablewidgetitem6->setText(QApplication::translate("Page_Inst", "Etendue v\303\251locit\303\251", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableInst->verticalHeaderItem(6);
        ___qtablewidgetitem7->setText(QApplication::translate("Page_Inst", "Att\303\251nuation (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableInst->verticalHeaderItem(7);
        ___qtablewidgetitem8->setText(QApplication::translate("Page_Inst", "Balance [-50;50]", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableInst->verticalHeaderItem(8);
        ___qtablewidgetitem9->setText(QApplication::translate("Page_Inst", "Lecture en boucle", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tableInst->verticalHeaderItem(9);
        ___qtablewidgetitem10->setText(QApplication::translate("Page_Inst", "Note de base", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tableInst->verticalHeaderItem(10);
        ___qtablewidgetitem11->setText(QApplication::translate("Page_Inst", "Accordage (demi-tons)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tableInst->verticalHeaderItem(11);
        ___qtablewidgetitem12->setText(QApplication::translate("Page_Inst", "Accordage (centi\303\250mes)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tableInst->verticalHeaderItem(12);
        ___qtablewidgetitem13->setText(QApplication::translate("Page_Inst", "Accordage (scale)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tableInst->verticalHeaderItem(13);
        ___qtablewidgetitem14->setText(QApplication::translate("Page_Inst", "Filtre, fr\303\251quence (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tableInst->verticalHeaderItem(14);
        ___qtablewidgetitem15->setText(QApplication::translate("Page_Inst", "Filtre, r\303\251sonance (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tableInst->verticalHeaderItem(15);
        ___qtablewidgetitem16->setText(QApplication::translate("Page_Inst", "Vol env delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tableInst->verticalHeaderItem(16);
        ___qtablewidgetitem17->setText(QApplication::translate("Page_Inst", "Vol env attack (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tableInst->verticalHeaderItem(17);
        ___qtablewidgetitem18->setText(QApplication::translate("Page_Inst", "Vol env hold (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = tableInst->verticalHeaderItem(18);
        ___qtablewidgetitem19->setText(QApplication::translate("Page_Inst", "Vol env decay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = tableInst->verticalHeaderItem(19);
        ___qtablewidgetitem20->setText(QApplication::translate("Page_Inst", "Vol env sustain (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem21 = tableInst->verticalHeaderItem(20);
        ___qtablewidgetitem21->setText(QApplication::translate("Page_Inst", "Vol env release (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem22 = tableInst->verticalHeaderItem(21);
        ___qtablewidgetitem22->setText(QApplication::translate("Page_Inst", "Note \342\206\222 Vol env hold (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem23 = tableInst->verticalHeaderItem(22);
        ___qtablewidgetitem23->setText(QApplication::translate("Page_Inst", "Note \342\206\222 Vol env decay (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem24 = tableInst->verticalHeaderItem(23);
        ___qtablewidgetitem24->setText(QApplication::translate("Page_Inst", "Mod env delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem25 = tableInst->verticalHeaderItem(24);
        ___qtablewidgetitem25->setText(QApplication::translate("Page_Inst", "Mod env attack (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem26 = tableInst->verticalHeaderItem(25);
        ___qtablewidgetitem26->setText(QApplication::translate("Page_Inst", "Mod env hold (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem27 = tableInst->verticalHeaderItem(26);
        ___qtablewidgetitem27->setText(QApplication::translate("Page_Inst", "Mod env decay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem28 = tableInst->verticalHeaderItem(27);
        ___qtablewidgetitem28->setText(QApplication::translate("Page_Inst", "Mod env sustain (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem29 = tableInst->verticalHeaderItem(28);
        ___qtablewidgetitem29->setText(QApplication::translate("Page_Inst", "Mod env release (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem30 = tableInst->verticalHeaderItem(29);
        ___qtablewidgetitem30->setText(QApplication::translate("Page_Inst", "Mod env \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem31 = tableInst->verticalHeaderItem(30);
        ___qtablewidgetitem31->setText(QApplication::translate("Page_Inst", "Mod env \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem32 = tableInst->verticalHeaderItem(31);
        ___qtablewidgetitem32->setText(QApplication::translate("Page_Inst", "Note \342\206\222 Mod env hold (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem33 = tableInst->verticalHeaderItem(32);
        ___qtablewidgetitem33->setText(QApplication::translate("Page_Inst", "Note \342\206\222 Mod env decay (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem34 = tableInst->verticalHeaderItem(33);
        ___qtablewidgetitem34->setText(QApplication::translate("Page_Inst", "Mod LFO delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem35 = tableInst->verticalHeaderItem(34);
        ___qtablewidgetitem35->setText(QApplication::translate("Page_Inst", "Mod LFO freq (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem36 = tableInst->verticalHeaderItem(35);
        ___qtablewidgetitem36->setText(QApplication::translate("Page_Inst", "Mod LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem37 = tableInst->verticalHeaderItem(36);
        ___qtablewidgetitem37->setText(QApplication::translate("Page_Inst", "Mod LFO \342\206\222 filtre (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem38 = tableInst->verticalHeaderItem(37);
        ___qtablewidgetitem38->setText(QApplication::translate("Page_Inst", "Mod LFO \342\206\222 volume (dB)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem39 = tableInst->verticalHeaderItem(38);
        ___qtablewidgetitem39->setText(QApplication::translate("Page_Inst", "Vib LFO delay (s)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem40 = tableInst->verticalHeaderItem(39);
        ___qtablewidgetitem40->setText(QApplication::translate("Page_Inst", "Vib LFO freq (Hz)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem41 = tableInst->verticalHeaderItem(40);
        ___qtablewidgetitem41->setText(QApplication::translate("Page_Inst", "Vib LFO \342\206\222 ton (c)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem42 = tableInst->verticalHeaderItem(41);
        ___qtablewidgetitem42->setText(QApplication::translate("Page_Inst", "Classe exclusive", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem43 = tableInst->verticalHeaderItem(42);
        ___qtablewidgetitem43->setText(QApplication::translate("Page_Inst", "Chorus (%)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem44 = tableInst->verticalHeaderItem(43);
        ___qtablewidgetitem44->setText(QApplication::translate("Page_Inst", "R\303\251verb\303\251ration (%)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem45 = tableInst->verticalHeaderItem(44);
        ___qtablewidgetitem45->setText(QApplication::translate("Page_Inst", "Note fixe", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem46 = tableInst->verticalHeaderItem(45);
        ___qtablewidgetitem46->setText(QApplication::translate("Page_Inst", "V\303\251locit\303\251 fixe", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem47 = tableInst->verticalHeaderItem(46);
        ___qtablewidgetitem47->setText(QApplication::translate("Page_Inst", "Offset d\303\251but sample", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem48 = tableInst->verticalHeaderItem(47);
        ___qtablewidgetitem48->setText(QApplication::translate("Page_Inst", "Offset fin sample", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem49 = tableInst->verticalHeaderItem(48);
        ___qtablewidgetitem49->setText(QApplication::translate("Page_Inst", "Offset d\303\251but boucle", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem50 = tableInst->verticalHeaderItem(49);
        ___qtablewidgetitem50->setText(QApplication::translate("Page_Inst", "Offset fin boucle", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = tableInst->isSortingEnabled();
        tableInst->setSortingEnabled(false);
        tableInst->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("Page_Inst", "Modulateurs", 0, QApplication::UnicodeUTF8));
        pushNouveauMod->setText(QString());
        pushSupprimerMod->setText(QString());
        QTableWidgetItem *___qtablewidgetitem51 = tableMod->horizontalHeaderItem(0);
        ___qtablewidgetitem51->setText(QApplication::translate("Page_Inst", "typeElement", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem52 = tableMod->horizontalHeaderItem(1);
        ___qtablewidgetitem52->setText(QApplication::translate("Page_Inst", "indexSf2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem53 = tableMod->horizontalHeaderItem(2);
        ___qtablewidgetitem53->setText(QApplication::translate("Page_Inst", "indexElt", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem54 = tableMod->horizontalHeaderItem(3);
        ___qtablewidgetitem54->setText(QApplication::translate("Page_Inst", "indexElt2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem55 = tableMod->horizontalHeaderItem(4);
        ___qtablewidgetitem55->setText(QApplication::translate("Page_Inst", "indexMod", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem56 = tableMod->horizontalHeaderItem(5);
        ___qtablewidgetitem56->setText(QApplication::translate("Page_Inst", "#", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem57 = tableMod->horizontalHeaderItem(6);
        ___qtablewidgetitem57->setText(QApplication::translate("Page_Inst", "Source", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem58 = tableMod->horizontalHeaderItem(7);
        ___qtablewidgetitem58->setText(QApplication::translate("Page_Inst", "Quantit\303\251", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem59 = tableMod->horizontalHeaderItem(8);
        ___qtablewidgetitem59->setText(QApplication::translate("Page_Inst", "Destination", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Page_Inst", "Source / Quantit\303\251", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Page_Inst", "Destination", 0, QApplication::UnicodeUTF8));
        checkAbs->setText(QApplication::translate("Page_Inst", "Valeur abs.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Page_Inst: public Ui_Page_Inst {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_INST_H
