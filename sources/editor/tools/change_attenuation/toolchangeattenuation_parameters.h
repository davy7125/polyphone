#ifndef TOOLCHANGEATTENUATION_PARAMETERS_H
#define TOOLCHANGEATTENUATION_PARAMETERS_H

#include "abstracttoolparameters.h"

class ToolChangeAttenuation_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    double getInstValue() { return _instValue; }
    void setInstValue(double instValue) { _instValue = instValue; }

    double getPrstValue() { return _prstValue; }
    void setPrstValue(double prstValue) { _prstValue = prstValue; }

private:
    double _instValue;
    double _prstValue;
};

#endif // TOOLCHANGEATTENUATION_PARAMETERS_H
