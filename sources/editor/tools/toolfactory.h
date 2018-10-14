#ifndef TOOLFACTORY_H
#define TOOLFACTORY_H

#include "basetypes.h"
class AbstractTool;
class QWidget;

class ToolFactory
{
public:
    ToolFactory(QWidget * parent);
    ~ToolFactory();

    /// Get all tools that can be used on the specified ids
    QList<AbstractTool *> getTools(IdList ids);

private:
    QList<AbstractTool *> _tools;
};

#endif // TOOLFACTORY_H
