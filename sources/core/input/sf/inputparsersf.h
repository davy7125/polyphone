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

#ifndef INPUTPARSERSF_H
#define INPUTPARSERSF_H

#include "abstractinputparser.h"
#include "basetypes.h"
#include "sf2reader.h"
class SoundfontManager;

// Soundfont read by Sf2Reader (shared with the synth of the plugin), the sample data being read when needed by
// SampleReaderSf
class InputParserSf : public AbstractInputParser
{
    Q_OBJECT
    
public:
    InputParserSf();
    bool canFastLoad() override { return true; }

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    void fillSf2(const Sf2Reader &reader, int &sf2Index);
    void addElements(const std::vector<Sf2Reader::Element> &elements, int sf2Index, bool isPreset);
    void addZone(EltID id, const Sf2Reader::Zone &zone, ElementType modulatorType);

    SoundfontManager * _sm;
    QString _filename;
};

#endif // INPUTPARSERSF_H
