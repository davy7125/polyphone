/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include <QFileInfo>
#include <QStyleFactory>
#include <QItemSelection>
#include <QDebug>
#include "soundfontmanager.h"
#include "inputfactory.h"
#include "abstractinputparser.h"
#include "outputfactory.h"
#include "abstractoutput.h"
#include "options.h"
#include "contextmanager.h"
#include "utils.h"
#include "qtsingleapplication.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDesktopWidget>
#endif

#include "mainwindow.h"
#include "translationmanager.h"
#include <QDir>

#ifdef _WIN32
#include <Windows.h>
#endif

void writeLine(QString line)
{
    QTextStream out(stdout);
    out << line << Qt::endl;
}

int launchApplication(QtSingleApplication * app, Options &options)
{
    // Application langage
    ContextManager::translation()->translate();

    // Application style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    try {
        app->setPalette(ContextManager::theme()->getPalette());
    } catch (...) { /* bug with mac */ }

    // Additional type used in signals
    qRegisterMetaType<EltID>();
    qRegisterMetaType<QList<int> >();

    // Display the main window
    MainWindow w;
    app->setActivationWindow(&w, true);
    QObject::connect(app, SIGNAL(messageReceived(const QString&)), &w, SLOT(openFiles(const QString&)));
    w.show();

    // Open files passed as argument
    w.openFiles(options.getInputFiles().join('|'));

    return app->exec();
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
        writeLine("The file " + inputFile.filePath() + " does not exist.");
        return 1;
    }

    // Check the output
    QFileInfo outputFile(options.getOutputFileFullPath());
    if (!QDir(options.getOutputDirectory()).exists())
    {
        writeLine("The directory " + options.getOutputDirectory() + " does not exist.");
        return 1;
    }
    if (outputFile.exists() && options.mode() != Options::MODE_CONVERSION_TO_SFZ)
    {
        writeLine("The file "  + outputFile.filePath() + " already exists.");
        return 1;
    }

    // Load input file
    writeLine("Loading file " + inputFile.filePath() + "...");
    AbstractInputParser * input = InputFactory::getInput(inputFile.filePath());
    input->process(false);
    if (!input->isSuccess())
    {
        writeLine("Couldn't load " + inputFile.filePath() + ": " + input->getError());
        delete input;
        return 1;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    writeLine("File loaded");

    // Prepare the output with the respective options
    AbstractOutput * output = OutputFactory::getOutput(outputFile.filePath());
    switch (options.mode())
    {
    case Options::MODE_CONVERSION_TO_SF2:
        writeLine("Saving file " + outputFile.filePath() + " ...");
        break;
    case Options::MODE_CONVERSION_TO_SF3:
        output->setOption("quality", options.quality());
        writeLine("Saving file " + outputFile.filePath() + "...");
        break;
    case Options::MODE_CONVERSION_TO_SFZ: {
        output->setOption("prefix", options.sfzPresetPrefix());
        output->setOption("bankdir", options.sfzOneDirPerBank());
        output->setOption("gmsort", options.sfzGeneralMidi());
        writeLine("Exporting in directory " + options.getOutputDirectory() + "...");
    } break;
    default:
        writeLine("fail");
        return 1;
    }

    // Convert
    output->process(sf2Index, false);
    if (!output->isSuccess())
    {
        writeLine("Couldn't create " + outputFile.filePath() + ": " + output->getError());
        delete output;
        return 1;
    }
    delete output;
    writeLine("done");

    // Destroy a singleton that has been silently created
    SoundfontManager::kill();
    return 0;
}

int resetConfig(Options &options)
{
    Q_UNUSED(options)

    QSettings settings;
    settings.clear();
    writeLine("Previous configuration is now cleared.");

    return 0;
}

int displayHelp(Options &options)
{
    Q_UNUSED(options)
#ifdef _WIN32
    writeLine("See 'https://www.polyphone-soundfonts.com/documentation/en/manual/annexes/command-line' for more information");
#else
    writeLine("Write 'man polyphone' to show usage");
#endif
    return 0;
}

int main(int argc, char *argv[])
{
    // Prior to everything
    Utils::prepareConversionTables();

    QtSingleApplication app("polyphone", argc, argv);
    QApplication::setApplicationName("Polyphone");
    QApplication::setOrganizationName("polyphone");

    Options options(argc, argv);
    int valRet = 0;

    // Possibly launch the application
    if (!options.error() && !options.help() && options.mode() == Options::MODE_GUI)
    {
        QSettings settings;
        bool uniqueInstance = settings.value("display/unique_instance", true).toBool();

        // Return immediately if there is already an instance
        if (uniqueInstance && app.sendMessage(options.getInputFiles().join('|')))
            return 0;

        // Or launch the application as a unique instance
        return launchApplication(&app, options);
    }

    // Otherwise, console mode
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif
    ContextManager::initializeNoAudioMidi();

    if (options.error() || options.help())
        valRet = displayHelp(options);
    else if (options.mode() == Options::MODE_RESET_CONFIG)
        valRet = resetConfig(options);
    else
        valRet = convert(options);

    return valRet;
}
