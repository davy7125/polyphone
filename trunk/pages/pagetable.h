/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
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

// Spécialisation de page pour inst et prst
class PageTable : public Page
{
    Q_OBJECT

public:
    PageTable(TypePage typePage, QWidget *parent = 0);
    void afficher();
    void reselect();
    void updateId(EltID id);
    void paramGlobal();
    void duplication();
    void spatialisation();
    void visualize();
    void keyPlayed(int key, int velocity);

protected:
    // Attributs protégés
    ElementType contenant;
    ElementType contenantGen;
    ElementType contenantMod;
    ElementType lien;
    ElementType lienGen;
    ElementType lienMod;
    ElementType contenu;
    TableWidget *table;
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
    QPushButton * _pushRangeMode;
    GraphicsViewRange * _rangeEditor;

    void select(EltID id);
    static void remplirComboSource(ComboBox *comboBox);
    int getDestIndex(int i);
    int getDestNumber(int i);
    quint16 getSrcIndex(quint16 wVal, bool bVal);
    quint16 getSrcNumber(quint16 wVal, bool &CC);

    /// Get all inst or prst (removing redundant entries)
    QList<EltID> getUniqueInstOrPrst(bool &error, bool errorIfNoDivision = false, bool errorIfKeyRangeMissing = false);

private:
    class Modulator
    {
    public:
        SFModulator modSrcOper;
        Champ modDestOper;
        qint32 modAmount;
        SFModulator modAmtSrcOper;
        SFTransform modTransOper;
        qint32 index;
    };

    void afficheTable();
    void addGlobal(EltID id, bool multiGlobal);
    void addDivisions(EltID id);
    void formatTable(bool multiGlobal);
    void afficheRange();
    void afficheMod(EltID id, int selectedIndex = -1);
    static void addAvailableReceiverMod(ComboBox *combo, EltID id);
    static void addAvailableSenderMod(ComboBox *combo, EltID id);
    int getAssociatedMod(EltID id);
    int limit(int iVal, Champ champ, EltID id);
    void duplication(EltID id);
    void duplication(EltID id, QVector<int> listeVelocite);
    void resetChamp(int colonne, Champ champ1, Champ champ2);
    void setOffset(int ligne, int colonne, Champ champ1, Champ champ2);
    void customizeKeyboard(bool withAllDivisions = false);
    void pasteMod(EltID id, QList<Modulator> modulators);
    QList<Modulator> getModList(EltID id);

    static QList<Modulator> _modulatorCopy;
    QList<int> _listKeyEnlighted;

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

protected slots:
    void copyMod();
    void pasteMod();
    void duplicateMod();
    void duplicateMod(QList<int> listIndex);
    void actionBegin();
    void actionFinished();
    void updateKeyboard();

private slots:
    void paramGlobal(QVector<double> dValues, int typeModif, int champ, int velMin, int velMax);
    void spatialisation(QMap<int, double> mapPan);
    void spatialisation(QMap<int, double> mapPan, EltID id);
    void duplication(QVector<int> listeVelocites, bool duplicKey, bool duplicVel);
};

#endif // PAGETABLE_H
