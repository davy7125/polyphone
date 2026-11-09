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
#include <QFile>
#include <QStyleFactory>
#include <QItemSelection>
#include <QDir>
#include <QScreen>
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "inputfactory.h"
#include "abstractinputparser.h"
#include "outputfactory.h"
#include "abstractoutput.h"
#include "options.h"
#include "contextmanager.h"
#include "qtsingleapplication.h"
#include "mainwindow.h"
#include "translationmanager.h"
#include "synth.h"
#include "synthcore.h"
#include "offlinerenderer.h"
#include "midifile.h"
#include "midifileplayer.h"
#include "midifilerenderer.h"
#include "synthmidi.h"
#include "tools/merge_samples/runnablemerger.h"
#include "mcpserver.h"


#ifdef _WIN32
#include "windows.h"
#endif

/// Exit codes of the command line
enum ExitCode
{
    EXIT_CODE_SUCCESS = 0,
    EXIT_CODE_FILE = 1,      // A file or the output directory does not exist, or the output file already exists
    EXIT_CODE_ARGUMENTS = 2, // Wrong arguments
    EXIT_CODE_READ = 3,      // An input file cannot be read (soundfont, MIDI file, events)
    EXIT_CODE_WRITE = 4      // The output cannot be written, or the events cannot be played
};

void writeLine(QString line)
{
    QTextStream out(stdout);
    out << line << Qt::endl;
}

void writeError(QString line)
{
    QTextStream err(stderr);
    err << line << Qt::endl;
}

int launchApplication(QtSingleApplication * app, Options &options)
{
    // Prepare arrays
    SynthCore::prepareTables();

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

    // MCP server required by the command line for this session, started with the main window
    if (options.mcp())
        McpServer::getInstance()->setSessionPort(options.mcpPort() != 0 ? options.mcpPort() : static_cast<quint16>(
            ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "port", McpServer::DEFAULT_PORT).toUInt()));

    // Display the main window
    MainWindow w(options.mode() == Options::MODE_SYNTHESIZER);
    app->setActivationWindow(&w, true);
    QObject::connect(app, SIGNAL(messageReceived(const QString&)), &w, SLOT(openFiles(const QString&)));
    w.show();

    if (options.mcp())
    {
        McpServer * server = McpServer::getInstance();
        if (server->isListening())
            writeLine("MCP: " + QCoreApplication::translate("ConfigSectionMcp", "Server started, address: %1").arg(server->getUrl()));
        else
            writeError("MCP: " + QCoreApplication::translate("ConfigSectionMcp", "The server cannot start: %1").arg(server->getError()));
    }

    // Reopen the tabs of the last session, then the files passed as argument
    w.restoreTabs();
    w.openFiles(options.getInputFilesAsString());

    return app->exec();
}

/// Path of a temporary file next to a file, with the same extension
QString getTemporaryPath(QString filePath)
{
    QFileInfo fileInfo(filePath);
    QString base = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + "_tmp";
    QString path = base + "." + fileInfo.suffix();
    for (int i = 1; QFileInfo::exists(path); i++)
        path = base + "-" + QString::number(i) + "." + fileInfo.suffix();
    return path;
}

/// Conversion of a file
int convertFile(Options &options, QString inputPath)
{
    // Check the input file
    QFileInfo inputFile(inputPath);
    if (!inputFile.exists())
    {
        writeError(QObject::tr("The file \"%1\" does not exist.").arg(inputPath));
        return EXIT_CODE_FILE;
    }

    // Check the output (sfz and csv conversions create a new directory, a number being added to its name if needed)
    QString outputPath = options.getOutputFilePath(inputPath);
    QFileInfo outputFile(outputPath);
    if (!outputFile.dir().exists())
    {
        writeError(QObject::tr("The directory \"%1\" does not exist.").arg(outputFile.path()));
        return EXIT_CODE_FILE;
    }
    bool replace = outputFile.exists() && options.mode() != Options::MODE_CONVERSION_TO_SFZ &&
                   options.mode() != Options::MODE_CONVERSION_TO_CSV;
    if (replace && !options.force())
    {
        writeError(QObject::tr("The file \"%1\" already exists.").arg(outputPath));
        writeError(QObject::tr("Use \"%1\" to replace it.").arg("--force"));
        return EXIT_CODE_FILE;
    }

    // Load input file
    writeLine(QObject::tr("Loading file \"%1\"...").arg(inputPath));
    AbstractInputParser * input = InputFactory::getInput(inputFile.absoluteFilePath());
    input->process(false);
    if (!input->isSuccess())
    {
        writeError(QObject::tr("Couldn't load file \"%1\": %2.").arg(inputPath).arg(input->getError()));
        delete input;
        return EXIT_CODE_READ;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    writeLine(QObject::tr("File successfully loaded!"));

    // Prepare the output with the respective options
    // A file replaced is first written in a temporary file: the input file, possibly the same, is read during the writing
    QString writtenPath = replace ? getTemporaryPath(outputPath) : outputPath;
    AbstractOutput * output = OutputFactory::getOutput(writtenPath);
    switch (options.mode())
    {
    case Options::MODE_CONVERSION_TO_SF2:
        SoundfontManager::getInstance()->setSoundfontVersion(sf2Index, false);
        writeLine(QObject::tr("Writing file \"%1\"...").arg(outputPath));
        break;
    case Options::MODE_CONVERSION_TO_SF3:
        SoundfontManager::getInstance()->setSoundfontVersion(sf2Index, true);
        output->setOption("quality", options.sf3Quality());
        writeLine(QObject::tr("Writing file \"%1\"...").arg(outputPath));
        break;
    case Options::MODE_CONVERSION_TO_SFZ:
        output->setOption("prefix", options.sfzPresetPrefix());
        output->setOption("bankdir", options.sfzOneDirPerBank());
        output->setOption("gmsort", options.sfzGeneralMidi());
        writeLine(QObject::tr("Writing files in directory \"%1\"...").arg(outputFile.path()));
        break;
    case Options::MODE_CONVERSION_TO_CSV:
        output->setOption("raw_values", options.csvRawValues());
        writeLine(QObject::tr("Writing files in directory \"%1\"...").arg(outputFile.path()));
        break;
    case Options::MODE_CONVERSION_TO_DLS:
        writeLine(QObject::tr("Writing file \"%1\"...").arg(outputPath));
        break;
    default:
        break;
    }

    // Convert
    output->process(sf2Index, false);
    bool success = output->isSuccess();
    QString error = output->getError();
    delete output;

    // Close the soundfont, which releases the input file
    SoundfontManager::getInstance()->remove(EltID(elementSf2, sf2Index));

    // Possibly replace the output file by the temporary file
    if (replace)
    {
        if (!success)
            QFile::remove(writtenPath);
        else if (!QFile::remove(outputPath))
        {
            QFile::remove(writtenPath);
            error = QCoreApplication::translate("OutputSf", "cannot delete file \"%1\"").arg(outputPath);
            success = false;
        }
        else if (!QFile::rename(writtenPath, outputPath))
        {
            error = QCoreApplication::translate("OutputSf", "cannot rename file \"%1\"").arg(writtenPath);
            success = false;
        }
    }

    if (!success)
    {
        writeError(QObject::tr("An error occurred: %1.").arg(error));
        return EXIT_CODE_WRITE;
    }
    writeLine(QObject::tr("File successfully converted!"));
    return EXIT_CODE_SUCCESS;
}

/// Conversion of all input files, the exit code being the one of the first failure
int convert(Options &options)
{
    int exitCode = EXIT_CODE_SUCCESS;
    foreach (QString inputFile, options.getInputFiles())
    {
        int result = convertFile(options, inputFile);
        if (exitCode == EXIT_CODE_SUCCESS)
            exitCode = result;
    }

    // Destroy a singleton that has been silently created
    SoundfontManager::kill();
    return exitCode;
}

/// MIDI file played by all channels with the soundfont as the MIDI player does, with the settings of the synth
int renderMidiFile(Options &options, QFileInfo inputFile, QString outputFile)
{
    // The renderer is created before loading the soundfont, which sets the default values of the controllers
    MidiFileRenderer renderer;
    MidiFile midiFile;
    if (!midiFile.read(options.renderEventFile()))
    {
        writeError(midiFile.getError());
        return EXIT_CODE_READ;
    }

    // Load input file
    writeLine(QObject::tr("Loading file \"%1\"...").arg(inputFile.filePath()));
    AbstractInputParser * input = InputFactory::getInput(inputFile.absoluteFilePath());
    input->process(false);
    if (!input->isSuccess())
    {
        writeError(QObject::tr("Couldn't load file \"%1\": %2.").arg(inputFile.filePath()).arg(input->getError()));
        delete input;
        return EXIT_CODE_READ;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    writeLine(QObject::tr("File successfully loaded!"));

    // The notes of a program that the soundfont does not have are not played
    ISynthSoundfont * soundfont = SoundfontManager::getInstance()->getSoundfonts()->getSynthSoundfont(sf2Index);
    for (int channel = 0; channel < 16; channel++)
    {
        foreach (int program, MidiFilePlayer::getUsedPrograms(&midiFile, channel))
        {
            if (SynthMidi::findPreset(soundfont, program / 128, program % 128) == -1)
                writeError(QObject::tr("Channel %1: no preset %2 in the soundfont, its notes are not played.").arg(channel + 1)
                               .arg(QString("%1:%2").arg(program / 128, 3, 10, QChar('0')).arg(program % 128, 3, 10, QChar('0'))));
        }
        renderer.setChannel(channel, sf2Index, -1, 0);
    }

    // Settings of the synth of the editor, at its default sample rate
    ConfManager * configuration = ContextManager::configuration();
    renderer.configure(configuration->getSynthConfig(),
                       configuration->getValue(ConfManager::SECTION_AUDIO, "polyphony", DEFAULT_POLYPHONY).toInt(), 48000);

    // Render
    writeLine(QObject::tr("Writing file \"%1\"...").arg(outputFile));
    if (!renderer.render(&midiFile, outputFile))
    {
        writeError(renderer.getError());
        return EXIT_CODE_WRITE;
    }
    writeLine(QObject::tr("File successfully rendered!"));
    return EXIT_CODE_SUCCESS;
}

/// Rendering of a MIDI file, or of the events of a text file (tests of the sound engine)
/// A missing preset stops the rendering of events (EXIT_CODE_WRITE), not the one of a MIDI file
int render(Options &options)
{
    // Check the input files and the output
    QFileInfo inputFile(options.getInputFiles()[0]);
    foreach (QString path, QStringList() << inputFile.filePath() << options.renderEventFile())
    {
        if (!QFileInfo::exists(path))
        {
            writeError(QObject::tr("The file \"%1\" does not exist.").arg(path));
            return EXIT_CODE_FILE;
        }
    }
    QString outputFile = options.getOutputFilePath(inputFile.filePath());
    if (!QFileInfo(outputFile).dir().exists())
    {
        writeError(QObject::tr("The directory \"%1\" does not exist.").arg(QFileInfo(outputFile).path()));
        return EXIT_CODE_FILE;
    }
    if (QFileInfo::exists(outputFile) && !options.force())
    {
        writeError(QObject::tr("The file \"%1\" already exists.").arg(outputFile));
        writeError(QObject::tr("Use \"%1\" to replace it.").arg("--force"));
        return EXIT_CODE_FILE;
    }

    // Prepare arrays
    SynthCore::prepareTables();

    // A MIDI file, or the events of a text file
    if (options.renderMidiFile())
    {
        int result = renderMidiFile(options, inputFile, outputFile);

        // Destroy a singleton that has been silently created, once the renderer is deleted
        SoundfontManager::kill();
        return result;
    }

    // Events are read before loading the soundfont, which sets the default values of the controllers
    OfflineRenderer renderer;
    if (!renderer.readEvents(options.renderEventFile()))
    {
        writeError(QObject::tr("An error occurred: %1.").arg(renderer.getError()));
        return EXIT_CODE_READ;
    }

    // Load input file
    writeLine(QObject::tr("Loading file \"%1\"...").arg(inputFile.filePath()));
    AbstractInputParser * input = InputFactory::getInput(inputFile.absoluteFilePath());
    input->process(false);
    if (!input->isSuccess())
    {
        writeError(QObject::tr("Couldn't load file \"%1\": %2.").arg(inputFile.filePath()).arg(input->getError()));
        delete input;
        return EXIT_CODE_READ;
    }
    int sf2Index = input->getSf2Index();
    delete input;
    writeLine(QObject::tr("File successfully loaded!"));

    // Render
    writeLine(QObject::tr("Writing file \"%1\"...").arg(outputFile));
    OfflineRenderer::SynthType synthType = OfflineRenderer::SYNTH_CORE;
    if (options.renderSynth() == "two_cores")
        synthType = OfflineRenderer::SYNTH_TWO_CORES;
    else if (options.renderSynth() == "threaded")
        synthType = OfflineRenderer::SYNTH_THREADED;
    else if (options.renderSynth() == "file")
        synthType = OfflineRenderer::SYNTH_FILE;
    if (!renderer.render(sf2Index, outputFile, synthType))
    {
        writeError(QObject::tr("An error occurred: %1.").arg(renderer.getError()));
        return EXIT_CODE_WRITE;
    }
    writeLine(QObject::tr("File successfully rendered!"));

    // Destroy a singleton that has been silently created
    SoundfontManager::kill();
    return EXIT_CODE_SUCCESS;
}

int resetConfig()
{
    QSettings settings;
    settings.clear();
    writeLine(QObject::tr("Previous configuration is now cleared."));
    return EXIT_CODE_SUCCESS;
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
    VoiceContext context;
    for (int key = 36; key <= 40; key++)
    {
        writeLine("Key: " + QString::number(key));
        RunnableMerger merger(
            nullptr,
            &context,
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
#else

#endif

    QtSingleApplication app("polyphone", argc, argv);
    QApplication::setApplicationName("Polyphone");
    QApplication::setOrganizationName("polyphone");
#ifdef _WIN32
    QFont f = app.font(); // Global font size so that it scales
    f.setPointSize(9);
    app.setFont(f);
#endif

    Options options(QCoreApplication::arguments());
    ContextManager::provideCoreSettings();

    // Possibly launch the application
    if (!options.error() && !options.help() && !options.version() &&
        (options.mode() == Options::MODE_GUI || options.mode() == Options::MODE_SYNTHESIZER))
    {
        QSettings settings;
        bool uniqueInstance = settings.value("display/unique_instance", true).toBool();

        // Return immediately if there is already an instance (not if an MCP server is required for this session)
        if (uniqueInstance && !options.mcp() && app.sendMessage(options.getInputFilesAsString()))
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
    ContextManager::translation()->translate();

    if (options.error())
    {
        writeError(options.getErrorMessage());
        writeError(QObject::tr("Write \"%1\" to show usage.").arg("polyphone --help"));
        return EXIT_CODE_ARGUMENTS;
    }
    if (options.help())
    {
        writeLine(Options::getHelpText());
        return EXIT_CODE_SUCCESS;
    }
    if (options.version())
    {
        writeLine(QString("Polyphone ") + SOFT_VERSION + (QString(IDENTIFIER).isEmpty() ? "" : QString(" ") + IDENTIFIER));
        return EXIT_CODE_SUCCESS;
    }

    if (options.mode() == Options::MODE_RESET_CONFIG)
        return resetConfig();
    if (options.mode() == Options::MODE_RENDER)
        return render(options);
    return convert(options);
}
