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

#ifndef ABSTRACTTOOL_H
#define ABSTRACTTOOL_H

#include "basetypes.h"
#include <QObject>
class QWidget;
class SoundfontManager;
class AbstractToolGui;
class AbstractToolParameters;
class ToolDialog;

class AbstractTool: public QObject
{
    Q_OBJECT

public:
    AbstractTool(AbstractToolParameters * parameters = nullptr, AbstractToolGui * gui = nullptr);
    virtual ~AbstractTool();

    /// Initialization of resources shared by all tools
    static void initialize(QWidget * parent);

    /// Icon, label and category displayed to the user to describe the tool
    virtual QString getIconName() const = 0;
    QString getLabel() const;
    virtual QString getCategory() const = 0;

    /// Internal identifier
    virtual QString getIdentifier() const = 0;

    /// Set the ids and return true if the tool can be used on the specified ids
    bool setIds(IdList ids);

    /// Run the tool on the specified list of ids
    void run();

signals:
    void finished(bool updateNeeded);

protected:
    /// Get the name of the tool. It will be then followed by "..." if there is a GUI
    virtual QString getLabelInternal() const = 0;

    /// Return true if the tool can be used on the specified ids
    virtual bool isCompatible(IdList ids) = 0;

    /// Run the tool, emit the signal "finished" at the end
    virtual void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) = 0;

    /// Get the warning to display after the tool is run
    virtual QString getWarning() { return ""; }

    /// Get a confirmation message after the tool is run
    virtual QString getConfirmation() { return ""; }

private slots:
    void onFinished(bool updateNeeded);
    void onParametersValidated();

private:
    AbstractToolParameters * _toolParameters;
    AbstractToolGui * _toolGui;
    ToolDialog * _toolDialog;
    IdList _currentIds;

    static QWidget * s_parent; // For opening dialogs
    static SoundfontManager * s_sm;
};

#endif // ABSTRACTTOOL_H
