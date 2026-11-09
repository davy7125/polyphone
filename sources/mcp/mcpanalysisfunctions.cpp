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
#include "sampleutils.h"
#include "offlinerenderer.h"
#include "utils.h"
#include <QBuffer>
#include <QFileInfo>

static QByteArray toPng(const QImage &image)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return bytes;
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

// Analyzer of a sample, the pitch being searched within a fifth around a key (by default the root key of the sample, -1
// for the whole range)
static SoundAnalyzer * createAnalyzer(EltID idSmpl, int pitchSearchKey = -2)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    double expected = 0;
    if (pitchSearchKey == -2)
    {
        int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
        int correction = sm->get(idSmpl, champ_chPitchCorrection).cValue;
        expected = SampleUtils::keyToFrequency(rootKey - correction / 100.0);
    }
    else if (pitchSearchKey >= 0)
        expected = SampleUtils::keyToFrequency(pitchSearchKey);
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
        double deviation = 100 * (SampleUtils::frequencyToKey(f0) - rootKey);
        pitch["ratio_to_root_key"] = Utils::roundSignificant(f0 / SampleUtils::keyToFrequency(rootKey), 6);
        pitch["cents_from_root_key"] = SoundAnalyzer::jsonNumber(deviation, 1);
        pitch["cents_from_root_key_with_correction"] = SoundAnalyzer::jsonNumber(deviation + correction, 1);
        pitch["pitch_note"] = "cents from the equal temperament note of the root key (A4 = 440 Hz), positive = higher; "
                              "with_correction: pitch heard when the sample is played alone at its root key";
    }
    return pitch;
}

static QJsonObject analyzeSample(EltID idSmpl, QStringList sections, int maxHarmonics, int pitchSearchKey)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QScopedPointer<SoundAnalyzer> analyzer(createAnalyzer(idSmpl, pitchSearchKey));

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

        // Settings differing between the two samples of the pair
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

// Linear slope of a measure along the keyboard (per octave) and spread of the measures around this line (robust
// standard deviation of the residuals), the values that are not finite being ignored
static QJsonValue keyboardSlope(const QVector<double> &keys, const QVector<double> &values, QString unit)
{
    QVector<double> x, y;
    for (int i = 0; i < values.size(); i++)
    {
        if (std::isfinite(values[i]))
        {
            x << keys[i];
            y << values[i];
        }
    }
    if (x.size() < 3)
        return QJsonValue();

    // Flat if all keys are the same
    int n = static_cast<int>(x.size());
    double slope, intercept;
    if (!SampleUtils::linearRegression(x, y, QVector<double>(), slope, intercept))
    {
        double sum = 0;
        foreach (double value, y)
            sum += value;
        slope = 0;
        intercept = sum / n;
    }
    QVector<double> absolute;
    for (int i = 0; i < n; i++)
        absolute << std::fabs(y[i] - (slope * x[i] + intercept));
    return QJsonObject({ { "per_octave", SoundAnalyzer::jsonNumber(12 * slope, 3) },
                        { "value_at_c4", SoundAnalyzer::jsonNumber(60 * slope + intercept, 2) },
                        { "spread", SoundAnalyzer::jsonNumber(1.4826 * SampleUtils::median(absolute), 2) },
                        { "unit", unit }, { "count", n } });
}

// Elements played together by the offline rendering (same soundfont)
static IdList readRenderedElements(const McpArguments &args)
{
    QList<ElementType> types = { elementPrst, elementInst, elementSmpl };
    IdList ids = args.getIds(args.contains("elements") ? "elements" : "element", types);
    IdList result;
    foreach (EltID id, ids)
    {
        if (id.indexSf2 != ids[0].indexSf2)
            throw McpException("all elements must be in the same soundfont");
        if (!result.contains(id))
            result << id;
    }
    return result;
}

// Keys played together, by default the root key of a sample played alone
static QList<int> readKeys(const McpArguments &args, const IdList &elements)
{
    QList<int> keys;
    QString name = args.contains("keys") ? "keys" : "key";
    if (args.contains(name))
    {
        QJsonValue value = args.value(name);
        if (value.isArray())
        {
            for (const QJsonValue item : value.toArray())
                keys << McpUtils::readKey(item, name);
        }
        else
            keys << McpUtils::readKey(value, name);
    }
    else if (elements.count() == 1 && elements[0].typeElement == elementSmpl)
        keys << SoundfontManager::getInstance()->get(elements[0], champ_byOriginalPitch).bValue;
    if (keys.isEmpty())
        throw McpException("missing argument \"keys\"");
    QList<int> result;
    foreach (int key, keys)
        if (!result.contains(key))
            result << key;
    return result;
}

// Reverb: a level from 0 to 100 (size, width and damping at 50) or [level, size, width, damping]
static QList<int> readReverb(const McpArguments &args)
{
    QList<int> reverb = { 0, 50, 50, 50 };
    if (!args.contains("reverb"))
        return reverb;
    QJsonValue value = args.value("reverb");
    if (value.isDouble())
        reverb[0] = qBound(0, qRound(value.toDouble()), 100);
    else if (value.isArray() && value.toArray().count() == 4)
    {
        for (int i = 0; i < 4; i++)
        {
            if (!value.toArray()[i].isDouble())
                throw McpException("\"reverb\" must contain numbers");
            reverb[i] = qBound(0, qRound(value.toArray()[i].toDouble()), 100);
        }
    }
    else
        throw McpException("\"reverb\" must be a level or [level, size, width, damping]");
    return reverb;
}

// Controllers set before the notes: {"15": 0, "cc16": 127} or [[15, 0], [16, 127]]
static QList<QPair<int, int> > readControllers(const McpArguments &args)
{
    QList<QPair<int, int> > controllers;
    if (!args.contains("controllers"))
        return controllers;
    QJsonValue value = args.value("controllers");
    auto add = [&](QString name, QJsonValue controllerValue) {
        bool ok;
        int number = (name.startsWith("cc", Qt::CaseInsensitive) ? name.mid(2) : name).toInt(&ok);
        if (!ok || number < 1 || number > 119 || number == 32)
            throw McpException("\"controllers\": \"" + name + "\" is not a controller number from 1 to 119 (32 excluded)");
        double number2 = controllerValue.toDouble(-1);
        if (!controllerValue.isDouble() || number2 != qRound(number2) || number2 < 0 || number2 > 127)
            throw McpException("\"controllers\": the value of the controller " + name + " must be an integer from 0 to 127");
        controllers << QPair<int, int>(number, qRound(number2));
    };
    if (value.isObject())
    {
        QJsonObject object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it)
            add(it.key(), it.value());
    }
    else if (value.isArray())
    {
        for (const QJsonValue item : value.toArray())
        {
            if (!item.isArray() || item.toArray().count() != 2)
                throw McpException("\"controllers\" must be an object or an array of [controller, value]");
            add(QString::number(item.toArray()[0].toDouble(-1)), item.toArray()[1]);
        }
    }
    else
        throw McpException("\"controllers\" must be an object or an array of [controller, value]");
    return controllers;
}

// Settings of the offline rendering: presets on their own MIDI channel (bank 128 on the channel 10), instruments and
// samples on the channel of the editor (-1), then the controllers on all these channels
static QStringList renderSettings(const IdList &elements, double duration, const QList<int> &reverb,
                                  const QList<QPair<int, int> > &controllers, QMap<int, int> &channels)
{
    QStringList lines;
    lines << "samplerate 48000" << "block 256" << "duration " + QString::number(duration, 'f', 4);
    if (reverb[0] > 0)
        lines << QString("reverb %1 %2 %3 %4").arg(reverb[0]).arg(reverb[1]).arg(reverb[2]).arg(reverb[3]);

    SoundfontManager * sm = SoundfontManager::getInstance();
    int nextChannel = 0;
    bool percussionUsed = false;
    QList<int> usedChannels;
    foreach (EltID id, elements)
    {
        if (id.typeElement != elementPrst)
        {
            if (!usedChannels.contains(-1))
                usedChannels << -1;
            continue;
        }
        int bank = sm->get(id, champ_wBank).wValue;
        int channel;
        if (bank == 128)
        {
            if (percussionUsed)
                throw McpException("only one preset of the bank 128 can be played at once");
            percussionUsed = true;
            channel = 9;
        }
        else
        {
            if (nextChannel == 9)
                nextChannel++;
            if (nextChannel > 15)
                throw McpException("15 presets at most can be played at once");
            channel = nextChannel++;
        }
        channels[id.indexElt] = channel;
        usedChannels << channel;
        lines << QString("0 program %1 %2 %3").arg(channel).arg(bank).arg(sm->get(id, champ_wPreset).wValue);
    }
    for (int i = 0; i < controllers.count(); i++)
        foreach (int channel, usedChannels)
            lines << QString("0 cc %1 %2 %3").arg(channel).arg(controllers[i].first).arg(controllers[i].second);
    return lines;
}

static void addNotes(QStringList &lines, const IdList &elements, const QMap<int, int> &channels, const QList<int> &keys,
                     int velocity, double on, double off)
{
    QString onTime = QString::number(on, 'f', 4);
    QString offTime = QString::number(off, 'f', 4);
    foreach (EltID id, elements)
    {
        foreach (int key, keys)
        {
            if (id.typeElement == elementPrst)
            {
                int channel = channels.value(id.indexElt);
                lines << QString("%1 on %2 %3 %4").arg(onTime).arg(channel).arg(key).arg(velocity)
                      << QString("%1 off %2 %3").arg(offTime).arg(channel).arg(key);
            }
            else
            {
                QString kind = id.typeElement == elementInst ? "instrument" : "sample";
                lines << QString("%1 %2 %3 %4 %5").arg(onTime, kind).arg(id.indexElt).arg(key).arg(velocity)
                      << QString("%1 %2 %3 %4 0").arg(offTime, kind).arg(id.indexElt).arg(key);
            }
        }
    }
}

// Rendering of event lines in two channels, possibly written in a wav file
static quint32 renderLines(int indexSf2, const QStringList &lines, QVector<float> &left, QVector<float> &right, QString wavFile)
{
    // Default values of the controllers as in the editor (the soundfont was loaded before the renderer was created)
    OfflineRenderer renderer;
    QVector<float> data;
    quint32 frameCount;
    if (!renderer.applyDefaultControllerValues(indexSf2) || !renderer.readEventLines(lines) ||
        !renderer.render(indexSf2, data, frameCount, OfflineRenderer::SYNTH_CORE))
        throw McpException("rendering failed: " + renderer.getError());
    quint32 rate = renderer.getSampleRate();
    if (!wavFile.isEmpty() && !OfflineRenderer::writeWavFile(wavFile, data, frameCount, rate))
        throw McpException("cannot write \"" + wavFile + "\"");

    left.resize(static_cast<int>(frameCount));
    right.resize(static_cast<int>(frameCount));
    for (int i = 0; i < static_cast<int>(frameCount); i++)
    {
        left[i] = data[2 * i];
        right[i] = data[2 * i + 1];
    }
    return rate;
}

// Analyzer of a stereo rendering between two frames: the louder channel gives the regions, the pitch and the envelope,
// the other one adding its energy to the levels, the loudness, the spectra and the partials
static SoundAnalyzer * stereoAnalyzer(const QVector<float> &left, const QVector<float> &right, int start, int end,
                                      quint32 rate, double expectedFrequency, bool &leftIsFirst)
{
    start = qBound(0, start, static_cast<int>(left.size()));
    end = qBound(start, end, static_cast<int>(left.size()));
    double energyLeft = 0, energyRight = 0;
    for (int i = start; i < end; i++)
    {
        energyLeft += static_cast<double>(left[i]) * left[i];
        energyRight += static_cast<double>(right[i]) * right[i];
    }
    leftIsFirst = (energyLeft >= energyRight);
    SoundAnalyzer * analyzer = new SoundAnalyzer((leftIsFirst ? left : right).mid(start, end - start), rate, 0, 0, expectedFrequency);
    analyzer->setSecondChannel((leftIsFirst ? right : left).mid(start, end - start));
    return analyzer;
}

// Quadratic mean of the channels (gives the RMS level of the stereo signal)
static QVector<float> stereoMagnitude(const QVector<float> &left, const QVector<float> &right, int start, int end)
{
    QVector<float> result(qMax(0, end - start));
    for (int i = start; i < end; i++)
        result[i - start] = std::sqrt(0.5f * (left[i] * left[i] + right[i] * right[i]));
    return result;
}

// Pitch over the whole rendering on a channel, searched within a fifth around the fundamental of the held part:
// cents from the key every 20 ms (null without pitch), and the largest change after the release
static QJsonObject pitchTrack(const QVector<float> &channel, int start, int end, quint32 rate, double fundamental,
                              int key, double releaseTime, double &releaseChange)
{
    releaseChange = std::nan("");
    QJsonObject result;
    SoundAnalyzer analyzer(channel.mid(start, end - start), rate, 0, 0, fundamental);
    double firstTime;
    QVector<double> track = analyzer.getPitchTrack(0.02, firstTime);
    double keyFrequency = SampleUtils::keyToFrequency(key);
    QJsonArray cents;
    for (int i = 0; i < track.size(); i++)
    {
        if (track[i] <= 0)
        {
            cents.append(QJsonValue());
            continue;
        }
        cents.append(SoundAnalyzer::jsonNumber(1200 * std::log2(track[i] / keyFrequency), 1));
        double time = firstTime + 0.02 * i;
        double change = 1200 * std::log2(track[i] / fundamental);
        if (time > releaseTime && (!std::isfinite(releaseChange) || std::fabs(change) > std::fabs(releaseChange)))
            releaseChange = change;
    }
    result["hop_s"] = 0.02;
    result["first_time_s"] = SoundAnalyzer::jsonNumber(firstTime, 3);
    result["release_time_s"] = SoundAnalyzer::jsonNumber(releaseTime, 3);
    result["cents_from_key"] = cents;
    return result;
}

// Partials as a table, with their ratios to the frequencies of the keys played
static QJsonObject partialsTable(const QList<SoundAnalyzer::Partial> &partials, const QList<int> &keys, double start, double end)
{
    QJsonArray rows;
    foreach (const SoundAnalyzer::Partial &partial, partials)
    {
        double key = SampleUtils::frequencyToKey(partial.frequency);
        int nearest = qRound(key);
        QJsonArray row;
        row.append(Utils::roundSignificant(partial.frequency, 7));
        row.append(SoundAnalyzer::jsonNumber(partial.levelDb, 1));
        row.append(nearest);
        row.append(SoundAnalyzer::jsonNumber(100 * (key - nearest), 1));
        if (keys.count() == 1)
            row.append(Utils::roundSignificant(partial.frequency / SampleUtils::keyToFrequency(keys[0]), 6));
        else
        {
            QJsonArray ratios;
            foreach (int playedKey, keys)
                ratios.append(Utils::roundSignificant(partial.frequency / SampleUtils::keyToFrequency(playedKey), 6));
            row.append(ratios);
        }
        row.append(SoundAnalyzer::jsonNumber(partial.variationDb, 1));
        row.append(std::isfinite(partial.beatRate) && partial.beatRate > 0 ? QJsonValue(SoundAnalyzer::jsonNumber(partial.beatRate, 2)) : QJsonValue());
        row.append(std::isfinite(partial.beatRate) && partial.beatRate > 0 ? QJsonValue(SoundAnalyzer::jsonNumber(partial.beatDepthDb, 1)) : QJsonValue());
        row.append(SoundAnalyzer::jsonNumber(partial.bandHz, 1));
        rows.append(row);
    }

    QJsonObject result;
    result["columns"] = QJsonArray({ "frequency_hz", "level_dbfs", "nearest_key", "cents_from_nearest_key",
                                     keys.count() == 1 ? "ratio_to_key" : "ratios_to_keys", "level_variation_db",
                                     "beat_rate_hz", "beat_depth_db", "band_hz" });
    result["rows"] = rows;
    result["analyzed_part_s"] = QJsonArray({ SoundAnalyzer::jsonNumber(start, 3), SoundAnalyzer::jsonNumber(end, 3) });
    return result;
}

static const char * PARTIALS_NOTE =
    "partials: strongest spectral peaks of the sustained part (from the end of the attack to the release), sorted by "
    "frequency; level_dbfs: amplitude of the sine; nearest_key: MIDI number (60 = C4), cents_from_nearest_key: from the "
    "equal temperament (A4 = 440 Hz); "
    "ratio: frequency divided by the equal temperament frequency of the key played; level_variation_db: range of the "
    "level of the partial over time (5th to 95th percentile) in a band of +/- band_hz; beat_rate_hz and "
    "beat_depth_db: sinusoidal modulation of its amplitude (depth: dB between the maximum and the minimum of this "
    "sinusoid, 60 dB at most), null if none: 3 periods at least in the analyzed part, and an amplitude 4 times the "
    "median of the other rates searched, random fluctuations being only in level_variation_db; two "
    "components closer than about 4 / analyzed duration Hz form one partial whose amplitude beats; farther apart they "
    "are listed separately, the difference of their frequencies being their beat rate";

// Measures of the rendering of a note or a chord: held part, then the release
static QJsonObject measureRendering(const QVector<float> &left, const QVector<float> &right, quint32 rate, double hold,
                                    const QList<int> &keys, int maxPartials, double &steadyLoudness)
{
    int frameCount = static_cast<int>(left.size());
    int holdFrames = qMin(frameCount, static_cast<int>(hold * rate));
    bool leftIsFirst;
    QScopedPointer<SoundAnalyzer> analyzer(stereoAnalyzer(left, right, 0, holdFrames, rate, 0, leftIsFirst));
    steadyLoudness = analyzer->getSteadyLoudness();

    QJsonObject result;
    QJsonObject levels = analyzer->getLevels();
    levels["peak_left_dbfs"] = SoundAnalyzer::jsonNumber(SampleUtils::toDb(SampleUtils::peak(left.constData(), frameCount)), 2);
    levels["peak_right_dbfs"] = SoundAnalyzer::jsonNumber(SampleUtils::toDb(SampleUtils::peak(right.constData(), frameCount)), 2);
    result["levels"] = levels;
    result["regions"] = analyzer->getRegions();

    if (keys.count() == 1)
    {
        // Fundamental searched over the whole range: the common period of what is heard, then followed during the
        // whole rendering
        QJsonObject pitch = analyzer->getPitch();
        double f0 = analyzer->getFundamental();
        if (f0 > 0)
        {
            pitch["ratio_to_key"] = Utils::roundSignificant(f0 / SampleUtils::keyToFrequency(keys[0]), 6);
            pitch["cents_from_key"] = SoundAnalyzer::jsonNumber(100 * (SampleUtils::frequencyToKey(f0) - keys[0]), 1);
            double releaseChange;
            QJsonObject track = pitchTrack(leftIsFirst ? left : right, 0, frameCount, rate, f0, keys[0], hold, releaseChange);
            pitch["release_max_change_cents"] = SoundAnalyzer::jsonNumber(releaseChange, 1);
            result["pitch_track"] = track;
        }
        result["pitch"] = pitch;
        result["harmonics"] = analyzer->getHarmonics(12);
    }

    QJsonObject envelope = analyzer->getEnvelope();
    envelope.remove("curve");
    envelope.remove("end_level_db");
    QJsonObject releaseInfo = SoundAnalyzer::getRelease(stereoMagnitude(left, right, 0, frameCount), rate, holdFrames);
    for (auto it = releaseInfo.constBegin(); it != releaseInfo.constEnd(); ++it)
        envelope[it.key()] = it.value();
    result["envelope"] = envelope;
    result["spectrum"] = analyzer->getSpectrum();

    double start, end;
    QList<SoundAnalyzer::Partial> partials = analyzer->getPartials(maxPartials, start, end);
    result["partials"] = partialsTable(partials, keys, start, end);
    result["stereo"] = SoundAnalyzer::getStereo(left, right, rate, 0, static_cast<quint32>(holdFrames));
    result["channels"] = QString("levels, loudness (sum of the channels), spectrum, harmonics and partials: both channels; "
                                 "regions, pitch and envelope: the louder channel (%1)").arg(leftIsFirst ? "left" : "right");
    return result;
}

void McpFunctions::addAnalysisFunctions(QList<McpFunction *> &functions)
{
    functions << new McpFunction(
        "analyze_sample", McpFunction::CATEGORY_ANALYSIS, true,
        "Measure a sample (you cannot hear: these numbers are your ears). Sections: regions (onset, attack, analyzed "
        "steady part), levels (peak, true peak, RMS, crest factor, BS.1770 loudness, A-weighted level), pitch "
        "(fundamental searched within a fifth around the root key unless pitch_search_key is given, ratio and cents "
        "from the root key, stability, drift, vibrato), "
        "harmonics (levels of the harmonics, centroid in harmonics, tilt, odd/even ratio, tristimulus, inharmonicity, "
        "harmonic to noise ratio), spectrum (centroid, rolloff, flatness, tilt, octave bands), envelope (attack time, "
        "decay rate and equivalent soundfont decay, tremolo, envelope curve), loop (seam correlation, click score, "
        "level / timbre / pitch jumps, periods), quality (clipping, DC offset, noise floor, hum, silences, effective "
        "bits), stereo (correlation, balance, width, delay, settings differing between the samples of the pair). The "
        "measures describe the sound, they don't say what it should be.",
        R"JSON({"properties":{
            "sample":{"type":"string","description":"Sample identifier"},
            "sections":{"type":"array","items":{"type":"string","enum":["regions","levels","pitch","harmonics","spectrum","envelope","loop","quality","stereo"]},"description":"Sections to compute, all by default"},
            "max_harmonics":{"type":"integer","minimum":1,"maximum":60,"description":"Number of harmonic levels listed (default 16)"},
            "pitch_search_key":{"type":["integer","string"],"description":"Key around which the fundamental is searched (within a fifth), -1 for the whole range (16 Hz to a quarter of the sample rate); default: root key of the sample"}},
            "required":["sample"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("sample", { elementSmpl });
            int pitchSearchKey = -2;
            if (args.contains("pitch_search_key"))
                pitchSearchKey = (args.value("pitch_search_key").isDouble() && args.value("pitch_search_key").toDouble() == -1) ?
                                     -1 : args.getKey("pitch_search_key");
            QStringList sections;
            if (args.contains("sections"))
            {
                for (const QJsonValue value : args.getArray("sections"))
                    sections << value.toString();
            }
            if (sections.isEmpty())
                sections << "regions" << "levels" << "pitch" << "harmonics" << "spectrum" << "envelope" << "loop" << "quality" << "stereo";
            return analyzeSample(id, sections, args.getInt("max_harmonics", 1, 60, 16), pitchSearchKey);
        });

    functions << new McpFunction(
        "compare_samples", McpFunction::CATEGORY_ANALYSIS, true,
        "Compare a sample with a reference sample: spectral distance after removing the level difference, mean "
        "differences per octave, loudness, brightness (centroid in Hz and in harmonics), harmonic tilt, levels of "
        "the first harmonics, harmonic to noise ratio, attack (all as other minus reference).",
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
        "Measures of the samples of an instrument across the keyboard, without rendering. For each division: keys, "
        "root key used, sample, sample mode, loudness of the sustained part of the sample (raw and minus the "
        "attenuation of the division), brightness, harmonic tilt, harmonic to noise ratio, attack, pitch heard at the "
        "root key of the division (sample correction and tuning parameters of the instrument included, fundamental "
        "searched within a fifth around the root key of the sample), loop seam correlation, keys below and above the "
        "root key. Then the linear slope of each measure along the keyboard and the spread around it, and the keys not "
        "covered. Preset parameters, filter, envelopes and modulators are not applied: see render_keyboard for what is "
        "heard.",
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
            QStringList columns = { "division", "keys", "root_key", "sample", "sample_mode", "loudness_lufs",
                                    "loudness_minus_attenuation_lufs", "centroid_hz", "centroid_in_harmonics",
                                    "harmonic_tilt_db_per_octave", "harmonic_to_noise_db", "attack_ms",
                                    "pitch_at_root_key_cents", "loop_seam_correlation", "keys_below_root",
                                    "keys_above_root" };
            QJsonArray rows;
            QVector<double> positions, loudness, centroidHarmonics, tilts, hnrs, attacks, pitches;
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
                int sampleMode = qRound(effective(idDiv, champ_sampleModes));
                bool isRelease = (sampleMode == 2);
                if (!isRelease)
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
                double pitch = std::nan("");
                if (f0 > 0)
                    pitch = 100 * (SampleUtils::frequencyToKey(f0) - rootKey) + sm->get(idSmpl, champ_chPitchCorrection).cValue +
                            effective(idDiv, champ_fineTune) + 100 * effective(idDiv, champ_coarseTune);

                QJsonArray row;
                row.append(McpUtils::idToString(idDiv));
                row.append(McpUtils::keyName(keyRange.byLo) + ".." + McpUtils::keyName(keyRange.byHi));
                row.append(rootKey);
                row.append(McpUtils::idToString(idSmpl));
                row.append(sampleMode);
                row.append(SoundAnalyzer::jsonNumber(lufs, 1));
                row.append(SoundAnalyzer::jsonNumber(lufs - attenuation, 1));
                row.append(SoundAnalyzer::jsonNumber(analyzer->getSpectralCentroid(), 0));
                row.append(SoundAnalyzer::jsonNumber(analyzer->getHarmonicCentroid(), 2));
                row.append(SoundAnalyzer::jsonNumber(analyzer->getHarmonicTilt(), 1));
                row.append(SoundAnalyzer::jsonNumber(analyzer->getHnr(), 1));
                row.append(SoundAnalyzer::jsonNumber(1000 * analyzer->getAttackTime(), 1));
                row.append(SoundAnalyzer::jsonNumber(pitch, 1));
                row.append(analyzer->isLooped() ? SoundAnalyzer::jsonNumber(analyzer->getLoopCorrelation(), 3) : QJsonValue());
                row.append(qMax(0, rootKey - keyRange.byLo));
                row.append(qMax(0, keyRange.byHi - rootKey));
                rows.append(row);

                // Release samples are not part of the slopes
                if (!isRelease)
                {
                    positions << 0.5 * (keyRange.byLo + keyRange.byHi);
                    loudness << lufs - attenuation;
                    centroidHarmonics << analyzer->getHarmonicCentroid();
                    tilts << analyzer->getHarmonicTilt();
                    hnrs << analyzer->getHnr();
                    attacks << 1000 * analyzer->getAttackTime();
                    pitches << pitch;
                }
            }
            qDeleteAll(analyzers);

            QJsonObject result = McpUtils::brief(idInst);
            result["columns"] = QJsonArray::fromStringList(columns);
            result["divisions"] = rows;

            QJsonObject slopes;
            slopes["loudness_minus_attenuation_lufs"] = keyboardSlope(positions, loudness, "dB");
            slopes["centroid_in_harmonics"] = keyboardSlope(positions, centroidHarmonics, "harmonic ranks");
            slopes["harmonic_tilt_db_per_octave"] = keyboardSlope(positions, tilts, "dB per octave");
            slopes["harmonic_to_noise_db"] = keyboardSlope(positions, hnrs, "dB");
            slopes["attack_ms"] = keyboardSlope(positions, attacks, "ms");
            slopes["pitch_at_root_key_cents"] = keyboardSlope(positions, pitches, "cents");
            result["slopes"] = slopes;

            // Keys not covered (release samples excluded) between the lowest and the highest covered keys
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
            result["keys_not_covered"] = gaps;
            result["note"] = "sample_mode: 0 = no loop, 1 = loop, 2 = played when the key is released (release "
                             "sample), 3 = loop then end of the sample at the release; loudness of the sustained part "
                             "(the loop if any); pitch_at_root_key_cents: cents from the equal temperament note of the "
                             "root key (A4 = 440 Hz), positive = higher, static tuning only (the modulation envelope, "
                             "the LFOs and the modulators acting on the pitch are not included: render_note measures "
                             "the pitch heard); slopes: linear regression of each measure on the "
                             "middle of the key ranges (release samples excluded), per_octave = change per 12 keys, value_at_c4 = value "
                             "of the line at key 60, spread = robust standard deviation of the measures around the line";
            return result;
        });

    functions << new McpFunction(
        "render_note", McpFunction::CATEGORY_ANALYSIS, true,
        "Play a note or a chord with the sound engine of Polyphone and measure what is heard. One or several elements "
        "together (a registration: presets, instruments or samples of the same soundfont). Offline rendering, A4 = "
        "440 Hz, equal temperament, controllers at their default values as in the editor unless given, no chorus, "
        "reverb only if asked. Measures: levels and loudness of the stereo signal, regions, envelope and release "
        "times, spectrum, partials (frequency, level, ratio to the keys, level variation and beats), stereo image; for "
        "a single key also the fundamental (common period of what is heard, searched over the whole range), the "
        "harmonics and the pitch every 20 ms during the whole rendering, release included, whatever produces its "
        "changes (envelopes, LFOs, modulators, the samples themselves). With solo, each element is also rendered "
        "alone and the ensemble without it. Optionally writes the rendering in a wav file for the user.",
        R"JSON({"properties":{
            "elements":{"type":["string","array"],"items":{"type":"string"},"description":"Presets, instruments or samples played together"},
            "element":{"type":"string","description":"A single element (same as elements)"},
            "keys":{"type":["integer","string","array"],"description":"Key or keys played together (numbers or note names); for a sample alone, default = its root key"},
            "key":{"type":["integer","string"],"description":"A single key (same as keys)"},
            "velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Default 100"},
            "hold_s":{"type":"number","minimum":0.05,"maximum":30,"description":"Duration before the keys are released (default 3)"},
            "release_s":{"type":"number","minimum":0,"maximum":20,"description":"Duration recorded after the release (default 1.5)"},
            "max_partials":{"type":"integer","minimum":0,"maximum":200,"description":"Number of partials listed (default 24)"},
            "solo":{"type":"boolean","description":"With several elements: also measure each element alone and the ensemble without it (default false)"},
            "controllers":{"type":["object","array"],"description":"MIDI controllers set before the notes, on all channels used: {\"7\": 100, \"cc11\": 64} or [[7, 100]]"},
            "reverb":{"type":["number","array"],"description":"Reverb level 0-100, or [level, size, width, damping] (default: none)"},
            "wav_file":{"type":"string","description":"Absolute path of a wav file to write (optional)"}}})JSON",
        [](const McpArguments &args) -> McpResult {
            IdList elements = readRenderedElements(args);
            QList<int> keys = readKeys(args, elements);
            int velocity = args.getInt("velocity", 1, 127, 100);
            double hold = args.getDouble("hold_s", 0.05, 30, 3);
            double release = args.getDouble("release_s", 0, 20, 1.5);
            int maxPartials = args.getInt("max_partials", 0, 200, 24);
            bool solo = args.getBool("solo", false);
            QList<int> reverb = readReverb(args);
            QList<QPair<int, int> > controllers = readControllers(args);
            QString wavFile = args.getString("wav_file", "");
            if (!wavFile.isEmpty())
                wavFile = QFileInfo(wavFile).absoluteFilePath();

            auto render = [&](const IdList &set, QString wav, QVector<float> &left, QVector<float> &right) -> quint32 {
                QMap<int, int> channels;
                QStringList lines = renderSettings(set, hold + release + 0.01, reverb, controllers, channels);
                addNotes(lines, set, channels, keys, velocity, 0, hold);
                return renderLines(set[0].indexSf2, lines, left, right, wav);
            };

            QVector<float> left, right;
            quint32 rate = render(elements, wavFile, left, right);
            QJsonObject result;
            QJsonArray elementArray, keyNames;
            foreach (EltID id, elements)
                elementArray.append(McpUtils::brief(id));
            foreach (int key, keys)
                keyNames.append(McpUtils::keyName(key));
            result["elements"] = elementArray;
            result["keys"] = keyNames;
            result["velocity"] = velocity;
            double ensembleLoudness;
            QJsonObject measures = measureRendering(left, right, rate, hold, keys, maxPartials, ensembleLoudness);
            for (auto it = measures.constBegin(); it != measures.constEnd(); ++it)
                result[it.key()] = it.value();

            if (solo && elements.count() > 1)
            {
                int holdFrames = static_cast<int>(hold * rate);
                QJsonArray perElement;
                foreach (EltID id, elements)
                {
                    // Alone
                    QVector<float> soloLeft, soloRight;
                    render(IdList(id), "", soloLeft, soloRight);
                    bool leftIsFirst;
                    QScopedPointer<SoundAnalyzer> analyzer(stereoAnalyzer(soloLeft, soloRight, 0, holdFrames, rate, 0, leftIsFirst));
                    QJsonObject item = McpUtils::brief(id);
                    QJsonObject levels = analyzer->getLevels();
                    item["steady_loudness_lufs"] = levels["steady_loudness_lufs"];
                    item["momentary_max_lufs"] = levels["momentary_max_lufs"];
                    item["steady_a_weighted_dbfs"] = levels["steady_a_weighted_dbfs"];
                    item["peak_dbfs"] = levels["peak_dbfs"];
                    QJsonObject regions = analyzer->getRegions();
                    item["onset_ms"] = regions["onset_ms"];
                    item["attack_ms"] = regions["attack_ms"];
                    item["centroid_hz"] = SoundAnalyzer::jsonNumber(analyzer->getSpectralCentroid(), 0);
                    double start, end;
                    QList<SoundAnalyzer::Partial> partials = analyzer->getPartials(qMin(maxPartials, 12), start, end);
                    item["partials"] = partialsTable(partials, keys, start, end);

                    // Ensemble without this element
                    IdList others = elements;
                    others.removeAll(id);
                    QVector<float> otherLeft, otherRight;
                    render(others, "", otherLeft, otherRight);
                    QScopedPointer<SoundAnalyzer> othersAnalyzer(stereoAnalyzer(otherLeft, otherRight, 0, holdFrames, rate, 0, leftIsFirst));
                    double withoutLoudness = othersAnalyzer->getSteadyLoudness();
                    item["ensemble_without_lufs"] = SoundAnalyzer::jsonNumber(withoutLoudness, 2);
                    item["contribution_db"] = SoundAnalyzer::jsonNumber(ensembleLoudness - withoutLoudness, 2);
                    perElement.append(item);
                }
                result["per_element"] = perElement;
            }
            if (!wavFile.isEmpty())
                result["wav_file"] = wavFile;
            result["note"] = QString(PARTIALS_NOTE) + "; pitch_track: cents from the equal temperament frequency of the "
                             "key every hop_s from first_time_s (null: no pitch found within a fifth around the held "
                             "fundamental), the key being released at release_time_s; release_max_change_cents: largest "
                             "pitch change after the release relative to the held fundamental; per_element: each element "
                             "rendered alone (onset and attack in ms from the note on), contribution_db = loudness of the "
                             "ensemble minus loudness of the ensemble without the element";
            return result;
        });

    functions << new McpFunction(
        "render_keyboard", McpFunction::CATEGORY_ANALYSIS, true,
        "Play keys one after the other with the sound engine of Polyphone (same settings as render_note), one or "
        "several elements together (a registration), and measure each note: levels and loudness of the stereo signal, "
        "brightness, fundamental searched over the whole range (with its ratio and cents to the key), largest pitch "
        "change after the release, attack, release, stereo balance and correlation, and the main partials (ratio to "
        "the key, level, level variation and beats); then the linear slope of the main measures along the keyboard "
        "and the spread around it. Optionally writes the whole sequence in a wav file for the user.",
        R"JSON({"properties":{
            "elements":{"type":["string","array"],"items":{"type":"string"},"description":"Presets, instruments or samples played together"},
            "element":{"type":"string","description":"A single element (same as elements)"},
            "keys":{"type":"array","description":"Keys played (numbers or note names), instead of key_range"},
            "key_range":{"type":"array","description":"[first, last] keys (numbers or note names)"},
            "step":{"type":"integer","minimum":1,"maximum":24,"description":"Interval between the keys of key_range (default 1)"},
            "velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Default 100"},
            "hold_s":{"type":"number","minimum":0.1,"maximum":20,"description":"Duration of each note before its release (default 1.5)"},
            "release_s":{"type":"number","minimum":0,"maximum":10,"description":"Silence after each release, before the next key (default 0.5)"},
            "max_partials":{"type":"integer","minimum":0,"maximum":40,"description":"Number of partials listed per key (default 8)"},
            "controllers":{"type":["object","array"],"description":"MIDI controllers set before the notes, on all channels used: {\"7\": 100, \"cc11\": 64} or [[7, 100]]"},
            "reverb":{"type":["number","array"],"description":"Reverb level 0-100, or [level, size, width, damping] (default: none)"},
            "wav_file":{"type":"string","description":"Absolute path of a wav file to write (optional)"}}})JSON",
        [](const McpArguments &args) -> McpResult {
            IdList elements = readRenderedElements(args);
            QList<int> keys;
            if (args.contains("keys"))
            {
                for (const QJsonValue value : args.getArray("keys"))
                    keys << McpUtils::readKey(value, "keys");
            }
            else
            {
                QJsonArray range = args.getArray("key_range");
                if (range.count() != 2)
                    throw McpException("key_range must be [first, last]");
                int first = McpUtils::readKey(range[0], "key_range");
                int last = McpUtils::readKey(range[1], "key_range");
                int step = args.getInt("step", 1, 24, 1);
                for (int key = qMin(first, last); key <= qMax(first, last); key += step)
                    keys << key;
            }
            if (keys.isEmpty())
                throw McpException("no keys to play");
            int velocity = args.getInt("velocity", 1, 127, 100);
            double hold = args.getDouble("hold_s", 0.1, 20, 1.5);
            double release = args.getDouble("release_s", 0, 10, 0.5);
            int maxPartials = args.getInt("max_partials", 0, 40, 8);
            QList<int> reverb = readReverb(args);
            QList<QPair<int, int> > controllers = readControllers(args);
            QString wavFile = args.getString("wav_file", "");
            if (!wavFile.isEmpty())
                wavFile = QFileInfo(wavFile).absoluteFilePath();
            double period = hold + release;
            if (keys.count() * period > 300)
                throw McpException(QString("the sequence would last %1 s, 300 s at most: fewer keys or shorter notes").arg(keys.count() * period));

            // One rendering of the whole sequence
            QMap<int, int> channels;
            QStringList lines = renderSettings(elements, keys.count() * period + 0.01, reverb, controllers, channels);
            for (int i = 0; i < keys.count(); i++)
                addNotes(lines, elements, channels, QList<int>({ keys[i] }), velocity, i * period, i * period + hold);
            QVector<float> left, right;
            quint32 rate = renderLines(elements[0].indexSf2, lines, left, right, wavFile);

            QStringList columns = { "key", "key_name", "peak_dbfs", "steady_loudness_lufs", "steady_a_weighted_dbfs",
                                    "centroid_hz", "fundamental_hz", "fundamental_ratio_to_key", "fundamental_cents_from_key",
                                    "release_max_pitch_change_cents", "centroid_in_harmonics", "onset_ms", "attack_ms",
                                    "release_minus_40_db_s", "stereo_balance_db", "stereo_correlation", "partials" };
            QJsonArray rows;
            QVector<double> keyValues, loudness, aWeighted, centroids, attacks;
            for (int i = 0; i < keys.count(); i++)
            {
                int start = qMin(static_cast<int>(left.size()), static_cast<int>(i * period * rate));
                int holdEnd = qMin(static_cast<int>(left.size()), start + static_cast<int>(hold * rate));
                int end = qMin(static_cast<int>(left.size()), start + static_cast<int>(period * rate));
                bool leftIsFirst;
                QScopedPointer<SoundAnalyzer> analyzer(stereoAnalyzer(left, right, start, holdEnd, rate, 0, leftIsFirst));
                QJsonObject levels = analyzer->getLevels();
                QJsonObject regions = analyzer->getRegions();
                double f0 = analyzer->getFundamental();
                double keyFrequency = SampleUtils::keyToFrequency(keys[i]);
                double releaseChange = std::nan("");
                if (f0 > 0)
                    pitchTrack(leftIsFirst ? left : right, start, end, rate, f0, keys[i], hold, releaseChange);
                QJsonObject releaseInfo = SoundAnalyzer::getRelease(stereoMagnitude(left, right, start, end), rate, holdEnd - start);
                QJsonObject stereo = SoundAnalyzer::getStereo(left.mid(start, holdEnd - start), right.mid(start, holdEnd - start), rate, 0,
                                                              static_cast<quint32>(holdEnd - start));
                double startS, endS;
                QJsonArray partials;
                foreach (const SoundAnalyzer::Partial &partial, analyzer->getPartials(maxPartials, startS, endS))
                    partials.append(QJsonArray({ Utils::roundSignificant(partial.frequency / keyFrequency, 6),
                                                 SoundAnalyzer::jsonNumber(partial.levelDb, 1),
                                                 SoundAnalyzer::jsonNumber(partial.variationDb, 1),
                                                 std::isfinite(partial.beatRate) && partial.beatRate > 0 ?
                                                     QJsonValue(SoundAnalyzer::jsonNumber(partial.beatRate, 2)) : QJsonValue() }));

                QJsonArray row;
                row.append(keys[i]);
                row.append(McpUtils::keyName(keys[i]));
                row.append(levels["peak_dbfs"]);
                row.append(levels["steady_loudness_lufs"]);
                row.append(levels["steady_a_weighted_dbfs"]);
                row.append(SoundAnalyzer::jsonNumber(analyzer->getSpectralCentroid(), 0));
                row.append(f0 > 0 ? QJsonValue(Utils::roundSignificant(f0, 7)) : QJsonValue());
                row.append(f0 > 0 ? QJsonValue(Utils::roundSignificant(f0 / keyFrequency, 6)) : QJsonValue());
                row.append(f0 > 0 ? SoundAnalyzer::jsonNumber(100 * (SampleUtils::frequencyToKey(f0) - keys[i]), 1) : QJsonValue());
                row.append(SoundAnalyzer::jsonNumber(releaseChange, 1));
                row.append(SoundAnalyzer::jsonNumber(analyzer->getHarmonicCentroid(), 2));
                row.append(regions["onset_ms"]);
                row.append(SoundAnalyzer::jsonNumber(1000 * analyzer->getAttackTime(), 1));
                row.append(releaseInfo["release"].toObject()["minus_40_db_s"]);
                row.append(stereo["balance_db"]);
                row.append(stereo["correlation"]);
                row.append(partials);
                rows.append(row);

                keyValues << keys[i];
                loudness << analyzer->getSteadyLoudness();
                aWeighted << levels["steady_a_weighted_dbfs"].toDouble(std::nan(""));
                centroids << analyzer->getHarmonicCentroid();
                attacks << 1000 * analyzer->getAttackTime();
            }

            QJsonObject result;
            QJsonArray elementArray;
            foreach (EltID id, elements)
                elementArray.append(McpUtils::brief(id));
            result["elements"] = elementArray;
            result["velocity"] = velocity;
            result["columns"] = QJsonArray::fromStringList(columns);
            result["notes"] = rows;
            QJsonObject slopes;
            slopes["steady_loudness_lufs"] = keyboardSlope(keyValues, loudness, "LU");
            slopes["steady_a_weighted_dbfs"] = keyboardSlope(keyValues, aWeighted, "dB");
            slopes["centroid_in_harmonics"] = keyboardSlope(keyValues, centroids, "harmonic ranks");
            slopes["attack_ms"] = keyboardSlope(keyValues, attacks, "ms");
            result["slopes"] = slopes;
            if (!wavFile.isEmpty())
                result["wav_file"] = wavFile;
            result["note"] = QString("each key is played at i x (hold_s + release_s) s; a release longer than release_s "
                                     "continues in the next note; levels, loudness (sum of the channels), centroids and "
                                     "partials on both channels, onset, attack and fundamental on the louder channel; "
                                     "fundamental: common period of what is heard, searched over the whole range, cents "
                                     "and ratio from the equal temperament frequency of the key (A4 = 440 Hz); "
                                     "release_max_pitch_change_cents: largest change of the pitch after the release "
                                     "relative to the held fundamental (followed every 20 ms within a fifth); onset_ms "
                                     "and attack_ms: from the note on; partials: [ratio to the key, level dBFS, level "
                                     "variation dB, beat rate Hz or null], strongest first selected then sorted by "
                                     "frequency; slopes: linear regression on the keys, per_octave = change per 12 keys, "
                                     "value_at_c4, spread = robust standard deviation around the line; ") + PARTIALS_NOTE;
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
        "Detailed measures of the loop of a sample (and of its linked sample): seam correlation, junction jump, click "
        "score, level / timbre / pitch jumps, number of periods, harmonic phase mismatch, whether the samples of a "
        "stereo pair have the same loop points, with an image of the waveform heard at the loop point (end of the loop "
        "followed by its start).",
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
