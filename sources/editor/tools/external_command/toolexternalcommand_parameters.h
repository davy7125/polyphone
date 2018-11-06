#ifndef TOOLEXTERNALCOMMAND_PARAMETERS_H
#define TOOLEXTERNALCOMMAND_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QStringList>

class ToolExternalCommand_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    bool getStereo() { return _stereo; }
    void setStereo(bool stereo) { _stereo = stereo; }

    bool getReplaceInfo() { return _replaceInfo; }
    void setReplaceInfo(bool replaceInfo) { _replaceInfo = replaceInfo; }

    QStringList getCommandHistory() { return _commandHistory; }
    void setCommandHistory(QStringList commandHistory) { _commandHistory = commandHistory; }

private:
    bool _stereo;
    bool _replaceInfo;
    QStringList _commandHistory;
    static const int HISTORY_SIZE;
};

#endif // TOOLEXTERNALCOMMAND_PARAMETERS_H
