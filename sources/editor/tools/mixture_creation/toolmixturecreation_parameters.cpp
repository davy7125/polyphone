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
    QStringList listDivision = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "divisions", QList<QVariant>()).toStringList();
    foreach (QString divisionStr, listDivision)
    {
        QStringList listTmp = divisionStr.split(' ');
        if (listTmp.count() >= 4)
        {
            // The two first elements of the list are the range of the division
            DivisionInfo division(listTmp.takeFirst().toInt(), listTmp.takeFirst().toInt());
            
            // Then, ranks are specified by two numbers
            while (listTmp.count() >= 2)
                division.addRank(listTmp.takeFirst().toInt(), listTmp.takeFirst().toInt());

            _divisions << division;
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
    QStringList listDivision;
    foreach (DivisionInfo division, _divisions)
    {
        QStringList divisionStr;
        
        // The two first elements are the key range
        divisionStr << QString::number(division.getMinKey()) << QString::number(division.getMaxKey());
        
        // Then the description of the ranks
        foreach (RankInfo rank, division.getRanks())
            divisionStr << QString::number(rank.getOctave()) << QString::number(rank.getOvertoneType());
        
        listDivision << divisionStr.join(' ');
    }
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "mixture", "divisions", listDivision);
}
