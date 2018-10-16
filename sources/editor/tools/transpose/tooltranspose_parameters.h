#ifndef TOOLTRANSPOSE_PARAMETERS_H
#define TOOLTRANSPOSE_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolTranspose_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;
    

private:

};

#endif // TOOLTRANSPOSE_PARAMETERS_H
