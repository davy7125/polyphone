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

#include "mcpmodulators.h"
#include "mcpattributes.h"
#include "mcputils.h"
#include "mcpfunction.h"
#include "soundfontmanager.h"
#include <QRegularExpression>

const char * McpModulators::FORMAT =
    "A modulator is {\"source\", \"amount_source\", \"destination\", \"amount\", \"transform\"}. "
    "Sources are \"none\", \"velocity\", \"key\", \"poly_pressure\", \"channel_pressure\", \"pitch_wheel\", "
    "\"pitch_wheel_sensitivity\", \"link\" (output of another modulator) or \"ccN\" (MIDI controller N, for instance "
    "\"cc1\" for the modulation wheel), either as a string or as an object {\"controller\", \"shape\" (linear, concave, "
    "convex, switch), \"polarity\" (unipolar, bipolar), \"direction\" (increasing, decreasing)}. The destination is a "
    "parameter name (see list_parameters) or \"modN\" to feed the modulator N of the same element. The amount is an "
    "integer in the native soundfont unit of the destination (cents for pitch and filter cutoff, centibels for "
    "attenuation, timecents for durations, 0.1% for pan, sends and sustain levels). The transform is \"linear\" or "
    "\"absolute\".";

EltID McpModulators::getModId(EltID idParent, int indexMod)
{
    EltID idMod = idParent;
    switch (idParent.typeElement)
    {
    case elementInst: idMod.typeElement = elementInstMod; break;
    case elementPrst: idMod.typeElement = elementPrstMod; break;
    case elementInstSmpl: idMod.typeElement = elementInstSmplMod; break;
    case elementPrstInst: idMod.typeElement = elementPrstInstMod; break;
    default:
        throw McpException("modulators belong to instruments, presets or divisions");
    }
    idMod.indexMod = indexMod;
    return idMod;
}

QJsonArray McpModulators::getAll(EltID idParent)
{
    EltID idMod = getModId(idParent, -1);
    QJsonArray result;
    foreach (int i, SoundfontManager::getInstance()->getSiblings(idMod))
    {
        idMod.indexMod = i;
        result.append(toJson(idMod));
    }
    return result;
}

QJsonValue McpModulators::sourceToJson(SFModulator source)
{
    QString controller;
    if (source.CC)
        controller = QString("cc%1").arg(source.Index);
    else
    {
        switch (source.Index)
        {
        case GC_noController: controller = "none"; break;
        case GC_noteOnVelocity: controller = "velocity"; break;
        case GC_noteOnKeyNumber: controller = "key"; break;
        case GC_polypressure: controller = "poly_pressure"; break;
        case GC_channelPressure: controller = "channel_pressure"; break;
        case GC_pitchWheel: controller = "pitch_wheel"; break;
        case GC_pitchWheelSensitivity: controller = "pitch_wheel_sensitivity"; break;
        case GC_link: controller = "link"; break;
        default: controller = QString("general_%1").arg(source.Index); break;
        }
    }
    if (controller == "none")
        return controller;

    static const char * shapes[4] = { "linear", "concave", "convex", "switch" };
    QJsonObject result;
    result["controller"] = controller;
    result["shape"] = source.Type < 4 ? shapes[source.Type] : "unknown";
    result["polarity"] = source.isBipolar ? "bipolar" : "unipolar";
    result["direction"] = source.isDescending ? "decreasing" : "increasing";
    return result;
}

SFModulator McpModulators::readSource(QJsonValue value, QString fieldName)
{
    QJsonObject object;
    if (value.isString())
        object["controller"] = value.toString();
    else if (value.isObject())
        object = value.toObject();
    else
        throw McpException("\"" + fieldName + "\" must be a controller name or an object");

    SFModulator source;
    QString controller = object.value("controller").toString("none").trimmed().toLower();
    static QRegularExpression ccRegex("^cc(\\d+)$");
    QRegularExpressionMatch match = ccRegex.match(controller);
    if (match.hasMatch())
    {
        int number = match.captured(1).toInt();
        if (number > 127)
            throw McpException("\"" + fieldName + "\": controller number must be between 0 and 127");
        source.CC = true;
        source.Index = static_cast<uint8_t>(number);
    }
    else
    {
        source.CC = false;
        if (controller == "none") source.Index = GC_noController;
        else if (controller == "velocity") source.Index = GC_noteOnVelocity;
        else if (controller == "key") source.Index = GC_noteOnKeyNumber;
        else if (controller == "poly_pressure") source.Index = GC_polypressure;
        else if (controller == "channel_pressure") source.Index = GC_channelPressure;
        else if (controller == "pitch_wheel") source.Index = GC_pitchWheel;
        else if (controller == "pitch_wheel_sensitivity") source.Index = GC_pitchWheelSensitivity;
        else if (controller == "link") source.Index = GC_link;
        else
            throw McpException("\"" + fieldName + "\": unknown controller \"" + controller + "\"");
    }

    QString shape = object.value("shape").toString("linear");
    if (shape == "linear") source.Type = typeLinear;
    else if (shape == "concave") source.Type = typeConcave;
    else if (shape == "convex") source.Type = typeConvex;
    else if (shape == "switch") source.Type = typeSwitch;
    else
        throw McpException("\"" + fieldName + "\": unknown shape \"" + shape + "\"");

    QString polarity = object.value("polarity").toString("unipolar");
    if (polarity != "unipolar" && polarity != "bipolar")
        throw McpException("\"" + fieldName + "\": polarity must be \"unipolar\" or \"bipolar\"");
    source.isBipolar = (polarity == "bipolar");

    QString direction = object.value("direction").toString("increasing");
    if (direction != "increasing" && direction != "decreasing")
        throw McpException("\"" + fieldName + "\": direction must be \"increasing\" or \"decreasing\"");
    source.isDescending = (direction == "decreasing");
    return source;
}

QString McpModulators::nativeUnit(AttributeType champ)
{
    switch (champ)
    {
    case champ_initialAttenuation: case champ_sustainVolEnv: case champ_modLfoToVolume: case champ_initialFilterQ:
        return "centibels";
    case champ_pan: case champ_chorusEffectsSend: case champ_reverbEffectsSend: case champ_sustainModEnv:
        return "0.1%";
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv:
    case champ_releaseModEnv: case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv:
    case champ_decayVolEnv: case champ_releaseVolEnv: case champ_delayModLFO: case champ_delayVibLFO:
        return "timecents";
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_keynumToVolEnvHold:
    case champ_keynumToVolEnvDecay:
        return "timecents per key";
    case champ_coarseTune:
        return "semitones";
    default:
        return "cents";
    }
}

QJsonObject McpModulators::toJson(EltID idMod)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QJsonObject result;
    result["id"] = McpUtils::idToString(idMod);
    result["source"] = sourceToJson(sm->get(idMod, champ_sfModSrcOper).sfModValue);
    result["amount_source"] = sourceToJson(sm->get(idMod, champ_sfModAmtSrcOper).sfModValue);

    quint16 destination = sm->get(idMod, champ_sfModDestOper).wValue;
    if (destination >= 32768)
    {
        result["destination"] = QString("mod%1").arg(destination - 32768);
        result["amount_unit"] = "contribution to the linked modulator";
    }
    else
    {
        QString name = McpAttributes::getName(static_cast<AttributeType>(destination));
        result["destination"] = name.isEmpty() ? QJsonValue(static_cast<int>(destination)) : QJsonValue(name);
        result["amount_unit"] = nativeUnit(static_cast<AttributeType>(destination));
    }
    result["amount"] = sm->get(idMod, champ_modAmount).shValue;
    result["transform"] = sm->get(idMod, champ_sfModTransOper).sfTransValue == absolute_value ? "absolute" : "linear";
    return result;
}

EltID McpModulators::add(EltID idParent, QJsonObject description)
{
    if (!description.contains("source") || !description.contains("destination") || !description.contains("amount"))
        throw McpException("a new modulator needs at least \"source\", \"destination\" and \"amount\"");

    EltID idMod = getModId(idParent, -1);
    idMod.indexMod = SoundfontManager::getInstance()->add(idMod);
    if (idMod.indexMod < 0)
        throw McpException("cannot add a modulator");

    // Default values, then the description
    QJsonObject complete;
    complete["amount_source"] = "none";
    complete["transform"] = "linear";
    for (QJsonObject::const_iterator it = description.constBegin(); it != description.constEnd(); ++it)
        complete[it.key()] = it.value();
    update(idMod, complete);
    return idMod;
}

void McpModulators::update(EltID idMod, QJsonObject description)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue value;

    for (QJsonObject::const_iterator it = description.constBegin(); it != description.constEnd(); ++it)
    {
        QString key = it.key();
        if (key == "source" || key == "amount_source")
        {
            value.sfModValue = readSource(it.value(), key);
            sm->set(idMod, key == "source" ? champ_sfModSrcOper : champ_sfModAmtSrcOper, value);
        }
        else if (key == "destination")
        {
            quint16 destination;
            static QRegularExpression modRegex("^mod(\\d+)$");
            QRegularExpressionMatch match = modRegex.match(it.value().toString());
            if (match.hasMatch())
            {
                // Link to another modulator of the same element
                EltID idTarget = idMod;
                idTarget.indexMod = match.captured(1).toInt();
                if (idTarget.indexMod == idMod.indexMod || !sm->isValid(idTarget))
                    throw McpException("\"destination\": modulator " + match.captured(0) + " doesn't exist");
                destination = static_cast<quint16>(32768 + idTarget.indexMod);
            }
            else if (it.value().isString())
            {
                AttributeType champ = McpAttributes::getChamp(it.value().toString());
                if (champ == champ_unknown || champ == champ_keyRange || champ == champ_velRange)
                    throw McpException("\"destination\": unknown parameter \"" + it.value().toString() + "\"");
                destination = static_cast<quint16>(champ);
            }
            else if (it.value().isDouble() && it.value().toInt(-1) >= 0 && it.value().toInt(-1) < champ_endOper)
                destination = static_cast<quint16>(it.value().toInt());
            else
                throw McpException("\"destination\" must be a parameter name or \"modN\"");
            value.wValue = destination;
            sm->set(idMod, champ_sfModDestOper, value);
        }
        else if (key == "amount")
        {
            double amount = it.value().toDouble(1e9);
            if (!it.value().isDouble() || amount < -32768 || amount > 32767)
                throw McpException("\"amount\" must be an integer between -32768 and 32767");
            value.shValue = static_cast<int16_t>(qRound(amount));
            sm->set(idMod, champ_modAmount, value);
        }
        else if (key == "transform")
        {
            QString transform = it.value().toString();
            if (transform != "linear" && transform != "absolute")
                throw McpException("\"transform\" must be \"linear\" or \"absolute\"");
            value.sfTransValue = (transform == "absolute" ? absolute_value : linear);
            sm->set(idMod, champ_sfModTransOper, value);
        }
        else if (key != "id" && key != "amount_unit")
            throw McpException("unknown modulator field \"" + key + "\"");
    }
}
