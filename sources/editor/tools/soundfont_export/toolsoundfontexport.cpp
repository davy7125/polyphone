#include "toolsoundfontexport.h"
#include "toolsoundfontexport_gui.h"
#include "toolsoundfontexport_parameters.h"
#include "soundfontmanager.h"
#include "duplicator.h"

ToolSoundfontExport::ToolSoundfontExport() : AbstractTool(new ToolSoundfontExport_parameters(), new ToolSoundfontExport_gui())
{

}

bool ToolSoundfontExport::isCompatible(IdList ids)
{
    Q_UNUSED(ids)

    // This tool is called via the main menu
    return false;
}

void ToolSoundfontExport::run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    Q_UNUSED(ids)
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *)parameters;
    
    // Flags
    bool presetPrefix = params->getPresetPrefix();
    bool bankDir = params->getBankDirectory();
    bool gmSort = params->getGmSort();

//    switch (params->getFormat())
//    {
//    case 0: case 1: {
//        // Export sf2 ou sf3, création d'un nouvel sf2 indépendant
//        SoundfontManager * newSf2 = SoundfontManager::getInstance();
//        EltID idDest(elementSf2, 0, 0, 0, 0);
//        idDest.indexSf2 = newSf2->add(idDest);

//        // Infos du nouvel sf2
//        QString name, comment;
//        if (listID.size() == 1)
//        {
//            EltID idSf2Source = listID.first().first();
//            idSf2Source.typeElement = elementSf2;
//            name = sf2->getQstr(idSf2Source, champ_name);
//            comment = sf2->getQstr(idSf2Source, champ_ICMT);
//            newSf2->set(idDest, champ_ISNG, sf2->getQstr(idSf2Source, champ_ISNG));
//            newSf2->set(idDest, champ_IROM, sf2->getQstr(idSf2Source, champ_IROM));
//            newSf2->set(idDest, champ_ICRD, sf2->getQstr(idSf2Source, champ_ICRD));
//            newSf2->set(idDest, champ_IENG, sf2->getQstr(idSf2Source, champ_IENG));
//            newSf2->set(idDest, champ_IPRD, sf2->getQstr(idSf2Source, champ_IPRD));
//            newSf2->set(idDest, champ_ICOP, sf2->getQstr(idSf2Source, champ_ICOP));
//            newSf2->set(idDest, champ_ISFT, sf2->getQstr(idSf2Source, champ_ISFT));
//        }
//        else
//        {
//            name = "soundfont";
//            comment = trUtf8("Fusion des soundfonts :");
//            foreach (QList<EltID> subList, listID)
//            {
//                EltID idSf2Source = subList.first();
//                idSf2Source.typeElement = elementSf2;
//                comment += "\n - " + sf2->getQstr(idSf2Source, champ_name);
//            }
//        }
//        newSf2->set(idDest, champ_name, name);
//        newSf2->set(idDest, champ_ICMT, comment);

//        // Ajout des presets
//        Duplicator duplicator(this->sf2, newSf2, this);
//        for (int nbBank = 0; nbBank < listID.size(); nbBank++)
//        {
//            QList<EltID> subList = listID[nbBank];
//            for (int nbPreset = 0; nbPreset < subList.size(); nbPreset++)
//            {
//                EltID id = subList[nbPreset];

//                if (listID.size() == 1)
//                {
//                    duplicator.copy(id, idDest);
//                }
//                else
//                {
//                    int originalBank = sf2->get(id, champ_wBank).wValue;
//                    int originalPreset = sf2->get(id, champ_wPreset).wValue;
//                    AttributeValue value;
//                    value.wValue = nbBank;
//                    sf2->set(id, champ_wBank, value);
//                    value.wValue = nbPreset;
//                    sf2->set(id, champ_wPreset, value);

//                    duplicator.copy(id, idDest);

//                    value.wValue = originalBank;
//                    sf2->set(id, champ_wBank, value);
//                    value.wValue = originalPreset;
//                    sf2->set(id, champ_wPreset, value);
//                }
//            }
//        }
//        sf2->clearNewEditing();

//        // Détermination du nom de fichier
//        name = name.replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");

//        QString extension = (format == 0 ? ".sf2" : ".sf3");
//        QFile fichier(dir + "/" + name + extension);
//        if (fichier.exists())
//        {
//            int i = 1;
//            while (QFile(dir + "/" + name + "-" + QString::number(i) + extension).exists())
//                i++;
//            name += "-" + QString::number(i);
//        }
//        name = dir + "/" + name + extension;

//        // Sauvegarde
//        newSf2->save(idDest.indexSf2, name, quality);
//        delete newSf2;
//    }break;
//    case 2:
//        // Export sfz
//        foreach (QList<EltID> sublist, listID)
//            ConversionSfz(sf2).convert(dir, sublist, presetPrefix, bankDir, gmSort);
//        break;
//    default:
//        break;
//    }
}

QString ToolSoundfontExport::getConfirmation()
{
    return "";
}
