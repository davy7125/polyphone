#ifndef TOOLSOUNDSPATIALIZATION_PARAMETERS_H
#define TOOLSOUNDSPATIALIZATION_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolSoundSpatialization_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

private:

};

#endif // TOOLSOUNDSPATIALIZATION_PARAMETERS_H
