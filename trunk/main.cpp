/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    // Traduction si nécessaire
    QTranslator translatorEN, translatorES;
    translatorEN.load(":/traductions/polyphone_en"); // Doit se placer avant QApplication
    translatorES.load(":/traductions/polyphone_es");
    QApplication a(argc, argv);

    // Nom de l'application
    a.setApplicationName("Polyphone");
    a.setOrganizationName("polyphone");
    QString locale = QLocale::system().name().section('_', 0, 0);
    /*if (locale.compare("es") == 0)
        a.installTranslator(&translatorES);
    else */if (locale.compare("fr") != 0)
        a.installTranslator(&translatorEN);

    // Affichage fenêtre
    MainWindow w;
    w.show();

    // Ouverture des fichiers sf2 passés en argument
    QStringList listeArg = QCoreApplication::arguments();
    int replace = 0;
    for (int i = 1; i < listeArg.size(); i++)
        w.dragAndDrop(listeArg.at(i), EltID(elementUnknown, -1, -1, -1, -1), &replace);

    return a.exec();
}
