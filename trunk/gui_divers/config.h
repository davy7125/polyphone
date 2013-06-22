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

namespace Ui
{
    class Config;
}

class MainWindow;

class Config : public QDialog
{
    Q_OBJECT
    
public:
    static Config * getInstance(QWidget *parent = NULL);
    static void kill();
    ~Config();
    // accesseurs
    QString getFile(int num){if (num < 5 && num >= 0) return listFiles.at(num); else return "";}
    QString getRecordFile()     {return recordFile;}
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
    // méthodes publiques
    void addFavorite(QString filePath);
    void setRecordFile(QString filePath);
    void show();

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

private:
    Ui::Config * ui;
    MainWindow * mainWindow;
    // Instance unique
    static Config * _instance;
    // Paramètres configuration
    QStringList listFiles;
    QString recordFile;
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
    // Autres
    bool loaded;
    // Méthodes privées
    explicit Config(QWidget *parent = 0);
    void load();
    void store();
};

#endif // CONFIG_H
