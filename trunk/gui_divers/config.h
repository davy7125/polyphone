/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

namespace Ui
{
    class Config;
}

class MainWindow;

class Config : public QDialog
{
    Q_OBJECT
    
public:
    enum TypeFichier
    {
        typeFichierSf2,
        typeFichierSample,
        typeFichierEnregistrement
    };

    static Config * getInstance(QWidget *parent = NULL);
    static void kill();
    ~Config();
    // accesseurs
    bool getRam()               {return false/*ram*/;}
    int  getAudioIndex()        {return audioIndex;}
    bool getAfficheMod()        {return afficheMod;}
    bool getAfficheToolBar()    {return afficheToolBar;}
    bool getWavAutoLoop()       {return wavAutoLoop;}
    bool getRemoveBlank()       {return wavRemoveBlank;}
    int  getKeyboardType()      {return keyboardType;}
    int  getKeyboardVelocity()  {return keyboardVelocity;}
    int  getNumPortMidi()       {return numPortMidi;}
    int  getAudioType()         {return audioType;}
    int  getSynthGain()         {return synthGain;}
    int  getSynthRevLevel()     {return revLevel;}
    int  getSynthRevSize()      {return revSize;}
    int  getSynthRevWidth()     {return revWidth;}
    int  getSynthRevDamp()      {return revDamping;}
    int  getSynthChoLevel()     {return choLevel;}
    int  getSynthChoDepth()     {return choDepth;}
    int  getSynthChoFrequency() {return choFrequency;}
    QList<QColor> getColors()   {return colorList;}
    // Accès aux paramètres des outils
    int    getTools_s_sifflements_debut()   { return settings.value("tools/sample/sifflements_debut", 8000).toInt(); }
    int    getTools_s_sifflements_fin()     { return settings.value("tools/sample/sifflements_fin", 20000).toInt();}
    double getTools_s_sifflements_raideur() { return settings.value("tools/sample/sifflements_raideur", 5.).toDouble(); }
    double getTools_s_mur_coupure()         { return settings.value("tools/sample/mur_coupure", 15000.).toDouble(); }
    double getTools_s_transpo_ton()         { return settings.value("tools/sample/transposition_ton", 12.).toDouble(); }
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

    // Gestion des fichiers
    QString getLastFile(TypeFichier typeFichier, int num=0);
    QString getLastDirectory(TypeFichier typeFichier);
    void addFile(TypeFichier typeFichier, QString filePath);
    // Affichage de la fenêtre
    void show();
    // Initialisation
    void setListeActions(QList<QAction *> actions);

signals:
    void colorsChanged();

public slots:
    void setAfficheMod(int val);
    void setAfficheToolBar(int val);
    void setKeyboardType(int val);
    void setKeyboardVelocity(int val);

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

private:
    QSettings settings;
    Ui::Config * ui;
    MainWindow * mainWindow;
    // Instance unique
    static Config * _instance;
    // Paramètres configuration
    QStringList listFiles;
    QString recordFile;
    QString sampleFile;
    bool ram;
    int audioType;
    int audioIndex;
    bool afficheMod;
    bool afficheToolBar;
    bool wavAutoLoop;
    bool wavRemoveBlank;
    int keyboardType;
    int keyboardVelocity;
    int numPortMidi;
    int synthGain;
    int revLevel, revSize, revDamping, revWidth;
    int choLevel, choDepth, choFrequency;
    QList<QColor> colorList;
    QList<QAction *> actionList;
    QByteArray actionListToolbar;
    // Autres
    bool loaded;
    // Méthodes privées
    explicit Config(QWidget *parent = 0);
    void load();
    void store();
    void setColors();
    void fillActions();
    QByteArray getDefaultListActions();
};

#endif // CONFIG_H
