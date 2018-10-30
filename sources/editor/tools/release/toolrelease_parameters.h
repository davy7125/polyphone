#ifndef TOOLRELEASE_PARAMETERS_H
#define TOOLRELEASE_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolRelease_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    double getBaseDuration() { return _baseDuration; }
    void setBaseDuration(double baseDuration) { _baseDuration = baseDuration; }

    double getEvolution() { return _evolution; }
    void setEvolution(double evolution) { _evolution = evolution; }

    double getDetune() { return _detune; }
    void setDetune(double detune) { _detune = detune; }

private:
    double _baseDuration;
    double _evolution;
    double _detune;
};

#endif // TOOLRELEASE_PARAMETERS_H
