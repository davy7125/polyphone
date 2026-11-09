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

#ifndef MCPEDITOR_H
#define MCPEDITOR_H

#include "basetypes.h"

// Editor of Polyphone seen by the MCP functions: tabs of the soundfonts and selection. It is given by the main window
// (TabManager), so that the MCP server does not depend on the interface.
class McpEditor
{
public:
    virtual ~McpEditor() {}

    /// Editor used by the MCP functions, null if there is none (command line, editor closed)
    static McpEditor * getInstance() { return s_instance; }
    static void setInstance(McpEditor * editor) { s_instance = editor; }

    /// Create a soundfont in a new tab, return its index (-1 if it cannot be created)
    virtual int openNewSoundfont() = 0;

    /// Open a file in a new tab (the errors are displayed by the editor)
    virtual void openSoundfont(QString filePath) = 0;

    /// Show an element in its tab, return false if this is not possible
    virtual bool showElement(EltID id) = 0;

    /// Soundfont of the current tab (-1 if there is none) and elements selected in it
    virtual int getCurrentSoundfont() = 0;
    virtual IdList getCurrentSelection() = 0;

private:
    static McpEditor * s_instance;
};

#endif // MCPEDITOR_H
