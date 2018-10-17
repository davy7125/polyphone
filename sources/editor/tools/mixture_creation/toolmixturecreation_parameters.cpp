#include "toolmixturecreation_parameters.h"
#include "contextmanager.h"

void ToolMixtureCreation_parameters::loadConfiguration()
{
    // Instrument properties
    _instrumentName = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "name", "").toString();
    _densityType = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "density", 0).toInt();
    
    // Sample properties
    _stereoSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "stereo", true).toBool();
    _loopSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "loop", true).toBool();
    
    // Divisions
    _divisions.clear();
    QList<QVariant> list1 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "divisions", QList<QVariant>()).toList();
    foreach (QVariant element, list1)
    {
        QList<QVariant> list2 = element.toList();
        if (list2.count() >= 4)
        {
            // The two first elements of the list are the range of the division
            DivisionInfo division(list2.takeFirst().toInt(), list2.takeFirst().toInt());
            
            // Then, ranks are specified by two numbers
            while (list2.count() >= 2)
                division.addRank(list2.takeFirst().toInt(), list2.takeFirst().toInt());
        }
    }
}

void ToolMixtureCreation_parameters::saveConfiguration()
{
    // Instrument properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "name", _instrumentName);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "density", _densityType);
    
    // Sample properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "stereo", _stereoSample);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "loop", _loopSample);
    
    // Divisions
    QList<QVariant> listTmp;
    foreach (DivisionInfo division, _divisions)
    {
        QList<QVariant> listDivision;
        
        // The two first elements are the key range
        listDivision << division.getMinKey();
        listDivision << division.getMaxKey();
        
        // Then the description of the ranks
        foreach (RankInfo rank, division.getRanks())
            listDivision << rank.getOvertoneType() << rank.getOctave();
        
        listTmp << listDivision;
    }
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "divisions", listTmp);
}
