/********************************************************************************
** Form generated from reading UI file 'dialog_help.ui'
**
** Created: Tue 5. Feb 21:43:56 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_HELP_H
#define UI_DIALOG_HELP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogHelp
{
public:
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *DialogHelp)
    {
        if (DialogHelp->objectName().isEmpty())
            DialogHelp->setObjectName(QString::fromUtf8("DialogHelp"));
        DialogHelp->resize(480, 524);
        DialogHelp->setMinimumSize(QSize(480, 0));
        DialogHelp->setMaximumSize(QSize(480, 16777215));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icones/icon"), QSize(), QIcon::Normal, QIcon::Off);
        DialogHelp->setWindowIcon(icon);
        DialogHelp->setModal(false);
        verticalLayout = new QVBoxLayout(DialogHelp);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textBrowser = new QTextBrowser(DialogHelp);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setPointSize(10);
        textBrowser->setFont(font);
        textBrowser->setAcceptDrops(false);
        textBrowser->setFrameShape(QFrame::NoFrame);

        verticalLayout->addWidget(textBrowser);


        retranslateUi(DialogHelp);

        QMetaObject::connectSlotsByName(DialogHelp);
    } // setupUi

    void retranslateUi(QDialog *DialogHelp)
    {
        DialogHelp->setWindowTitle(QApplication::translate("DialogHelp", "Aide", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogHelp: public Ui_DialogHelp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_HELP_H
