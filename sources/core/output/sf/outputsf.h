/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef OUTPUTSF_H
#define OUTPUTSF_H

#include "abstractoutput.h"
#include "basetypes.h"
#include "../../input/sf/sf2pdtapart_mod.h"
#include "../../input/sf/sf2pdtapart_gen.h"
#include "sfindexconverter.h"
class SoundfontManager;
class Sf2Header;
class Sf2SdtaPart;
class Sf2PdtaPart;

class OutputSf : public AbstractOutput
{
    Q_OBJECT
    
public:
    OutputSf();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;

private:
    SoundfontManager * _sm;

    void save(QString fileName, bool &success, QString &error, int sf2Index, double qualityValue);
    void fillSf2(int sf2Index, Sf2Header * header, Sf2SdtaPart * sdtaPart, Sf2PdtaPart * pdtaPart);
    void loadMods(EltID idMod, QList<Sf2PdtaPart_mod> &mods, quint32 &index);
    void loadGens(EltID idGen, EltID idDiv, QList<Sf2PdtaPart_gen> &gens, quint32 &index, SfIndexConverter &indexConverter);
    static void convertTo16bit(QVector<float> dataSrc, QByteArray &dataDest);
    static void convertTo24bit(QVector<float> dataSrc, QByteArray &dataDest);
};

#endif // OUTPUTSF_H
