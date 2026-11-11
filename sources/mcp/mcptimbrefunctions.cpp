/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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
#include "utils.h"
#include "soundfontmanager.h"
#include "soundanalyzer.h"
#include "timbremodel.h"
#include "timbreactions.h"
#include <cmath>

static QJsonValue number(double value, int decimals = 2)
{
    return SoundAnalyzer::jsonNumber(value, decimals);
}

static QJsonArray numbers(const QVector<double> &values, int decimals = 2)
{
    QJsonArray result;
    foreach (double value, values)
        result << number(value, decimals);
    return result;
}

// Settings of the model read from the arguments (same names for all functions)
static TimbreSettings readSettings(const McpArguments &args)
{
    TimbreSettings settings;
    settings.velocity = args.getInt("velocity", 1, 127, 100);
    settings.detailedRanks = args.getInt("detailed_ranks", 4, 32, 10);
    QString bands = args.getString("bands", "third_octave");
    if (bands != "third_octave" && bands != "octave")
        throw McpException("argument \"bands\" must be \"third_octave\" or \"octave\"");
    settings.octaveBands = (bands == "octave");
    settings.upperLimit = args.getDouble("upper_limit_hz", 0, 24000, 16000);
    settings.dynamicRange = args.getDouble("dynamic_range_db", 20, 120, 60);
    QString reference = args.getString("reference", "neighbors");
    if (reference != "neighbors" && reference != "regression")
        throw McpException("argument \"reference\" must be \"neighbors\" or \"regression\"");
    settings.reference = reference == "regression" ? TimbreSettings::REFERENCE_REGRESSION : TimbreSettings::REFERENCE_INTERPOLATION;
    settings.regressionCount = args.getInt("regression_count", 2, 8, 3);
    settings.includeMuted = args.getBool("include_muted", false);
    settings.outputSampleRate = 48000;
    return settings;
}

static const char * SETTINGS_SCHEMA =
    R"JSON("velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Velocity played (default 100): the subset is the divisions playing at it"},
        "detailed_ranks":{"type":"integer","minimum":4,"maximum":32,"description":"Ranks given one by one (default 10), the next ones being grouped in bands"},
        "bands":{"type":"string","enum":["third_octave","octave"],"description":"Bands of ranks beyond the detailed ones (default third_octave: 11-12, 13-16, 17-20...)"},
        "upper_limit_hz":{"type":"number","description":"Partials above this frequency are ignored (default 16000, 0 for the half of the sample rate)"},
        "dynamic_range_db":{"type":"number","description":"Ranks further below the strongest one of a key are not compared (default 60)"},
        "reference":{"type":"string","enum":["neighbors","regression"],"description":"Reference of a root key: interpolation between its two neighbors played at its key (default), or robust local regression on several neighbors on each side"},
        "regression_count":{"type":"integer","minimum":2,"maximum":8,"description":"Neighbors on each side for the regression (default 3)"},
        "include_muted":{"type":"boolean","description":"Include the muted divisions (default false: muting excludes noises or other sounds)"})JSON";

static EltID readInstrument(const McpArguments &args)
{
    EltID id = args.getId("instrument", { elementInst });
    id.typeElement = elementInst;
    return id;
}

static QJsonObject comparisonToJson(const TimbreComparison &comparison)
{
    QJsonObject result;
    if (!comparison.valid)
        return result;
    result["level_db"] = number(comparison.levelDifference);
    result["timbre_db"] = number(comparison.timbreDifference);
    result["confidence"] = number(comparison.confidence, 3);
    result["rows_db"] = numbers(comparison.rowDifferences);
    return result;
}

static QJsonArray divisionIds(const TimbreModel &model, const TimbrePosition &position)
{
    QJsonArray result;
    EltID idInst = model.getInstrument();
    foreach (int index, position.divisions)
        result << McpUtils::idToString(EltID(elementInstSmpl, idInst.indexSf2, idInst.indexElt, model.getDivisions()[index].index));
    return result;
}

static TimbreModel * computeModel(EltID idInst, const TimbreSettings &settings)
{
    TimbreModel * model = new TimbreModel();
    model->compute(idInst, settings, true);
    if (model->getDivisions().isEmpty())
    {
        delete model;
        throw McpException("no division of the instrument plays at the velocity " + QString::number(settings.velocity));
    }
    return model;
}

void McpFunctions::addTimbreFunctions(QList<McpFunction *> &functions)
{
    functions << new McpFunction(
        "get_timbre_map", McpFunction::CATEGORY_ANALYSIS, true,
        "Timbre of an instrument along the keyboard as heard at a velocity, without rendering (as the timbre view of "
        "the editor): for each key, the level of each harmonic rank (or band of ranks) of the steady part, once the "
        "samples are transposed and the attenuation, the filter and the modulators of the divisions applied (sound "
        "engine at 48 kHz, envelopes at their sustain level, LFOs at their mean value). A rank is absent (null) beyond "
        "the half of the sample rate of the sample or of the output, or beyond the upper limit. The divisions "
        "playing a key are combined by adding their powers. For each root key (\"position\", all divisions of the "
        "subset having this root key): its levels played at its key, the reference predicted by its neighbors played "
        "at the same key, the deviation from it (level, timbre: weighted quadratic mean of the differences by row "
        "once the level difference is removed, the weight of a row being (power / strongest power)^0.3 with a floor "
        "of 0.05), and what removing it would change (largest deviations over its keys if the neighbors took them, "
        "jump at the new junction). For each junction between two keys played by different divisions: the jump. "
        "Measures only, without thresholds: deciding is left to the caller.",
        (QString(R"JSON({"properties":{
            "instrument":{"type":"string","description":"Instrument identifier"},
            "keys":{"type":"array","items":{"type":["integer","string"]},"description":"Keys described (default: all keys played)"},)JSON") +
         SETTINGS_SCHEMA + R"JSON(},"required":["instrument"]})JSON").toUtf8().constData(),
        [](const McpArguments &args) -> McpResult {
            EltID idInst = readInstrument(args);
            TimbreSettings settings = readSettings(args);
            QScopedPointer<TimbreModel> model(computeModel(idInst, settings));
            QList<int> keys;
            if (args.contains("keys"))
            {
                for (const QJsonValue value : args.getArray("keys"))
                    keys << McpUtils::readKey(value, "keys");
            }
            else
                for (int k = model->getMinKey(); k >= 0 && k <= model->getMaxKey(); k++)
                    keys << k;

            QJsonObject result = McpUtils::brief(idInst);
            result["velocity"] = settings.velocity;
            QJsonArray rows;
            foreach (TimbreRow row, model->getRows())
            {
                QJsonObject item;
                item["label"] = row.label;
                item["first_rank"] = row.firstRank;
                item["last_rank"] = row.lastRank;
                rows << item;
            }
            result["rows"] = rows;

            SoundfontManager * sm = SoundfontManager::getInstance();
            QJsonArray keyArray;
            foreach (int k, keys)
            {
                const TimbreKey &key = model->getKey(k);
                if (key.sources.isEmpty())
                    continue;
                QJsonObject item;
                item["key"] = k;
                item["key_name"] = McpUtils::keyName(k);
                QJsonArray sources;
                foreach (TimbreVoice voice, key.voices)
                {
                    QJsonObject source;
                    source["division"] = McpUtils::idToString(EltID(elementInstSmpl, idInst.indexSf2, idInst.indexElt,
                                                                    model->getDivisions()[voice.divisionIndex].index));
                    source["sample"] = McpUtils::brief(EltID(elementSmpl, idInst.indexSf2, voice.sampleIndex));
                    source["root_key"] = model->getDivisions()[voice.divisionIndex].rootKey;
                    source["transposition_cents"] = number(voice.transposition, 1);
                    source["attenuation_db"] = number(voice.attenuation);
                    source["filter_cutoff_hz"] = number(voice.filterFrequency, 0);
                    source["filter_q_db"] = number(voice.filterQ, 1);
                    sources << source;
                }
                item["sources"] = sources;
                item["analyzed"] = key.analyzed;
                item["total_level_db"] = number(key.totalLevel);
                item["rows_db"] = numbers(model->getRowLevels(key.spectrum));
                item["transposition_semitones"] = number(key.transposition, 2);
                item["duration_factor"] = number(key.durationFactor, 4);
                item["centroid_rank"] = number(key.centroid, 2);
                item["slope_db_per_octave"] = number(key.slope, 2);
                item["odd_even_db"] = number(key.oddEven, 2);
                item["tuning_cents"] = number(key.tuning, 1);
                item["hnr_db"] = number(key.hnr, 1);
                item["loop_correlation"] = number(key.loopCorrelation, 4);
                if (key.junction)
                {
                    QJsonObject junction = comparisonToJson(key.jump);
                    junction["sample_change"] = key.sampleChange;
                    item["junction_with_previous_key"] = junction;
                }
                keyArray << item;
            }
            result["keys"] = keyArray;
            Q_UNUSED(sm)

            QJsonArray positions;
            foreach (TimbrePosition position, model->getPositions().values())
            {
                QJsonObject item;
                item["root_key"] = position.key;
                item["root_key_name"] = McpUtils::keyName(position.key);
                item["divisions"] = divisionIds(*model, position);
                item["key_range"] = QJsonArray({ position.keyMin, position.keyMax });
                item["rows_db"] = numbers(model->getRowLevels(position.spectrum));
                if (!position.reference.isEmpty())
                {
                    item["reference_rows_db"] = numbers(model->getRowLevels(position.reference));
                    item["reference_extrapolated"] = position.referenceAtEdge;
                    item["deviation"] = comparisonToJson(position.deviation);
                }
                if (position.redundancyComputed)
                {
                    QJsonObject removal;
                    removal["largest_timbre_deviation_db"] = number(position.redundancy);
                    removal["largest_level_deviation_db"] = number(position.redundancyLevel);
                    if (position.newJunction.valid)
                        removal["new_junction"] = comparisonToJson(position.newJunction);
                    item["if_removed"] = removal;
                }
                positions << item;
            }
            result["positions"] = positions;
            result["note"] = "levels in dBFS of the sine of each rank as heard (a row of several ranks: power sum); "
                             "deviations are \"heard - reference\" in dB; rows_db of a comparison are by row with the level "
                             "difference included; confidence: part of the power of the reference in the ranks compared "
                             "(found in both, within the dynamic range); duration_factor = 2^(-transposition / 1200): "
                             "attack, tremolo and beats are faster below 1; reference_extrapolated: root key at an end, "
                             "the reference being extrapolated from the neighbors on one side";
            return result;
        });

    functions << new McpFunction(
        "remove_root_key", McpFunction::CATEGORY_EDITING, false,
        "Remove a root key (\"position\") of the subset of an instrument playing at a velocity: its divisions are "
        "removed and its keys are given to the neighbors, the limit between them being the one of the tool \"auto "
        "distribution\" (a neighbor takes all keys up to the end if the root key was the first or the last one). The "
        "samples not used anymore can be removed too. See get_timbre_map (if_removed) for the consequences.",
        (QString(R"JSON({"properties":{
            "instrument":{"type":"string","description":"Instrument identifier"},
            "root_key":{"type":["integer","string"],"description":"Root key to remove"},
            "remove_unused_samples":{"type":"boolean","description":"Also remove the samples not used anymore (default true)"},)JSON") +
         SETTINGS_SCHEMA + R"JSON(},"required":["instrument","root_key"]})JSON").toUtf8().constData(),
        [](const McpArguments &args) -> McpResult {
            EltID idInst = readInstrument(args);
            TimbreSettings settings = readSettings(args);
            int rootKey = args.getKey("root_key");
            QScopedPointer<TimbreModel> model(computeModel(idInst, settings));
            if (!model->getPositions().contains(rootKey))
                throw McpException("no division of the subset has the root key " + McpUtils::keyName(rootKey));
            const TimbrePosition position = model->getPositions()[rootKey];
            QJsonObject result;
            result["removed_divisions"] = divisionIds(*model, position);
            QMap<int, int> keys = model->getKeysAfterRemoval(rootKey);
            QJsonObject given;
            foreach (int key, keys.keys())
                if (keys[key] >= 0)
                    given[QString::number(key)] = keys[key];
            result["keys_given_to_root_keys"] = given;
            TimbreActions::removePosition(SoundfontManager::getInstance(), *model, rootKey,
                                          args.getBool("remove_unused_samples", true));
            return result;
        });

    functions << new McpFunction(
        "correct_root_key", McpFunction::CATEGORY_EDITING, false,
        "Correct the timbre and the level of a root key (\"position\") of the subset of an instrument playing at a "
        "velocity, toward a target: the interpolation of its neighbors, a local regression, or another root key "
        "played at its key, possibly with a brightness slope and a level of the odd ranks added. The gain of each row "
        "(rank or band) is the strength times the part of the deviation (target - heard, level removed) beyond the "
        "tolerance, bounded by the maximum correction, for the rows well measured. The level part goes through the "
        "attenuation of the divisions (beyond 0 dB, in the data within the full scale), the timbre part through a "
        "minimum phase gain curve with note tracking applied to the data of the samples (loop kept, same curve for the "
        "two channels of a stereo pair), the data being lowered and the attenuation compensating if the full scale "
        "would be exceeded. Samples also used elsewhere are duplicated for the root key, or modified everywhere. The "
        "result is measured again after the change.",
        (QString(R"JSON({"properties":{
            "instrument":{"type":"string","description":"Instrument identifier"},
            "root_key":{"type":["integer","string"],"description":"Root key to correct"},
            "target":{"type":"string","enum":["neighbors","regression","root_key"],"description":"Target: interpolation of the neighbors (default), local regression (regression_count), or the root key aligned_root_key played at the key"},
            "aligned_root_key":{"type":["integer","string"],"description":"Root key used as target with target \"root_key\""},
            "correct_timbre":{"type":"boolean","description":"Change the timbre (sample data), default true"},
            "correct_level":{"type":"boolean","description":"Change the level (attenuation), default true"},
            "tolerance_db":{"type":"number","description":"Dead zone of each row (default 1.5)"},
            "strength_percent":{"type":"number","description":"Part of the deviation corrected beyond the tolerance (default 100)"},
            "max_correction_db":{"type":"number","description":"Largest gain of a row, on each side (default 12)"},
            "max_rank":{"type":"integer","minimum":0,"maximum":64,"description":"0 (default): all rows, otherwise only the ranks 1 to max_rank"},
            "brightness_db_per_octave":{"type":"number","description":"Slope added to the target (dB per octave of rank, default 0)"},
            "odd_even_db":{"type":"number","description":"Level added to the odd ranks 3, 5, 7... of the target (default 0)"},
            "shared_samples":{"type":"string","enum":["duplicate","modify_all"],"description":"Samples also used by other divisions or instruments: duplicated for this root key (default) or modified everywhere"},)JSON") +
         SETTINGS_SCHEMA + R"JSON(},"required":["instrument","root_key"]})JSON").toUtf8().constData(),
        [](const McpArguments &args) -> McpResult {
            EltID idInst = readInstrument(args);
            TimbreSettings settings = readSettings(args);
            int rootKey = args.getKey("root_key");
            QScopedPointer<TimbreModel> model(computeModel(idInst, settings));
            if (!model->getPositions().contains(rootKey))
                throw McpException("no division of the subset has the root key " + McpUtils::keyName(rootKey));

            TimbreCorrectionParameters parameters;
            QString target = args.getString("target", "neighbors");
            if (target == "regression")
                parameters.reference = TimbreCorrectionParameters::REFERENCE_REGRESSION;
            else if (target == "root_key")
            {
                parameters.reference = TimbreCorrectionParameters::REFERENCE_POSITION;
                parameters.alignedPosition = args.getKey("aligned_root_key");
                if (!model->getPositions().contains(parameters.alignedPosition) || parameters.alignedPosition == rootKey)
                    throw McpException("argument \"aligned_root_key\" must be another root key of the subset");
            }
            else if (target != "neighbors")
                throw McpException("argument \"target\" must be \"neighbors\", \"regression\" or \"root_key\"");
            parameters.regressionCount = settings.regressionCount;
            parameters.correctTimbre = args.getBool("correct_timbre", true);
            parameters.correctLevel = args.getBool("correct_level", true);
            parameters.tolerance = args.getDouble("tolerance_db", 0, 20, 1.5);
            parameters.strength = args.getDouble("strength_percent", 0, 100, 100) / 100;
            parameters.maxCorrection = args.getDouble("max_correction_db", 0, 24, 12);
            parameters.maxRank = args.getInt("max_rank", 0, 64, 0);
            parameters.brightness = args.getDouble("brightness_db_per_octave", -6, 6, 0);
            parameters.oddEven = args.getDouble("odd_even_db", -12, 12, 0);
            QString shared = args.getString("shared_samples", "duplicate");
            if (shared != "duplicate" && shared != "modify_all")
                throw McpException("argument \"shared_samples\" must be \"duplicate\" or \"modify_all\"");

            TimbreCorrectionPlan plan = TimbreActions::planCorrection(*model, rootKey, parameters);
            QJsonObject result;
            result["before"] = comparisonToJson(plan.before);
            if (!plan.valid)
            {
                result["changed"] = false;
                result["note"] = plan.before.valid ? "nothing to correct with these parameters" : "no reference for this root key";
                return result;
            }
            QJsonArray gains;
            foreach (QPointF point, plan.gains)
                gains << QJsonArray({ point.x(), number(point.y()) });
            result["gain_curve_rank_db"] = gains;
            result["level_change_through_attenuation_db"] = number(plan.levelGain);
            result["expected"] = comparisonToJson(plan.expected);

            SoundfontManager * sm = SoundfontManager::getInstance();
            QList<int> sharedSamples = TimbreActions::getSharedSamples(sm, idInst, plan.divisions, plan.samples);
            double compensation = 0;
            QList<TimbreCorrectedSample> samples = TimbreActions::computeCorrection(idInst.indexSf2, plan, compensation);
            QStringList warnings;
            TimbreActions::applyCorrection(sm, plan, samples, compensation, shared == "duplicate", warnings);
            QJsonArray sharedArray;
            foreach (int sampleIndex, sharedSamples)
                sharedArray << McpUtils::brief(EltID(elementSmpl, idInst.indexSf2, sampleIndex));
            result["shared_samples"] = sharedArray;
            result["shared_samples_handling"] = shared;
            if (!warnings.isEmpty())
                result["warnings"] = QJsonArray::fromStringList(warnings);

            // Measured again with the new data (same target)
            TimbreModel after;
            after.compute(idInst, settings, true);
            if (after.getPositions().contains(rootKey))
                result["measured_after"] = comparisonToJson(after.compare(after.getPositions()[rootKey].spectrum, plan.target));
            result["changed"] = true;
            return result;
        });
}
