#ifndef TOOLCHANGEVOLUME_PARAMETERS_H
#define TOOLCHANGEVOLUME_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QVector>

class ToolChangeVolume_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    int getMode() { return _mode; }
    void setMode(int mode) { _mode = mode; }

    double getAddValue() { return _addValue; }
    void setAddValue(double addValue) { _addValue = addValue; }

    double getMultiplyValue() { return _multiplyValue; }
    void setMultiplyValue(double multiplyValue) { _multiplyValue = multiplyValue; }

    double getNormalizeValue() { return _normalizeValue; }
    void setNormalizeValue(double normalizeValue) { _normalizeValue = normalizeValue; }

private:
    int _mode;
    double _addValue;
    double _multiplyValue;
    double _normalizeValue;
};

#endif // TOOLCHANGEVOLUME_PARAMETERS_H
