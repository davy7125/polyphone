#ifndef ABSTRACTTOOLGUI_H
#define ABSTRACTTOOLGUI_H

#include <QWidget>
#include "basetypes.h"
class AbstractToolParameters;

class AbstractToolGui: public QWidget
{
    Q_OBJECT

public:
    AbstractToolGui(QWidget * parent) : QWidget(parent) {}
    virtual ~AbstractToolGui() {}

    /// Update the interface with the parameters
    virtual void updateInterface(AbstractToolParameters * parameters, IdList ids) = 0;

    /// Save the parameters based on the interface
    virtual void saveParameters(AbstractToolParameters * parameters) = 0;

signals:
    /// The user validated the configuration
    void validated();

    /// The user canceled the configuration
    void canceled();
};

#endif // ABSTRACTTOOLGUI_H
