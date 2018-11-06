#ifndef TOOLCELESTETUNING_PARAMETERS_H
#define TOOLCELESTETUNING_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolCelesteTuning_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    double getBaseFrequency() { return _baseFrequency; }
    void setBaseFrequency(double baseFrequency) { _baseFrequency = baseFrequency; }

    double getCoefficient() { return _coefficient; }
    void setCoefficient(double coefficient) { _coefficient = coefficient; }

private:
    double _baseFrequency;
    double _coefficient;
};

#endif // TOOLCELESTETUNING_PARAMETERS_H
