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
#include "soundanalyzer.h"
#include "offlinerenderer.h"
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

static double keyFrequency(double key)
{
    return 440.0 * std::pow(2.0, (key - 69) / 12);
}

static QByteArray toPng(const QImage &image)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return bytes;
}

static QJsonValue number(double value, int decimals)
{
    if (!std::isfinite(value))
        return QJsonValue();
    double factor = std::pow(10.0, decimals);
    return std::round(value * factor) / factor;
}

static EltID getLinkedSample(EltID idSmpl)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    SFSampleLink link = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
    if (link == monoSample || link == RomMonoSample)
        return EltID();
    EltID idLinked(elementSmpl, idSmpl.indexSf2, sm->get(idSmpl, champ_wSampleLink).wValue);
    return sm->isValid(idLinked) ? idLinked : EltID();
}

// Analyzer of a sample, the pitch being searched around the root key
static SoundAnalyzer * createAnalyzer(EltID idSmpl)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
    int correction = sm->get(idSmpl, champ_chPitchCorrection).cValue;
    double expected = keyFrequency(rootKey) * std::pow(2.0, -correction / 1200.0);
    return new SoundAnalyzer(sm->getDataFloat(idSmpl), sm->get(idSmpl, champ_dwSampleRate).dwValue,
                             sm->get(idSmpl, champ_dwStartLoop).dwValue, sm->get(idSmpl, champ_dwEndLoop).dwValue, expected);
}

// Pitch section completed with the tuning of the sample
static QJsonObject samplePitch(EltID idSmpl, SoundAnalyzer &analyzer)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QJsonObject pitch = analyzer.getPitch();
    int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
    int correction = sm->get(idSmpl, champ_chPitchCorrection).cValue;
    pitch["root_key"] = rootKey;
    pitch["pitch_correction"] = correction;
    double f0 = analyzer.getFundamental();
    if (f0 > 0)
    {
        double deviation = 1200 * std::log2(f0 / keyFrequency(rootKey));
        pitch["deviation_from_root_key_cents"] = number(deviation, 1);
        pitch["tuning_error_cents"] = number(deviation + correction, 1);
        double key = 69 + 12 * std::log2(f0 / 440.0);
        int nearest = qRound(key);
        pitch["suggested_root_key"] = nearest;
        pitch["suggested_pitch_correction"] = qBound(-99, qRound(-100 * (key - nearest)), 99);
        pitch["tuning_note"] = "tuning_error_cents: pitch error when the sample is played at its root key with its "
                               "current correction (0 = in tune); a sample a little sharp needs a negative correction";
    }
    return pitch;
}

static QJsonObject analyzeSample(EltID idSmpl, QStringList sections, int maxHarmonics)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QScopedPointer<SoundAnalyzer> analyzer(createAnalyzer(idSmpl));

    QJsonObject result = McpUtils::brief(idSmpl);
    result["sample_rate"] = static_cast<double>(sm->get(idSmpl, champ_dwSampleRate).dwValue);
    result["length"] = static_cast<double>(sm->get(idSmpl, champ_dwLength).dwValue);
    if (sections.contains("regions"))
        result["regions"] = analyzer->getRegions();
    if (sections.contains("levels"))
        result["levels"] = analyzer->getLevels();
    if (sections.contains("pitch"))
        result["pitch"] = samplePitch(idSmpl, *analyzer);
    if (sections.contains("harmonics"))
        result["harmonics"] = analyzer->getHarmonics(maxHarmonics);
    if (sections.contains("spectrum"))
        result["spectrum"] = analyzer->getSpectrum();
    if (sections.contains("envelope"))
        result["envelope"] = analyzer->getEnvelope();
    if (sections.contains("loop"))
        result["loop"] = analyzer->getLoop();
    if (sections.contains("quality"))
        result["quality"] = analyzer->getQuality();

    EltID idLinked = getLinkedSample(idSmpl);
    if (sections.contains("stereo") && idLinked.typeElement == elementSmpl)
    {
        SFSampleLink link = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
        bool isLeft = (link == leftSample || link == RomLeftSample);
        QVector<float> data = sm->getDataFloat(idSmpl), linkedData = sm->getDataFloat(idLinked);
        QJsonObject regions = analyzer->getRegions();
        QJsonArray part = regions.value("analyzed_part_s").toArray();
        quint32 rate = sm->get(idSmpl, champ_dwSampleRate).dwValue;
        quint32 start = static_cast<quint32>(part[0].toDouble() * rate), end = static_cast<quint32>(part[1].toDouble() * rate);
        QJsonObject stereo = isLeft ? SoundAnalyzer::getStereo(data, linkedData, rate, start, end) :
                                      SoundAnalyzer::getStereo(linkedData, data, rate, start, end);
        stereo["linked_sample"] = McpUtils::idToString(idLinked);

        // The two samples of a pair should share their settings
        QJsonArray mismatches;
        QList<QPair<AttributeType, QString> > fields = {
            { champ_dwLength, "length" }, { champ_dwSampleRate, "sample rate" }, { champ_dwStartLoop, "loop start" },
            { champ_dwEndLoop, "loop end" }, { champ_byOriginalPitch, "root key" }, { champ_chPitchCorrection, "pitch correction" } };
        for (int i = 0; i < fields.count(); i++)
            if (sm->get(idSmpl, fields[i].first).dwValue != sm->get(idLinked, fields[i].first).dwValue)
                mismatches.append(fields[i].second);
        stereo["setting_differences"] = mismatches;
        result["stereo"] = stereo;
    }
    return result;
}

static bool writeWav(QString filePath, const QVector<float> &interleaved, quint32 frameCount, quint32 sampleRate)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    quint32 dataSize = frameCount * 8;
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.writeRawData("RIFF", 4);
    stream << static_cast<quint32>(4 + 26 + 12 + 8 + dataSize);
    stream.writeRawData("WAVE", 4);
    stream.writeRawData("fmt ", 4);
    stream << static_cast<quint32>(18) << static_cast<quint16>(3) << static_cast<quint16>(2) << sampleRate
           << static_cast<quint32>(sampleRate * 8) << static_cast<quint16>(8) << static_cast<quint16>(32)
           << static_cast<quint16>(0);
    stream.writeRawData("fact", 4);
    stream << static_cast<quint32>(4) << frameCount;
    stream.writeRawData("data", 4);
    stream << dataSize;
    for (quint32 i = 0; i < 2 * frameCount; i++)
        stream << interleaved[static_cast<int>(i)];
    return stream.status() == QDataStream::Ok;
}

// Statistics of a numeric column for the keyboard analysis: trend along the keys and outliers
static void findOutliers(QString name, const QVector<double> &keys, const QVector<double> &values, double minimumDeviation,
                         const QStringList &ids, QJsonArray &outliers, QJsonObject &trends, QString unit)
{
    QVector<double> x, y;
    QVector<int> indexes;
    for (int i = 0; i < values.size(); i++)
    {
        if (std::isfinite(values[i]))
        {
            x << keys[i];
            y << values[i];
            indexes << i;
        }
    }
    if (x.size() < 4)
        return;

    // Linear trend along the keyboard
    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    int n = static_cast<int>(x.size());
    for (int i = 0; i < n; i++)
    {
        sx += x[i];
        sy += y[i];
        sxx += x[i] * x[i];
        sxy += x[i] * y[i];
    }
    double denominator = n * sxx - sx * sx;
    double slope = std::fabs(denominator) > 1e-12 ? (n * sxy - sx * sy) / denominator : 0;
    double intercept = (sy - slope * sx) / n;
    trends[name] = QJsonObject({ { "per_octave", number(12 * slope, 3) }, { "unit", unit } });

    // Robust deviation of the residuals
    QVector<double> residuals, absolute;
    for (int i = 0; i < n; i++)
    {
        residuals << y[i] - (slope * x[i] + intercept);
        absolute << std::fabs(residuals.last());
    }
    QVector<double> sorted = absolute;
    std::sort(sorted.begin(), sorted.end());
    double mad = 1.4826 * sorted[n / 2];
    for (int i = 0; i < n; i++)
    {
        double z = mad > 1e-9 ? residuals[i] / mad : 0;
        if (std::fabs(z) > 2.5 && std::fabs(residuals[i]) >= minimumDeviation)
        {
            QJsonObject outlier;
            outlier["division"] = ids[indexes[i]];
            outlier["measure"] = name;
            outlier["deviation_from_trend"] = number(residuals[i], 2);
            outlier["unit"] = unit;
            outliers.append(outlier);
        }
    }
}

void McpFunctions::addAnalysisFunctions(QList<McpFunction *> &functions)
{
    functions << new McpFunction(
        "analyze_sample", McpFunction::CATEGORY_ANALYSIS, true,
        "Measure a sample (you cannot hear: these numbers are your ears). Sections: regions (onset, attack, analyzed "
        "steady part), levels (peak, true peak, RMS, crest factor, BS.1770 loudness, A-weighted level), pitch "
        "(fundamental, deviation from the root key, suggested root key and correction, stability, drift, vibrato), "
        "harmonics (levels of the harmonics, centroid in harmonics, tilt, odd/even ratio, tristimulus, inharmonicity, "
        "harmonic to noise ratio), spectrum (centroid, rolloff, flatness, tilt, octave bands), envelope (attack time, "
        "decay rate and equivalent soundfont decay, tremolo, envelope curve), loop (seam correlation, click score, "
        "level / timbre / pitch jumps, periods), quality (clipping, DC offset, noise floor, hum, silences, effective "
        "bits), stereo (correlation, balance, width, delay, consistency of the pair). Measure before and after each edit.",
        R"JSON({"properties":{
            "sample":{"type":"string","description":"Sample identifier"},
            "sections":{"type":"array","items":{"type":"string","enum":["regions","levels","pitch","harmonics","spectrum","envelope","loop","quality","stereo"]},"description":"Sections to compute, all by default"},
            "max_harmonics":{"type":"integer","minimum":1,"maximum":60,"description":"Number of harmonic levels listed (default 16)"}},
            "required":["sample"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("sample", { elementSmpl });
            QStringList sections;
            if (args.contains("sections"))
            {
                for (const QJsonValue value : args.getArray("sections"))
                    sections << value.toString();
            }
            if (sections.isEmpty())
                sections << "regions" << "levels" << "pitch" << "harmonics" << "spectrum" << "envelope" << "loop" << "quality" << "stereo";
            return analyzeSample(id, sections, args.getInt("max_harmonics", 1, 60, 16));
        });

    functions << new McpFunction(
        "compare_samples", McpFunction::CATEGORY_ANALYSIS, true,
        "Compare a sample with a reference: spectral distance after removing the level difference, differences per "
        "octave (equalization hint), loudness, brightness (centroid in Hz and in harmonics), harmonic tilt, levels of "
        "the first harmonics, harmonic to noise ratio, attack. Useful to make adjacent samples consistent, or to check "
        "an edit against a copy made before.",
        R"JSON({"properties":{
            "reference":{"type":"string","description":"Reference sample"},
            "other":{"type":"string","description":"Sample compared to the reference"}},
            "required":["reference","other"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idRef = args.getId("reference", { elementSmpl });
            EltID idOther = args.getId("other", { elementSmpl });
            QScopedPointer<SoundAnalyzer> reference(createAnalyzer(idRef));
            QScopedPointer<SoundAnalyzer> other(createAnalyzer(idOther));
            QJsonObject result = SoundAnalyzer::compare(*reference, *other);
            result["reference"] = McpUtils::brief(idRef);
            result["other"] = McpUtils::brief(idOther);
            return result;
        });

    functions << new McpFunction(
        "analyze_instrument", McpFunction::CATEGORY_ANALYSIS, true,
        "Check the consistency of an instrument across the keyboard: for each division, the measures of its sample "
        "(loudness, effective loudness after the attenuation of the instrument, brightness, harmonic tilt, harmonic to "
        "noise ratio, attack, pitch error including the tuning parameters, loop quality, transposition range), the "
        "trends along the keyboard and the divisions breaking them (outliers), and the keys not covered.",
        R"JSON({"properties":{
            "instrument":{"type":"string","description":"Instrument identifier"},
            "velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Only the divisions playing at this velocity (all by default)"}},
            "required":["instrument"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idInst = args.getId("instrument", { elementInst });
            int velocity = args.getInt("velocity", 1, 127, -1);
            SoundfontManager * sm = SoundfontManager::getInstance();

            auto effective = [&](EltID idDiv, AttributeType champ) -> double {
                if (sm->isSet(idDiv, champ))
                    return Attribute::toRealValue(champ, false, sm->get(idDiv, champ));
                if (sm->isSet(idInst, champ))
                    return Attribute::toRealValue(champ, false, sm->get(idInst, champ));
                return AttributeBase::getDefaultRealValue(champ, false);
            };
            auto range = [&](EltID idDiv, AttributeType champ) -> RangesType {
                if (sm->isSet(idDiv, champ))
                    return sm->get(idDiv, champ).rValue;
                if (sm->isSet(idInst, champ))
                    return sm->get(idInst, champ).rValue;
                return AttributeBase::getDefaultStoredValue(champ, false).rValue;
            };

            QMap<int, SoundAnalyzer *> analyzers; // By sample
            QStringList columns = { "division", "keys", "root_key", "sample", "loudness_lufs", "effective_loudness_lufs",
                                    "centroid_hz", "centroid_in_harmonics", "harmonic_tilt_db_per_octave",
                                    "harmonic_to_noise_db", "attack_ms", "pitch_error_cents", "loop_seam_correlation",
                                    "transposition_down", "transposition_up" };
            QJsonArray rows;
            QVector<double> rootKeys, loudness, centroidHarmonics, tilts, hnrs, attacks, pitchErrors;
            QStringList ids;
            QVector<int> coverage(128, 0);

            EltID idDiv(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
            foreach (int i, sm->getSiblings(idDiv))
            {
                idDiv.indexElt2 = i;
                RangesType keyRange = range(idDiv, champ_keyRange);
                RangesType velRange = range(idDiv, champ_velRange);
                if (velocity > 0 && (velocity < velRange.byLo || velocity > velRange.byHi))
                    continue;
                EltID idSmpl(elementSmpl, idInst.indexSf2, sm->get(idDiv, champ_sampleID).wValue);
                if (!sm->isValid(idSmpl))
                    continue;
                for (int key = keyRange.byLo; key <= keyRange.byHi; key++)
                    coverage[key]++;

                if (!analyzers.contains(idSmpl.indexElt))
                    analyzers[idSmpl.indexElt] = createAnalyzer(idSmpl);
                SoundAnalyzer * analyzer = analyzers[idSmpl.indexElt];

                double overridingRoot = effective(idDiv, champ_overridingRootKey);
                int rootKey = overridingRoot >= 0 ? qRound(overridingRoot) : sm->get(idSmpl, champ_byOriginalPitch).bValue;
                double attenuation = effective(idDiv, champ_initialAttenuation);
                double lufs = analyzer->getSteadyLoudness();
                double f0 = analyzer->getFundamental();
                double pitchError = std::nan("");
                if (f0 > 0)
                    pitchError = 1200 * std::log2(f0 / keyFrequency(rootKey)) + sm->get(idSmpl, champ_chPitchCorrection).cValue +
                                 effective(idDiv, champ_fineTune) + 100 * effective(idDiv, champ_coarseTune);

                QJsonArray row;
                row.append(McpUtils::idToString(idDiv));
                row.append(McpUtils::keyName(keyRange.byLo) + ".." + McpUtils::keyName(keyRange.byHi));
                row.append(rootKey);
                row.append(McpUtils::idToString(idSmpl));
                row.append(number(lufs, 1));
                row.append(number(lufs - attenuation, 1));
                row.append(number(analyzer->getSpectralCentroid(), 0));
                row.append(number(analyzer->getHarmonicCentroid(), 2));
                row.append(number(analyzer->getHarmonicTilt(), 1));
                row.append(number(analyzer->getHnr(), 1));
                row.append(number(1000 * analyzer->getAttackTime(), 1));
                row.append(number(pitchError, 1));
                row.append(analyzer->isLooped() ? number(analyzer->getLoopCorrelation(), 3) : QJsonValue());
                row.append(qMax(0, rootKey - keyRange.byLo));
                row.append(qMax(0, keyRange.byHi - rootKey));
                rows.append(row);

                ids << McpUtils::idToString(idDiv);
                rootKeys << rootKey;
                loudness << lufs - attenuation;
                centroidHarmonics << analyzer->getHarmonicCentroid();
                tilts << analyzer->getHarmonicTilt();
                hnrs << analyzer->getHnr();
                attacks << 1000 * analyzer->getAttackTime();
                pitchErrors << pitchError;
            }
            qDeleteAll(analyzers);

            QJsonObject result = McpUtils::brief(idInst);
            result["columns"] = QJsonArray::fromStringList(columns);
            result["divisions"] = rows;

            QJsonArray outliers;
            QJsonObject trends;
            findOutliers("effective_loudness_lufs", rootKeys, loudness, 1.5, ids, outliers, trends, "dB");
            findOutliers("centroid_in_harmonics", rootKeys, centroidHarmonics, 0.3, ids, outliers, trends, "harmonic ranks");
            findOutliers("harmonic_tilt_db_per_octave", rootKeys, tilts, 2, ids, outliers, trends, "dB per octave");
            findOutliers("harmonic_to_noise_db", rootKeys, hnrs, 3, ids, outliers, trends, "dB");
            findOutliers("attack_ms", rootKeys, attacks, 10, ids, outliers, trends, "ms");
            result["trends"] = trends;
            result["outliers"] = outliers;

            QJsonArray tuning;
            for (int i = 0; i < pitchErrors.size(); i++)
                if (std::isfinite(pitchErrors[i]) && std::fabs(pitchErrors[i]) > 5)
                    tuning.append(QJsonObject({ { "division", ids[i] }, { "pitch_error_cents", number(pitchErrors[i], 1) } }));
            result["out_of_tune"] = tuning;

            // Keys not covered between the lowest and the highest covered keys
            QJsonArray gaps;
            int first = 0, last = 127;
            while (first < 128 && coverage[first] == 0)
                first++;
            while (last >= 0 && coverage[last] == 0)
                last--;
            for (int key = first; key <= last; key++)
            {
                if (coverage[key] == 0)
                {
                    int end = key;
                    while (end + 1 <= last && coverage[end + 1] == 0)
                        end++;
                    gaps.append(McpUtils::keyName(key) + (end > key ? ".." + McpUtils::keyName(end) : ""));
                    key = end;
                }
            }
            if (first <= last)
                result["covered_keys"] = McpUtils::keyName(first) + ".." + McpUtils::keyName(last);
            result["gaps"] = gaps;
            result["note"] = "effective loudness = loudness of the sustained part minus the attenuation; outliers deviate "
                             "from the linear trend along the keyboard by more than 2.5 robust deviations; transposition: "
                             "semitones between the root key and the ends of the key range (large values change the timbre)";
            return result;
        });

    functions << new McpFunction(
        "render_note", McpFunction::CATEGORY_ANALYSIS, true,
        "Play a note with the sound engine of Polyphone (offline, no reverb / chorus) and measure what is heard: all "
        "parameters, envelopes, filter, modulators and layers are applied. Returns levels, loudness, pitch (cents from "
        "the equal temperament with A4 = 440 Hz), attack, decay, level at the release, release times, spectrum, "
        "harmonics and stereo image. Optionally writes the rendering in a wav file for the user.",
        R"JSON({"properties":{
            "element":{"type":"string","description":"Preset, instrument or sample"},
            "key":{"type":["integer","string"],"description":"Key played (number or note name); for a sample, default = its root key"},
            "velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Default 100"},
            "hold_s":{"type":"number","minimum":0.05,"maximum":20,"description":"Duration before the key is released (default 2)"},
            "release_s":{"type":"number","minimum":0,"maximum":20,"description":"Duration recorded after the release (default 1.5)"},
            "wav_file":{"type":"string","description":"Absolute path of a wav file to write (optional)"}},
            "required":["element"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("element", { elementPrst, elementInst, elementSmpl });
            SoundfontManager * sm = SoundfontManager::getInstance();
            int key;
            if (id.typeElement == elementSmpl)
                key = args.getKey("key", sm->get(id, champ_byOriginalPitch).bValue);
            else
                key = args.getKey("key");
            int velocity = args.getInt("velocity", 1, 127, 100);
            double hold = args.getDouble("hold_s", 0.05, 20, 2);
            double release = args.getDouble("release_s", 0, 20, 1.5);
            QString wavFile = args.getString("wav_file", "");

            QStringList lines;
            lines << "samplerate 48000" << "block 256" << QString("duration %1").arg(hold + release + 0.01);
            if (id.typeElement == elementPrst)
            {
                int bank = sm->get(id, champ_wBank).wValue;
                int preset = sm->get(id, champ_wPreset).wValue;
                int channel = (bank == 128) ? 9 : 0;
                lines << QString("0 program %1 %2 %3").arg(channel).arg(bank).arg(preset)
                      << QString("0 on %1 %2 %3").arg(channel).arg(key).arg(velocity)
                      << QString("%1 off %2 %3").arg(hold).arg(channel).arg(key);
            }
            else
            {
                QString kind = id.typeElement == elementInst ? "instrument" : "sample";
                lines << QString("0 %1 %2 %3 %4").arg(kind).arg(id.indexElt).arg(key).arg(velocity)
                      << QString("%1 %2 %3 %4 0").arg(hold).arg(kind).arg(id.indexElt).arg(key);
            }

            OfflineRenderer renderer;
            QVector<float> data;
            quint32 frameCount;
            if (!renderer.readEventLines(lines) || !renderer.render(id.indexSf2, data, frameCount, OfflineRenderer::SYNTH_CORE))
                throw McpException("rendering failed: " + renderer.getError());
            quint32 rate = renderer.getSampleRate();

            QVector<float> left(static_cast<int>(frameCount)), right(static_cast<int>(frameCount)), mid(static_cast<int>(frameCount));
            for (int i = 0; i < static_cast<int>(frameCount); i++)
            {
                left[i] = data[2 * i];
                right[i] = data[2 * i + 1];
                mid[i] = 0.5f * (left[i] + right[i]);
            }

            if (!wavFile.isEmpty())
            {
                wavFile = QFileInfo(wavFile).absoluteFilePath();
                if (!writeWav(wavFile, data, frameCount, rate))
                    throw McpException("cannot write \"" + wavFile + "\"");
            }

            // Analysis of the held part (before the release), then of the release
            int holdFrames = qMin(static_cast<int>(frameCount), static_cast<int>(hold * rate));
            SoundAnalyzer analyzer(mid.mid(0, holdFrames), rate, 0, 0, keyFrequency(key));
            QJsonObject result = McpUtils::brief(id);
            result["key"] = key;
            result["velocity"] = velocity;
            result["signal"] = "mid (L + R) / 2 of the rendering, held part";

            QJsonObject levels = analyzer.getLevels();
            float peakLeft = 0, peakRight = 0;
            for (int i = 0; i < static_cast<int>(frameCount); i++)
            {
                peakLeft = qMax(peakLeft, qAbs(left[i]));
                peakRight = qMax(peakRight, qAbs(right[i]));
            }
            levels["peak_left_dbfs"] = number(peakLeft > 0 ? 20 * std::log10(peakLeft) : -200, 2);
            levels["peak_right_dbfs"] = number(peakRight > 0 ? 20 * std::log10(peakRight) : -200, 2);
            result["levels"] = levels;

            QJsonObject pitch = analyzer.getPitch();
            double f0 = analyzer.getFundamental();
            if (f0 > 0)
                pitch["cents_from_key"] = number(1200 * std::log2(f0 / keyFrequency(key)), 1);
            result["pitch"] = pitch;

            QJsonObject envelope = analyzer.getEnvelope();
            envelope.remove("curve");

            // Release: time for the level to drop by 20, 40 and 60 dB after the key is released
            int window = static_cast<int>(0.02 * rate), hop = static_cast<int>(0.005 * rate);
            auto rmsAt = [&](int start) -> double {
                double sum = 0;
                for (int i = start; i < start + window && i < mid.size(); i++)
                    sum += static_cast<double>(mid[i]) * mid[i];
                return std::sqrt(sum / window);
            };
            double releaseLevel = rmsAt(qMax(0, holdFrames - window));
            double peakRms = 0;
            for (int i = 0; i + window < holdFrames; i += hop)
                peakRms = qMax(peakRms, rmsAt(i));
            if (releaseLevel > 0 && peakRms > 0)
            {
                envelope["level_at_release_db"] = number(20 * std::log10(releaseLevel / peakRms), 1);
                QJsonObject releaseTimes;
                foreach (int drop, QList<int>({ 20, 40, 60 }))
                {
                    double target = releaseLevel * std::pow(10.0, -drop / 20.0);
                    for (int i = holdFrames; i + window <= mid.size(); i += hop)
                    {
                        if (rmsAt(i) < target)
                        {
                            releaseTimes[QString("minus_%1_db_s").arg(drop)] = number(static_cast<double>(i - holdFrames) / rate, 3);
                            break;
                        }
                    }
                }
                envelope["release"] = releaseTimes;
            }
            result["envelope"] = envelope;
            result["spectrum"] = analyzer.getSpectrum();
            result["harmonics"] = analyzer.getHarmonics(12);
            result["stereo"] = SoundAnalyzer::getStereo(left, right, rate, 0, static_cast<quint32>(holdFrames));
            if (!wavFile.isEmpty())
                result["wav_file"] = wavFile;
            return result;
        });

    functions << new McpFunction(
        "get_spectrum", McpFunction::CATEGORY_ANALYSIS, true,
        "Spectrum of a sample (steady part): third-octave levels and spectral envelope given by the harmonic peaks, "
        "both relative to the maximum, plus the spectral descriptors. Optionally an image of the spectrogram (time / "
        "logarithmic frequency, dashed lines at the loop points) to see the evolution of the timbre, noise, hum, "
        "attack transients...",
        R"JSON({"properties":{
            "sample":{"type":"string","description":"Sample identifier"},
            "image":{"type":"boolean","description":"Return a spectrogram image (default false)"}},
            "required":["sample"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("sample", { elementSmpl });
            QScopedPointer<SoundAnalyzer> analyzer(createAnalyzer(id));
            QJsonObject data = McpUtils::brief(id);
            data["spectrum"] = analyzer->getSpectrum();
            data["curves"] = analyzer->getSpectrumCurves();
            McpResult result(data);
            if (args.getBool("image", false))
                result.pngImages << toPng(analyzer->getSpectrogram(720, 320));
            return result;
        });

    functions << new McpFunction(
        "get_loop_analysis", McpFunction::CATEGORY_ANALYSIS, true,
        "Detailed quality of the loop of a sample (and of its linked sample): seam correlation, junction jump, click "
        "score, level / timbre / pitch jumps, number of periods, harmonic phase mismatch, consistency of the loop points "
        "of a stereo pair, with an image of the waveform heard at the loop point (end of the loop followed by its "
        "start). Use it after auto_loop, loop_crossfade or a change of the loop points.",
        R"JSON({"properties":{
            "sample":{"type":"string","description":"Sample identifier"},
            "image":{"type":"boolean","description":"Return the image of the waveform at the loop point (default true)"}},
            "required":["sample"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("sample", { elementSmpl });
            SoundfontManager * sm = SoundfontManager::getInstance();
            QScopedPointer<SoundAnalyzer> analyzer(createAnalyzer(id));
            QJsonObject data = McpUtils::brief(id);
            data["loop"] = analyzer->getLoop();
            data["regions"] = analyzer->getRegions();

            EltID idLinked = getLinkedSample(id);
            if (idLinked.typeElement == elementSmpl)
            {
                QScopedPointer<SoundAnalyzer> linked(createAnalyzer(idLinked));
                QJsonObject linkedLoop = linked->getLoop();
                linkedLoop["id"] = McpUtils::idToString(idLinked);
                data["linked_sample_loop"] = linkedLoop;
                data["same_loop_points"] = sm->get(id, champ_dwStartLoop).dwValue == sm->get(idLinked, champ_dwStartLoop).dwValue &&
                                           sm->get(id, champ_dwEndLoop).dwValue == sm->get(idLinked, champ_dwEndLoop).dwValue;
            }

            McpResult result(data);
            if (analyzer->isLooped() && args.getBool("image", true))
                result.pngImages << toPng(analyzer->getLoopSeamImage(720, 240));
            return result;
        });
}
