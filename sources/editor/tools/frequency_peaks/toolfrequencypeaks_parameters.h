#ifndef TOOLFREQUENCYPEAKS_PARAMETERS_H
#define TOOLFREQUENCYPEAKS_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QList>
#include <QString>
#include <QMetaType>

class FrequencyInfo
{
public:
    double frequency;
    double factor;
    int key;
    int correction;
};

Q_DECLARE_METATYPE(FrequencyInfo)

class SampleFrequencyInfo
{
public:
    QString name;
    QList<FrequencyInfo> frequencies;
};

Q_DECLARE_METATYPE(SampleFrequencyInfo)

class ToolFrequencyPeaks_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    /// Transitional data
    QList<SampleFrequencyInfo> getSampleFrequencies() { return _sampleFrequencies; }
    void setSampleFrequencies(QList<SampleFrequencyInfo> sampleFrequencies) { _sampleFrequencies = sampleFrequencies; }

    QString getFilePath() { return _filePath; }
    void setFilePath(QString filePath) { _filePath = filePath; }

private:
    QList<SampleFrequencyInfo> _sampleFrequencies;
    QString _filePath;
};

#endif // TOOLFREQUENCYPEAKS_PARAMETERS_H
