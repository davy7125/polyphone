#ifndef TOOLMIXTURECREATION_PARAMETERS_H
#define TOOLMIXTURECREATION_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolMixtureCreation_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;


private:

};

#endif // TOOLMIXTURECREATION_PARAMETERS_H
