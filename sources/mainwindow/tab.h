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

#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include "keyboarddisplaycontent.h"

class Tab: public QWidget
{
public:
    enum TabAction
    {
        SEARCH,
        UNDO,
        REDO,
        PREVIOUS_ELEMENT, // Element played by the virtual keyboard
        NEXT_ELEMENT,
        PREVIOUS_FILE, // File of a soundfont explorer
        NEXT_FILE
    };

    Tab(QWidget *parent = nullptr);

public:
    virtual void onActionRequired(TabAction action) { Q_UNUSED(action) }

    // Element played by the virtual keyboard in this tab, with the navigation possibilities
    virtual KeyboardDisplayContent getKeyboardDisplayContent() { return KeyboardDisplayContent(); }
};

#endif // TAB_H
