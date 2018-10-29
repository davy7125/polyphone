#ifndef TOOLMIXTURECREATION_H
#define TOOLMIXTURECREATION_H

#include "abstracttool.h"
#include "toolmixturecreation_parameters.h"
#include "waitingtooldialog.h"

class ToolMixtureCreation: public AbstractTool
{
    Q_OBJECT

public:
    ToolMixtureCreation();
    ~ToolMixtureCreation();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/mixture.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Créer une mixture...");
    }

    QString getCategory() const override
    {
        return trUtf8("Transformation");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:mixtureCreation";
    }

    void sampleDataReady(EltID idSmpl, int key, int minKey, double minAtt);

signals:
    void elementProcessed(EltID idSmpl, int key, int minKey, double minAtt);

protected:
    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private slots:
    void onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt);
    void onCancel();

private:
    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;

    QString _warning;
    QMap<int, EltID> _sampleKey;
    QString _instrumentName;
    bool _stereoSamples;
    EltID _idNewInst;
    QMutex _mutex;
};

#endif // TOOLMIXTURECREATION_H
