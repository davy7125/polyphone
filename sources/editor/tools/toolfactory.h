#ifndef TOOLFACTORY_H
#define TOOLFACTORY_H

#include "basetypes.h"
class AbstractTool;

class ToolFactory
{
public:
    ToolFactory() {}
    ~ToolFactory();

    /// Get all tools that can be used on the specified ids
    QList<AbstractTool *> getTools(IdList ids);

private:
    void initialize();

    QList<AbstractTool *> _tools;
};

#endif // TOOLFACTORY_H
