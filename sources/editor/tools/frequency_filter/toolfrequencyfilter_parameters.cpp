#include "toolfrequencyfilter_parameters.h"
#include "contextmanager.h"

void ToolFrequencyFilter_parameters::loadConfiguration()
{
    // Curve
    QList<QVariant> listTmp = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_SAMPLE,
                "filter_frequencies", "curve", QList<QVariant>()).toList();
    _curve.resize(listTmp.size());
    for (int i = 0; i < listTmp.size(); i++)
        _curve[i] = listTmp.at(i).toDouble();
}

void ToolFrequencyFilter_parameters::saveConfiguration()
{
    // Curve
    QVariantList listTmp;
    for (int i = 0; i < _curve.size(); i++)
        listTmp << _curve.at(i);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "filter_frequencies", "curve", listTmp);
}
