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

#include "options.h"
#include <QFileInfo>
#include <QDir>
#include <QCommandLineParser>
#include "inputfactory.h"
#include "playeroptions.h"
#include "midifile.h"
#include "utils.h"

static const char * INVALID_VALUE = QT_TRANSLATE_NOOP("Options", "Invalid value \"%1\" for the option \"%2\".");

// Option of the command line, also described in the help
struct OptionDefinition
{
    QStringList names;
    QString valueName;
    QString description;
};

// Options shown in the help, the descriptions being translated if the translations are loaded
static QList<OptionDefinition> getDefinitions()
{
    QList<OptionDefinition> definitions;
    definitions << OptionDefinition{QStringList() << "h" << "help", "",
                                    Options::tr("Display this help.")}
                << OptionDefinition{QStringList() << "v" << "version", "",
                                    Options::tr("Display the version.")}
                << OptionDefinition{QStringList() << "convert", "format",
                                    Options::tr("Convert the files into sf2, sf3, sfz, csv or dls. Without this option, the format is the extension of the output file if given.")}
                << OptionDefinition{QStringList() << "render", "MIDI file",
                                    Options::tr("Play a MIDI file with the soundfont and write the result in a wav file.")}
                << OptionDefinition{QStringList() << "s" << "synth", "",
                                    Options::tr("Open Polyphone in synthesizer mode.")}
                << OptionDefinition{QStringList() << "r" << "reset-config", "",
                                    Options::tr("Reset the configuration of Polyphone (settings, window geometry, history, etc.).")}
                << OptionDefinition{QStringList() << "o" << "output", "path",
                                    Options::tr("Output file, the extension being optional. A name alone is written in the output directory, a relative path starts from the current directory.")}
                << OptionDefinition{QStringList() << "d" << "output-dir", "directory",
                                    Options::tr("Output directory, by default the directory of the input file (or of the MIDI file).")}
                << OptionDefinition{QStringList() << "f" << "force", "",
                                    Options::tr("Replace an existing output file.")}
                << OptionDefinition{QStringList() << "quality", "quality",
                                    Options::tr("Conversion to sf3: compression quality, \"low\", \"medium\" (default) or \"high\".")}
                << OptionDefinition{QStringList() << "preset-prefix", "",
                                    Options::tr("Conversion to sfz: prefix each preset with its number.")}
                << OptionDefinition{QStringList() << "bank-dir", "",
                                    Options::tr("Conversion to sfz: create a directory per bank.")}
                << OptionDefinition{QStringList() << "gm-sort", "",
                                    Options::tr("Conversion to sfz: sort the presets according to the General MIDI classification.")}
                << OptionDefinition{QStringList() << "raw", "",
                                    Options::tr("Conversion to csv: export the raw values stored in the soundfont instead of values with units.")}
                << OptionDefinition{QStringList() << "channel", "channel",
                                    Options::tr("Synthesizer mode: MIDI channel, from 1 to 16, \"all\" (default) for all channels processed as one, \"each\" for each channel separately.")}
                << OptionDefinition{QStringList() << "multiple-selection", "",
                                    Options::tr("Synthesizer mode: several presets can be selected and played at the same time.")}
                << OptionDefinition{QStringList() << "key-selection", "mode",
                                    Options::tr("Synthesizer mode: the keys corresponding to the preset numbers select the presets (\"on\") or select and deselect them (\"toggle\").")}
                << OptionDefinition{QStringList() << "mcp", "",
                                    Options::tr("Start the MCP server for this session even if it is disabled in the settings. A new instance of Polyphone is opened.")}
                << OptionDefinition{QStringList() << "mcp-port", "port",
                                    Options::tr("Port of the MCP server, implying --mcp. By default, the port of the settings.")};
    return definitions;
}

// Conversion mode from a format or an extension, MODE_GUI if unknown
static Options::Mode getConversionMode(QString format)
{
    format = format.toLower();
    if (format == "sf2")
        return Options::MODE_CONVERSION_TO_SF2;
    if (format == "sf3")
        return Options::MODE_CONVERSION_TO_SF3;
    if (format == "sfz")
        return Options::MODE_CONVERSION_TO_SFZ;
    if (format == "csv")
        return Options::MODE_CONVERSION_TO_CSV;
    if (format == "dls")
        return Options::MODE_CONVERSION_TO_DLS;
    return Options::MODE_GUI;
}

// Compression quality of the sf3 format (0, 1 or 2), -1 if the text is not valid
static int getSf3Quality(QString quality)
{
    quality = quality.toLower();
    if (quality == "low" || quality == "0")
        return 0;
    if (quality == "medium" || quality == "1")
        return 1;
    if (quality == "high" || quality == "2")
        return 2;
    return -1;
}

Options::Options(QStringList arguments) :
    _mode(MODE_GUI),
    _help(false),
    _version(false),
    _force(false),
    _errorSource(nullptr),
    _sf3Quality(1),
    _sfzPresetPrefix(false),
    _sfzOneDirPerBank(false),
    _sfzGeneralMidi(false),
    _csvRawValues(false),
    _playerOptions(nullptr),
    _mcp(false),
    _mcpPort(0)
{
    // Process serial number given by old versions of macOS when the Finder opens the application
    for (int i = arguments.count() - 1; i > 0; i--)
        if (arguments[i].startsWith("-psn_"))
            arguments.removeAt(i);

    QCommandLineParser parser;
    addOptions(parser);
    if (!parser.parse(arguments))
    {
        if (!parser.unknownOptionNames().isEmpty())
            setError(QT_TRANSLATE_NOOP("Options", "Unknown option \"%1\"."),
                     QStringList() << getOptionName(parser.unknownOptionNames().first()));
        else
            _errorText = parser.errorText(); // Missing value
        return;
    }

    parse(parser);
}

Options::~Options()
{
    delete _playerOptions;
}

void Options::addOptions(QCommandLineParser &parser)
{
    // "-help" and "--help" are the same, short options are not merged ("-sf" is "--sf")
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    foreach (OptionDefinition definition, getDefinitions())
        parser.addOption(definition.valueName.isEmpty() ?
                             QCommandLineOption(definition.names) :
                             QCommandLineOption(definition.names, "", definition.valueName));

    // Options of the previous versions: -0 (editor), -1 to -6 (modes), -i (input file), -c (configuration)
    QList<QCommandLineOption> legacyOptions;
    for (int i = 0; i <= 6; i++)
        legacyOptions << QCommandLineOption(QString::number(i));
    legacyOptions << QCommandLineOption("i", "", "file")
                  << QCommandLineOption("c", "", "configuration");
    foreach (QCommandLineOption option, legacyOptions)
    {
        option.setFlags(QCommandLineOption::HiddenFromHelp);
        parser.addOption(option);
    }
}

void Options::parse(QCommandLineParser &parser)
{
    _help = parser.isSet("help");
    _version = parser.isSet("version");
    if (_help || _version)
        return;

    // Input files, written after -i or alone
    foreach (QString file, parser.values("i") + parser.positionalArguments())
        if (!file.isEmpty())
            _inputFiles << file;
    _outputFile = parser.value("output");
    _outputDirectory = parser.value("output-dir");
    _force = parser.isSet("force");

    if (readMode(parser) && checkOptions(parser))
        readModeOptions(parser);
}

bool Options::readMode(QCommandLineParser &parser)
{
    // All modes required, including the ones of the previous versions (-0 to -6)
    const Mode legacyModes[] = {
        MODE_GUI, MODE_CONVERSION_TO_SF2, MODE_CONVERSION_TO_SF3, MODE_CONVERSION_TO_SFZ,
        MODE_CONVERSION_TO_CSV, MODE_RENDER, MODE_CONVERSION_TO_DLS
    };
    QList<Mode> modes;
    for (int i = 0; i <= 6; i++)
        if (parser.isSet(QString::number(i)))
            modes << legacyModes[i];

    if (parser.isSet("convert"))
    {
        Mode mode = getConversionMode(parser.value("convert"));
        if (mode == MODE_GUI)
        {
            setError(INVALID_VALUE, QStringList() << parser.value("convert") << "--convert");
            return false;
        }
        modes << mode;
    }
    if (parser.isSet("render"))
        modes << MODE_RENDER;
    if (parser.isSet("synth"))
        modes << MODE_SYNTHESIZER;
    if (parser.isSet("reset-config"))
        modes << MODE_RESET_CONFIG;

    for (int i = modes.count() - 1; i > 0; i--)
        if (modes.indexOf(modes[i]) < i)
            modes.removeAt(i);
    if (modes.count() > 1)
    {
        setError(QT_TRANSLATE_NOOP("Options", "Only one of the options --convert, --render, --synth and --reset-config can be used."));
        return false;
    }

    if (!modes.isEmpty())
        _mode = modes.first();
    else if (!_outputFile.isEmpty())
    {
        // Conversion into the format given by the extension of the output file
        _mode = getConversionMode(QFileInfo(_outputFile).suffix());
        if (_mode == MODE_GUI)
        {
            setError(QT_TRANSLATE_NOOP("Options", "The format of the output file \"%1\" is unknown, it can be specified with --convert."),
                     QStringList() << _outputFile);
            return false;
        }
    }

    return true;
}

bool Options::checkOptions(QCommandLineParser &parser)
{
    // Options used only in some modes
    QList<Mode> conversionModes = QList<Mode>() << MODE_CONVERSION_TO_SF2 << MODE_CONVERSION_TO_SF3 << MODE_CONVERSION_TO_SFZ
                                                << MODE_CONVERSION_TO_CSV << MODE_CONVERSION_TO_DLS;
    QList<Mode> outputModes = conversionModes;
    outputModes << MODE_RENDER;

    QList<QPair<QString, QList<Mode> > > usages;
    usages << qMakePair(QString("output"), outputModes)
           << qMakePair(QString("output-dir"), outputModes)
           << qMakePair(QString("force"), outputModes)
           << qMakePair(QString("quality"), QList<Mode>() << MODE_CONVERSION_TO_SF3)
           << qMakePair(QString("preset-prefix"), QList<Mode>() << MODE_CONVERSION_TO_SFZ)
           << qMakePair(QString("bank-dir"), QList<Mode>() << MODE_CONVERSION_TO_SFZ)
           << qMakePair(QString("gm-sort"), QList<Mode>() << MODE_CONVERSION_TO_SFZ)
           << qMakePair(QString("raw"), QList<Mode>() << MODE_CONVERSION_TO_CSV)
           << qMakePair(QString("channel"), QList<Mode>() << MODE_SYNTHESIZER)
           << qMakePair(QString("multiple-selection"), QList<Mode>() << MODE_SYNTHESIZER)
           << qMakePair(QString("key-selection"), QList<Mode>() << MODE_SYNTHESIZER)
           << qMakePair(QString("mcp"), QList<Mode>() << MODE_GUI)
           << qMakePair(QString("mcp-port"), QList<Mode>() << MODE_GUI)
           << qMakePair(QString("c"), QList<Mode>() << MODE_CONVERSION_TO_SF3 << MODE_CONVERSION_TO_SFZ
                                                     << MODE_CONVERSION_TO_CSV << MODE_RENDER << MODE_SYNTHESIZER);
    for (int i = 0; i < usages.count(); i++)
    {
        if (!parser.isSet(usages[i].first) || usages[i].second.contains(_mode))
            continue;
        if (_mode == MODE_GUI)
            setError(QT_TRANSLATE_NOOP("Options", "The option \"%1\" is not used when files are opened in the editor."),
                     QStringList() << getOptionName(usages[i].first));
        else
            setError(QT_TRANSLATE_NOOP("Options", "The option \"%1\" cannot be used with \"%2\"."),
                     QStringList() << getOptionName(usages[i].first) << getModeOption());
        return false;
    }

    // Input files
    if (_mode == MODE_RESET_CONFIG && !_inputFiles.isEmpty())
        setError(QT_TRANSLATE_NOOP("Options", "No file is expected with \"%1\"."), QStringList() << getModeOption());
    else if (_mode == MODE_RENDER && _inputFiles.count() != 1)
        setError(QT_TRANSLATE_NOOP("Options", "One soundfont is expected with \"%1\"."), QStringList() << getModeOption());
    else if (conversionModes.contains(_mode))
    {
        if (_inputFiles.isEmpty())
            setError(QT_TRANSLATE_NOOP("Options", "No file to convert."));
        else if (_inputFiles.count() > 1 && !_outputFile.isEmpty())
            setError(QT_TRANSLATE_NOOP("Options", "The option \"%1\" cannot be used with several input files."),
                     QStringList() << getOptionName("output"));
    }
    if (_mode != MODE_GUI && _mode != MODE_SYNTHESIZER && !error())
    {
        // The files that cannot be opened are reported by the editor
        foreach (QString inputFile, _inputFiles)
        {
            if (!InputFactory::isSuffixSupported(QFileInfo(inputFile).suffix()))
            {
                setError(QT_TRANSLATE_NOOP("Options", "The format of the file \"%1\" is not supported."), QStringList() << inputFile);
                break;
            }
        }
    }

    return !error();
}

void Options::readModeOptions(QCommandLineParser &parser)
{
    // Configuration of the previous versions (-c), then the options replacing it
    QStringList configurations = parser.values("c");
    if (configurations.count() > 1)
    {
        setError(QT_TRANSLATE_NOOP("Options", "The option \"%1\" can only be used once."), QStringList() << "-c");
        return;
    }
    if (!configurations.isEmpty())
    {
        readLegacyConfiguration(configurations.first(), "-c");
        if (error())
            return;
    }

    switch (_mode)
    {
    case MODE_CONVERSION_TO_SF3:
        if (parser.isSet("quality"))
        {
            _sf3Quality = getSf3Quality(parser.value("quality"));
            if (_sf3Quality == -1)
                setError(INVALID_VALUE, QStringList() << parser.value("quality") << "--quality");
        }
        break;
    case MODE_CONVERSION_TO_SFZ:
        _sfzPresetPrefix |= parser.isSet("preset-prefix");
        _sfzOneDirPerBank |= parser.isSet("bank-dir");
        _sfzGeneralMidi |= parser.isSet("gm-sort");
        break;
    case MODE_CONVERSION_TO_CSV:
        _csvRawValues |= parser.isSet("raw");
        break;
    case MODE_RENDER:
        if (parser.isSet("render"))
        {
            if (!configurations.isEmpty())
            {
                setError(QT_TRANSLATE_NOOP("Options", "The options \"%1\" and \"%2\" cannot be used together."),
                         QStringList() << "--render" << "-c");
                return;
            }
            readLegacyConfiguration(parser.value("render"), "--render");
        }
        if (!error() && _renderEventFile.isEmpty())
            setError(QT_TRANSLATE_NOOP("Options", "The MIDI file to play is missing."));
        break;
    case MODE_SYNTHESIZER:
        if (_playerOptions == nullptr)
            _playerOptions = new PlayerOptions();
        if (parser.isSet("channel"))
        {
            PlayerOptions channelOptions;
            if (channelOptions.parse(QStringList() << parser.value("channel").toLower()))
                _playerOptions->setPlayerChannel(channelOptions.playerChannel());
            else
                setError(INVALID_VALUE, QStringList() << parser.value("channel") << "--channel");
        }
        if (parser.isSet("multiple-selection"))
            _playerOptions->setMultipleSelection(true);
        if (parser.isSet("key-selection"))
        {
            QString keySelection = parser.value("key-selection").toLower();
            if (keySelection == "off")
                _playerOptions->setKeySelection(0);
            else if (keySelection == "on")
                _playerOptions->setKeySelection(1);
            else if (keySelection == "toggle")
                _playerOptions->setKeySelection(2);
            else
                setError(INVALID_VALUE, QStringList() << parser.value("key-selection") << "--key-selection");
        }
        break;
    case MODE_GUI:
        _mcp = parser.isSet("mcp") || parser.isSet("mcp-port");
        if (parser.isSet("mcp-port"))
        {
            bool ok = false;
            uint port = parser.value("mcp-port").toUInt(&ok);
            if (ok && port > 0 && port <= 65535)
                _mcpPort = static_cast<quint16>(port);
            else
                setError(INVALID_VALUE, QStringList() << parser.value("mcp-port") << "--mcp-port");
        }
        break;
    default:
        break;
    }
}

void Options::readLegacyConfiguration(QString configuration, QString optionName)
{
    // Values separated by "|" (also the value of --render: the file to play, possibly followed by the synth to use)
    QStringList values = configuration.split('|');
    bool valid = true;

    switch (_mode)
    {
    case MODE_CONVERSION_TO_SF3:
        _sf3Quality = getSf3Quality(values[0]);
        valid = values.count() == 1 && _sf3Quality != -1;
        break;
    case MODE_CONVERSION_TO_SFZ: {
        // Preset prefix, directory per bank, General MIDI classification: "1" or "0"
        bool * sfzOptions[3] = { &_sfzPresetPrefix, &_sfzOneDirPerBank, &_sfzGeneralMidi };
        valid = values.count() <= 3;
        for (int i = 0; i < values.count() && valid; i++)
        {
            valid = values[i] == "0" || values[i] == "1";
            *sfzOptions[i] = values[i] == "1";
        }
    } break;
    case MODE_CONVERSION_TO_CSV:
        valid = values.count() == 1 && values[0] == "raw";
        _csvRawValues = valid;
        break;
    case MODE_RENDER:
        _renderEventFile = values[0];
        if (values.count() == 2 && (values[1] == "two_cores" || values[1] == "threaded" || values[1] == "file"))
            _renderSynth = values[1];
        else if (values.count() > 1)
            valid = false;

        // The synth cannot be chosen for a MIDI file
        if (renderMidiFile() && !_renderSynth.isEmpty())
            valid = false;
        break;
    case MODE_SYNTHESIZER:
        if (_playerOptions == nullptr)
            _playerOptions = new PlayerOptions();
        valid = _playerOptions->parse(values);
        break;
    default:
        valid = false;
        break;
    }

    if (!valid)
        setError(INVALID_VALUE, QStringList() << configuration << optionName);
}

bool Options::isConversion()
{
    return _mode == MODE_CONVERSION_TO_SF2 || _mode == MODE_CONVERSION_TO_SF3 || _mode == MODE_CONVERSION_TO_SFZ ||
           _mode == MODE_CONVERSION_TO_CSV || _mode == MODE_CONVERSION_TO_DLS;
}

bool Options::renderMidiFile()
{
    // Extensions of the MIDI player
    return MidiFile::isSuffixSupported(QFileInfo(_renderEventFile).suffix());
}

QString Options::getModeOption()
{
    switch (_mode)
    {
    case MODE_SYNTHESIZER:
        return "--synth";
    case MODE_RESET_CONFIG:
        return "--reset-config";
    case MODE_RENDER:
        return "--render";
    case MODE_GUI:
        return "";
    default:
        return "--convert " + getExtension();
    }
}

QString Options::getExtension()
{
    switch (_mode)
    {
    case MODE_CONVERSION_TO_SF2:
        return "sf2";
    case MODE_CONVERSION_TO_SF3:
        return "sf3";
    case MODE_CONVERSION_TO_SFZ:
        return "sfz";
    case MODE_CONVERSION_TO_CSV:
        return "csv";
    case MODE_CONVERSION_TO_DLS:
        return "dls";
    case MODE_RENDER:
        return "wav";
    default:
        return "";
    }
}

QString Options::getOptionName(QString name)
{
    return (name.size() == 1 ? "-" : "--") + name;
}

void Options::setError(const char * source, QStringList arguments)
{
    // Only the first error is kept
    if (error())
        return;
    _errorSource = source;
    _errorArguments = arguments;
}

QString Options::getErrorMessage()
{
    if (_errorSource == nullptr)
        return _errorText;

    QString message = tr(_errorSource);
    foreach (QString argument, _errorArguments)
        message = message.arg(argument);
    return message;
}

QString Options::getOutputFilePath(QString inputFile)
{
    // The default name and directory are the ones of the input file, or of the MIDI file in a rendering
    QFileInfo reference(_mode == MODE_RENDER ? _renderEventFile : inputFile);
    QString extension = getExtension();
    QString directory = _outputDirectory.isEmpty() ? reference.absolutePath() : QDir(_outputDirectory).absolutePath();

    QString path;
    if (_outputFile.isEmpty())
        path = directory + "/" + reference.completeBaseName();
    else
    {
        // The extension is optional
        QString output = QDir::fromNativeSeparators(_outputFile);
        if (QFileInfo(output).suffix().compare(extension, Qt::CaseInsensitive) == 0)
            output.chop(extension.length() + 1);

        // A name alone is in the output directory, a relative path starts from the current directory (or from -d)
        if (QDir::isAbsolutePath(output))
            path = output;
        else if (output.contains('/') && _outputDirectory.isEmpty())
            path = QDir::current().absoluteFilePath(output);
        else
            path = directory + "/" + output;
    }

    return QDir::cleanPath(path) + "." + extension;
}

QString Options::getInputFilesAsString()
{
    // All files, as absolute paths since they can be sent to another instance
    QStringList filesToOpen;
    foreach (QString file, _inputFiles)
        filesToOpen << QFileInfo(Utils::fixFilePath(file)).absoluteFilePath();
    QString str = filesToOpen.join('|');

    // Possible add the player options
    if (_playerOptions != nullptr)
        str += "||" + _playerOptions->toString();

    return str;
}

// Wide characters (Chinese, Japanese, Korean) take two columns in a terminal
static bool isWide(QChar c)
{
    ushort u = c.unicode();
    return (u >= 0x1100 && u <= 0x115F) || (u >= 0x2E80 && u <= 0xA4CF) || (u >= 0xAC00 && u <= 0xD7A3) ||
           (u >= 0xF900 && u <= 0xFAFF) || (u >= 0xFE30 && u <= 0xFE4F) || (u >= 0xFF00 && u <= 0xFF60) ||
           (u >= 0xFFE0 && u <= 0xFFE6);
}

static int getDisplayWidth(QString text)
{
    int width = 0;
    foreach (QChar c, text)
        width += isWide(c) ? 2 : 1;
    return width;
}

// Text wrapped in lines of 80 columns at most (unless a word is longer), with an indentation
static QString wrapText(QString text, int indentation, QString firstLine = "")
{
    // Tokens: words separated by spaces, or wide characters that can be separated anywhere
    QStringList tokens;
    QList<bool> spaceBefore;
    bool space = false;
    for (int i = 0; i < text.size(); i++)
    {
        if (text[i] == ' ')
            space = true;
        else
        {
            if (tokens.isEmpty() || space || isWide(text[i]) || isWide(tokens.last().back()))
            {
                tokens << "";
                spaceBefore << space;
            }
            tokens.last() += text[i];
            space = false;
        }
    }

    // An opening quote stays with the next token, a closing quote or a punctuation mark with the previous one
    const QString opening = QString::fromUtf8("«（「『");
    const QString closing = QString::fromUtf8("»:;!?、。，）」』：；！？ー");
    for (int i = tokens.count() - 1; i > 0; i--)
    {
        if (opening.contains(tokens[i - 1].back()) || closing.contains(tokens[i][0]))
        {
            tokens[i - 1] += (spaceBefore[i] ? " " : "") + tokens[i];
            tokens.removeAt(i);
            spaceBefore.removeAt(i);
        }
    }

    // Lines
    const int lineWidth = 80;
    QString result;
    QString line = firstLine.leftJustified(indentation);
    int width = getDisplayWidth(line);
    bool lineEmpty = true;
    for (int i = 0; i < tokens.count(); i++)
    {
        int tokenWidth = getDisplayWidth(tokens[i]) + (spaceBefore[i] && !lineEmpty ? 1 : 0);
        if (!lineEmpty && width + tokenWidth > lineWidth)
        {
            result += line + "\n";
            line = QString(indentation, ' ');
            width = indentation;
            lineEmpty = true;
            tokenWidth = getDisplayWidth(tokens[i]);
        }
        if (spaceBefore[i] && !lineEmpty)
            line += " ";
        line += tokens[i];
        width += tokenWidth;
        lineEmpty = false;
    }
    return result + line + "\n";
}

QString Options::getHelpText()
{
    QString text = tr("Usage:") + "\n" +
                   "  polyphone [options] [files...]\n\n" +
                   wrapText(tr("Polyphone, a soundfont editor: the files and directories are opened in the editor, "
                               "unless one of the options --convert, --render, --synth or --reset-config is used."), 0) + "\n" +
                   tr("Files:") + "\n" +
                   wrapText(tr("Soundfonts (sf2, sf3, sfz, dls, sfArk, organ) or directories."), 2) + "\n" +
                   tr("Options:") + "\n";

    // Option names in a first column, descriptions in a second column
    const int nameWidth = 28;
    foreach (OptionDefinition definition, getDefinitions())
    {
        QStringList names;
        foreach (QString name, definition.names)
            names << getOptionName(name);
        QString optionText = "  " + names.join(", ");
        if (!definition.valueName.isEmpty())
            optionText += " <" + definition.valueName + ">";
        if (optionText.size() >= nameWidth)
        {
            text += optionText + "\n";
            optionText.clear();
        }
        text += wrapText(definition.description, nameWidth, optionText);
    }

    text += "\n" + tr("Examples:") + "\n" +
            "  polyphone file.sf2 -o file.sf3 --quality high\n"
            "  polyphone --convert sf3 *.sf2\n"
            "  polyphone --convert sfz --gm-sort file.sf2 -d /path/to/directory\n"
            "  polyphone --render song.mid file.sf2\n"
            "  polyphone --synth --channel each file.sf2\n"
            "  polyphone --mcp file.sf2\n\n" +
            wrapText(tr("The options -1 to -6, -i and -c of the previous versions are still accepted."), 0) +
            wrapText(tr("Documentation: %1").arg("https://www.polyphone.io/documentation/manual/annexes/command-line"), 0);
    return text.trimmed();
}
