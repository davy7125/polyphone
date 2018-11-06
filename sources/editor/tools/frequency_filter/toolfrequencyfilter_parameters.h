#ifndef TOOLFREQUENCYFILTER_PARAMETERS_H
#define TOOLFREQUENCYFILTER_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QVector>

class ToolFrequencyFilter_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    QVector<double> getCurve() { return _curve; }
    void setCurve(QVector<double> curve) { _curve = curve; }

private:
    QVector<double> _curve;
};

#endif // TOOLFREQUENCYFILTER_PARAMETERS_H
