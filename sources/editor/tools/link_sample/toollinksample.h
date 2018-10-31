#ifndef TOOLLINKSAMPLE_H
#define TOOLLINKSAMPLE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolLinkSample: public AbstractToolIterating
{
public:
    ToolLinkSample() : AbstractToolIterating(elementSmpl)
    {
        _async = false;
    }

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/link.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Trouver un lien");
    }

    QString getCategory() const override
    {
        return trUtf8("Échantillons stéréo");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:link";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

private:
    void scan(SoundfontManager * sm, int indexSf2);
    void associate(SoundfontManager * sm, EltID leftSmpl, EltID rightSmpl);
    void disconnectTarget(SoundfontManager * sm, EltID id);

    QList<int> _smplIds;
    QStringList _names;
    QList<bool> _leftRight; // false is left, true is right
    QList<int> _processedIds;
    QStringList _sampleErrors;
};

#endif // TOOLLINKSAMPLE_H
