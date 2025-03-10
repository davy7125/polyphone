/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include <QFileInfo>
#include <QStyleFactory>
#include <QItemSelection>
#include <QDir>
#include <QScreen>
#include <QDebug>
#include "soundfontmanager.h"
#include "inputfactory.h"
#include "abstractinputparser.h"
#include "outputfactory.h"
#include "abstractoutput.h"
#include "options.h"
#include "contextmanager.h"
#include "qtsingleapplication.h"
#include "mainwindow.h"
#include "translationmanager.h"
#include "modulatordata.h"
#include "fastmaths.h"
#include "voice.h"
#include "tools/merge_samples/runnablemerger.h"


#ifdef _WIN32
#include "windows.h"
#endif

void writeLine(QString line)
{
    QTextStream out(stdout);
    out << line << Qt::endl;
}

int launchApplication(QtSingleApplication * app, Options &options)
{
    // Prepare arrays
    SFModulator::prepareConversionTables();
    FastMaths::initialize();
    Voice::prepareTables();

    // Application style
    QSettings settings;
    QString styleName = settings.value("display/style", "Fusion").toString();
    if (styleName == "Windows Vista")
        styleName = "windowsvista";
    if (!QStyleFactory::keys().contains(styleName))
        styleName = "Fusion";
    QApplication::setStyle(QStyleFactory::create(styleName));

    try {
        app->setPalette(ContextManager::theme()->getPalette());
    } catch (...) { /* bug with mac */ }

    // Application language
    ContextManager::translation()->translate();

    // Additional type used in signals
    qRegisterMetaType<EltID>();
    qRegisterMetaType<QList<int> >();
    qRegisterMetaType<QVector<int> >();

    // Display the main window
    MainWindow w(options.mode() == Options::MODE_SYNTHESIZER);
    app->setActivationWindow(&w, true);
    QObject::connect(app, SIGNAL(messageReceived(const QString&)), &w, SLOT(openFiles(const QString&)));
    w.show();

    // Open files passed as argument
    w.openFiles(options.getInputFilesAsString());

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
        output->setOption("quality", options.sf3Quality());
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
    writeLine("See 'https://www.polyphone.io/documentation/manual/annexes/command-line' for more information");
#else
    writeLine("Write 'man polyphone' to show usage");
#endif
    return 0;
}

/// This test, using the merge tool, can be called for profiling the sound generation in voice.cpp
int testVoice()
{
    ContextManager::initializeNoAudioMidi();

    QString filePath = "/home/davy/Téléchargements/GO - Plein jeu V.sf2";
    AbstractInputParser * input = InputFactory::getInput(filePath);
    input->process(false);
    if (!input->isSuccess())
    {
        writeLine("Couldn't load " + filePath + ": " + input->getError());
        delete input;
        return 1;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    writeLine("File loaded");

    // Launch a tool
    for (int key = 36; key <= 40; key++)
    {
        writeLine("Key: " + QString::number(key));
        RunnableMerger merger(
            nullptr,
            EltID(elementPrst, sf2Index, 0),  // The first preset is used
            key, key,
            false, // No loop
            true, // Stereo enabled
            4, // Create samples lasting 4s (sustained phase)
            2  // With a 2s releaase
        );
        merger.run();
        writeLine("ok");
    }

    return 0;
}

/// This test can be used for testing the synth parallel computing mechanisms
int testSynth()
{
    int bufferLength = 512;
    int sampleRate = 48000;
    int duration = 400; // in seconds

    ContextManager::initializeNoAudioMidi();
    Synth * synth = new Synth(SoundfontManager::getInstance()->getSoundfonts(), SoundfontManager::getInstance()->getMutex());
    synth->configure(ContextManager::configuration()->getSynthConfig());
    synth->setSampleRateAndBufferSize(sampleRate, bufferLength);

    float * lBuffer = new float[bufferLength];
    float * rBuffer = new float[bufferLength];
    int iterationNumber = duration * sampleRate / bufferLength;
    for (int i = 0; i < iterationNumber; i++)
        synth->readData(lBuffer, rBuffer, bufferLength);
    delete [] lBuffer;
    delete [] rBuffer;
    delete synth;

    return 0;
}

int main(int argc, char *argv[])
{
    // Uncomment for testing purposes
    //return testVoice();
    //return testSynth();

#ifdef Q_OS_LINUX
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);
#endif

#if QT_VERSION < 0x060000
    // Dpi scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
#endif
    QtSingleApplication app("polyphone", argc, argv);
    QApplication::setApplicationName("Polyphone");
    QApplication::setOrganizationName("polyphone");
#ifdef _WIN32
    QFont f = app.font(); // Global font size so that it scales
    f.setPointSize(9);
    app.setFont(f);
#endif

    Options options(argc, argv);
    int valRet = 0;

    // Possibly launch the application
    if (!options.error() && !options.help() && (options.mode() == Options::MODE_GUI || options.mode() == Options::MODE_SYNTHESIZER))
    {
        QSettings settings;
        bool uniqueInstance = settings.value("display/unique_instance", true).toBool();

        // Return immediately if there is already an instance
        if (uniqueInstance && app.sendMessage(options.getInputFilesAsString()))
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
