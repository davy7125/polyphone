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
#include <stdlib.h>
#include <stdio.h>
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
    explicit Config(QWidget *parent = 0);
    ~Config();
    // accesseurs
    QString getFile(int num){if (num < 5 && num >= 0) return listFiles.at(num); else return "";}
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
    // méthodes publiques
    void addFavorite(QString filePath);
    void show();

public slots:
    void setRam(int val);
    void setAfficheMod(int val);
    void setAfficheToolBar(int val);
    void setAudioOutput(int index);
    void setWavAutoLoop(bool checked);
    void setWavRemoveBlank(bool checked);
    void setKeyboardType(int val);
    void setKeyboardVelocity(int val);
    void setNumPortMidi(int val);
    void setSynthGain(int val);

private:
    Ui::Config *ui;
    MainWindow * mainWindow;
    // Paramètres configuration
    QStringList listFiles;
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
    // Autres
    QString confFile;
    bool loaded;
    // Méthodes privées
    void load();
    void store();
};

#endif // CONFIG_H
