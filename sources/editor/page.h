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

#ifndef PAGE_H
#define PAGE_H

#include "soundfontmanager.h"
#include "synth.h"
#include <QStackedWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QHeaderView>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QApplication>
#include <QPushButton>
#include <QPair>

class Config;

namespace Ui
{
    class Page;
    class PageTable;
}

class Page : public QWidget
{
    Q_OBJECT

public:
    Page(QWidget *parent, QString editingSource);

    // Prepare the page, possibly with other selected ids
    // Return true if success
    void preparePage(QString editingSource, IdList selectedIds = IdList());

    virtual bool isSuitableFor(ElementType elementType) = 0;
    virtual QString getLabel() { return ""; }
    virtual QString getIconName() { return ""; }

    // A key is being played or not played anymore (if velocity is 0)
    void onKeyPlayed(int key, int velocity)
    {
        if (key != -1 && this->isVisible())
            this->keyPlayedInternal(key, velocity);
    }

public slots:
    // Key "space" is pressed in the tree
    void onSpacePressed()
    {
        if (this->isVisible())
            this->onSpacePressedInternal();
    }

signals:
    // Emitted when the selected ids changed
    void selectedIdsChanged(IdList ids);

    // Emitted when the rootkey of a sample changed (for updating the keyboard)
    void rootKeyChanged(int rootKey);

    // Emitted when the page is hidden (clearing thus the customization of the keyboard)
    void pageHidden();

protected:
    // Update the interface
    virtual void updateInterface(QString editingSource) = 0;

    // A key is being played or not played anymore (if velocity is 0)
    virtual void keyPlayedInternal(int key, int velocity)
    {
        Q_UNUSED(key)
        Q_UNUSED(velocity)
    }

    // Key "space" is pressed in the tree
    virtual void onSpacePressedInternal() {}

    void hideEvent(QHideEvent * event) override;

    static SoundfontManager * _sf2;
    static Synth * _synth;

    bool _preparingPage;
    IdList _currentIds;
    QString _editingSource;
};

#endif // PAGE_H
