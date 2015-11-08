/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
#include <QFileInfo>
#include <QSettings>
#include "pile_sf2.h"
#include "conversion_sfz.h"
#include "import_sfz.h"
#include "sfarkextractor.h"
#include "options.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif
#include "mainwindow.h"
#include "translationsystem.h"

#ifdef Q_OS_MAC
#include "macapplication.h"
#endif

int launchApplication(Options &options, QApplication &a)
{
    // Nom de l'application
    a.setApplicationName("Polyphone");
    a.setOrganizationName("polyphone");
    TranslationSystem::translate(&a);

    // Affichage fenêtre
    MainWindow w;
    w.show();

    // Ouverture des fichiers passés en argument
    QStringList inputFiles = options.getInputFiles();
    int numSf2 = -1;
    foreach (QString file, inputFiles)
        w.dragAndDrop(file, EltID(elementUnknown, -1, -1, -1, -1), &numSf2);

#ifdef Q_OS_MACX
    QObject::connect(&a, SIGNAL(openFile(QString)), &w, SLOT(dragAndDrop(QString)));
#endif

    return a.exec();
}

// Error codes
// 1: input file does not exist, or output file already exists
// 2: bad extension
// 3: cannot open the input file (corrupted are not accessible)
// 4: cannot save the output file (internal problem or write access denied)
int convert(Options &options)
{
    QFileInfo inputFile(options.getInputFiles()[0]);
    QFileInfo outputFile(options.getOutputFileFullPath());
    if (!QDir(options.getOutputDirectory()).exists())
    {
        qWarning() << "The directory" << options.getOutputDirectory() << "does not exist.";
        return 1;
    }
    if (!inputFile.exists())
    {
        qWarning() << "The file" << inputFile.filePath() << "does not exist.";
        return 1;
    }
    if (outputFile.exists() && options.mode() != Options::MODE_CONVERSION_TO_SFZ)
    {
        qWarning() << "The file" << outputFile.filePath() << "already exists.";
        return 1;
    }

    // Load inputfile
    Pile_sf2 sf2(NULL, false);
    QString inputExtension = inputFile.suffix().toLower();
    qDebug() << "Loading file" << inputFile.filePath() << "...";
    if (inputExtension == "sf2" || inputExtension == "sf3")
    {
        if (sf2.open(inputFile.filePath()) > 0)
        {
            qWarning() << "fail";
            return 3;
        }
    }
    else if (inputExtension == "sfz")
    {
        ImportSfz importSfz(&sf2);
        int num = -1;
        qDebug() << "conversion sfz" <<  inputFile.filePath();
        importSfz.import(inputFile.filePath(), &num);
        qDebug() << "import ok" << num;
    }
    else if (inputExtension == "sfark")
    {
        bool ok = false;

        SfArkExtractor sfArkExtractor(inputFile.filePath().toStdString().c_str(), NULL);
        sfArkExtractor.extract();
        int size = 0;
        char * rawData = NULL;
        if (sfArkExtractor.getData(rawData, size))
        {
            QByteArray data(rawData, size);
            QDataStream streamSf2(&data, QIODevice::ReadOnly);
            int indexSf2 = -1;
            ok = (sf2.open("", &streamSf2, indexSf2, true) <= 0);
        }

        if (!ok)
        {
            qWarning() << "fail";
            return 3;
        }
    }
    else
    {
        qWarning() << "fail";
        return 2;
    }
    qWarning() << "done";

    // Save in outputfile
    switch (options.mode())
    {
    case Options::MODE_CONVERSION_TO_SF2: case Options::MODE_CONVERSION_TO_SF3:
        qDebug() << "Saving file" << outputFile.filePath() << "...";
        if (sf2.save(0, outputFile.filePath(), options.quality()) != 0)
        {
            qWarning() << "fail";
            return 4;
        }
        break;
    case Options::MODE_CONVERSION_TO_SFZ: {
        qDebug() << "Exporting in directory" << options.getOutputDirectory() << "...";

        // Preset number
        EltID id = EltID(elementPrst, 0, -1, 0, 0);
        int presetNumber = sf2.count(id);

        // Preset list
        QList<EltID> presets;
        for (int i = 0; i < presetNumber; i++)
        {
            EltID idTmp = id;
            idTmp.indexElt = i;
            presets << idTmp;
        }

        // Conversion sfz
        ConversionSfz conversionSfz(&sf2);
        conversionSfz.convert(QDir(options.getOutputDirectory()).absolutePath(), presets,
                              options.sfzPresetPrefix(), options.sfzOneDirPerBank(), options.sfzGeneralMidi());

    } break;
    default:
        qWarning() << "fail";
        return 2;
    }

    qWarning() << "done";
    return 0;
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_MACX
    MacApplication a(argc, argv);
#else
    QApplication a(argc, argv);
#endif

    Options options(argc, argv);

    int valRet = 0;

    if (options.error())
    {
        // Show usage
        qDebug() << "write \"man polyphone\" to show usage";
    }
    else
    {
        if (options.mode() == Options::MODE_GUI)
            valRet = launchApplication(options, a);
        else
            valRet = convert(options);
    }

    return valRet;
}
