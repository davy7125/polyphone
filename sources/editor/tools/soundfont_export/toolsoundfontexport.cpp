/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "toolsoundfontexport.h"
#include "toolsoundfontexport_gui.h"
#include "toolsoundfontexport_parameters.h"
#include "soundfontmanager.h"
#include "duplicator.h"
#include "outputfactory.h"
#include "abstractoutput.h"

ToolSoundfontExport::ToolSoundfontExport() : AbstractToolOneStep(new ToolSoundfontExport_parameters(), new ToolSoundfontExport_gui())
{

}

ToolSoundfontExport::~ToolSoundfontExport()
{

}

bool ToolSoundfontExport::isCompatible(IdList ids)
{
    Q_UNUSED(ids)

    // This tool is called via the main menu
    return false;
}

void ToolSoundfontExport::process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters)
{
    Q_UNUSED(ids)
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *)parameters;
    
    // Create a new soundfont with the presets to export
    EltID idExport = mergeSoundfonts(sm, params->getSelectedPresets());

    // Destination
    QString name = getName(sm, params->getSelectedPresets().keys());
    QString filePath = getFilePath(params->getDirectory(), name, params->getFormat());

    // Get a parser and configure it
    AbstractOutput * output = OutputFactory::getOutput(filePath);
    switch (params->getFormat())
    {
    case 0: // sf2
        // Nothing special
        break;
    case 1: // sf3
        output->setOption("quality", params->getQuality());
        break;
    case 2: // sfz
        output->setOption("prefix", params->getPresetPrefix());
        output->setOption("bankdir", params->getBankDirectory());
        output->setOption("gmsort", params->getGmSort());
        break;
    default:
        break;
    }

    // Export
    output->process(idExport.indexSf2, false);
    _error = output->getError();
    delete output;

    // Close the temporary soundfont and discard possible changes
    sm->remove(idExport);
    sm->revertNewEditing();
}

EltID ToolSoundfontExport::mergeSoundfonts(SoundfontManager * sm, QMap<int,  QList<int> > presets)
{
    // Merge all data in a new soundfont
    EltID idDest(elementSf2);
    idDest.indexSf2 = sm->add(idDest);

    // Infos du nouvel sf2
    QString name, comment;
    if (presets.size() == 1)
    {
        EltID idSf2Source(elementSf2, presets.keys()[0]);
        name = sm->getQstr(idSf2Source, champ_name);
        comment = sm->getQstr(idSf2Source, champ_ICMT);
        sm->set(idDest, champ_ISNG, sm->getQstr(idSf2Source, champ_ISNG));
        sm->set(idDest, champ_IROM, sm->getQstr(idSf2Source, champ_IROM));
        sm->set(idDest, champ_ICRD, sm->getQstr(idSf2Source, champ_ICRD));
        sm->set(idDest, champ_IENG, sm->getQstr(idSf2Source, champ_IENG));
        sm->set(idDest, champ_IPRD, sm->getQstr(idSf2Source, champ_IPRD));
        sm->set(idDest, champ_ICOP, sm->getQstr(idSf2Source, champ_ICOP));
        sm->set(idDest, champ_ISFT, sm->getQstr(idSf2Source, champ_ISFT));
    }
    else
    {
        name = "soundfont";
        comment = trUtf8("Merge soudfonts:");
        foreach (int sf2Index, presets.keys())
            comment += "\n - " + sm->getQstr(EltID(elementSf2, sf2Index), champ_name);
    }
    sm->set(idDest, champ_name, name);
    sm->set(idDest, champ_ICMT, comment);

    // Ajout des presets
    Duplicator duplicator;
    for (int nbBank = 0; nbBank < presets.keys().count(); nbBank++)
    {
        QList<EltID> subList;
        int sf2Index = presets.keys()[nbBank];
        foreach (int presetIndex, presets[sf2Index])
            subList << EltID(elementPrst, sf2Index, presetIndex);

        for (int nbPreset = 0; nbPreset < subList.size(); nbPreset++)
        {
            EltID id = subList[nbPreset];

            if (presets.count() == 1)
            {
                // Simple copy, the preset numbers will be the same
                duplicator.copy(id, idDest);
            }
            else
            {
                int originalBank = sm->get(id, champ_wBank).wValue;
                int originalPreset = sm->get(id, champ_wPreset).wValue;
                AttributeValue value;
                value.wValue = nbBank;
                sm->set(id, champ_wBank, value);
                value.wValue = nbPreset;
                sm->set(id, champ_wPreset, value);

                duplicator.copy(id, idDest);

                value.wValue = originalBank;
                sm->set(id, champ_wBank, value);
                value.wValue = originalPreset;
                sm->set(id, champ_wPreset, value);
            }
        }
    }

    return idDest;
}

QString ToolSoundfontExport::getName(SoundfontManager * sm, QList<int> sf2Indexes)
{
    QString name = "";
    if (sf2Indexes.count() == 1)
    {
        EltID idSf2(elementSf2, sf2Indexes[0]);
        name = QFileInfo(sm->getQstr(idSf2, champ_filenameInitial)).baseName();
        if (name.isEmpty())
            name = sm->getQstr(idSf2, champ_name);
        name = name.replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");
    }
    if (name.isEmpty())
        name = "export";
    return name;
}

QString ToolSoundfontExport::getFilePath(QString directory, QString name, int format)
{
    QString extension;
    switch (format)
    {
    case 0: extension = ".sf2"; break;
    case 1: extension = ".sf3"; break;
    case 2: extension = ".sfz"; break;
    default: extension = ".xxx"; break;
    }

    // Find a filepath that doesn't exist yet
    QFile fichier(directory + "/" + name + extension);
    if (fichier.exists())
    {
        int i = 1;
        while (QFile(directory + "/" + name + "-" + QString::number(i) + extension).exists())
            i++;
        name += "-" + QString::number(i);
    }

    return directory + "/" + name + extension;
}

QString ToolSoundfontExport::getWarning()
{
    return _error;
}

QString ToolSoundfontExport::getConfirmation()
{
    return _error.isEmpty() ? trUtf8("Export successful!") : "";
}
