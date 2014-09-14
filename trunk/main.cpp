/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#include <QApplication>
#include <QSettings>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif
#include "mainwindow.h"

#ifdef Q_OS_MAC
#include "macapplication.h"
#endif

int main(int argc, char *argv[])
{
    // Traduction si nécessaire
    QTranslator translatorEN, translatorES, translatorIT;
    translatorEN.load(":/traductions/polyphone_en"); // Should stand before QApplication
    translatorES.load(":/traductions/polyphone_es");
    translatorIT.load(":/traductions/polyphone_it");

#ifdef Q_OS_MACX
    QStringList listPathMac;
    MacApplication a(argc, argv, &listPathMac);
#else
    QApplication a(argc, argv);
#endif

    // Nom de l'application
    a.setApplicationName("Polyphone");
    a.setOrganizationName("polyphone");
    QString locale = QLocale::system().name().section('_', 0, 0);
    QSettings settings;
    locale = settings.value("language", locale).toString();
    if (locale.compare("es") == 0)
        a.installTranslator(&translatorES);
    else if (locale.compare("it") == 0)
        a.installTranslator(&translatorIT);
    else if (locale.compare("fr") != 0)
        a.installTranslator(&translatorEN);

    // Affichage fenêtre
    MainWindow w;
    w.show();

    // Ouverture des fichiers passés en argument
    QStringList listeArg = QCoreApplication::arguments();
    int numSf2 = -1;
    for (int i = 1; i < listeArg.size(); i++)
    {
        QString extension = QFileInfo(listeArg.at(i)).suffix().toLower();
        if (extension == "sf2" || extension == "sfark" || extension == "sfz")
            w.dragAndDrop(listeArg.at(i), EltID(elementUnknown, -1, -1, -1, -1), &numSf2);
    }

#ifdef Q_OS_MACX
    for (int i = 0; i < listPathMac.size(); i++)
    {
        QString extension = QFileInfo(listPathMac.at(i)).suffix().toLower();
        if (extension == "sf2" || extension == "sfark" || extension == "sfz")
            w.dragAndDrop(listPathMac.at(i), EltID(elementUnknown, -1, -1, -1, -1), &numSf2);
    }
    a.stopAppending();
    QObject::connect(&a, SIGNAL(openFile(QString)), &w, SLOT(dragAndDrop(QString)));
#endif

    return a.exec();
}
