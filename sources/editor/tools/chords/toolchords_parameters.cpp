#include "toolchords_parameters.h"
#include "contextmanager.h"

void ToolChords_parameters::loadConfiguration()
{
    // Instrument properties
    _instrumentName = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "name", "").toString();
    _densityType = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "density", 1).toInt();
    
    // Sample properties
    _stereoSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "stereo", true).toBool();
    _loopSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "loop", true).toBool();
    
    // Chord configuration
    _chordInfo.chordType3 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3", 1).toInt();
    _chordInfo.chordType5 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5", 1).toInt();
    _chordInfo.chordType7 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7", 0).toInt();
    _chordInfo.chordType9 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9", 0).toInt();
    _chordInfo.rootNote = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "rootNote", -1).toInt();
    _chordInfo.harmonicNumber = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "harmonicNumber", 2).toInt();
    _chordInfo.attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "attenuation", 5.0).toDouble();
    _chordInfo.position = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "position", 0).toInt();
}

void ToolChords_parameters::saveConfiguration()
{
    // Instrument properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "name", _instrumentName);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "density", _densityType);
    
    // Sample properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "stereo", _stereoSample);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "loop", _loopSample);
    
    // Chord configuration
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3", _chordInfo.chordType3);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5", _chordInfo.chordType5);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7", _chordInfo.chordType7);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9", _chordInfo.chordType9);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "rootNote", _chordInfo.rootNote);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "harmonicNumber", _chordInfo.harmonicNumber);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "attenuation", _chordInfo.attenuation);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "position", _chordInfo.position);
}
