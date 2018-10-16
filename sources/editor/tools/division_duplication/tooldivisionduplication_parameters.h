#ifndef TOOLDIVISIONDUPLICATION_PARAMETERS_H
#define TOOLDIVISIONDUPLICATION_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolDivisionDuplication_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

private:

};

#endif // TOOLDIVISIONDUPLICATION_PARAMETERS_H
