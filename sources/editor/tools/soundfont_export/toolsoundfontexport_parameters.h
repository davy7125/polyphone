#ifndef TOOLSOUNDFONTEXPORT_PARAMETERS_H
#define TOOLSOUNDFONTEXPORT_PARAMETERS_H

#include "abstracttoolparameters.h"
#include "basetypes.h"

class ToolSoundfontExport_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    QString getDirectory() { return _directory; }
    void setDirectory(QString directory) { _directory = directory; }

    int getFormat() { return _format; }
    void setFormat(int format) { _format = format; }

    int getQuality() { return _quality; }
    void setQuality(int quality) { _quality = quality; }

    bool getPresetPrefix() { return _presetPrefix; }
    void setPresetPrefix(bool presetPrefix) { _presetPrefix = presetPrefix; }

    bool getBankDirectory() { return _bankDirectory; }
    void setBankDirectory(bool bankDirectory) { _bankDirectory = bankDirectory; }

    bool getGmSort() { return _gmSort; }
    void setGmSort(bool gmSort) { _gmSort = gmSort; }
    
    // Transitional data
    QMap<int, QList<int> > & getSelectedPresets() { return _selectedPresets; }
    void setSelectedPresets(QMap<int, QList<int> > selectedPresets) { _selectedPresets = selectedPresets; }

private:
    QString _directory;
    int _format;
    int _quality;
    bool _presetPrefix;
    bool _bankDirectory;
    bool _gmSort;

    QMap<int, QList<int> > _selectedPresets;
};

#endif // TOOLSOUNDFONTEXPORT_PARAMETERS_H
