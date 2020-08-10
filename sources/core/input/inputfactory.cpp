/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "inputfactory.h"
#include <QFileInfo>
#include "soundfontmanager.h"
#include "abstractinputparser.h"
#include "abstractinput.h"
#include "sf2/inputsf2.h"
#include "sfz/inputsfz.h"
#include "sfark/inputsfark.h"
#include "grandorgue/inputgrandorgue.h"
#include "not_supported/inputparsernotsupported.h"
#include "empty/inputparserempty.h"
#include <QObject>

InputFactory * InputFactory::s_instance = nullptr;

InputFactory::InputFactory()
{
    // Load all kinds of input
    AbstractInput * input = new InputSf2();
    _inputs[input->getInputExtension().toLower()] = input;
    _orderedInputs << input;

    input = new InputSfz();
    _inputs[input->getInputExtension().toLower()] = input;
    _orderedInputs << input;

    input = new InputSfArk();
    _inputs[input->getInputExtension().toLower()] = input;
    _orderedInputs << input;

    input = new InputGrandOrgue();
    _inputs[input->getInputExtension().toLower()] = input;
    _orderedInputs << input;
}

void InputFactory::clear()
{
    if (s_instance == nullptr)
        return;
    delete s_instance;
    s_instance = nullptr;
}

InputFactory::~InputFactory()
{
    while (!_inputs.empty())
        delete _inputs.take(s_instance->_inputs.firstKey());
    _orderedInputs.clear();
}

AbstractInputParser * InputFactory::getInput(QString fileName)
{
    if (s_instance == nullptr)
        s_instance = new InputFactory();

    AbstractInputParser * input = nullptr;

    if (fileName.isEmpty())
    {
        // New soundfont from scratch
        input = new InputParserEmpty();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (s_instance->_inputs.contains(extension))
            input = s_instance->_inputs[extension]->getParser();
        else
            input = new InputParserNotSupported();
    }
    input->initialize(fileName, SoundfontManager::getInstance());

    return input;
}

bool InputFactory::isSuffixSupported(QString suffix)
{
    if (s_instance == nullptr)
        s_instance = new InputFactory();
    return s_instance->_inputs.contains(suffix.toLower());
}

QString InputFactory::getFileFilter()
{
    if (s_instance == nullptr)
        s_instance = new InputFactory();

    QString extConcat = QObject::tr("All") + " (";
    QString typeConcat;

    bool first = true;
    foreach (AbstractInput * input, s_instance->_orderedInputs)
    {
        if (first)
            first = false;
        else
            extConcat += " ";
        extConcat += "*." + input->getInputExtension();
        typeConcat += ";;" + input->getInputDescription() + " (*." + input->getInputExtension() + ")";
    }

    return extConcat + ")" + typeConcat;
}
