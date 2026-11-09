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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QStringList>
#include <QCoreApplication>
class PlayerOptions;
class QCommandLineParser;

/// Options of the command line
/// The options of the previous versions (-0 to -6, -i, -c) are still accepted, hidden in the help
class Options
{
    Q_DECLARE_TR_FUNCTIONS(Options)

public:
    enum Mode
    {
        MODE_GUI = 0,
        MODE_SYNTHESIZER = 1,
        MODE_RESET_CONFIG = 2,
        MODE_CONVERSION_TO_SF2 = 100,
        MODE_CONVERSION_TO_SF3 = 101,
        MODE_CONVERSION_TO_SFZ = 102,
        MODE_CONVERSION_TO_CSV = 103,
        MODE_RENDER = 104,
        MODE_CONVERSION_TO_DLS = 105
    };

    /// Parse the arguments of QCoreApplication::arguments(), the first one being the executable
    Options(QStringList arguments);
    ~Options();

    /// Get all files that will be open (or the files to be converted, or the soundfont to render)
    QStringList getInputFiles() { return _inputFiles; }

    /// Get all files to open, as absolute paths, with possible options for the player
    QString getInputFilesAsString();

    /// Full path of the file written from an input file, extension included (conversion or rendering)
    QString getOutputFilePath(QString inputFile);

    /// Return the mode describing how the executable will be used
    Mode mode() { return _mode; }

    /// True if the mode is a conversion
    bool isConversion();

    /// True if an existing output file can be replaced
    bool force() { return _force; }

    /// Sfz option: preset number as prefix
    bool sfzPresetPrefix() { return _sfzPresetPrefix; }

    /// Sfz option: one directory per bank during the export
    bool sfzOneDirPerBank() { return _sfzOneDirPerBank; }

    /// Sfz option: use general midi classement
    bool sfzGeneralMidi() { return _sfzGeneralMidi; }

    /// Sf3 option: compression quality (0 is low, 1 is medium, 2 is high);
    int sf3Quality() { return _sf3Quality; }

    /// Csv option: write raw values
    bool csvRawValues() { return _csvRawValues; }

    /// Render options: file describing the events to play, and synth used
    /// (empty: the synth core in the calling thread, "two_cores": the same next to another one,
    /// "threaded": the synth of the editor with its threads, "file": the synth core playing the file read without the editor)
    QString renderEventFile() { return _renderEventFile; }
    QString renderSynth() { return _renderSynth; }

    /// Render option: the events are the ones of a MIDI file, played as the MIDI player does (the synth cannot be chosen)
    bool renderMidiFile();

    /// Player options
    PlayerOptions * playerOptions() { return _playerOptions; }

    /// MCP server started for this session, on a port (0: port of the settings)
    bool mcp() { return _mcp; }
    quint16 mcpPort() { return _mcpPort; }

    /// Return true in case of bad arguments
    bool error() { return _errorSource != nullptr || !_errorText.isEmpty(); }

    /// Description of the error, translated (the translations must be loaded first)
    QString getErrorMessage();

    /// If true, display help or the version
    bool help() { return _help; }
    bool version() { return _version; }

    /// Help text, translated (the translations must be loaded first)
    static QString getHelpText();

private:
    static void addOptions(QCommandLineParser &parser);
    void parse(QCommandLineParser &parser);
    bool readMode(QCommandLineParser &parser);
    void readLegacyConfiguration(QString configuration, QString optionName);
    bool checkOptions(QCommandLineParser &parser);
    void readModeOptions(QCommandLineParser &parser);
    QString getModeOption();
    QString getExtension();
    static QString getOptionName(QString name);
    void setError(const char * source, QStringList arguments = QStringList());

    QStringList _inputFiles;
    QString _outputFile, _outputDirectory;
    Mode _mode;
    bool _help, _version, _force;
    const char * _errorSource;
    QStringList _errorArguments;
    QString _errorText;

    // Sf3 option
    int _sf3Quality;

    // Sfz options
    bool _sfzPresetPrefix;
    bool _sfzOneDirPerBank;
    bool _sfzGeneralMidi;

    // Csv option
    bool _csvRawValues;

    // Render options
    QString _renderEventFile;
    QString _renderSynth;

    // Player options
    PlayerOptions * _playerOptions;

    // MCP server
    bool _mcp;
    quint16 _mcpPort;
};

#endif // OPTIONS_H
