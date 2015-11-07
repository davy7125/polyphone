/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <QMessageBox>
#include <QDialog>
#include <QString>
#include <QSettings>
#include "pianokeybdcustom.h"

namespace Ui
{
    class Config;
}

class MainWindow;
class AudioDevice;

class Config : public QDialog
{
    Q_OBJECT
    
public:
    enum TypeFichier
    {
        typeFichierSf2,
        typeFichierSample,
        typeFichierEnregistrement,
        typeFichierSoundfont,
        typeFichierExport,
        typeFichierFrequences
    };

    enum NameMiddleC
    {
        middleC_60,
        middleC_C3_with_sharps,
        middleC_C3_with_flats,
        middleC_C4_with_sharps,
        middleC_C4_with_flats,
        middleC_C5_with_sharps,
        middleC_C5_with_flats
    };

    static Config * getInstance(PianoKeybdCustom *keyboard = NULL, AudioDevice *audioDevice = NULL, QWidget *parent = NULL);
    static void kill();
    ~Config();

    // accesseurs
    bool getRam()               {return false/*ram*/;}
    int  getAudioType();
    int  getAudioIndex();
    int  getBufferSize()        {return _bufferSize;}
    bool getAfficheMod()        {return afficheMod;}
    bool getAfficheToolBar()    {return afficheToolBar;}
    bool getWavAutoLoop()       {return wavAutoLoop;}
    bool getRemoveBlank()       {return wavRemoveBlank;}
    int  getKeyboardType()      {return keyboardType;}
    int  getKeyboardDocked()    {return keyboardDocked;}
    int  getNumPortMidi()       {return numPortMidi;}
    int  getSynthGain()         {return synthGain;}
    int  getSynthRevLevel()     {return revLevel;}
    int  getSynthRevSize()      {return revSize;}
    int  getSynthRevWidth()     {return revWidth;}
    int  getSynthRevDamp()      {return revDamping;}
    int  getSynthChoLevel()     {return choLevel;}
    int  getSynthChoDepth()     {return choDepth;}
    int  getSynthChoFrequency() {return choFrequency;}
    bool getSameWidthTable()    {return sameWidthTable;}
    QList<QColor> getColors()   {return colorList;}
    bool getRepercussionStereo(){return modifStereo;}
    NameMiddleC getNameMiddleC(){return nameMiddleC;}
    int  getDefaultVelocity()   {return _velocity;}
    QString getKeyName(int keyNum, bool forceTexte = false, bool with0 = false, bool forceC4 = false);
    int getKeyNum(QString keyName, bool forceC4 = false);

    // Accès aux paramètres des outils
    int    getTools_s_sifflements_debut()   { return settings.value("tools/sample/sifflements_debut", 8000).toInt(); }
    int    getTools_s_sifflements_fin()     { return settings.value("tools/sample/sifflements_fin", 20000).toInt();}
    double getTools_s_sifflements_raideur() { return settings.value("tools/sample/sifflements_raideur", 5.).toDouble(); }
    double getTools_s_mur_coupure()         { return settings.value("tools/sample/mur_coupure", 15000.).toDouble(); }
    double getTools_s_transpo_ton()         { return settings.value("tools/sample/transposition_ton", 12.).toDouble(); }
    double getTools_i_transpo_ton()         { return settings.value("tools/instrument/transposition_ton", 12.).toDouble(); }
    bool   getTools_i_transpo_keyrange()    { return settings.value("tools/instrument/transposition_keyrange", true).toBool(); }
    double getTools_i_release_dureeDo()     { return settings.value("tools/instrument/release_durationC", 0.3).toDouble(); }
    double getTools_i_release_division()    { return settings.value("tools/instrument/release_division", 2.).toDouble(); }
    double getTools_i_release_desaccordage(){ return settings.value("tools/instrument/release_desaccordage", 0.).toDouble(); }
    double getTools_i_celeste_herzDo()      { return settings.value("tools/instrument/ondulation_herz", 4.).toDouble(); }
    double getTools_i_celeste_division()    { return settings.value("tools/instrument/ondulation_division", 1.).toDouble(); }
    double getTools_2_attenuation_dB()      { return settings.value("tools/sf2/attenuation_dB", 5.).toDouble(); }
    QList<QList<int> > getTools_i_mixture_ranks();
    QString getTools_i_mixture_nom()        { return settings.value("tools/instrument/mixture_nom", "").toString(); }
    bool   getTools_i_mixture_boucle()      { return settings.value("tools/instrument/mixture_boucle", true).toBool(); }
    int    getTools_i_mixture_density()     { return settings.value("tools/instrument/mixture_density", 0).toInt(); }
    bool   getTools_i_mixture_stereo()      { return settings.value("tools/instrument/mixture_stereo", true).toBool(); }
    QVector<double> getTools_global_courbe(bool isPrst)
    {
        QList<QVariant> listTmp;
        if (isPrst)
            listTmp = settings.value("tools/preset/global_courbe", QList<QVariant>()).toList();
        else
            listTmp = settings.value("tools/instrument/global_courbe", QList<QVariant>()).toList();
        QVector<double> vectRet;
        vectRet.resize(listTmp.size());
        for (int i = 0; i < listTmp.size(); i++)
            vectRet[i] = listTmp.at(i).toDouble();
        return vectRet;
    }
    int    getTools_global_motif(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_motif", 0).toInt();
        else
            return settings.value("tools/instrument/global_motif", 0).toInt();
    }
    int    getTools_global_modification(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_modification", 0).toInt();
        else
            return settings.value("tools/instrument/global_modification", 0).toInt();
    }
    int    getTools_global_parametre(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_parametre", 0).toInt();
        else
            return settings.value("tools/instrument/global_parametre", 0).toInt();
    }
    double getTools_global_raideur(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_raideur", 50.).toDouble();
        else
            return settings.value("tools/instrument/global_raideur", 50.).toDouble();
    }
    double getTools_global_mini(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_mini", 0.).toDouble();
        else
            return settings.value("tools/instrument/global_mini", 0.).toDouble();
    }
    double getTools_global_maxi(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_maxi", 1.).toDouble();
        else
            return settings.value("tools/instrument/global_maxi", 1.).toDouble();
    }
    int    getTools_global_miniX(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_miniX", 0).toInt();
        else
            return settings.value("tools/instrument/global_miniX", 0).toInt();
    }
    int    getTools_global_maxiX(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_maxiX", 140).toInt();
        else
            return settings.value("tools/instrument/global_maxiX", 140).toInt();
    }
    int    getTools_global_miniVel(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_miniVel", 0).toInt();
        else
            return settings.value("tools/instrument/global_miniVel", 0).toInt();
    }
    int    getTools_global_maxiVel(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/global_maxiVel", 127).toInt();
        else
            return settings.value("tools/instrument/global_maxiVel", 127).toInt();
    }
    QVector<int> getTools_duplication_velocites(bool isPrst)
    {
        QList<QVariant> listTmp, defaultList;
        defaultList << 0 << 127;
        if (isPrst)
            listTmp = settings.value("tools/preset/duplication_velocites", defaultList).toList();
        else
            listTmp = settings.value("tools/instrument/duplication_velocites", defaultList).toList();
        QVector<int> vectRet;
        vectRet.resize(listTmp.size());
        for (int i = 0; i < listTmp.size(); i++)
            vectRet[i] = listTmp.at(i).toDouble();
        return vectRet;
    }
    bool   getTools_duplication_duplicKey(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/duplication_duplicationKeys", true).toBool();
        else
            return settings.value("tools/instrument/duplication_duplicationKeys", true).toBool();
    }
    bool   getTools_duplication_duplicVel(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/duplication_duplicationVelocity", false).toBool();
        else
            return settings.value("tools/instrument/duplication_duplicationVelocity", false).toBool();
    }
    int    getTools_visualizer_parameter(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/visualizer_parameter", 0).toInt();
        else
            return settings.value("tools/instrument/visualizer_parameter", 0).toInt();
    }
    bool   getTools_visualizer_logScale(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/visualizer_logScale", false).toBool();
        else
            return settings.value("tools/instrument/visualizer_logScale", false).toBool();
    }
    int    getTools_i_space_motif(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_motif", 0).toInt();
        else
            return settings.value("tools/instrument/space_motif", 0).toInt();
    }
    int    getTools_i_space_divisions(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_divisions", 1).toInt();
        else
            return settings.value("tools/instrument/space_divisions", 1).toInt();
    }
    int    getTools_i_space_etalement(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_etalement", 100).toInt();
        else
            return settings.value("tools/instrument/space_etalement", 100).toInt();
    }
    int    getTools_i_space_occupation(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_occupation", 100).toInt();
        else
            return settings.value("tools/instrument/space_occupation", 100).toInt();
    }
    int    getTools_i_space_offset(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_offset", 50).toInt();
        else
            return settings.value("tools/instrument/space_offset", 50).toInt();
    }
    bool   getTools_i_space_renversement1(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_renversement1", false).toBool();
        else
            return settings.value("tools/instrument/space_renversement1", false).toBool();
    }
    bool   getTools_i_space_renversement2(bool isPrst)
    {
        if (isPrst)
            return settings.value("tools/preset/space_renversement2", false).toBool();
        else
            return settings.value("tools/instrument/space_renversement2", false).toBool();
    }

    // Modification des paramètres des outils
    void setTools_s_sifflements_debut(int val)      { settings.setValue("tools/sample/sifflements_debut", val); }
    void setTools_s_sifflements_fin(int val)        { settings.setValue("tools/sample/sifflements_fin", val); }
    void setTools_s_sifflements_raideur(double val) { settings.setValue("tools/sample/sifflements_raideur", val); }
    void setTools_s_mur_coupure(double val)         { settings.setValue("tools/sample/mur_coupure", val); }
    void setTools_s_transpo_ton(double val)         { settings.setValue("tools/sample/transposition_ton", val); }
    void setTools_i_transpo_ton(double val)         { settings.setValue("tools/instrument/transposition_ton", val); }
    void setTools_i_transpo_keyrange(bool val)       { settings.setValue("tools/instrument/transposition_keyrange", val); }
    void setTools_i_release_dureeDo(double val)     { settings.setValue("tools/instrument/release_durationC", val); }
    void setTools_i_release_division(double val)    { settings.setValue("tools/instrument/release_division", val); }
    void setTools_i_release_desaccordage(double val){ settings.setValue("tools/instrument/release_desaccordage", val); }
    void setTools_i_celeste_herzDo(double val)      { settings.setValue("tools/instrument/ondulation_herz", val); }
    void setTools_i_celeste_division(double val)    { settings.setValue("tools/instrument/ondulation_division", val); }
    void setTools_2_attenuation_dB(double val)      { settings.setValue("tools/sf2/attenuation_dB", val); }
    void setTools_i_mixture_ranks(QList<QList<int> > val);
    void setTools_i_mixture_nom(QString val)        { settings.setValue("tools/instrument/mixture_nom", val); }
    void setTools_i_mixture_boucle(bool val)        { settings.setValue("tools/instrument/mixture_boucle", val); }
    void setTools_i_mixture_density(int val)        { settings.setValue("tools/instrument/mixture_density", val); }
    void setTools_i_mixture_stereo(bool val)        { settings.setValue("tools/instrument/mixture_stereo", val); }
    void setTools_global_courbe(bool isPrst, QVector<double> val)
    {
        QVariantList listTmp;
        for (int i = 0; i < val.size(); i++)
            listTmp << val.at(i);
        if (isPrst)
            settings.setValue("tools/preset/global_courbe", listTmp);
        else
            settings.setValue("tools/instrument/global_courbe", listTmp);
    }
    void setTools_global_motif(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_motif", val);
        else
            settings.setValue("tools/instrument/global_motif", val);
    }
    void setTools_global_modification(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_modification", val);
        else
            settings.setValue("tools/instrument/global_modification", val);
    }
    void setTools_global_parametre(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_parametre", val);
        else
            settings.setValue("tools/instrument/global_parametre", val);
    }
    void setTools_global_raideur(bool isPrst, double val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_raideur", val);
        else
            settings.setValue("tools/instrument/global_raideur", val);
    }
    void setTools_global_mini(bool isPrst, double val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_mini", val);
        else
            settings.setValue("tools/instrument/global_mini", val);
    }
    void setTools_global_maxi(bool isPrst, double val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_maxi", val);
        else
            settings.setValue("tools/instrument/global_maxi", val);
    }
    void setTools_global_miniX(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_miniX", val);
        else
            settings.setValue("tools/instrument/global_miniX", val);
    }
    void setTools_global_maxiX(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_maxiX", val);
        else
            settings.setValue("tools/instrument/global_maxiX", val);
    }
    void setTools_global_miniVel(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_miniVel", val);
        else
            settings.setValue("tools/instrument/global_miniVel", val);
    }
    void setTools_global_maxiVel(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/global_maxiVel", val);
        else
            settings.setValue("tools/instrument/global_maxiVel", val);
    }
    void setTools_duplication_velocites(bool isPrst, QVector<int> val)
    {
        QVariantList listTmp;
        for (int i = 0; i < val.size(); i++)
            listTmp << val.at(i);
        if (isPrst)
            settings.setValue("tools/preset/duplication_velocites", listTmp);
        else
            settings.setValue("tools/instrument/duplication_velocites", listTmp);
    }
    void setTools_duplication_duplicKey(bool isPrst, bool val)
    {
        if (isPrst)
            settings.setValue("tools/preset/duplication_duplicationKeys", val);
        else
            settings.setValue("tools/instrument/duplication_duplicationKeys", val);
    }
    void setTools_duplication_duplicVel(bool isPrst, bool val)
    {
        if (isPrst)
            settings.setValue("tools/preset/duplication_duplicationVelocity", val);
        else
            settings.setValue("tools/instrument/duplication_duplicationVelocity", val);
    }
    void setTools_visualizer_parameter(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/visualizer_parameter", val);
        else
            settings.setValue("tools/instrument/visualizer_parameter", val);
    }
    void setTools_visualizer_logScale(bool isPrst, bool val)
    {
        if (isPrst)
            settings.setValue("tools/preset/visualizer_logScale", val);
        else
            settings.setValue("tools/instrument/visualizer_logScale", val);
    }
    void setTools_space_motif(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_motif", val);
        else
            settings.setValue("tools/instrument/space_motif", val);
    }
    void setTools_space_divisions(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_divisions", val);
        else
            settings.setValue("tools/instrument/space_divisions", val);
    }
    void setTools_space_etalement(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_etalement", val);
        else
            settings.setValue("tools/instrument/space_etalement", val);
    }
    void setTools_space_occupation(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_occupation", val);
        else
            settings.setValue("tools/instrument/space_occupation", val);
    }
    void setTools_space_offset(bool isPrst, int val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_offset", val);
        else
            settings.setValue("tools/instrument/space_offset", val);
    }
    void setTools_space_renversement1(bool isPrst, bool val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_renversement1", val);
        else
            settings.setValue("tools/instrument/space_renversement1", val);
    }
    void setTools_space_renversement2(bool isPrst, bool val)
    {
        if (isPrst)
            settings.setValue("tools/preset/space_renversement2", val);
        else
            settings.setValue("tools/instrument/space_renversement2", val);
    }

    // Paramètres divers
    bool getActivationSaveWarning_toManyGenerators()
    {
        return settings.value("warnings/to_many_generators", true).toBool();
    }
    void setActivationSaveWarning_toManyGenerators(bool activated)
    {
        settings.setValue("warnings/to_many_generators", activated);
    }
    bool getActivationWarning_GlobalNotOverwritten()
    {
        return settings.value("warnings/global_not_overwritten", true).toBool();
    }
    void setActivationWarning_GlobalNotOverwritten(bool activated)
    {
        settings.setValue("warnings/global_not_overwritten", activated);
    }

    // Mappage
    QString getKeyMapped(int numOctave, PianoKeybd::Key key)
    {
        QString txt = "";
        switch (numOctave)
        {
        case 0:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("w"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("s"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("x"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("d"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("c"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("v"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("g"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("b"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("h"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("n"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("j"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8(","); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8(";"); break;
            }
            break;
        case 1:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("a"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("é"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("z"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("\""); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("e"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("r"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("("); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("t"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("-"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("y"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("è"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("u"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("i"); break;
            }
            break;
        case 2:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("Shift+w"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("Shift+s"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("Shift+x"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("Shift+d"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("Shift+c"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("Shift+v"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("Shift+g"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("Shift+b"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("Shift+h"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("Shift+n"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("Shift+j"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("Shift+?"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("Shift+."); break;
            }
            break;
        case 3:
            switch (key)
            {
            case PianoKeybd::KEY_C_LEFT:  txt = trUtf8("Shift+a"); break;
            case PianoKeybd::KEY_C_SHARP: txt = trUtf8("Shift+2"); break;
            case PianoKeybd::KEY_D:       txt = trUtf8("Shift+z"); break;
            case PianoKeybd::KEY_D_SHARP: txt = trUtf8("Shift+3"); break;
            case PianoKeybd::KEY_E:       txt = trUtf8("Shift+e"); break;
            case PianoKeybd::KEY_F:       txt = trUtf8("Shift+r"); break;
            case PianoKeybd::KEY_F_SHARP: txt = trUtf8("Shift+5"); break;
            case PianoKeybd::KEY_G:       txt = trUtf8("Shift+t"); break;
            case PianoKeybd::KEY_G_SHARP: txt = trUtf8("Shift+6"); break;
            case PianoKeybd::KEY_A:       txt = trUtf8("Shift+y"); break;
            case PianoKeybd::KEY_A_SHARP: txt = trUtf8("Shift+7"); break;
            case PianoKeybd::KEY_B:       txt = trUtf8("Shift+u"); break;
            case PianoKeybd::KEY_C_RIGHT: txt = trUtf8("Shift+i"); break;
            }
            break;
        default:
            break;
        }
        return settings.value("map/key_" + QString::number(numOctave) + "_" +
                              QString::number((int)key), txt).toString();
    }
    int getOctaveMap()
    {
        return settings.value("map/octave_offset", 3).toInt();
    }
    void setOctaveMap(int octave);

    // Gestion des fichiers
    QString getLastFile(TypeFichier typeFichier, int num=0);
    QString getLastDirectory(TypeFichier typeFichier);
    void addFile(TypeFichier typeFichier, QString filePath);

    // Etat fenêtre principale
    void setWindowGeometry(QByteArray ba)   { settings.setValue("affichage/windowGeometry", ba); }
    void setWindowState(QByteArray ba)      { settings.setValue("affichage/windowState", ba); }
    QByteArray getWindowGeometry()          { return settings.value("affichage/windowGeometry", QByteArray()).toByteArray(); }
    QByteArray getWindowState()             { return settings.value("affichage/windowState", QByteArray()).toByteArray(); }
    void setDockWidth(int val)              { settings.setValue("affichage/dock_width", val); }
    int getDockWidth()                      { return settings.value("affichage/dock_width", 150).toInt(); }
    void setKeyboardGeometry(QByteArray ba) { settings.setValue("affichage/keyboardGeometry", ba); }
    QByteArray getKeyboardGeometry()        { return settings.value("affichage/keyboardGeometry", QByteArray()).toByteArray(); }

    // Last version installed
    double getLastVersionInstalled() { return settings.value("last_version_installed", 0).toDouble(); }
    void setLastVersionInstalled(double version) { settings.setValue("last_version_installed", version); }

    // Export
    void setExportType(int val)             { settings.setValue("export/type", val); }
    void setExportPreset(bool val)          { settings.setValue("export/preset_prefix", val); }
    void setExportBank(bool val)            { settings.setValue("export/bank_directory", val); }
    void setExportGM(bool val)              { settings.setValue("export/gm_sort", val); }
    void setExportQuality(int val)          { settings.setValue("export/quality", val); }
    int  getExportType()                    { return settings.value("export/type", 0).toInt(); }
    bool getExportPreset()                  { return settings.value("export/preset_prefix", true).toBool(); }
    bool getExportBank()                    { return settings.value("export/bank_directory", false).toBool(); }
    bool getExportGM()                      { return settings.value("export/gm_sort", false).toBool(); }
    int  getExportQuality()                 { return settings.value("export/quality", 1).toInt(); }

    // Bulk rename
    int getBulkRenameOption()               { return settings.value("bulk_rename/option", 0).toInt(); }
    void setBulkRenameOption(int option)    { settings.setValue("bulk_rename/option", option); }
    QString getBulkRenameText1()            { return settings.value("bulk_rename/text_1", "").toString(); }
    QString getBulkRenameText2()            { return settings.value("bulk_rename/text_2", "").toString(); }
    void setBulkRenameText1(QString str)    { settings.setValue("bulk_rename/text_1", str); }
    void setBulkRenameText2(QString str)    { settings.setValue("bulk_rename/text_2", str); }
    int getBulkRenameInt1()                 { return settings.value("bulk_rename/int_1", 0).toInt(); }
    int getBulkRenameInt2()                 { return settings.value("bulk_rename/int_2", 0).toInt(); }
    void setBulkRenameInt1(int val)         { settings.setValue("bulk_rename/int_1", val); }
    void setBulkRenameInt2(int val)         { settings.setValue("bulk_rename/int_2", val); }

    // Clavier
    void setKeyboardType(int val);
    void setKeyboardDocked(bool val);

    // Affichage de la fenêtre
    void show();

    // Initialisation
    void setListeActions(QList<QAction *> actions);
    void setVolume(int val);

    void storeAudioConfig();

signals:
    void colorsChanged();

public slots:
    void setAfficheMod(int val);
    void setAfficheToolBar(int val);

private slots:
    void setRam(int val);
    void setAudioOutput(int index);
    void setWavAutoLoop(bool checked);
    void setWavRemoveBlank(bool checked);
    void setNumPortMidi(int val);
    void setSynthGain(int val);
    void on_dialRevNiveau_valueChanged(int value);
    void on_dialRevProfondeur_valueChanged(int value);
    void on_dialRevDensite_valueChanged(int value);
    void on_dialRevAttenuation_valueChanged(int value);
    void on_dialChoNiveau_valueChanged(int value);
    void on_dialChoAmplitude_valueChanged(int value);
    void on_dialChoFrequence_valueChanged(int value);
    void on_pushColorBackground_clicked();
    void on_pushColorForeground_clicked();
    void on_pushColorGrid_clicked();
    void on_pushColorStartloop_clicked();
    void on_pushColorEndloop_clicked();
    void on_pushColorPlay_clicked();
    void on_pushColorRestore_clicked();
    void on_pushUp_clicked();
    void on_pushRight_clicked();
    void on_pushLeft_clicked();
    void on_pushDown_clicked();
    void on_pushResetToolbar_clicked();
    void on_listToolbar_itemSelectionChanged();
    void on_listActions_itemSelectionChanged();
    void combinaisonChanged(int key, int numOctave, QString combinaison);
    void on_checkRepercussionStereo_clicked();
    void on_comboBufferSize_activated(int index);
    void on_comboKeyName_currentIndexChanged(int index);
    void on_comboDo_currentIndexChanged(int index);
    void on_spinDefaultVelocity_editingFinished();
    void on_comboLangue_currentIndexChanged(int index);
    void on_checkRowSameWidth_clicked();
    void setColors();

private:
    QSettings settings;
    Ui::Config * ui;
    MainWindow * mainWindow;
    // Instance unique
    static Config * _instance;
    // Paramètres configuration
    QStringList listFiles;
    QString recordFile, sampleFile, exportFile, importFile, pngFile;
    bool ram;
    int _audioType;
    int _audioIndex;
    bool afficheMod;
    bool afficheToolBar;
    bool wavAutoLoop;
    bool wavRemoveBlank;
    int keyboardType;
    bool keyboardDocked;
    int numPortMidi;
    int synthGain;
    int revLevel, revSize, revDamping, revWidth;
    int choLevel, choDepth, choFrequency;
    int _velocity;
    QList<QColor> colorList;
    QList<QAction *> actionList;
    QByteArray actionListToolbar;
    bool modifStereo;
    int _bufferSize;
    NameMiddleC nameMiddleC;
    bool sameWidthTable;

    // Autres
    bool loaded;
    PianoKeybdCustom * _keyboard;
    int octaveMapping;

    Config(QWidget *parent, PianoKeybdCustom *keyboard, AudioDevice *audioDevice);
    void load();
    void store();
    void fillActions();
    QByteArray getDefaultListActions();
    void renameComboDo();
    void initComboLanguage();
};

#endif // CONFIG_H
