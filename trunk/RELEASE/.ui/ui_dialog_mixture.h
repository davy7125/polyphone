/********************************************************************************
** Form generated from reading UI file 'dialog_mixture.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_MIXTURE_H
#define UI_DIALOG_MIXTURE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogMixture
{
public:
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLineEdit *lineNom;
    QLabel *label_4;
    QComboBox *comboFreq;
    QCheckBox *checkBouclage;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QListWidget *listDivisions;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushAddDiv;
    QPushButton *pushRemoveDiv;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer_3;
    QLabel *label;
    QSpinBox *spinDivStart;
    QSpinBox *spinDivEnd;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listRangs;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushAddRank;
    QPushButton *pushRemoveRank;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_4;
    QLabel *label_2;
    QComboBox *comboType1;
    QComboBox *comboType2;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogMixture)
    {
        if (DialogMixture->objectName().isEmpty())
            DialogMixture->setObjectName(QString::fromUtf8("DialogMixture"));
        DialogMixture->setWindowModality(Qt::ApplicationModal);
        DialogMixture->resize(520, 260);
        DialogMixture->setMinimumSize(QSize(0, 260));
        DialogMixture->setMaximumSize(QSize(16777215, 260));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogMixture->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(DialogMixture);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(-1, 0, -1, -1);
        label_3 = new QLabel(DialogMixture);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        lineNom = new QLineEdit(DialogMixture);
        lineNom->setObjectName(QString::fromUtf8("lineNom"));

        gridLayout->addWidget(lineNom, 0, 1, 1, 1);

        label_4 = new QLabel(DialogMixture);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        comboFreq = new QComboBox(DialogMixture);
        comboFreq->setObjectName(QString::fromUtf8("comboFreq"));

        gridLayout->addWidget(comboFreq, 1, 1, 1, 1);

        checkBouclage = new QCheckBox(DialogMixture);
        checkBouclage->setObjectName(QString::fromUtf8("checkBouclage"));
        checkBouclage->setChecked(true);

        gridLayout->addWidget(checkBouclage, 1, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, -1, -1, 0);
        groupBox = new QGroupBox(DialogMixture);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 9, 0, 0);
        listDivisions = new QListWidget(groupBox);
        listDivisions->setObjectName(QString::fromUtf8("listDivisions"));
        listDivisions->setMinimumSize(QSize(150, 0));
        listDivisions->setEditTriggers(QAbstractItemView::NoEditTriggers);

        horizontalLayout->addWidget(listDivisions);

        frame = new QFrame(groupBox);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMaximumSize(QSize(100, 16777215));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, -1, 0, -1);
        pushAddDiv = new QPushButton(frame);
        pushAddDiv->setObjectName(QString::fromUtf8("pushAddDiv"));
        pushAddDiv->setMaximumSize(QSize(24, 24));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icones/add"), QSize(), QIcon::Normal, QIcon::Off);
        pushAddDiv->setIcon(icon1);

        horizontalLayout_3->addWidget(pushAddDiv);

        pushRemoveDiv = new QPushButton(frame);
        pushRemoveDiv->setObjectName(QString::fromUtf8("pushRemoveDiv"));
        pushRemoveDiv->setMaximumSize(QSize(24, 24));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icones/remove"), QSize(), QIcon::Normal, QIcon::Off);
        pushRemoveDiv->setIcon(icon2);

        horizontalLayout_3->addWidget(pushRemoveDiv);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_3);

        verticalSpacer_3 = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_3->addItem(verticalSpacer_3);

        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        spinDivStart = new QSpinBox(frame);
        spinDivStart->setObjectName(QString::fromUtf8("spinDivStart"));
        spinDivStart->setMaximum(128);
        spinDivStart->setValue(36);

        verticalLayout_3->addWidget(spinDivStart);

        spinDivEnd = new QSpinBox(frame);
        spinDivEnd->setObjectName(QString::fromUtf8("spinDivEnd"));
        spinDivEnd->setMaximum(128);
        spinDivEnd->setValue(96);

        verticalLayout_3->addWidget(spinDivEnd);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout->addWidget(frame);


        horizontalLayout_5->addWidget(groupBox);

        groupBox_2 = new QGroupBox(DialogMixture);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 9, 0, 0);
        listRangs = new QListWidget(groupBox_2);
        listRangs->setObjectName(QString::fromUtf8("listRangs"));
        listRangs->setMinimumSize(QSize(150, 0));
        listRangs->setEditTriggers(QAbstractItemView::NoEditTriggers);

        horizontalLayout_2->addWidget(listRangs);

        frame_2 = new QFrame(groupBox_2);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(100, 16777215));
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame_2);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, -1, 0, -1);
        pushAddRank = new QPushButton(frame_2);
        pushAddRank->setObjectName(QString::fromUtf8("pushAddRank"));
        pushAddRank->setMaximumSize(QSize(24, 24));
        pushAddRank->setIcon(icon1);

        horizontalLayout_4->addWidget(pushAddRank);

        pushRemoveRank = new QPushButton(frame_2);
        pushRemoveRank->setObjectName(QString::fromUtf8("pushRemoveRank"));
        pushRemoveRank->setMaximumSize(QSize(24, 24));
        pushRemoveRank->setIcon(icon2);

        horizontalLayout_4->addWidget(pushRemoveRank);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer_4 = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_4);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        comboType1 = new QComboBox(frame_2);
        comboType1->setObjectName(QString::fromUtf8("comboType1"));

        verticalLayout->addWidget(comboType1);

        comboType2 = new QComboBox(frame_2);
        comboType2->setObjectName(QString::fromUtf8("comboType2"));

        verticalLayout->addWidget(comboType2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout_2->addWidget(frame_2);


        horizontalLayout_5->addWidget(groupBox_2);


        verticalLayout_2->addLayout(horizontalLayout_5);

        buttonBox = new QDialogButtonBox(DialogMixture);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(DialogMixture);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogMixture, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogMixture, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogMixture);
    } // setupUi

    void retranslateUi(QDialog *DialogMixture)
    {
        DialogMixture->setWindowTitle(QApplication::translate("DialogMixture", "Cr\303\251ation mixture", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DialogMixture", "Nom de la mixture :", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DialogMixture", "Cr\303\251ation de son :", 0, QApplication::UnicodeUTF8));
        comboFreq->clear();
        comboFreq->insertItems(0, QStringList()
         << QApplication::translate("DialogMixture", "\303\240 chaque note", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "toutes les 3 notes", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "toutes les 6 notes", 0, QApplication::UnicodeUTF8)
        );
        checkBouclage->setText(QApplication::translate("DialogMixture", "Bouclage", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DialogMixture", "Divisions", 0, QApplication::UnicodeUTF8));
        pushAddDiv->setText(QString());
        pushRemoveDiv->setText(QString());
        label->setText(QApplication::translate("DialogMixture", "Etendue", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("DialogMixture", "Rangs", 0, QApplication::UnicodeUTF8));
        pushAddRank->setText(QString());
        pushRemoveRank->setText(QString());
        label_2->setText(QApplication::translate("DialogMixture", "Type", 0, QApplication::UnicodeUTF8));
        comboType1->clear();
        comboType1->insertItems(0, QStringList()
         << QApplication::translate("DialogMixture", "octave", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "quinte", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "tierce", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "septi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "neuvi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "onzi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "treizi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "quinzi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "dix-septi\303\250me", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DialogMixture", "dix-neuvi\303\250me", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class DialogMixture: public Ui_DialogMixture {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_MIXTURE_H
