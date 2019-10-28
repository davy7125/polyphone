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

#ifndef MODULATORSPLITTER_H
#define MODULATORSPLITTER_H

#include <QSplitter>
class ModulatorEditor;

class ModulatorSplitterHandle: public QSplitterHandle
{
    Q_OBJECT

public:
    ModulatorSplitterHandle(Qt::Orientation orientation, QSplitter * parent) :
        QSplitterHandle(orientation, parent),
        _enabled(true)
    {}

    void setMoveEnabled(bool isEnabled)
    {
        _enabled = isEnabled;
        this->setCursor(_enabled ? Qt::SplitVCursor : Qt::ArrowCursor);
    }

protected:
    void mouseMoveEvent(QMouseEvent * event);

private:
    bool _enabled;
};

class ModulatorSplitter: public QSplitter
{
    Q_OBJECT

public:
    ModulatorSplitter(QWidget *parent, QWidget *table, ModulatorEditor *modEditor, bool isPrst);

protected:
    QSplitterHandle *createHandle();

private slots:
    void splitterMoved(int pos, int index);
    void expandedStateChanged(bool isExpanded);

private:
    static QVariantList toVariantList(QList<int> list);
    static QList<int> fromVariantList(QVariantList list);

    bool _isPrst;
    bool _isExpanded;
    ModulatorEditor * _modEditor;
};

#endif // MODULATORSPLITTER_H
