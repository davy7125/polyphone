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
****************************************************************************/

#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Nom de l'application
    QCoreApplication::setApplicationName("Polyphone");
    QCoreApplication::setOrganizationName("polyphone");
    // Traduction si nécessaire
    QTranslator translator;
    translator.load(":/traductions/polyphone_en"); // Doit se placer avant QApplication
    QApplication a(argc, argv);
    QString locale = QLocale::system().name().section('_', 0, 0);
    if (locale.compare("fr") != 0)
        a.installTranslator(&translator);
    // Affichage fenêtre
    MainWindow w;
    w.show();
    // Centrage de la fenêtre (doit être après w.show)
    w.move((a.desktop()->width()  - w.size().width() ) / 2,
           (a.desktop()->height() - w.size().height()) / 2 - 20);
    // Ouverture des fichiers sf2 passés en argument
    QStringList listeArg = QCoreApplication::arguments();
    for (int i = 1; i < listeArg.size(); i++)
    {
        QFileInfo fileInfo(listeArg.at(i));
        QString suffix = fileInfo.suffix().toLower();
        if (!suffix.compare("sf2"))
            w.ouvrir(listeArg.at(i));
    }
    return a.exec();
}
