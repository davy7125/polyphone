/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include <QApplication>
#include <QFileInfo>
#include <QStyleFactory>
#include <QItemSelection>
#include <QDebug>
#include "soundfontmanager.h"
#include "inputfactory.h"
#include "abstractinput.h"
#include "outputfactory.h"
#include "abstractoutput.h"
#include "options.h"
#include "contextmanager.h"
#include "utils.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif
#include "mainwindow.h"
#include "translationmanager.h"
#include <QDir>

#ifdef Q_OS_MAC
#include "macapplication.h"
#endif

int launchApplication(Options &options)
{
    // Application name
    QApplication::setApplicationName("Polyphone");
    QApplication::setOrganizationName("polyphone");
    ContextManager::translation()->translate();

    // Application style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    try {
        qApp->setPalette(ContextManager::theme()->getPalette());
    } catch (...) { /* bug with mac */ }

    // Additional type used in signals
    qRegisterMetaType<EltID>("EltID");

    // Display the main window
    MainWindow w;
    w.show();

    // Open files passed as argument
    QStringList inputFiles = options.getInputFiles();
    foreach (QString file, inputFiles)
        w.openFile(file);

#ifdef Q_OS_MAC
    QObject::connect(qApp, SIGNAL(openFile(QString)), &w, SLOT(openFile(QString)));
#endif

    return QApplication::exec();
}

/// Error codes
/// 1: input file does not exist, or output file already exists
/// 2: bad extension
/// 3: cannot open the input file (corrupted are not accessible)
/// 4: cannot save the output file (internal problem or write access denied)
int convert(Options &options)
{
    // Check the input file
    QFileInfo inputFile(options.getInputFiles()[0]);
    if (!inputFile.exists())
    {
        qWarning() << "The file" << inputFile.filePath() << "does not exist.";
        return 1;
    }

    // Check the output
    QFileInfo outputFile(options.getOutputFileFullPath());
    if (!QDir(options.getOutputDirectory()).exists())
    {
        qWarning() << "The directory" << options.getOutputDirectory() << "does not exist.";
        return 1;
    }
    if (outputFile.exists() && options.mode() != Options::MODE_CONVERSION_TO_SFZ)
    {
        qWarning() << "The file" << outputFile.filePath() << "already exists.";
        return 1;
    }

    // Load input file
    qInfo() << "Loading file" << inputFile.filePath() << "...";
    AbstractInput * input = InputFactory::getInput(inputFile.filePath());
    input->process(false);
    if (!input->isSuccess())
    {
        qWarning() << "Couldn't load" << inputFile.filePath() + ":" << input->getError();
        delete input;
        return 1;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    qInfo() << "File loaded";

    // Prepare the output with the respective options
    AbstractOutput * output = OutputFactory::getOutput(outputFile.filePath());
    switch (options.mode())
    {
    case Options::MODE_CONVERSION_TO_SF2:
        qInfo() << "Saving file" << outputFile.filePath() << "...";
        break;
    case Options::MODE_CONVERSION_TO_SF3:
        output->setOption("quality", options.quality());
        qInfo() << "Saving file" << outputFile.filePath() << "...";
        break;
    case Options::MODE_CONVERSION_TO_SFZ: {
        output->setOption("prefix", options.sfzPresetPrefix());
        output->setOption("bankdir", options.sfzOneDirPerBank());
        output->setOption("gmsort", options.sfzGeneralMidi());
        qInfo() << "Exporting in directory" << options.getOutputDirectory() << "...";
    } break;
    default:
        qWarning() << "fail";
        return 1;
    }

    // Convert
    output->process(sf2Index, false);
    if (!output->isSuccess())
    {
        qWarning() << "Couldn't create" << outputFile.filePath() + ":" << output->getError();
        delete output;
        return 1;
    }
    delete output;
    qInfo() << "done";

    // Destroy a singleton that has been silently created
    SoundfontManager::kill();
    return 0;
}

int resetConfig(Options &options)
{
    Q_UNUSED(options)

    QApplication::setApplicationName("Polyphone");
    QApplication::setOrganizationName("polyphone");
    QSettings settings;
    settings.clear();
    qInfo() << "Previous configuration is now cleared.";

    return 0;
}

int displayHelp(Options &options)
{
    Q_UNUSED(options)
    qInfo() << "write \"man polyphone\" to show usage";
    return 0;
}

int main(int argc, char *argv[])
{
    // Prior to everything
    Utils::prepareConversionTables();

#ifdef Q_OS_MAC
    MacApplication a(argc, argv);
#else
    QApplication a(argc, argv);
#endif
    Options options(argc, argv);
    int valRet = 0;

    if (options.error() || options.help())
        valRet = displayHelp(options);
    else if (options.mode() == Options::MODE_GUI)
        valRet = launchApplication(options);
    else if (options.mode() == Options::MODE_RESET_CONFIG)
        valRet = resetConfig(options);
    else
        valRet = convert(options);

    return valRet;
}
