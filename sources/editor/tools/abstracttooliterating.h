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

#ifndef ABSTRACTTOOLITERATING_H
#define ABSTRACTTOOLITERATING_H

#include "abstracttool.h"
class WaitingToolDialog;
class AbstractToolParameters;

class AbstractToolIterating: public AbstractTool
{
    Q_OBJECT

public:
    /// Constructor, with the type of element to manage (can be elementSmpl, elementInst, elementPrst, elementSf2 or a combination)
    AbstractToolIterating(ElementType elementType, AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    AbstractToolIterating(QList<ElementType> elementTypes, AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    virtual ~AbstractToolIterating() override;

    /// Method executed before the iterating process
    virtual void beforeProcess(IdList ids) { Q_UNUSED(ids) }

    /// Process an element
    virtual void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) = 0;

signals:
    void elementProcessed();

protected:
    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool on a list of id
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

    // Additional configurations
    bool _openWaitDialogJustInProcess; // false by default
    bool _async; // true by default

private slots:
    void onElementProcessed();
    void onCancel();

private:
    QList<ElementType> _elementTypes;
    WaitingToolDialog * _waitingDialog;
    int _steps;
    int _currentStep;
    bool _canceled;
    IdList _idsToProcess;
    SoundfontManager * _sm;
    AbstractToolParameters * _parameters;
};

#endif // ABSTRACTTOOLITERATING_H
