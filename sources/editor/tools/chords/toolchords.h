#ifndef TOOLCHORDS_H
#define TOOLCHORDS_H

#include "abstracttool.h"
#include "toolchords_parameters.h"
#include "waitingtooldialog.h"

class ToolChords: public AbstractTool
{
    Q_OBJECT

public:
    ToolChords();
    ~ToolChords() override;

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/chord.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Transformation");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "inst:chords";
    }

signals:
    void elementProcessed(EltID idSmpl, int key, int minKey, double minAtt);

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Créer des accords");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private slots:
    void onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt);
    void onCancel();

private:
    rangesType getInstrumentRange(EltID idInst);

    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;

    EltID _idNewInst;
    QString _instrumentName;
    bool _stereoSamples;
    QString _warning;
    QMap<int, EltID> _sampleKey;
};

#endif // TOOLCHORDS_H
