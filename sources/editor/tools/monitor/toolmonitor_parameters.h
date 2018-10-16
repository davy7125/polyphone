#ifndef TOOLMONITOR_PARAMETERS_H
#define TOOLMONITOR_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolMonitor_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

private:

};

#endif // TOOLMONITOR_PARAMETERS_H
