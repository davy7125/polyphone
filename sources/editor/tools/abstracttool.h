#ifndef ABSTRACTTOOL_H
#define ABSTRACTTOOL_H

#include "basetypes.h"
#include <QObject>
class QWidget;
class SoundfontManager;
class AbstractToolGui;
class AbstractToolParameters;
class ToolDialog;

class AbstractTool: public QObject
{
    Q_OBJECT

public:
    AbstractTool(AbstractToolParameters * parameters = NULL, AbstractToolGui * gui = NULL);
    virtual ~AbstractTool();

    /// Initialization of resources shared by all tools
    static void initialize(QWidget * parent);

    /// Icon, label and category displayed to the user to describe the tool
    virtual QString getIconName() const = 0;
    virtual QString getLabel() const = 0;
    virtual QString getCategory() const = 0;

    /// Internal identifier
    virtual QString getIdentifier() const = 0;

    /// Set the ids and return true if the tool can be used on the specified ids
    bool setIds(IdList ids);

    /// Run the tool on the specified list of ids
    void run();

signals:
    void finished(bool updateNeeded);

protected:
    /// Return true if the tool can be used on the specified ids
    virtual bool isCompatible(IdList ids) = 0;

    /// Run the tool, emit the signal "finished" at the end
    virtual void run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) = 0;

    /// Get the warning to display after the tool is run
    virtual QString getWarning() { return ""; }

    /// Get a confirmation message after the tool is run
    virtual QString getConfirmation() { return ""; }

private slots:
    void onFinished(bool updateNeeded);
    void onParametersValidated();

private:
    AbstractToolParameters * _toolParameters;
    AbstractToolGui * _toolGui;
    ToolDialog * _toolDialog;
    IdList _currentIds;

    static QWidget * s_parent; // For opening dialogs
    static SoundfontManager * s_sm;
};

#endif // ABSTRACTTOOL_H
