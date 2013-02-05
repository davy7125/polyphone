/********************************************************************************
** Form generated from reading UI file 'page_smpl.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAGE_SMPL_H
#define UI_PAGE_SMPL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "page_smpl.h"

QT_BEGIN_NAMESPACE

class Ui_Page_Smpl
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_6;
    QLabel *labelInst;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_5;
    Graphique *graphe;
    QScrollBar *sliderGraphe;
    QFrame *frame_5;
    QGridLayout *gridLayout;
    QLabel *label_22;
    QLabel *label_20;
    QLabel *label_21;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_8;
    QSlider *verticalSlider_1;
    QLabel *label_8;
    QVBoxLayout *verticalLayout_10;
    QSlider *verticalSlider_2;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_11;
    QSlider *verticalSlider_3;
    QLabel *label_10;
    QVBoxLayout *verticalLayout_12;
    QSlider *verticalSlider_4;
    QLabel *label_11;
    QVBoxLayout *verticalLayout_13;
    QSlider *verticalSlider_5;
    QLabel *label_12;
    QVBoxLayout *verticalLayout_14;
    QSlider *verticalSlider_6;
    QLabel *label_13;
    QVBoxLayout *verticalLayout_15;
    QSlider *verticalSlider_7;
    QLabel *label_14;
    QVBoxLayout *verticalLayout_16;
    QSlider *verticalSlider_8;
    QLabel *label_15;
    QVBoxLayout *verticalLayout_19;
    QSlider *verticalSlider_9;
    QLabel *label_16;
    QVBoxLayout *verticalLayout_17;
    QSlider *verticalSlider_10;
    QLabel *label_17;
    QFrame *frame_9;
    QVBoxLayout *verticalLayout_9;
    QSpacerItem *verticalSpacer_4;
    QLabel *label_18;
    QSpacerItem *verticalSpacer;
    QPushButton *pushEgalRestore;
    QPushButton *pushEgaliser;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_19;
    QSpacerItem *verticalSpacer_3;
    QVBoxLayout *layoutMilieu;
    QFrame *frame_6;
    QHBoxLayout *horizontalLayout_4;
    GraphiqueFourier *grapheFourier;
    QFrame *frame_10;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_18;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushLecture;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkLectureBoucle;
    QSlider *sliderLectureVolume;
    QVBoxLayout *verticalLayout_20;
    QCheckBox *checkLectureLien;
    QCheckBox *checkLectureSinus;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_5;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelTaille;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *spinStartLoop;
    QSpinBox *spinEndLoop;
    QSpinBox *spinRootKey;
    QSpinBox *spinTune;
    QComboBox *comboSampleRate;
    QComboBox *comboType;
    QComboBox *comboLink;

    void setupUi(QWidget *Page_Smpl)
    {
        if (Page_Smpl->objectName().isEmpty())
            Page_Smpl->setObjectName(QString::fromUtf8("Page_Smpl"));
        Page_Smpl->resize(983, 592);
        verticalLayout = new QVBoxLayout(Page_Smpl);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(6, 0, 6, 6);
        frame_4 = new QFrame(Page_Smpl);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_4->sizePolicy().hasHeightForWidth());
        frame_4->setSizePolicy(sizePolicy);
        frame_4->setMaximumSize(QSize(16777215, 20));
        frame_4->setFrameShape(QFrame::NoFrame);
        frame_4->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(frame_4);
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        labelInst = new QLabel(frame_4);
        labelInst->setObjectName(QString::fromUtf8("labelInst"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelInst->sizePolicy().hasHeightForWidth());
        labelInst->setSizePolicy(sizePolicy1);
        labelInst->setMinimumSize(QSize(0, 20));
        labelInst->setMaximumSize(QSize(16777215, 20));
        labelInst->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout_6->addWidget(labelInst);


        verticalLayout->addWidget(frame_4);

        frame_3 = new QFrame(Page_Smpl);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(frame_3);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        graphe = new Graphique(frame_3);
        graphe->setObjectName(QString::fromUtf8("graphe"));
        graphe->setMinimumSize(QSize(0, 280));

        verticalLayout_5->addWidget(graphe);

        sliderGraphe = new QScrollBar(frame_3);
        sliderGraphe->setObjectName(QString::fromUtf8("sliderGraphe"));
        sliderGraphe->setMaximum(1000);
        sliderGraphe->setOrientation(Qt::Horizontal);

        verticalLayout_5->addWidget(sliderGraphe);


        verticalLayout->addWidget(frame_3);

        frame_5 = new QFrame(Page_Smpl);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setFrameShape(QFrame::NoFrame);
        frame_5->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_5);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(4);
        gridLayout->setContentsMargins(0, 4, 0, 0);
        label_22 = new QLabel(frame_5);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(label_22, 0, 1, 1, 1);

        label_20 = new QLabel(frame_5);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setMinimumSize(QSize(0, 0));
        label_20->setMaximumSize(QSize(16777215, 16));

        gridLayout->addWidget(label_20, 0, 0, 1, 1);

        label_21 = new QLabel(frame_5);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(label_21, 0, 2, 1, 1);

        frame_7 = new QFrame(frame_5);
        frame_7->setObjectName(QString::fromUtf8("frame_7"));
        frame_7->setFrameShape(QFrame::Panel);
        frame_7->setFrameShadow(QFrame::Sunken);
        horizontalLayout_5 = new QHBoxLayout(frame_7);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(6, -1, 6, -1);
        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalSlider_1 = new QSlider(frame_7);
        verticalSlider_1->setObjectName(QString::fromUtf8("verticalSlider_1"));
        verticalSlider_1->setMinimum(-15);
        verticalSlider_1->setMaximum(15);
        verticalSlider_1->setPageStep(5);
        verticalSlider_1->setOrientation(Qt::Vertical);
        verticalSlider_1->setTickPosition(QSlider::TicksBelow);
        verticalSlider_1->setTickInterval(5);

        verticalLayout_8->addWidget(verticalSlider_1);

        label_8 = new QLabel(frame_7);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(label_8);


        horizontalLayout_5->addLayout(verticalLayout_8);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalSlider_2 = new QSlider(frame_7);
        verticalSlider_2->setObjectName(QString::fromUtf8("verticalSlider_2"));
        verticalSlider_2->setMinimum(-15);
        verticalSlider_2->setMaximum(15);
        verticalSlider_2->setPageStep(5);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setTickPosition(QSlider::TicksBelow);
        verticalSlider_2->setTickInterval(5);

        verticalLayout_10->addWidget(verticalSlider_2);

        label_9 = new QLabel(frame_7);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignCenter);

        verticalLayout_10->addWidget(label_9);


        horizontalLayout_5->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalSlider_3 = new QSlider(frame_7);
        verticalSlider_3->setObjectName(QString::fromUtf8("verticalSlider_3"));
        verticalSlider_3->setMinimum(-15);
        verticalSlider_3->setMaximum(15);
        verticalSlider_3->setPageStep(5);
        verticalSlider_3->setValue(0);
        verticalSlider_3->setOrientation(Qt::Vertical);
        verticalSlider_3->setTickPosition(QSlider::TicksBelow);
        verticalSlider_3->setTickInterval(5);

        verticalLayout_11->addWidget(verticalSlider_3);

        label_10 = new QLabel(frame_7);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAlignment(Qt::AlignCenter);

        verticalLayout_11->addWidget(label_10);


        horizontalLayout_5->addLayout(verticalLayout_11);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        verticalSlider_4 = new QSlider(frame_7);
        verticalSlider_4->setObjectName(QString::fromUtf8("verticalSlider_4"));
        verticalSlider_4->setMinimum(-15);
        verticalSlider_4->setMaximum(15);
        verticalSlider_4->setPageStep(5);
        verticalSlider_4->setOrientation(Qt::Vertical);
        verticalSlider_4->setTickPosition(QSlider::TicksBelow);
        verticalSlider_4->setTickInterval(5);

        verticalLayout_12->addWidget(verticalSlider_4);

        label_11 = new QLabel(frame_7);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(label_11);


        horizontalLayout_5->addLayout(verticalLayout_12);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        verticalSlider_5 = new QSlider(frame_7);
        verticalSlider_5->setObjectName(QString::fromUtf8("verticalSlider_5"));
        verticalSlider_5->setMinimum(-15);
        verticalSlider_5->setMaximum(15);
        verticalSlider_5->setPageStep(5);
        verticalSlider_5->setOrientation(Qt::Vertical);
        verticalSlider_5->setTickPosition(QSlider::TicksBelow);
        verticalSlider_5->setTickInterval(5);

        verticalLayout_13->addWidget(verticalSlider_5);

        label_12 = new QLabel(frame_7);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setAlignment(Qt::AlignCenter);

        verticalLayout_13->addWidget(label_12);


        horizontalLayout_5->addLayout(verticalLayout_13);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(0);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        verticalSlider_6 = new QSlider(frame_7);
        verticalSlider_6->setObjectName(QString::fromUtf8("verticalSlider_6"));
        verticalSlider_6->setMinimum(-15);
        verticalSlider_6->setMaximum(15);
        verticalSlider_6->setPageStep(5);
        verticalSlider_6->setOrientation(Qt::Vertical);
        verticalSlider_6->setTickPosition(QSlider::TicksBelow);
        verticalSlider_6->setTickInterval(5);

        verticalLayout_14->addWidget(verticalSlider_6);

        label_13 = new QLabel(frame_7);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setAlignment(Qt::AlignCenter);

        verticalLayout_14->addWidget(label_13);


        horizontalLayout_5->addLayout(verticalLayout_14);

        verticalLayout_15 = new QVBoxLayout();
        verticalLayout_15->setSpacing(0);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        verticalSlider_7 = new QSlider(frame_7);
        verticalSlider_7->setObjectName(QString::fromUtf8("verticalSlider_7"));
        verticalSlider_7->setMinimum(-15);
        verticalSlider_7->setMaximum(15);
        verticalSlider_7->setPageStep(5);
        verticalSlider_7->setOrientation(Qt::Vertical);
        verticalSlider_7->setTickPosition(QSlider::TicksBelow);
        verticalSlider_7->setTickInterval(5);

        verticalLayout_15->addWidget(verticalSlider_7);

        label_14 = new QLabel(frame_7);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setAlignment(Qt::AlignCenter);

        verticalLayout_15->addWidget(label_14);


        horizontalLayout_5->addLayout(verticalLayout_15);

        verticalLayout_16 = new QVBoxLayout();
        verticalLayout_16->setSpacing(0);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        verticalSlider_8 = new QSlider(frame_7);
        verticalSlider_8->setObjectName(QString::fromUtf8("verticalSlider_8"));
        verticalSlider_8->setMinimum(-15);
        verticalSlider_8->setMaximum(15);
        verticalSlider_8->setPageStep(5);
        verticalSlider_8->setOrientation(Qt::Vertical);
        verticalSlider_8->setTickPosition(QSlider::TicksBelow);
        verticalSlider_8->setTickInterval(5);

        verticalLayout_16->addWidget(verticalSlider_8);

        label_15 = new QLabel(frame_7);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setAlignment(Qt::AlignCenter);

        verticalLayout_16->addWidget(label_15);


        horizontalLayout_5->addLayout(verticalLayout_16);

        verticalLayout_19 = new QVBoxLayout();
        verticalLayout_19->setSpacing(0);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        verticalSlider_9 = new QSlider(frame_7);
        verticalSlider_9->setObjectName(QString::fromUtf8("verticalSlider_9"));
        verticalSlider_9->setMinimum(-15);
        verticalSlider_9->setMaximum(15);
        verticalSlider_9->setPageStep(5);
        verticalSlider_9->setOrientation(Qt::Vertical);
        verticalSlider_9->setTickPosition(QSlider::TicksBelow);
        verticalSlider_9->setTickInterval(5);

        verticalLayout_19->addWidget(verticalSlider_9);

        label_16 = new QLabel(frame_7);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setAlignment(Qt::AlignCenter);

        verticalLayout_19->addWidget(label_16);


        horizontalLayout_5->addLayout(verticalLayout_19);

        verticalLayout_17 = new QVBoxLayout();
        verticalLayout_17->setSpacing(0);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        verticalSlider_10 = new QSlider(frame_7);
        verticalSlider_10->setObjectName(QString::fromUtf8("verticalSlider_10"));
        verticalSlider_10->setMinimum(-15);
        verticalSlider_10->setMaximum(15);
        verticalSlider_10->setPageStep(5);
        verticalSlider_10->setOrientation(Qt::Vertical);
        verticalSlider_10->setTickPosition(QSlider::TicksBelow);
        verticalSlider_10->setTickInterval(5);

        verticalLayout_17->addWidget(verticalSlider_10);

        label_17 = new QLabel(frame_7);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setAlignment(Qt::AlignCenter);

        verticalLayout_17->addWidget(label_17);


        horizontalLayout_5->addLayout(verticalLayout_17);

        frame_9 = new QFrame(frame_7);
        frame_9->setObjectName(QString::fromUtf8("frame_9"));
        frame_9->setMaximumSize(QSize(80, 16777215));
        frame_9->setFrameShape(QFrame::NoFrame);
        frame_9->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(frame_9);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(3, 0, 0, 0);
        verticalSpacer_4 = new QSpacerItem(20, 4, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_9->addItem(verticalSpacer_4);

        label_18 = new QLabel(frame_9);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        verticalLayout_9->addWidget(label_18);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer);

        pushEgalRestore = new QPushButton(frame_9);
        pushEgalRestore->setObjectName(QString::fromUtf8("pushEgalRestore"));

        verticalLayout_9->addWidget(pushEgalRestore);

        pushEgaliser = new QPushButton(frame_9);
        pushEgaliser->setObjectName(QString::fromUtf8("pushEgaliser"));
        pushEgaliser->setMaximumSize(QSize(80, 16777215));

        verticalLayout_9->addWidget(pushEgaliser);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_2);

        label_19 = new QLabel(frame_9);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        verticalLayout_9->addWidget(label_19);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_9->addItem(verticalSpacer_3);


        horizontalLayout_5->addWidget(frame_9);


        gridLayout->addWidget(frame_7, 1, 2, 1, 1);

        layoutMilieu = new QVBoxLayout();
        layoutMilieu->setSpacing(0);
        layoutMilieu->setObjectName(QString::fromUtf8("layoutMilieu"));
        frame_6 = new QFrame(frame_5);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setFrameShape(QFrame::Panel);
        frame_6->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(frame_6);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        grapheFourier = new GraphiqueFourier(frame_6);
        grapheFourier->setObjectName(QString::fromUtf8("grapheFourier"));

        horizontalLayout_4->addWidget(grapheFourier);


        layoutMilieu->addWidget(frame_6);

        frame_10 = new QFrame(frame_5);
        frame_10->setObjectName(QString::fromUtf8("frame_10"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frame_10->sizePolicy().hasHeightForWidth());
        frame_10->setSizePolicy(sizePolicy2);
        frame_10->setFrameShape(QFrame::NoFrame);
        frame_10->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(frame_10);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(-1, 1, -1, 1);
        verticalLayout_18 = new QVBoxLayout();
        verticalLayout_18->setSpacing(0);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushLecture = new QPushButton(frame_10);
        pushLecture->setObjectName(QString::fromUtf8("pushLecture"));
        pushLecture->setCheckable(true);

        horizontalLayout_7->addWidget(pushLecture);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        checkLectureBoucle = new QCheckBox(frame_10);
        checkLectureBoucle->setObjectName(QString::fromUtf8("checkLectureBoucle"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(checkLectureBoucle->sizePolicy().hasHeightForWidth());
        checkLectureBoucle->setSizePolicy(sizePolicy3);

        horizontalLayout_7->addWidget(checkLectureBoucle);


        verticalLayout_18->addLayout(horizontalLayout_7);

        sliderLectureVolume = new QSlider(frame_10);
        sliderLectureVolume->setObjectName(QString::fromUtf8("sliderLectureVolume"));
        sliderLectureVolume->setMinimum(-20);
        sliderLectureVolume->setMaximum(80);
        sliderLectureVolume->setValue(50);
        sliderLectureVolume->setOrientation(Qt::Horizontal);
        sliderLectureVolume->setTickPosition(QSlider::TicksBelow);
        sliderLectureVolume->setTickInterval(10);

        verticalLayout_18->addWidget(sliderLectureVolume);


        horizontalLayout_8->addLayout(verticalLayout_18);

        verticalLayout_20 = new QVBoxLayout();
        verticalLayout_20->setSpacing(0);
        verticalLayout_20->setObjectName(QString::fromUtf8("verticalLayout_20"));
        checkLectureLien = new QCheckBox(frame_10);
        checkLectureLien->setObjectName(QString::fromUtf8("checkLectureLien"));

        verticalLayout_20->addWidget(checkLectureLien);

        checkLectureSinus = new QCheckBox(frame_10);
        checkLectureSinus->setObjectName(QString::fromUtf8("checkLectureSinus"));

        verticalLayout_20->addWidget(checkLectureSinus);


        horizontalLayout_8->addLayout(verticalLayout_20);


        layoutMilieu->addWidget(frame_10);


        gridLayout->addLayout(layoutMilieu, 1, 1, 1, 1);

        frame_2 = new QFrame(frame_5);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(300, 16777215));
        frame_2->setFrameShape(QFrame::Panel);
        frame_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(frame_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(-1, -1, -1, 0);
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_4->addWidget(label);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_4->addWidget(label_2);

        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        label_4 = new QLabel(frame_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_4->addWidget(label_4);

        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_4->addWidget(label_6);

        label_7 = new QLabel(frame_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_4->addWidget(label_7);

        label_5 = new QLabel(frame_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_4->addWidget(label_5);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, -1, -1, 0);
        labelTaille = new QLabel(frame_2);
        labelTaille->setObjectName(QString::fromUtf8("labelTaille"));
        labelTaille->setMinimumSize(QSize(156, 20));
        labelTaille->setMaximumSize(QSize(156, 20));

        verticalLayout_2->addWidget(labelTaille);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        spinStartLoop = new QSpinBox(frame_2);
        spinStartLoop->setObjectName(QString::fromUtf8("spinStartLoop"));
        spinStartLoop->setMinimumSize(QSize(75, 0));
        spinStartLoop->setMaximumSize(QSize(75, 16777215));

        horizontalLayout_3->addWidget(spinStartLoop);

        spinEndLoop = new QSpinBox(frame_2);
        spinEndLoop->setObjectName(QString::fromUtf8("spinEndLoop"));
        spinEndLoop->setMinimumSize(QSize(75, 0));
        spinEndLoop->setMaximumSize(QSize(75, 16777215));

        horizontalLayout_3->addWidget(spinEndLoop);


        verticalLayout_2->addLayout(horizontalLayout_3);

        spinRootKey = new QSpinBox(frame_2);
        spinRootKey->setObjectName(QString::fromUtf8("spinRootKey"));
        spinRootKey->setMinimumSize(QSize(156, 0));
        spinRootKey->setMaximumSize(QSize(154, 16777215));
        spinRootKey->setMaximum(127);

        verticalLayout_2->addWidget(spinRootKey);

        spinTune = new QSpinBox(frame_2);
        spinTune->setObjectName(QString::fromUtf8("spinTune"));
        spinTune->setMinimumSize(QSize(156, 0));
        spinTune->setMaximumSize(QSize(156, 16777215));
        spinTune->setMinimum(-100);
        spinTune->setMaximum(100);

        verticalLayout_2->addWidget(spinTune);

        comboSampleRate = new QComboBox(frame_2);
        comboSampleRate->setObjectName(QString::fromUtf8("comboSampleRate"));
        comboSampleRate->setMinimumSize(QSize(156, 0));
        comboSampleRate->setMaximumSize(QSize(156, 16777215));

        verticalLayout_2->addWidget(comboSampleRate);

        comboType = new QComboBox(frame_2);
        comboType->setObjectName(QString::fromUtf8("comboType"));
        comboType->setMinimumSize(QSize(156, 0));
        comboType->setMaximumSize(QSize(156, 16777215));

        verticalLayout_2->addWidget(comboType);

        comboLink = new QComboBox(frame_2);
        comboLink->setObjectName(QString::fromUtf8("comboLink"));
        comboLink->setMinimumSize(QSize(156, 0));
        comboLink->setMaximumSize(QSize(156, 16777215));

        verticalLayout_2->addWidget(comboLink);


        horizontalLayout_2->addLayout(verticalLayout_2);


        gridLayout->addWidget(frame_2, 1, 0, 1, 1);


        verticalLayout->addWidget(frame_5);


        retranslateUi(Page_Smpl);
        QObject::connect(pushLecture, SIGNAL(clicked()), Page_Smpl, SLOT(lecture()));
        QObject::connect(sliderGraphe, SIGNAL(valueChanged(int)), graphe, SLOT(setPosX(int)));
        QObject::connect(graphe, SIGNAL(startLoopChanged()), Page_Smpl, SLOT(setStartLoop()));
        QObject::connect(graphe, SIGNAL(endLoopChanged()), Page_Smpl, SLOT(setEndLoop()));
        QObject::connect(pushEgalRestore, SIGNAL(clicked()), Page_Smpl, SLOT(initEQ()));
        QObject::connect(pushEgaliser, SIGNAL(clicked()), Page_Smpl, SLOT(applyEQ()));
        QObject::connect(comboSampleRate, SIGNAL(currentIndexChanged(int)), Page_Smpl, SLOT(setRate(int)));
        QObject::connect(spinEndLoop, SIGNAL(editingFinished()), Page_Smpl, SLOT(setEndLoop()));
        QObject::connect(spinStartLoop, SIGNAL(editingFinished()), Page_Smpl, SLOT(setStartLoop()));
        QObject::connect(spinStartLoop, SIGNAL(valueChanged(int)), graphe, SLOT(setStartLoop(int)));
        QObject::connect(spinTune, SIGNAL(editingFinished()), Page_Smpl, SLOT(setTune()));
        QObject::connect(spinRootKey, SIGNAL(editingFinished()), Page_Smpl, SLOT(setRootKey()));
        QObject::connect(spinEndLoop, SIGNAL(valueChanged(int)), graphe, SLOT(setEndLoop(int)));
        QObject::connect(comboType, SIGNAL(currentIndexChanged(int)), Page_Smpl, SLOT(setType(int)));
        QObject::connect(comboLink, SIGNAL(currentIndexChanged(int)), Page_Smpl, SLOT(setLinkedSmpl(int)));
        QObject::connect(checkLectureBoucle, SIGNAL(stateChanged(int)), Page_Smpl, SLOT(setLoopEnabled(int)));
        QObject::connect(checkLectureLien, SIGNAL(stateChanged(int)), Page_Smpl, SLOT(setStereo(int)));
        QObject::connect(checkLectureSinus, SIGNAL(stateChanged(int)), Page_Smpl, SLOT(setSinusEnabled(int)));
        QObject::connect(sliderLectureVolume, SIGNAL(valueChanged(int)), Page_Smpl, SLOT(setGainSample(int)));
        QObject::connect(spinStartLoop, SIGNAL(valueChanged(int)), Page_Smpl, SLOT(setStartLoop(int)));
        QObject::connect(spinEndLoop, SIGNAL(valueChanged(int)), Page_Smpl, SLOT(setEndLoop(int)));
        QObject::connect(spinRootKey, SIGNAL(valueChanged(int)), Page_Smpl, SLOT(setRootKey(int)));
        QObject::connect(spinTune, SIGNAL(valueChanged(int)), Page_Smpl, SLOT(setTune(int)));

        QMetaObject::connectSlotsByName(Page_Smpl);
    } // setupUi

    void retranslateUi(QWidget *Page_Smpl)
    {
        Page_Smpl->setWindowTitle(QApplication::translate("Page_Smpl", "Form", 0, QApplication::UnicodeUTF8));
        labelInst->setText(QApplication::translate("Page_Smpl", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("Page_Smpl", "<b>Fr\303\251quences</b>", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("Page_Smpl", "<html><head/><body><p><span style=\" font-weight:600;\">Informations</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("Page_Smpl", "<b>Egaliseur</b>", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Page_Smpl", "32", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Page_Smpl", "64", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Page_Smpl", "125", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Page_Smpl", "250", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Page_Smpl", "500", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("Page_Smpl", "1k", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("Page_Smpl", "2k", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("Page_Smpl", "4k", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("Page_Smpl", "8k", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("Page_Smpl", "16k", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("Page_Smpl", " +15dB", 0, QApplication::UnicodeUTF8));
        pushEgalRestore->setText(QApplication::translate("Page_Smpl", "R\303\251initialiser", 0, QApplication::UnicodeUTF8));
        pushEgaliser->setText(QApplication::translate("Page_Smpl", "Appliquer", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("Page_Smpl", " -15dB", 0, QApplication::UnicodeUTF8));
        pushLecture->setText(QApplication::translate("Page_Smpl", "Lecture", 0, QApplication::UnicodeUTF8));
        checkLectureBoucle->setText(QApplication::translate("Page_Smpl", "en boucle", 0, QApplication::UnicodeUTF8));
        checkLectureLien->setText(QApplication::translate("Page_Smpl", "st\303\251r\303\251o", 0, QApplication::UnicodeUTF8));
        checkLectureSinus->setText(QApplication::translate("Page_Smpl", "sinus", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Page_Smpl", "Taille", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Page_Smpl", "Boucle", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Page_Smpl", "Note de base", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Page_Smpl", "Correction (centi\303\250me)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Page_Smpl", "Echantillonnage", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Page_Smpl", "Type", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Page_Smpl", "Lien", 0, QApplication::UnicodeUTF8));
        labelTaille->setText(QApplication::translate("Page_Smpl", "TextLabel", 0, QApplication::UnicodeUTF8));
        comboSampleRate->clear();
        comboSampleRate->insertItems(0, QStringList()
         << QApplication::translate("Page_Smpl", "192000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "96000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "48000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "44100", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "32000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "22050", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Page_Smpl", "11025", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class Page_Smpl: public Ui_Page_Smpl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PAGE_SMPL_H
