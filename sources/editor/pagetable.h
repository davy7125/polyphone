/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include "page.h"
#include "combobox.h"
#include "tablecombobox.h"
#include "tablewidget.h"
#include "tablewidgetmod.h"
class GraphicsViewRange;
class EnvelopEditor;

// Spécialisation de page pour inst et prst
class PageTable : public Page
{
    Q_OBJECT

public:
    PageTable(TypePage typePage, QWidget *parent = nullptr);

    void reselect();
    void displayModInTable();

public slots:
    void set(int ligne, int colonne, bool allowPropagation = true);
    void setAmount();
    void setAbs();
    void selected();
    void afficheEditMod();
    void setSourceType(int row, int column);
    void setSourceAmountType(int row, int column);
    void setDest(int index);
    void setSource(int index);
    void setSource2(int index);
    void supprimerMod();
    void nouveauMod();

protected:
    QList<EltID> getEltIds(bool &error, bool allWithDivisions, bool allDivWithRange);
    void afficheTable(bool justSelection);
    void afficheRanges(bool justSelection);
    void afficheEnvelops(bool justSelection);
    void keyPlayedInternal(int key, int velocity) override;
    virtual void keyPlayedInternal2(int key, int velocity) = 0;

    // Refresh things after a page is shown
    void onShow() override;

    IdList _currentParentIds;
    IdList _currentIds;

    ElementType contenant;
    ElementType contenantGen;
    ElementType contenantMod;
    ElementType lien;
    ElementType lienGen;
    ElementType lienMod;
    ElementType contenu;
    TableWidget *_table;
    TableWidgetMod *tableMod;
    QSpinBox *spinAmount;
    ComboBox *comboSource1;
    TableComboBox *comboSource1Courbe;
    ComboBox *comboSource2;
    TableComboBox *comboSource2Courbe;
    ComboBox *comboDestination;
    QCheckBox *checkAbs;
    QPushButton *pushSupprimerMod;
    QPushButton *pushNouveauMod;
    QMenu * _menu;
    QPushButton * _pushCopyMod;
    GraphicsViewRange * _rangeEditor;
    EnvelopEditor * _envelopEditor;

    void select(EltID id);
    static void remplirComboSource(ComboBox *comboBox);
    virtual int getDestIndex(AttributeType type) = 0;
    virtual AttributeType getDestNumber(int row) = 0;
    quint16 getSrcIndex(quint16 wVal, bool bVal);
    quint16 getSrcNumber(quint16 wVal, bool &CC);

protected slots:
    void copyMod();
    void pasteMod();
    void duplicateMod();
    void duplicateMod(QList<int> listIndex);
    void actionBegin();
    void actionFinished();
    void customizeKeyboard();
    void onOpenElement(EltID id);

private slots:
    void divisionSortChanged();

private:
    class Modulator
    {
    public:
        SFModulator modSrcOper;
        AttributeType modDestOper;
        qint32 modAmount;
        SFModulator modAmtSrcOper;
        SFTransform modTransOper;
        qint32 index;
    };

    void addGlobal(IdList listIds);
    void addDivisions(EltID id);
    void formatTable(bool multiGlobal);
    void afficheMod(EltID id, AttributeType selectedField);
    void afficheMod(EltID id, int selectedIndex = -1);
    static void addAvailableReceiverMod(ComboBox *combo, EltID id);
    static void addAvailableSenderMod(ComboBox *combo, EltID id);
    int getAssociatedMod(EltID id);
    int limit(int iVal, AttributeType champ, EltID id);
    void resetChamp(int colonne, AttributeType champ1, AttributeType champ2);
    void setOffset(int ligne, int colonne, AttributeType champ1, AttributeType champ2);
    void pasteMod(EltID id, QList<Modulator> modulators);
    QList<Modulator> getModList(EltID id);

    static QList<Modulator> _modulatorCopy;
    QList<int> _listKeyEnlighted;
};

#endif // PAGETABLE_H
