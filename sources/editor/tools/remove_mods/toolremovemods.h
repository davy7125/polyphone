#ifndef TOOLREMOVEMODS_H
#define TOOLREMOVEMODS_H

#include "abstracttooliterating.h"

class ToolRemoveMods: public AbstractToolIterating
{
public:
    ToolRemoveMods();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_mods.svg";
    }

    QString getLabel() const override
    {
        return trUtf8("Enlever les modulateurs");
    }

    QString getCategory() const override
    {
        return trUtf8("Nettoyage");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        QString identifier;
        switch (_deletionType)
        {
        case DeletionGlobal:
            identifier = "sf2:removeMods";
            break;
        case DeletionForInstrument:
            identifier = "inst:removeMods";
            break;
        case DeletionForPreset:
            identifier = "prst:removeMods";
            break;
        }
        return identifier;
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    enum DeletionType
    {
        DeletionGlobal,
        DeletionForInstrument,
        DeletionForPreset
    };

    void clearModInst(SoundfontManager *sm, EltID idInst);
    void clearModPrst(SoundfontManager *sm, EltID idPrst);
    void clearMod(SoundfontManager *sm, EltID idMod);

    DeletionType _deletionType;
    int _count;
};

#endif // TOOLREMOVEMODS_H
