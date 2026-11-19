/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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
**           Author: Davy Triponney & Others                              **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 18.11.2026                                           **
***************************************************************************/


#ifndef PAGEDEFAULTMOD_H
#define PAGEDEFAULTMOD_H

#include "page.h"

namespace Ui {
class PageDefaultMod;
}

// Default modulators of a soundfont: the ones of the sf2 format (read only), or its own list once customized
class PageDefaultMod : public Page
{
    Q_OBJECT

public:
    explicit PageDefaultMod(QWidget *parent = nullptr);
    ~PageDefaultMod() override;

    bool isSuitableFor(ElementType elementType) override { return elementType == elementSf2; }
    QString getLabel() override { return tr("Default modulators"); }
    QString getIconName() override { return ":/tool/defaultmod.svg"; }

protected:
    void updateInterface(QString editingSource) override;

private slots:
    void on_pushCustomize_clicked();
    void on_pushReset_clicked();

private:
    void setCustom(bool isCustom);

    Ui::PageDefaultMod *ui;
};

#endif // PAGEDEFAULTMOD_H
