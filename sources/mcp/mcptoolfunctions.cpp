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

#include "mcpfunctions.h"
#include "mcputils.h"
#include "mcpattributes.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "soundanalyzer.h"
#include "sampleutils.h"
#include "utils.h"
#include "trim_end/tooltrimend.h"
#include "auto_loop/toolautoloop.h"
#include "trim_start/tooltrimstart.h"
#include "frequency_filter/toolfrequencyfilter.h"
#include "frequency_filter/toolfrequencyfilter_parameters.h"
#include "change_volume/toolchangevolume.h"
#include "change_volume/toolchangevolume_parameters.h"
#include "balance_adjustment/toolbalanceadjustment.h"
#include "transpose_smpl/tooltransposesmpl.h"
#include "transpose_smpl/tooltransposesmpl_parameters.h"
#include "link_sample/toollinksample.h"
#include "unlink_sample/toolunlinksample.h"
#include "mono_conversion/toolmonoconversion.h"
#include "mono_conversion/toolmonoconversion_parameters.h"
#include "sample_export/toolsampleexport.h"
#include "fast_edit_smpl/toolfasteditsmpl.h"
#include "fast_edit_smpl/toolfasteditsmpl_parameters.h"
#include "load_from_inst/toolloadfrominst.h"
#include "load_from_inst/toolloadfrominst_parameters.h"
#include "loop_crossfade/toolloopcrossfade.h"
#include "loop_crossfade/toolloopcrossfade_parameters.h"
#include "frequency_peaks/toolfrequencypeaks.h"
#include "frequency_peaks/toolfrequencypeaks_parameters.h"
#include "celeste_tuning/toolcelestetuning.h"
#include "celeste_tuning/toolcelestetuning_parameters.h"
#include "auto_distribution/toolautodistribution.h"
#include "mixture_creation/toolmixturecreation.h"
#include "mixture_creation/toolmixturecreation_parameters.h"
#include "chords/toolchords.h"
#include "chords/toolchords_parameters.h"
#include "missing_samples/toolmissingsamples.h"
#include "missing_samples/toolmissingsamples_parameters.h"
#include "sound_spatialization/toolsoundspatialization.h"
#include "sound_spatialization/toolsoundspatialization_parameters.h"
#include "transpose/tooltranspose.h"
#include "transpose/tooltranspose_parameters.h"
#include "release/toolrelease.h"
#include "release/toolrelease_parameters.h"
#include "default_mod/tooldefaultmod.h"
#include "default_mod/tooldefaultmod_parameters.h"
#include "change_attenuation/toolchangeattenuation.h"
#include "change_attenuation/toolchangeattenuation_parameters.h"
#include "global_settings/toolglobalsettings.h"
#include "global_settings/toolglobalsettings_parameters.h"
#include "division_duplication/tooldivisionduplication.h"
#include "division_duplication/tooldivisionduplication_parameters.h"
#include "remove_mods/toolremovemods.h"
#include "merge_samples/toolmergesamples.h"
#include "merge_samples/toolmergesamples_parameters.h"
#include "clean_unused_elements/toolcleanunused.h"
#include "clean_duplicates/toolcleanduplicates.h"
#include "soundfont_export/toolsoundfontexport.h"
#include "soundfont_export/toolsoundfontexport_parameters.h"
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QDir>

typedef std::function<void(const McpArguments &, AbstractTool *, AbstractToolParameters *)> ParameterSetter;
typedef std::function<void(AbstractTool *, QJsonObject &)> ResultFiller;

// Tools are created once, separately from the tools of the menus
static AbstractTool * getTool(QString name, std::function<AbstractTool *()> create)
{
    static QMap<QString, AbstractTool *> tools;
    if (!tools.contains(name))
        tools[name] = create();
    return tools[name];
}

static QString cleanMessage(QString message)
{
    static QRegularExpression lineBreak("<br\\s*/?>", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression listItem("<li>", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression tag("<[^>]*>");
    message.replace(lineBreak, "\n");
    message.replace(listItem, "\n- ");
    message.remove(tag);
    return message.trimmed();
}

// Function running a tool on elements given in the argument "elements"
static McpFunction * toolFunction(QString name, QString description, QString elementsDescription, QList<ElementType> types,
                                  QString properties, QStringList required, std::function<AbstractTool *()> create,
                                  ParameterSetter setter = ParameterSetter(), ResultFiller filler = ResultFiller())
{
    QString schema = "{\"properties\":{\"elements\":{\"type\":[\"string\",\"array\"],\"items\":{\"type\":\"string\"},"
                     "\"description\":\"" + elementsDescription + "\"}" + (properties.isEmpty() ? "" : "," + properties) +
                     "},\"required\":[\"elements\"" + (required.isEmpty() ? "" : ",\"" + required.join("\",\"") + "\"") + "]}";

    return new McpFunction(name, McpFunction::CATEGORY_TOOLS, false, description, schema.toUtf8().constData(),
        [name, types, create, setter, filler](const McpArguments &args) -> McpResult {
            IdList ids = args.getIds("elements", types);
            AbstractTool * tool = getTool(name, create);
            QString message;
            QString error = tool->runWithoutDialog(ids, [&](AbstractToolParameters * parameters) -> QString {
                if (setter)
                {
                    try
                    {
                        setter(args, tool, parameters);
                    }
                    catch (const McpException &exception)
                    {
                        return exception.message();
                    }
                }
                return QString();
            }, message);
            if (!error.isEmpty())
                throw McpException(cleanMessage(error));

            QJsonObject result;
            result["done"] = true;
            if (!message.isEmpty())
                result["message"] = cleanMessage(message);
            if (filler)
                filler(tool, result);
            return result;
        });
}

static int readChoice(const McpArguments &args, QString key, QStringList choices, int defaultValue = -1)
{
    if (!args.contains(key))
    {
        if (defaultValue < 0)
            throw McpException("missing argument \"" + key + "\"");
        return defaultValue;
    }
    int index = choices.indexOf(args.getString(key));
    if (index < 0)
        throw McpException("argument \"" + key + "\" must be one of: " + choices.join(", "));
    return index;
}

static int readDensity(const McpArguments &args)
{
    return readChoice(args, "density", { "every_key", "every_3_keys", "every_6_keys" }, 1);
}

// Points [[x, y], ...] sorted by x, linearly interpolated
static QList<QPointF> readPoints(const McpArguments &args, QString key, bool xIsKey)
{
    QList<QPointF> points;
    for (const QJsonValue value : args.getArray(key))
    {
        QJsonArray pair = value.toArray();
        if (!value.isArray() || pair.count() != 2 || !pair[1].isDouble())
            throw McpException("argument \"" + key + "\" must be an array of [x, y] pairs");
        double x = xIsKey ? McpUtils::readKey(pair[0], key) : pair[0].toDouble(-1);
        if (!xIsKey && (!pair[0].isDouble() || x < 0))
            throw McpException("argument \"" + key + "\": x values must be positive numbers");
        points << QPointF(x, pair[1].toDouble());
    }
    if (points.isEmpty())
        throw McpException("argument \"" + key + "\" is empty");
    std::sort(points.begin(), points.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    return points;
}

static double interpolate(const QList<QPointF> &points, double x, bool logX = false)
{
    if (x <= points.first().x())
        return points.first().y();
    if (x >= points.last().x())
        return points.last().y();
    for (int i = 1; i < points.count(); i++)
    {
        if (x <= points[i].x())
        {
            double x1 = points[i - 1].x(), x2 = points[i].x();
            if (x2 <= x1)
                return points[i].y();
            double ratio = logX && x1 > 0 ? std::log(x / x1) / std::log(x2 / x1) : (x - x1) / (x2 - x1);
            return points[i - 1].y() + ratio * (points[i].y() - points[i - 1].y());
        }
    }
    return points.last().y();
}

// Same computation as the dialog of the spatialization tool
class SpatializationCurve
{
public:
    SpatializationCurve(int minKey, int maxKey) : _minKey(minKey), _maxKey(maxKey) {}

    static double getAxe(int note, int nbDiv, bool sens)
    {
        int axe = note % nbDiv;
        if (axe % 2 == 0)
            axe = axe / 2;
        else
            axe = nbDiv - (axe + 1) / 2;
        if (sens)
            axe = nbDiv - 1 - axe;
        return static_cast<double>(2 * axe + 1) / (2 * nbDiv);
    }

    double spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending)
    {
        int noteRelative = isAscending ? note - _minKey : _maxKey - note;
        double posAxe = getAxe(noteRelative, nbDiv, sens);
        double e = 1. / (_maxKey - _minKey + nbDiv - 1);
        double ecart_axe = e * ((static_cast<double>(_maxKey - _minKey) / 2) - noteRelative) / nbDiv;
        double mult = static_cast<double>(nbDiv) / (nbDiv - 1 + static_cast<double>(occupation) / 100);
        ecart_axe *= static_cast<double>(occupation) / 100;
        return static_cast<double>(etalement) / 100 * (mult * (posAxe + ecart_axe - 0.5) + 0.5) +
               (1. - static_cast<double>(etalement) / 100) * static_cast<double>(offset) / 100;
    }

    double spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow)
    {
        int noteRelative = 0;
        if (isHollow)
        {
            int resteNotes = (_maxKey - _minKey + 1) % (2 * nbDiv);
            int decalage = 0;
            if (resteNotes < nbDiv)
                decalage = nbDiv - resteNotes;
            else if (resteNotes > nbDiv)
                decalage = 3 * nbDiv - resteNotes;
            noteRelative = _maxKey - note + decalage;
        }
        else
            noteRelative = note - _minKey;

        double posAxe = getAxe(noteRelative, nbDiv, sens);
        double e = 1.0 / (ceil(static_cast<double>(_maxKey - _minKey + 1 - nbDiv) / (2 * nbDiv)) * (2 * nbDiv) + nbDiv);
        int ne_axe = (noteRelative + nbDiv) / (2 * nbDiv);
        double ecart_axe = e * ne_axe;
        double mult = 1;
        if (isHollow)
            ecart_axe = (ecart_axe * occupation + static_cast<double>(100 - occupation) / (2 * nbDiv)) / 100;
        else
        {
            ecart_axe *= static_cast<double>(occupation) / 100;
            mult = static_cast<double>(nbDiv) / (nbDiv - 1 + static_cast<double>(occupation) / 100);
        }
        if (noteRelative % (2 * nbDiv) >= nbDiv)
            ecart_axe = -ecart_axe;
        if (sens2 != 0)
            ecart_axe = -ecart_axe;
        return static_cast<double>(etalement) / 100 * (mult * (posAxe + ecart_axe - 0.5) + 0.5) +
               (1. - static_cast<double>(etalement) / 100) * static_cast<double>(offset) / 100;
    }

private:
    int _minKey, _maxKey;
};

static void addSampleTools(QList<McpFunction *> &functions)
{
    QList<ElementType> samples = { elementSmpl };
    QString samplesText = "Samples to process (the linked sample of a stereo pair is included)";

    functions << toolFunction(
        "auto_loop",
        "Tool \"Auto loop\": find a loop in the steady part of each sample, crossfade it and remove the data after the "
        "loop end. Both samples of a stereo pair get the same loop. Check the result with get_loop_analysis.",
        samplesText, samples, "", {}, []() { return new ToolAutoLoop(); });

    functions << toolFunction(
        "trim_to_loop",
        "Tool \"Trim to end of loop\": remove the data after the loop end of each sample.",
        samplesText, samples, "", {}, []() { return new ToolTrimEnd(); });

    functions << toolFunction(
        "trim_start",
        "Tool \"Remove blank at start\": remove the silence or the background noise before the sound of each sample, "
        "up to the onset given by analyze_sample (stereo pairs are trimmed together, the loop points are shifted).",
        samplesText, samples, "", {}, []() { return new ToolTrimStart(); });

    functions << toolFunction(
        "filter_frequencies",
        "Tool \"Filter frequencies\": change the spectrum of the samples with a curve of points [x, dB], interpolated on a "
        "logarithmic scale of x and constant beyond the first and the last points. x is a frequency in Hz or, with "
        "note_tracking, a ratio to the fundamental frequency of each sample (its root key shifted by its pitch correction): "
        "1 = fundamental, 0.5 = octave below, 2 = octave, 3 = twelfth, 4 = second octave... so that the same curve acts on "
        "the same harmonics of every sample. Mode \"gain\": attenuation or amplification from -60 to +24 dB, applied with "
        "a minimum phase (causal, no pre-echo before the attack), the loop being processed as if it were played endlessly "
        "(it stays seamless). Mode \"ceiling\": maximum level allowed relative to the strongest frequency of the sample "
        "(0 dB = nothing removed, -40 dB = components above -40 dB are reduced to -40 dB, -80 dB or less = removed). "
        "If the result exceeds the full scale, the whole sample is attenuated to the full scale. Examples: "
        "{\"mode\":\"ceiling\",\"curve\":[[7000,0],[8000,-100]]} removes everything above 8 kHz; "
        "{\"mode\":\"gain\",\"note_tracking\":true,\"curve\":[[2,0],[4,3],[8,3],[16,0]]} raises the harmonics 4 to 8 "
        "by 3 dB. Result for each sample: fundamental used (Hz, with note_tracking), peaks before and after the processing "
        "(dBFS, the second one before the attenuation to the full scale).",
        samplesText, samples,
        R"JSON("mode":{"type":"string","enum":["gain","ceiling"],"description":"Gain in dB, or ceiling relative to the strongest frequency"},
           "note_tracking":{"type":"boolean","description":"x is a ratio to the fundamental frequency of each sample (default: false, x in Hz)"},
           "curve":{"type":"array","items":{"type":"array"},"description":"Points [x > 0, dB]: gain from -60 to 24, or ceiling <= 0"})JSON",
        { "mode", "curve" }, []() { return new ToolFrequencyFilter(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolFrequencyFilter_parameters * params = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters);
            bool isGain = readChoice(args, "mode", { "ceiling", "gain" }) == 1;
            QList<QPointF> points = readPoints(args, "curve", false);
            for (const QPointF &point : points)
            {
                if (point.x() <= 0)
                    throw McpException("argument \"curve\": x values must be greater than 0");
                if (isGain && (point.y() < GainCurve::MIN_GAIN || point.y() > GainCurve::MAX_GAIN))
                    throw McpException("argument \"curve\": gains must be between -60 and 24 dB");
                if (!isGain && point.y() > 0)
                    throw McpException("argument \"curve\": ceilings must be 0 dB or less");
            }
            params->setMode(isGain ? GainCurve::MODE_GAIN : GainCurve::MODE_CEILING);
            params->setNoteTracking(args.getBool("note_tracking", false));
            params->setPoints(points);
        },
        [](AbstractTool * tool, QJsonObject &result) {
            QJsonArray samples;
            for (const ToolFrequencyFilter::Report &report : dynamic_cast<ToolFrequencyFilter *>(tool)->getReports())
            {
                QJsonObject sample = McpUtils::brief(report.id);
                if (report.fundamental > 0)
                    sample["fundamental_hz"] = SoundAnalyzer::jsonNumber(report.fundamental, 3);
                sample["peak_before_dbfs"] = SoundAnalyzer::jsonNumber(SampleUtils::toDb(report.peakBefore), 2);
                sample["peak_after_dbfs"] = SoundAnalyzer::jsonNumber(SampleUtils::toDb(report.peakAfter), 2);
                samples << sample;
            }
            result["samples"] = samples;
        });

    functions << toolFunction(
        "change_sample_volume",
        "Tool \"Change volume\" of samples: add dB, multiply by a factor, or normalize the peak to a percentage of "
        "the full scale. There is no protection against clipping: check the peak with analyze_sample.",
        samplesText, samples,
        R"JSON("mode":{"type":"string","enum":["add_db","multiply","normalize"]},
           "value":{"type":"number","description":"dB to add (-200 to 200), factor (0 to 10) or peak in % of the full scale (0 to 100)"})JSON",
        { "mode", "value" }, []() { return new ToolChangeVolume(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolChangeVolume_parameters * params = dynamic_cast<ToolChangeVolume_parameters *>(parameters);
            int mode = readChoice(args, "mode", { "add_db", "multiply", "normalize" });
            params->setMode(mode);
            if (mode == 0)
                params->setAddValue(args.getDouble("value", -200, 200));
            else if (mode == 1)
                params->setMultiplyValue(args.getDouble("value", 0, 10));
            else
                params->setNormalizeValue(args.getDouble("value", 0.1, 100));
        });

    functions << toolFunction(
        "balance_stereo",
        "Tool \"Balance adjustment\": give the same level to the left and right samples of stereo pairs (level "
        "measured in the loop or in the steady part).",
        "Stereo samples", samples, "", {}, []() { return new ToolBalanceAdjustment(); });

    functions << toolFunction(
        "transpose_samples",
        "Tool \"Transpose\" of samples: resample the audio data so that it sounds higher or lower; the root key and "
        "the correction are updated so that the sample still plays at the right pitch in the instruments (the timbre "
        "and the duration change).",
        samplesText, samples,
        R"JSON("semitones":{"type":"number","minimum":-120,"maximum":120,"description":"Shift in semitones (decimals allowed)"})JSON",
        { "semitones" }, []() { return new ToolTransposeSmpl(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            dynamic_cast<ToolTransposeSmpl_parameters *>(parameters)->setSemiTones(args.getDouble("semitones", -120, 120));
        });

    functions << toolFunction(
        "link_stereo_samples",
        "Tool \"Find a link\": pair samples into stereo pairs according to their names (same name ending with L / R).",
        "Samples", samples, "", {}, []() { return new ToolLinkSample(); });

    functions << toolFunction(
        "unlink_stereo_samples",
        "Tool \"Unlink\": make the samples of stereo pairs independent (mono).",
        "Samples", samples, "", {}, []() { return new ToolUnlinkSample(); });

    functions << toolFunction(
        "convert_to_mono",
        "Tool \"Mono conversion\": convert stereo pairs into mono samples; the instruments using them are updated.",
        "Stereo samples", samples,
        R"JSON("mode":{"type":"string","enum":["sum","average","constant_power","left","right"],"description":"How the channels are merged (default average)"},
           "merge_divisions":{"type":"boolean","description":"Merge the left / right divisions of the instruments (default true)"})JSON",
        {}, []() { return new ToolMonoConversion(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolMonoConversion_parameters * params = dynamic_cast<ToolMonoConversion_parameters *>(parameters);
            params->setMode(readChoice(args, "mode", { "sum", "average", "constant_power", "left", "right" }, 1));
            params->setMergeDivisions(args.getBool("merge_divisions", true));
        });

    functions << toolFunction(
        "export_samples",
        "Tool \"Wav export\": write samples as wav files in a directory (a stereo pair gives one stereo file).",
        samplesText, samples,
        R"JSON("directory":{"type":"string","description":"Absolute path of an existing directory"})JSON",
        { "directory" }, []() { return new ToolSampleExport(); },
        [](const McpArguments &args, AbstractTool * tool, AbstractToolParameters *) {
            QString directory = args.getString("directory");
            if (!QFileInfo(directory).isDir())
                throw McpException("directory \"" + directory + "\" doesn't exist");
            dynamic_cast<ToolSampleExport *>(tool)->setDirectory(QFileInfo(directory).absoluteFilePath() + "/");
        });

    functions << toolFunction(
        "fast_edit_samples",
        "Tool \"Fast edit\" of samples: add a value to, or multiply, the root key, the pitch correction, the loop start "
        "or the loop end of several samples.",
        "Samples", samples,
        R"JSON("parameter":{"type":"string","enum":["root_key","pitch_correction","loop_start","loop_end"]},
           "operation":{"type":"string","enum":["add","multiply"]},
           "value":{"type":"number"})JSON",
        { "parameter", "operation", "value" }, []() { return new ToolFastEditSmpl(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolFastEditSmpl_parameters * params = dynamic_cast<ToolFastEditSmpl_parameters *>(parameters);
            static const AttributeType champs[4] = { champ_byOriginalPitch, champ_chPitchCorrection, champ_dwStartLoop, champ_dwEndLoop };
            params->setParameter(champs[readChoice(args, "parameter", { "root_key", "pitch_correction", "loop_start", "loop_end" })]);
            int operation = readChoice(args, "operation", { "add", "multiply" });
            params->setMode(operation);
            if (operation == 0)
                params->setAddValue(args.getInt("value", -1000000, 1000000));
            else
                params->setMultiplyValue(args.getDouble("value", -100, 100));
        });

    functions << toolFunction(
        "load_sample_settings_from_instruments",
        "Tool \"Load from instruments\": move into the samples a setting made in the instruments using them (root key "
        "override, coarse / fine tuning or loop offsets), and reset it in the instruments. All divisions using a "
        "sample must share the same value.",
        "Samples", samples,
        R"JSON("parameter":{"type":"string","enum":["root_key","tuning_semitones","tuning_cents","loop"]})JSON",
        { "parameter" }, []() { return new ToolLoadFromInst(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            dynamic_cast<ToolLoadFromInst_parameters *>(parameters)->setParameter(
                readChoice(args, "parameter", { "root_key", "tuning_semitones", "tuning_cents", "loop" }));
        });

    functions << toolFunction(
        "loop_crossfade",
        "Tool \"Loop crossfade\": crossfade the end of the loop with the audio before the loop start, to remove a click "
        "at the loop point.",
        samplesText, samples,
        R"JSON("duration_ms":{"type":"integer","minimum":1,"maximum":2000,"description":"Crossfade duration in ms"})JSON",
        { "duration_ms" }, []() { return new ToolLoopCrossfade(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            dynamic_cast<ToolLoopCrossfade_parameters *>(parameters)->setDurationMs(args.getInt("duration_ms", 1, 2000));
        });

    functions << toolFunction(
        "export_frequency_peaks",
        "Tool \"Export peaks\": write in a csv file the main frequency peaks of samples (measured in the loop).",
        "Samples", samples,
        R"JSON("file":{"type":"string","description":"Absolute path of the csv file to write"})JSON",
        { "file" }, []() { return new ToolFrequencyPeaks(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            QString file = QFileInfo(args.getString("file")).absoluteFilePath();
            if (!QFileInfo(QFileInfo(file).absolutePath()).isDir())
                throw McpException("the directory of \"" + file + "\" doesn't exist");
            SoundfontManager * sm = SoundfontManager::getInstance();
            QList<SampleFrequencyInfo> infos;
            foreach (EltID id, args.getIds("elements", { elementSmpl }))
            {
                QVector<float> data = sm->getDataFloat(id);
                QVector<float> vectFourier;
                int posMax;
                SampleFrequencyInfo info;
                info.name = sm->getQstr(id, champ_name);
                info.frequencies = SoundAnalyzer::computePeaks(data, sm->get(id, champ_dwSampleRate).dwValue,
                                                                 sm->get(id, champ_dwStartLoop).dwValue,
                                                                 sm->get(id, champ_dwEndLoop).dwValue, vectFourier, posMax);
                if (!info.frequencies.isEmpty())
                    infos << info;
            }
            ToolFrequencyPeaks_parameters * params = dynamic_cast<ToolFrequencyPeaks_parameters *>(parameters);
            params->setFilePath(file);
            params->setSampleFrequencies(infos);
        });
}

static void addInstrumentTools(QList<McpFunction *> &functions)
{
    QList<ElementType> instruments = { elementInst };

    functions << toolFunction(
        "detune",
        "Tool \"Detune\" (celeste): set the fine tuning of each division to produce beats with an instrument tuned "
        "normally, as an organ celeste stop. The number of beats per second is given at key 60 and changes by the "
        "coefficient at each octave.",
        "Instruments", instruments,
        R"JSON("beats_per_second":{"type":"number","minimum":-10,"maximum":10,"description":"Beats per second at key 60, the sign giving the direction (default 4)"},
           "coefficient":{"type":"number","minimum":0.5,"maximum":2,"description":"Division of the beats for each octave above (default 1)"})JSON",
        {}, []() { return new ToolCelesteTuning(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolCelesteTuning_parameters * params = dynamic_cast<ToolCelesteTuning_parameters *>(parameters);
            params->setBaseFrequency(args.getDouble("beats_per_second", -10, 10, 4));
            params->setCoefficient(args.getDouble("coefficient", 0.5, 2, 1));
        });

    functions << toolFunction(
        "auto_distribute_keys",
        "Tool \"Auto distribution\": set the key range of each division according to the root keys of the samples, "
        "so that the whole keyboard is covered.",
        "Instruments", instruments, "", {}, []() { return new ToolAutoDistribution(); });

    functions << toolFunction(
        "create_mixture",
        "Tool \"Mixture creation\" (organ): create a new instrument whose samples are computed by mixing, for each key, "
        "the samples of the instrument transposed to several harmonics (ranks). Each division of the mixture covers a "
        "key range with its own ranks (breaks of a mixture stop).",
        "One instrument (source of the sounds)", instruments,
        R"JSON("name":{"type":"string","description":"Name of the new instrument"},
           "density":{"type":"string","enum":["every_key","every_3_keys","every_6_keys"],"description":"Samples computed for every key, every 3 or every 6 keys (default every_3_keys)"},
           "stereo":{"type":"boolean","description":"Stereo samples (default true)"},
           "loop":{"type":"boolean","description":"Loop the samples (default true)"},
           "divisions":{"type":"array","description":"Key ranges and their ranks: [{\"min_key\":36,\"max_key\":60,\"ranks\":[{\"harmonic\":1,\"octave\":2},{\"harmonic\":3,\"octave\":1}]}]. harmonic: odd number from 1 to 27 (1 = octaves, 3 = fifths, 5 = thirds...); octave: 0 = the harmonic of the played key, 1 = one octave higher, -1 = one octave lower (harmonic 1 octave 0 = unison 8', octave 1 = 4', octave 2 = 2')"})JSON",
        { "name", "divisions" }, []() { return new ToolMixtureCreation(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolMixtureCreation_parameters * params = dynamic_cast<ToolMixtureCreation_parameters *>(parameters);
            QString name = args.getString("name").trimmed();
            if (name.isEmpty())
                throw McpException("the name cannot be empty");
            params->setInstrumentName(name);
            params->setDensityType(readDensity(args));
            params->setStereoSample(args.getBool("stereo", true));
            params->setLoopSample(args.getBool("loop", true));

            static const int maxOctaveIndexes[14] = { 10, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5 };
            QList<DivisionInfo> divisions;
            for (const QJsonValue value : args.getArray("divisions"))
            {
                McpArguments division(value.toObject());
                DivisionInfo info(division.getKey("min_key"), division.getKey("max_key"));
                for (const QJsonValue rankValue : division.getArray("ranks"))
                {
                    McpArguments rank(rankValue.toObject());
                    int harmonic = rank.getInt("harmonic", 1, 27);
                    if (harmonic % 2 == 0)
                        throw McpException("a harmonic must be an odd number");
                    int type = (harmonic - 1) / 2;
                    int octaveIndex = rank.getInt("octave", -3, 7) + 3;
                    if (octaveIndex > maxOctaveIndexes[type])
                        throw McpException(QString("octave too high for the harmonic %1").arg(harmonic));
                    info.addRank(type, octaveIndex);
                }
                if (info.getRanks().isEmpty())
                    throw McpException("each division needs at least one rank");
                divisions << info;
            }
            if (divisions.isEmpty())
                throw McpException("at least one division is required");
            params->setDivisions(divisions);
        });

    functions << toolFunction(
        "create_chords",
        "Tool \"Chords\": create a new instrument whose samples are chords computed from the samples of the "
        "instrument (each key plays a chord built on it).",
        "One instrument (source of the sounds)", instruments,
        R"JSON("name":{"type":"string","description":"Name of the new instrument"},
           "density":{"type":"string","enum":["every_key","every_3_keys","every_6_keys"],"description":"Default every_3_keys"},
           "stereo":{"type":"boolean","description":"Default true"},
           "loop":{"type":"boolean","description":"Default true"},
           "chord":{"type":"object","description":"{\"root\":true, \"third\":\"none|major|minor\", \"fifth\":\"none|perfect|diminished|augmented\", \"seventh\":\"none|minor|major|diminished\", \"ninth\":\"none|major|minor\", \"octave\":-3..3 (octave of the notes above the root), \"inversions\":number of inverted notes (lowered by an octave, from the ninth down), \"levels_db\":{\"root\":0,\"third\":-3,...} relative levels (0 or negative)}"})JSON",
        { "name", "chord" }, []() { return new ToolChords(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolChords_parameters * params = dynamic_cast<ToolChords_parameters *>(parameters);
            params->setInstrumentName(args.getString("name").trimmed());
            params->setDensityType(readDensity(args));
            params->setStereoSample(args.getBool("stereo", true));
            params->setLoopSample(args.getBool("loop", true));

            McpArguments chord(args.getObject("chord"));
            ChordInfo ci;
            ci.chordType1 = chord.getBool("root", true) ? 1 : 0;
            ci.chordType3 = readChoice(chord, "third", { "none", "major", "minor" }, 1);
            ci.chordType5 = readChoice(chord, "fifth", { "none", "perfect", "diminished", "augmented" }, 1);
            ci.chordType7 = readChoice(chord, "seventh", { "none", "minor", "major", "diminished" }, 0);
            ci.chordType9 = readChoice(chord, "ninth", { "none", "major", "minor" }, 0);
            ci.octave = chord.getInt("octave", -3, 3, 0);
            McpArguments levels(chord.contains("levels_db") ? chord.getObject("levels_db") : QJsonObject());
            ci.chordType1Attenuation = qRound(10 * levels.getDouble("root", -144, 0, 0));
            ci.chordType3Attenuation = qRound(10 * levels.getDouble("third", -144, 0, 0));
            ci.chordType5Attenuation = qRound(10 * levels.getDouble("fifth", -144, 0, 0));
            ci.chordType7Attenuation = qRound(10 * levels.getDouble("seventh", -144, 0, 0));
            ci.chordType9Attenuation = qRound(10 * levels.getDouble("ninth", -144, 0, 0));
            int inversions = chord.getInt("inversions", 0, 4, 0); // Given in the order 9, 7, 5, 3
            ci.chordType9Inversion = inversions-- > 0;
            ci.chordType7Inversion = inversions-- > 0;
            ci.chordType5Inversion = inversions-- > 0;
            ci.chordType3Inversion = inversions-- > 0;
            params->setChordConfiguration(ci);
        });

    functions << toolFunction(
        "create_missing_samples",
        "Tool \"Create missing samples\": add samples at keys having no sample in instruments. A new sample is the "
        "sample of the closest root key transposed (the higher one at equal distance), the levels of its harmonics "
        "being corrected in dB by (key - closest) / (second - closest) times the difference of level with a second "
        "root key: the closest one on the other side of the key (interpolation), otherwise the next one on the same "
        "side (extrapolation). Only the harmonics found in both root keys are corrected; if none is found, the level "
        "is corrected with the steady loudness. The new sample is looped automatically if both root keys are looped. "
        "Its division copies the division of the transposed sample (attenuation reduced if the sample had to be "
        "attenuated below the full scale) and the key ranges around are adjusted as by the tool \"auto distribution\" "
        "(existing ranges can only shrink). Instruments with several velocity ranges are refused.",
        "Instruments having at least two root keys", instruments,
        R"JSON("keys":{"type":"array","items":{"type":["integer","string"]},"description":"Keys to create (number or name such as \"C#4\"); keys already having a sample are ignored"},
           "max_spacing":{"type":"integer","minimum":1,"maximum":24,"description":"Instead of keys: create the keys needed so that two consecutive root keys are at most this number of semitones apart, evenly spread"})JSON",
        {}, []() { return new ToolMissingSamples(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolMissingSamples_parameters * params = dynamic_cast<ToolMissingSamples_parameters *>(parameters);
            if (args.contains("keys") == args.contains("max_spacing"))
                throw McpException("one of the arguments \"keys\" and \"max_spacing\" is required");
            if (args.contains("keys"))
            {
                QList<int> keys;
                for (const QJsonValue value : args.getArray("keys"))
                {
                    int key = McpUtils::readKey(value, "keys");
                    if (!keys.contains(key))
                        keys << key;
                }
                if (keys.isEmpty())
                    throw McpException("argument \"keys\" is empty");
                std::sort(keys.begin(), keys.end());
                params->setMode(ToolMissingSamples_parameters::MODE_KEYS);
                params->setKeys(keys);
            }
            else
            {
                params->setMode(ToolMissingSamples_parameters::MODE_MAX_SPACING);
                params->setMaxSpacing(args.getInt("max_spacing", 1, 24));
            }
        },
        [](AbstractTool * tool, QJsonObject &result) {
            QJsonArray created;
            for (const ToolMissingSamples::Creation &creation : dynamic_cast<ToolMissingSamples *>(tool)->getCreations())
            {
                QJsonObject item;
                item["instrument"] = McpUtils::brief(creation.idInst);
                item["key"] = creation.key;
                item["transposed_root_key"] = creation.closestKey;
                item["second_root_key"] = creation.secondKey;
                QJsonArray samples;
                for (const EltID &id : creation.samples)
                    samples << McpUtils::brief(id);
                item["samples"] = samples;
                created << item;
            }
            result["created"] = created;
        });

    functions << toolFunction(
        "transpose_instruments",
        "Tool \"Transpose\" of instruments: shift the pitch of the instruments (root keys and tuning of the divisions), "
        "the key ranges being possibly shifted so that each key keeps playing the same samples.",
        "Instruments", instruments,
        R"JSON("semitones":{"type":"number","minimum":-120,"maximum":120,"description":"Shift in semitones (decimals = cents)"},
           "adapt_key_ranges":{"type":"boolean","description":"Shift the key ranges (default true)"})JSON",
        { "semitones" }, []() { return new ToolTranspose(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolTranspose_parameters * params = dynamic_cast<ToolTranspose_parameters *>(parameters);
            params->setSemiTones(args.getDouble("semitones", -120, 120));
            params->setAdaptKeyRanges(args.getBool("adapt_key_ranges", true));
        });

    functions << toolFunction(
        "natural_release",
        "Tool \"Natural release\": set the release duration of each division according to its key, the duration "
        "decreasing with the pitch as for acoustic instruments, and optionally a pitch drop during the release.",
        "Instruments", instruments,
        R"JSON("duration_at_c2":{"type":"number","minimum":0.001,"maximum":10,"description":"Release duration in seconds at key 36 (default 0.3)"},
           "division_per_octave":{"type":"number","minimum":1,"maximum":10,"description":"The duration is divided by this number at each octave above (default 2)"},
           "detune_semitones":{"type":"number","minimum":-12,"maximum":12,"description":"Pitch change during the release (default 0)"})JSON",
        {}, []() { return new ToolRelease(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolRelease_parameters * params = dynamic_cast<ToolRelease_parameters *>(parameters);
            params->setBaseDuration(args.getDouble("duration_at_c2", 0.001, 10, 0.3));
            params->setEvolution(args.getDouble("division_per_octave", 1, 10, 2));
            params->setDetune(args.getDouble("detune_semitones", -12, 12, 0));
        });

    functions << toolFunction(
        "add_default_modulators",
        "Tool \"Default modulators\": add to the global division of the instruments copies of the default modulators "
        "of the soundfont specification (velocity to attenuation, velocity to filter, modulation wheel to vibrato, "
        "volume, pan, expression, reverb, chorus, pitch wheel...), to override them or to disable them.",
        "Instruments", instruments,
        R"JSON("numbers":{"type":"array","items":{"type":"integer"},"description":"Numbers of the default modulators (1-based, in the order of the specification), all by default"},
           "disable":{"type":"boolean","description":"true: amounts set to 0 so that the default modulators are disabled (default false)"})JSON",
        {}, []() { return new ToolDefaultMod(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            int number = 0;
            const ModulatorData * defaults = ModulatorData::getDefaultModulators(
                ContextManager::configuration()->getModulatorVelToFilterType(), number);
            QList<int> numbers;
            if (args.contains("numbers"))
            {
                for (const QJsonValue value : args.getArray("numbers"))
                {
                    int n = value.toInt(-1);
                    if (n < 1 || n > number)
                        throw McpException(QString("modulator numbers must be between 1 and %1").arg(number));
                    numbers << n;
                }
            }
            else
                for (int i = 1; i <= number; i++)
                    numbers << i;

            QList<ModulatorData> modulators;
            bool disable = args.getBool("disable", false);
            foreach (int n, numbers)
            {
                ModulatorData data = defaults[n - 1];
                data.index = 0;
                if (disable)
                    data.amount = 0;
                modulators << data;
            }
            dynamic_cast<ToolDefaultMod_parameters *>(parameters)->setModulators(modulators);
        });
}

static void addInstrumentPresetTools(QList<McpFunction *> &functions)
{
    QList<ElementType> instPrst = { elementInst, elementPrst };

    functions << toolFunction(
        "change_attenuation",
        "Tool \"Change attenuation\": add a value to the attenuation of instruments or presets (global and divisions), "
        "changing their volume while keeping the balance between divisions.",
        "Instruments or presets (same type)", instPrst,
        R"JSON("offset_db":{"type":"number","minimum":-57.6,"maximum":57.6,"description":"dB added to the attenuation (positive = quieter)"})JSON",
        { "offset_db" }, []() { return new ToolChangeAttenuation(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            dynamic_cast<ToolChangeAttenuation_parameters *>(parameters)->setValue(args.getDouble("offset_db", -57.6, 57.6));
        });

    functions << toolFunction(
        "set_parameter_by_key",
        "Tool \"Key-based configuration\": change a parameter of all divisions according to their key (middle of "
        "their key range), following a curve. Either give points [[key, value], ...] linearly interpolated, or a "
        "pattern between min and max. The values are in the units of list_parameters and are added to, multiply or "
        "replace the current values. Example: release decreasing with the key: {\"parameter\":\"vol_env_release\","
        "\"operation\":\"replace\",\"points\":[[36,2.0],[96,0.4]]}.",
        "Instruments or presets (same type)", instPrst,
        R"JSON("parameter":{"type":"string","description":"Parameter name (key_range, velocity_range, attenuation, pan, root_key, tuning, filter, envelopes, LFOs, sends)"},
           "operation":{"type":"string","enum":["add","multiply","replace"]},
           "points":{"type":"array","items":{"type":"array"},"description":"Points [key, value]"},
           "pattern":{"type":"object","description":"Instead of points: {\"type\":\"linear_ascending|linear_descending|exponential_ascending|exponential_descending|random\", \"min\":value, \"max\":value, \"min_key\":0, \"max_key\":127, \"stiffness\":0-100 (exponential curves; for random: distribution)}"},
           "velocity_range":{"type":"array","description":"[min, max]: only the divisions whose velocity range is inside (default [0, 127])"})JSON",
        { "parameter", "operation" }, []() { return new ToolGlobalSettings(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolGlobalSettings_parameters * params = dynamic_cast<ToolGlobalSettings_parameters *>(parameters);
            static const QList<AttributeType> allowed = {
                champ_initialAttenuation, champ_pan, champ_overridingRootKey, champ_keyRange, champ_velRange, champ_coarseTune,
                champ_fineTune, champ_scaleTuning, champ_initialFilterFc, champ_initialFilterQ, champ_delayVolEnv,
                champ_attackVolEnv, champ_holdVolEnv, champ_decayVolEnv, champ_sustainVolEnv, champ_releaseVolEnv,
                champ_keynumToVolEnvHold, champ_keynumToVolEnvDecay, champ_delayModEnv, champ_attackModEnv, champ_holdModEnv,
                champ_decayModEnv, champ_sustainModEnv, champ_releaseModEnv, champ_modEnvToPitch, champ_modEnvToFilterFc,
                champ_keynumToModEnvHold, champ_keynumToModEnvDecay, champ_delayModLFO, champ_freqModLFO, champ_modLfoToPitch,
                champ_modLfoToFilterFc, champ_modLfoToVolume, champ_delayVibLFO, champ_freqVibLFO, champ_vibLfoToPitch,
                champ_chorusEffectsSend, champ_reverbEffectsSend };
            AttributeType champ = McpAttributes::getChamp(args.getString("parameter"));
            if (!allowed.contains(champ))
                throw McpException("this parameter cannot be changed by this tool");
            params->setAttribute(champ);
            params->setModifType(readChoice(args, "operation", { "add", "multiply", "replace" }));

            // Curve in real values for the 128 keys, then normalized
            QVector<double> values(128);
            if (args.contains("points"))
            {
                QList<QPointF> points = readPoints(args, "points", true);
                for (int key = 0; key < 128; key++)
                    values[key] = interpolate(points, key);
            }
            else if (args.contains("pattern"))
            {
                McpArguments pattern(args.getObject("pattern"));
                int type = readChoice(pattern, "type", { "linear_ascending", "linear_descending", "exponential_ascending",
                                                        "exponential_descending", "random" });
                double min = pattern.getDouble("min", -1e6, 1e6);
                double max = pattern.getDouble("max", -1e6, 1e6);
                int minKey = pattern.getKey("min_key", 0);
                int maxKey = pattern.getKey("max_key", 127);
                if (maxKey <= minKey)
                    throw McpException("max_key must be greater than min_key");
                double stiffness = pattern.getDouble("stiffness", 0, 100, 50);
                for (int i = 0; i < 128; i++)
                {
                    // Same curves as the dialog, in [0, 1]
                    double x;
                    bool ascending = (type == 0 || type == 2);
                    if (type == 4)
                    {
                        double v = (QRandomGenerator::global()->bounded(2000)) / 1000. - 1;
                        double e = qExp((50 - stiffness) / 15);
                        double c = (v < 0 ? -1 : 1) * qPow(qAbs(v), e);
                        x = (c + 1) / 2;
                    }
                    else if (i < minKey)
                        x = ascending ? 0 : 1;
                    else if (i >= maxKey)
                        x = ascending ? 1 : 0;
                    else if (type == 0)
                        x = static_cast<double>(i - minKey) / (maxKey - minKey);
                    else if (type == 1)
                        x = 1. - static_cast<double>(i - minKey) / (maxKey - minKey);
                    else
                    {
                        double b = 1. + 1. / (101. - stiffness);
                        if (type == 2)
                            x = (qPow(b, i - minKey) - 1) / (qPow(b, maxKey - minKey) - 1);
                        else
                            x = 1. - (qPow(b, minKey - i) - 1) / (qPow(b, minKey - maxKey) - 1);
                    }
                    values[i] = min + x * (max - min);
                }
            }
            else
                throw McpException("\"points\" or \"pattern\" is required");

            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            QVector<float> normalized(128);
            for (int i = 0; i < 128; i++)
                normalized[i] = max > min ? static_cast<float>((values[i] - min) / (max - min)) : 0.f;
            params->setPattern(0);
            params->setMin(min);
            params->setMax(max);
            params->setValues(normalized);

            int minVel = 0, maxVel = 127;
            if (args.contains("velocity_range"))
            {
                QJsonArray range = args.getArray("velocity_range");
                if (range.count() != 2)
                    throw McpException("velocity_range must be [min, max]");
                minVel = qBound(0, range[0].toInt(), 127);
                maxVel = qBound(0, range[1].toInt(), 127);
            }
            params->setMinVel(minVel);
            params->setMaxVel(maxVel);
        });

    functions << toolFunction(
        "spatialize",
        "Tool \"Sound spatialization\": set the pan of the divisions according to their key. Either give positions "
        "[[key, pan], ...] (pan from -50 = left to 50 = right, linearly interpolated), or a pattern as in the dialog: "
        "ascending (low keys on the right, high keys on the left), descending (the opposite), hollow, spike or random, "
        "with a number of divisions (keys alternating "
        "between several positions, as the pipes of an organ), the spreading (100 = whole stereo width), the filling "
        "and the offset (center when the spreading is reduced). Stereo pairs keep their width.",
        "Instruments or presets (same type)", instPrst,
        R"JSON("positions":{"type":"array","items":{"type":"array"},"description":"Points [key, pan]"},
           "pattern":{"type":"string","enum":["ascending","descending","hollow","spike","random"]},
           "divisions":{"type":"integer","minimum":1,"maximum":10,"description":"Default 1"},
           "spreading":{"type":"integer","minimum":0,"maximum":100,"description":"Default 100"},
           "filling":{"type":"integer","minimum":0,"maximum":100,"description":"Default 100"},
           "offset":{"type":"integer","minimum":0,"maximum":100,"description":"Default 50"},
           "flip":{"type":"boolean"},"flop":{"type":"boolean"})JSON",
        {}, []() { return new ToolSoundSpatialization(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolSoundSpatialization_parameters * params = dynamic_cast<ToolSoundSpatialization_parameters *>(parameters);
            QMap<int, double> mapPan;
            if (args.contains("positions"))
            {
                QList<QPointF> points = readPoints(args, "positions", true);
                for (int key = 0; key < 128; key++)
                    mapPan[key] = qBound(0.0, (interpolate(points, key) + 50) / 100, 1.0);
            }
            else if (args.contains("pattern"))
            {
                // Key span of the divisions, as in the dialog
                IdList ids = args.getIds("elements", { elementInst, elementPrst });
                bool isInst = ids[0].typeElement == elementInst;
                SoundfontManager * sm = SoundfontManager::getInstance();
                int minKey = 127, maxKey = 0;
                foreach (EltID id, ids.getSelectedIds(isInst ? elementInst : elementPrst))
                {
                    id.typeElement = isInst ? elementInstSmpl : elementPrstInst;
                    foreach (int i, sm->getSiblings(id))
                    {
                        id.indexElt2 = i;
                        if (sm->isSet(id, champ_keyRange))
                        {
                            RangesType range = sm->get(id, champ_keyRange).rValue;
                            minKey = qMin(minKey, static_cast<int>(range.byLo));
                            maxKey = qMax(maxKey, static_cast<int>(range.byHi));
                        }
                    }
                }
                if (minKey > maxKey)
                {
                    minKey = 0;
                    maxKey = 127;
                }

                int pattern = readChoice(args, "pattern", { "ascending", "descending", "hollow", "spike", "random" });
                int nbDiv = args.getInt("divisions", 1, 10, 1);
                int spreading = args.getInt("spreading", 0, 100, 100);
                int filling = args.getInt("filling", 0, 100, 100);
                int offset = args.getInt("offset", 0, 100, 50);
                bool flip = args.getBool("flip", false);
                bool flop = args.getBool("flop", false);
                if (nbDiv == 1 && (filling == 0 || maxKey == minKey))
                    throw McpException("with one division, the filling must be positive and the key span not empty");

                SpatializationCurve curve(minKey, maxKey);
                int nbNotes = maxKey - minKey + 1;
                QVector<double> x(nbNotes);
                if (pattern <= 1)
                    for (int i = 0; i < nbNotes; i++)
                        x[i] = curve.spaceLinear(i + minKey, nbDiv, spreading, filling, offset, flip, pattern == 0);
                else if (pattern <= 3)
                    for (int i = 0; i < nbNotes; i++)
                        x[i] = curve.spaceCurve(i + minKey, nbDiv, spreading, filling, offset, flip, flop, pattern == 2);
                else
                {
                    QList<int> remaining;
                    for (int i = 0; i < nbNotes; i++)
                        remaining << i;
                    for (int i = 0; i < nbNotes; i++)
                        x[remaining.takeAt(static_cast<int>(QRandomGenerator::global()->generate() % static_cast<quint32>(remaining.size())))] =
                            curve.spaceLinear(i + minKey, nbDiv, spreading, filling, offset, flip, false);
                }
                for (int i = 0; i < nbNotes; i++)
                    mapPan[minKey + i] = x[i];
            }
            else
                throw McpException("\"positions\" or \"pattern\" is required");
            params->setMapPan(mapPan);
        });

    functions << toolFunction(
        "duplicate_divisions",
        "Tool \"Division duplication\": split divisions so that there is one division per key, and / or one division "
        "per velocity range (the divisions are copied with their parameters).",
        "Instruments or presets (same type)", instPrst,
        R"JSON("by_key":{"type":"boolean","description":"One division for each key (default false)"},
           "velocity_ranges":{"type":"array","items":{"type":"array"},"description":"Velocity ranges [[0,63],[64,127]]: one division for each range"})JSON",
        {}, []() { return new ToolDivisionDuplication(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolDivisionDuplication_parameters * params = dynamic_cast<ToolDivisionDuplication_parameters *>(parameters);
            QList<QPair<int, int> > ranges;
            if (args.contains("velocity_ranges"))
            {
                for (const QJsonValue value : args.getArray("velocity_ranges"))
                {
                    QJsonArray range = value.toArray();
                    if (range.count() != 2)
                        throw McpException("each velocity range must be [min, max]");
                    int a = qBound(0, range[0].toInt(), 127), b = qBound(0, range[1].toInt(), 127);
                    ranges << QPair<int, int>(qMin(a, b), qMax(a, b));
                }
            }
            bool byKey = args.getBool("by_key", false);
            if (!byKey && ranges.isEmpty())
                throw McpException("nothing to do: give \"by_key\" or \"velocity_ranges\"");
            params->setDuplicKey(byKey);
            params->setDuplicVel(!ranges.isEmpty());
            params->setVelocityRanges(ranges);
        });

    functions << toolFunction(
        "remove_modulators",
        "Tool \"Remove modulators\": remove all modulators of instruments, presets (global and divisions) or of all "
        "instruments and presets of soundfonts.",
        "Instruments, presets or soundfonts (same type)", { elementInst, elementPrst, elementSf2 }, "", {},
        []() { return new ToolRemoveMods(); });
}

static void addPresetAndSoundfontTools(QList<McpFunction *> &functions)
{
    functions << toolFunction(
        "merge_samples_per_key",
        "Tool \"Merge samples\": render a preset key by key (all its layers mixed) into new samples, and create a new "
        "instrument and a new preset using them.",
        "One preset", { elementPrst },
        R"JSON("name":{"type":"string","description":"Name of the new instrument and preset"},
           "density":{"type":"string","enum":["every_key","every_3_keys","every_6_keys"],"description":"Default every_3_keys"},
           "stereo":{"type":"boolean","description":"Default true"},
           "loop":{"type":"boolean","description":"Default true"},
           "sustain_s":{"type":"integer","minimum":1,"maximum":30,"description":"Duration of the sustain rendered (default 5)"},
           "release_s":{"type":"integer","minimum":0,"maximum":30,"description":"Duration of the release rendered (default 2)"})JSON",
        { "name" }, []() { return new ToolMergeSamples(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolMergeSamples_parameters * params = dynamic_cast<ToolMergeSamples_parameters *>(parameters);
            params->setPresetName(args.getString("name").trimmed());
            params->setDensityType(readDensity(args));
            params->setStereoSample(args.getBool("stereo", true));
            params->setLoopSample(args.getBool("loop", true));
            params->setSampleSustainDuration(args.getInt("sustain_s", 1, 30, 5));
            params->setSampleReleaseDuration(args.getInt("release_s", 0, 30, 2));
        });

    functions << toolFunction(
        "remove_unused_elements",
        "Tool \"Remove unused elements\": remove the instruments used by no presets, then the samples used by no "
        "instruments.",
        "Soundfonts", { elementSf2 }, "", {}, []() { return new ToolCleanUnused(); });

    functions << toolFunction(
        "remove_duplicate_samples",
        "Tool \"Remove duplicate samples\": find identical samples (data and settings), make the instruments use only "
        "one of them and remove the others.",
        "Soundfonts", { elementSf2 }, "", {}, []() { return new ToolCleanDuplicates(); });

    functions << toolFunction(
        "export_soundfonts",
        "Tool \"Export\": export presets (with what they use) into new files: sf2, sf3 (compressed), sfz, dls or csv. "
        "Presets of several soundfonts can be merged into one file.",
        "Presets to export, or soundfonts (all their presets)", { elementSf2, elementPrst },
        R"JSON("directory":{"type":"string","description":"Absolute path of an existing directory"},
           "format":{"type":"string","enum":["sf2","sf3","sfz","csv","dls"]},
           "quality":{"type":"string","enum":["low","medium","high"],"description":"sf3 compression quality (default medium)"},
           "one_file_per_preset":{"type":"boolean","description":"sf2, sf3 and dls only (default false)"},
           "sfz_preset_prefix":{"type":"boolean","description":"sfz: preset number as prefix of the files (default true)"},
           "sfz_bank_directory":{"type":"boolean","description":"sfz: one directory per bank (default false)"},
           "sfz_gm_sort":{"type":"boolean","description":"sfz: General MIDI classification (default false)"},
           "csv_raw_values":{"type":"boolean","description":"csv: raw values of the soundfont format (default false)"})JSON",
        { "directory", "format" }, []() { return new ToolSoundfontExport(); },
        [](const McpArguments &args, AbstractTool *, AbstractToolParameters * parameters) {
            ToolSoundfontExport_parameters * params = dynamic_cast<ToolSoundfontExport_parameters *>(parameters);
            QString directory = args.getString("directory");
            if (!QFileInfo(directory).isDir())
                throw McpException("directory \"" + directory + "\" doesn't exist");
            params->setDirectory(QFileInfo(directory).absoluteFilePath());
            int format = readChoice(args, "format", { "sf2", "sf3", "sfz", "csv", "dls" });
            params->setFormat(format);
            params->setQuality(readChoice(args, "quality", { "low", "medium", "high" }, 1));
            bool filePreset = args.getBool("one_file_per_preset", false);
            params->setFilePreset(filePreset);
            params->setPresetPrefix(args.getBool("sfz_preset_prefix", true));
            params->setBankDirectory(args.getBool("sfz_bank_directory", false));
            params->setGmSort(args.getBool("sfz_gm_sort", false));
            params->setCsvRaw(args.getBool("csv_raw_values", false));

            QMap<int, QList<int> > presets;
            SoundfontManager * sm = SoundfontManager::getInstance();
            foreach (EltID id, args.getIds("elements", { elementSf2, elementPrst }))
            {
                if (id.typeElement == elementSf2)
                {
                    EltID idPrst(elementPrst, id.indexSf2);
                    foreach (int i, sm->getSiblings(idPrst))
                        if (!presets[id.indexSf2].contains(i))
                            presets[id.indexSf2] << i;
                }
                else if (!presets[id.indexSf2].contains(id.indexElt))
                    presets[id.indexSf2] << id.indexElt;
            }
            foreach (int key, presets.keys())
                if (presets[key].isEmpty())
                    presets.remove(key);
            if (presets.isEmpty())
                throw McpException("no presets to export");
            if (presets.count() > 1 && !filePreset)
            {
                if (presets.count() > 128)
                    throw McpException("at most 128 soundfonts can be merged");
                foreach (QList<int> list, presets.values())
                    if (list.count() > 128)
                        throw McpException("at most 128 presets per soundfont can be merged");
            }
            params->setSelectedPresets(presets);
        });

    functions << new McpFunction(
        "get_preset_list", McpFunction::CATEGORY_TOOLS, true,
        "Tool \"Preset list\": list of the presets of a soundfont (bank, preset number, name) as a csv text (tab "
        "separated) or an html table.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "format":{"type":"string","enum":["csv","html"],"description":"Default csv"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            bool html = readChoice(args, "format", { "csv", "html" }, 0) == 1;
            SoundfontManager * sm = SoundfontManager::getInstance();
            QMap<int, QMap<int, QString> > list;
            EltID id(elementPrst, idSf2.indexSf2);
            foreach (int i, sm->getSiblings(id))
            {
                id.indexElt = i;
                list[sm->get(id, champ_wBank).wValue][sm->get(id, champ_wPreset).wValue] = sm->getQstr(id, champ_name);
            }
            QStringList lines;
            foreach (int bank, list.keys())
                foreach (int preset, list[bank].keys())
                    lines << (html ? QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>").arg(bank).arg(preset).arg(list[bank][preset].toHtmlEscaped()) :
                                     QString("%1\t%2\t%3").arg(bank).arg(preset).arg(list[bank][preset]));
            if (html)
                return QString("<table><thead><tr><th>Bank</th><th>Preset</th><th>Name</th></tr></thead><tbody>") +
                       lines.join("") + "</tbody></table>";
            return lines.join("\n");
        });

    functions << new McpFunction(
        "get_parameter_map", McpFunction::CATEGORY_TOOLS, true,
        "Tool \"Display parameters\": table of parameters in all divisions of an instrument or a preset, with their "
        "key and velocity ranges. A cell is the value set in the division, null if not set (then the global value, "
        "otherwise the default value applies: both are given). Without \"parameters\", all parameters set somewhere "
        "in the element are listed. For an instrument, include_sample adds the properties of the sample of each "
        "division (root key, pitch correction, loop, length, sample rate, channel). describe_key gives the values "
        "combined for a key.",
        R"JSON({"properties":{
            "element":{"type":"string","description":"Instrument or preset"},
            "parameters":{"type":"array","items":{"type":"string"},"description":"Parameter names (see list_parameters)"},
            "parameter":{"type":"string","description":"A single parameter name (same as parameters with one name)"},
            "include_sample":{"type":"boolean","description":"Instrument only: add the sample properties (default false)"}},
            "required":["element"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("element", { elementInst, elementPrst });
            bool isPrst = id.typeElement == elementPrst;
            SoundfontManager * sm = SoundfontManager::getInstance();
            EltID idDiv(isPrst ? elementPrstInst : elementInstSmpl, id.indexSf2, id.indexElt);
            QList<int> divisionIndexes = sm->getSiblings(idDiv);

            // Parameters listed
            QStringList names;
            if (args.contains("parameter"))
                names << args.getString("parameter");
            if (args.contains("parameters"))
            {
                for (const QJsonValue value : args.getArray("parameters"))
                {
                    if (!value.isString())
                        throw McpException("\"parameters\" must contain parameter names");
                    names << value.toString();
                }
            }
            foreach (QString name, names)
                if (McpAttributes::getChamp(name) == champ_unknown)
                    throw McpException("unknown parameter \"" + name + "\" (see list_parameters)");
            if (names.isEmpty())
            {
                // All parameters set in the global division or in a division
                foreach (QString name, McpAttributes::getNames())
                {
                    if (name == "key_range" || name == "velocity_range")
                        continue;
                    bool isSet = !McpAttributes::getValue(id, name).isNull();
                    for (int i = 0; i < divisionIndexes.count() && !isSet; i++)
                    {
                        idDiv.indexElt2 = divisionIndexes[i];
                        isSet = !McpAttributes::getValue(idDiv, name).isNull();
                    }
                    if (isSet)
                        names << name;
                }
            }
            names.removeAll("key_range");
            names.removeAll("velocity_range");
            names.removeDuplicates();
            bool includeSample = args.getBool("include_sample", false);
            if (includeSample && isPrst)
                throw McpException("include_sample is only for instruments");

            QStringList columns = { "division", isPrst ? "instrument" : "sample", "key_range", "velocity_range" };
            columns << names;
            if (includeSample)
                columns << "sample_root_key" << "sample_pitch_correction" << "sample_loop_start" << "sample_loop_end"
                        << "sample_length" << "sample_rate" << "sample_channel";

            QJsonObject global, defaults;
            foreach (QString name, names)
            {
                global[name] = McpAttributes::getValue(id, name);
                if (isPrst && McpAttributes::isInstrumentOnly(name))
                    defaults[name] = QJsonValue();
                else if (McpAttributes::isOffset(name))
                    defaults[name] = 0;
                else
                    defaults[name] = Utils::roundSignificant(AttributeBase::getDefaultRealValue(McpAttributes::getChamp(name), isPrst));
            }

            QJsonArray rows;
            foreach (int i, divisionIndexes)
            {
                idDiv.indexElt2 = i;
                QJsonArray row;
                row.append(McpUtils::idToString(idDiv));
                EltID idElement(isPrst ? elementInst : elementSmpl, id.indexSf2,
                                sm->get(idDiv, isPrst ? champ_instrument : champ_sampleID).wValue);
                row.append(McpUtils::idToString(idElement));
                RangesType keyRange = sm->isSet(idDiv, champ_keyRange) ? sm->get(idDiv, champ_keyRange).rValue :
                                          AttributeBase::getDefaultStoredValue(champ_keyRange, false).rValue;
                RangesType velRange = sm->isSet(idDiv, champ_velRange) ? sm->get(idDiv, champ_velRange).rValue :
                                          AttributeBase::getDefaultStoredValue(champ_velRange, false).rValue;
                row.append(QJsonArray({ keyRange.byLo, keyRange.byHi }));
                row.append(QJsonArray({ velRange.byLo, velRange.byHi }));
                foreach (QString name, names)
                    row.append(McpAttributes::getValue(idDiv, name));
                if (includeSample)
                {
                    row.append(sm->get(idElement, champ_byOriginalPitch).bValue);
                    row.append(sm->get(idElement, champ_chPitchCorrection).cValue);
                    row.append(static_cast<double>(sm->get(idElement, champ_dwStartLoop).dwValue));
                    row.append(static_cast<double>(sm->get(idElement, champ_dwEndLoop).dwValue));
                    row.append(static_cast<double>(sm->get(idElement, champ_dwLength).dwValue));
                    row.append(static_cast<double>(sm->get(idElement, champ_dwSampleRate).dwValue));
                    row.append(McpUtils::sampleChannel(sm->get(idElement, champ_sfSampleType).sfLinkValue));
                }
                rows.append(row);
            }

            QJsonObject result = McpUtils::brief(id);
            result["columns"] = QJsonArray::fromStringList(columns);
            result["divisions"] = rows;
            result["global"] = global;
            result["defaults"] = defaults;
            result["note"] = QString("null cell: not set in the division, the global value applies, otherwise the "
                                     "default value") + (isPrst ? "; preset values are added to or multiply the "
                                     "instrument values (see list_parameters)" : "");
            return result;
        });
}

void McpFunctions::addToolFunctions(QList<McpFunction *> &functions)
{
    addSampleTools(functions);
    addInstrumentTools(functions);
    addInstrumentPresetTools(functions);
    addPresetAndSoundfontTools(functions);
}
