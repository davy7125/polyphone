#ifndef TOOLTRANSPOSESMPL_PARAMETERS_H
#define TOOLTRANSPOSESMPL_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolTransposeSmpl_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    double getSemiTones() { return _semiTones; }
    void setSemiTones(double semiTones) { _semiTones = semiTones; }

private:
    double _semiTones;
};

#endif // TOOLTRANSPOSESMPL_PARAMETERS_H
