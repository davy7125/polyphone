#ifndef TOOLSAMPLEEXPORT_H
#define TOOLSAMPLEEXPORT_H

#include "abstracttooliterating.h"
#include <QObject>
#include <QMutex>

class ToolSampleExport: public AbstractToolIterating
{
public:
    ToolSampleExport() : AbstractToolIterating(elementSmpl)
    {
        _openWaitDialogJustInProcess = true;
    }

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/file-audio.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Exporter au format wav");
    }

    QString getCategory() const override
    {
        return trUtf8("Fichiers");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:wavExport";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    QString getFilePath(SoundfontManager *sm, EltID id1, EltID id2, bool isStereo);

    QMutex _mutex;
    IdList _exportedSamples;
    QString _dirPath;
};

#endif // TOOLSAMPLEEXPORT_H
