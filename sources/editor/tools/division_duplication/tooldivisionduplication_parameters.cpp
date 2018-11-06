#include "tooldivisionduplication_parameters.h"
#include "contextmanager.h"

void ToolDivisionDuplication_parameters::loadConfiguration()
{
    // Duplication types
    _instDuplicKey = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationKeys", true).toBool();
    _prstDuplicKey = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationKeys", true).toBool();
    _instDuplicVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationVelocity", true).toBool();
    _prstDuplicVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationVelocity", true).toBool();
    
    // Velocity ranges
    QList<QVariant> defaultList;
    defaultList << 0 << 127;
    
    _instVelocityRanges.clear();
    QList<QVariant> listTmp = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "velocities", defaultList).toList();
    for (int i = 0; i < listTmp.size() / 2; i++)
        _instVelocityRanges << QPair<int, int>(listTmp[2 * i].toInt(), listTmp[2 * i + 1].toInt());
    
    _prstVelocityRanges.clear();
    listTmp = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "velocities", defaultList).toList();
    for (int i = 0; i < listTmp.size() / 2; i++)
        _prstVelocityRanges << QPair<int, int>(listTmp[2 * i].toInt(), listTmp[2 * i + 1].toInt());
}

void ToolDivisionDuplication_parameters::saveConfiguration()
{
    // Duplication types
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationKeys", _instDuplicKey);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationKeys", _prstDuplicKey);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationVelocity", _instDuplicVel);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationVelocity", _prstDuplicVel);
    
    // Velocity ranges
    QVariantList listTmp;
    for (int i = 0; i < _instVelocityRanges.size(); i++)
        listTmp << _instVelocityRanges[i].first << _instVelocityRanges[i].second;
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "velocities", listTmp);
    
    listTmp.clear();
    for (int i = 0; i < _prstVelocityRanges.size(); i++)
        listTmp << _prstVelocityRanges[i].first << _prstVelocityRanges[i].second;
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "velocities", listTmp);
}
