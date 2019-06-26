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
    class DisplayOption
    {
    public:
        DisplayOption(int id, QString iconName, QString label, bool isEnabled = true) :
            _id(id),
            _iconName(iconName),
            _label(label),
            _isEnabled(isEnabled),
            _isSelected(false)
        {}

        int _id;
        QString _iconName;
        QString _label;
        bool _isEnabled;
        bool _isSelected;
    };

    enum TypePage
    {
        PAGE_SF2,
        PAGE_SMPL,
        PAGE_INST,
        PAGE_PRST
    };
    Page(QWidget *parent, TypePage typePage, QString editingSource);

    // Prepare the page, possibly with other selected ids
    // Return true if success
    bool preparePage(QString editingSource, IdList selectedIds = IdList());

    // Get the current display options, the list being updated in "preparePage"
    QList<DisplayOption> getDisplayOptions() { return _displayOptions; }

    // Change the display option
    void setDisplayOption(int displayOption);

public slots:
    // A key is being played or not played anymore (if velocity is 0)
    void keyPlayed(int key, int velocity)
    {
        if (key != -1 && this->isVisible())
            this->keyPlayedInternal(key, velocity);
    }

signals:
    // Emitted when the selected ids changed
    void selectedIdsChanged(IdList ids);

protected:
    // Get the list of all display options
    virtual QList<DisplayOption> getDisplayOptions(IdList selectedIds);

    // Update the interface
    virtual bool updateInterface(QString editingSource, IdList selectedIds, int displayOption) = 0;

    // A key is being played or not played anymore (if velocity is 0)
    virtual void keyPlayedInternal(int key, int velocity)
    {
        Q_UNUSED(key)
        Q_UNUSED(velocity)
    }

    // Refresh things after a page is shown
    virtual void onShow() = 0;

    // Get the editing source when a soundfont is edited
    // Can be for instance page:smpl, page:inst, page:prst
    QString getEditingSource() { return _editingSource; }

    static SoundfontManager * _sf2;
    static Synth * _synth;
    bool _preparingPage;
    TypePage _typePage;
    IdList _currentIds;

    static QString getIndexName(quint16 iVal, int CC);

    void showEvent(QShowEvent * event);
    void hideEvent(QHideEvent * event);

private:
    QString _editingSource;
    int _currentDisplayOption;
    QList<DisplayOption> _displayOptions;
};

#endif // PAGE_H
