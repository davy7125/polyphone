#ifndef TOOLFACTORY_H
#define TOOLFACTORY_H

#include "idlist.h"
#include <QList>
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
