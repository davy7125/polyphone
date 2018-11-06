#ifndef TOOLPRESETLIST_PARAMETERS_H
#define TOOLPRESETLIST_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolPresetList_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    int getFormat() { return _format; }
    void setFormat(int format) { _format = format; }
    
private:
    int _format;
};

#endif // TOOLPRESETLIST_PARAMETERS_H
