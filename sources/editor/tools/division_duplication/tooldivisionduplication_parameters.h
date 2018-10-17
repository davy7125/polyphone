#ifndef TOOLDIVISIONDUPLICATION_PARAMETERS_H
#define TOOLDIVISIONDUPLICATION_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QMap>

class ToolDivisionDuplication_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;
    
    bool getInstDuplicKey() { return _instDuplicKey; }
    void setInstDuplicKey(bool instDuplicKey) { _instDuplicKey = instDuplicKey; }
    bool getPrstDuplicKey() { return _prstDuplicKey; }
    void setPrstDuplicKey(bool prstDuplicKey) { _prstDuplicKey = prstDuplicKey; }
    
    bool getInstDuplicVel() { return _instDuplicVel; }
    void setInstDuplicVel(bool instDuplicVel) { _instDuplicVel = instDuplicVel; }
    bool getPrstDuplicVel() { return _prstDuplicVel; }
    void setPrstDuplicVel(bool prstDuplicVel) { _prstDuplicVel = prstDuplicVel; }
    
    QList<QPair<int, int> > getInstVelocityRanges() { return _instVelocityRanges; }
    void setInstVelocityRanges(QList<QPair<int, int> > instVelocityRanges) { _instVelocityRanges = instVelocityRanges; }
    QList<QPair<int, int> > getPrstVelocityRanges() { return _prstVelocityRanges; }
    void setPrstVelocityRanges(QList<QPair<int, int> > prstVelocityRanges) { _prstVelocityRanges = prstVelocityRanges; }

private:
    bool _instDuplicKey, _prstDuplicKey;
    bool _instDuplicVel, _prstDuplicVel;
    QList<QPair<int, int> > _instVelocityRanges, _prstVelocityRanges;
};

#endif // TOOLDIVISIONDUPLICATION_PARAMETERS_H
