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

#ifndef INPUTFACTORY_H
#define INPUTFACTORY_H

#include <QString>
#include <QMap>
class AbstractInputParser;
class AbstractInput;

class InputFactory
{
public:
    /// Get an input related to a file
    static AbstractInputParser * getInput(QString fileName);

    /// Return true if the suffix (extension) is supported
    static bool isSuffixSupported(QString suffix);

    /// Return a file filter for the open file names dialog
    static QString getFileFilter();

    /// Clear all inputs
    static void clear();

private:
    InputFactory();
    ~InputFactory();

    static InputFactory * s_instance;

    // All inputs sorted by extension, with no uppercase
    QMap<QString, AbstractInput *> _inputs;
    QList<AbstractInput *> _orderedInputs;
};

#endif // INPUTFACTORY_H
