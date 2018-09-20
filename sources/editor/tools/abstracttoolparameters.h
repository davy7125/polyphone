#ifndef ABSTRACTTOOLPARAMETERS_H
#define ABSTRACTTOOLPARAMETERS_H


class AbstractToolParameters
{
public:
    AbstractToolParameters() {}
    virtual ~AbstractToolParameters() {}

    /// Load the configuration from the ini file
    virtual void loadConfiguration() = 0;

    /// Save the configuration in the ini file
    virtual void saveConfiguration() = 0;
};

#endif // ABSTRACTTOOLPARAMETERS_H
