/********************************************************************************
** Form generated from reading UI file 'dialog_list.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_LIST_H
#define UI_DIALOG_LIST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogList
{
public:
    QGridLayout *gridLayout;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogList)
    {
        if (DialogList->objectName().isEmpty())
            DialogList->setObjectName(QString::fromUtf8("DialogList"));
        DialogList->resize(240, 320);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogList->setWindowIcon(icon);
        gridLayout = new QGridLayout(DialogList);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        listWidget = new QListWidget(DialogList);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DialogList);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(DialogList);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogList, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogList, SLOT(reject()));
        QObject::connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), DialogList, SLOT(accept()));

        QMetaObject::connectSlotsByName(DialogList);
    } // setupUi

    void retranslateUi(QDialog *DialogList)
    {
        Q_UNUSED(DialogList);
    } // retranslateUi

};

namespace Ui {
    class DialogList: public Ui_DialogList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_LIST_H
