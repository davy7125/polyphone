/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "page.h"
#include "tablewidget.h"
class ModulatorEditor;

// Common class for inst and prst
class PageTable : public Page
{
    Q_OBJECT

public:
    PageTable(bool isPrst, QWidget *parent = nullptr);

    QString getLabel() override { return tr("Table"); }
    QString getIconName() override { return ":/icons/table.svg"; }

    void displayModInTable();

public slots:
    void set(int ligne, int colonne, bool allowPropagation = true);
    void selected();

protected:
    void updateInterface(QString editingSource) override;
    void afficheTable(bool justSelection);
    bool keyPlayedInternal(int key, int velocity) override;

    IdList _currentParentIds;

    ElementType contenant;
    ElementType contenantGen;
    ElementType contenantMod;
    ElementType lien;
    ElementType lienGen;
    ElementType lienMod;
    ElementType contenu;
    TableWidget *_table;
    ModulatorEditor * _modulatorEditor;

    void select(EltID id);
    quint16 getSrcIndex(quint16 wVal, bool bVal);
    quint16 getSrcNumber(quint16 wVal, bool &CC);

protected slots:
    void actionBegin();
    void actionFinished();
    void onOpenElement(EltID id);
    void onModSelectionChanged(QList<AttributeType> attributes);

private slots:
    void divisionSortChanged();

private:
    void reselect();
    void addGlobal(IdList listIds);
    void addDivisions(EltID id);
    void formatTable(bool multiGlobal);
    int limit(int iVal, AttributeType champ, EltID id);
    void resetChamp(int colonne, AttributeType champ1, AttributeType champ2);
    void setOffset(int ligne, int colonne, AttributeType champ1, AttributeType champ2);
    void styleFixedRow(int numRow);
    bool isPlayingInInst(int key, int vel, EltID idInst);

    // Attribute <-> string conversion
    static QString attributeToString(AttributeType champ, bool isPrst, AttributeValue storedValue);
    static AttributeValue attributeFromString(AttributeType champ, bool isPrst, QString strValue, bool &ok);

    QMap<int, int> _listKeyEnlighted;
    int _sortType;
    bool _isPrst;
};

#endif // PAGETABLE_H
