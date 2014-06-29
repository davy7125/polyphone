/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

// Classe TableComboBox pour les formes de courbes
class TableComboBox : public QComboBox
{
    Q_OBJECT

public:
    TableComboBox(QWidget* parent = 0) : QComboBox(parent)
    {
        char T[60];
        // Nouvelle vue
        QTableView * view = new QTableView();
        view->viewport()->installEventFilter(this);
        view->horizontalHeader()->setVisible(false);
        view->verticalHeader()->setVisible(false);
        view->setShowGrid(false);
        view->setFixedSize(28*4, 30*4);
        this->setView(view);
        // Préparation du modèle
        QStandardItemModel * model = new QStandardItemModel();
        model->setColumnCount(4);
        model->setRowCount(4);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                sprintf(T, ":/icones/courbe%.2d", 4*j+i+1);
                model->setItem(i, j, new QStandardItem(QIcon(T), ""));
            }
        }
        this->setModel(model);
        view->resizeColumnsToContents();
    }

    ~TableComboBox()
    {
        delete this->model();
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            view()->indexAt(mouseEvent->pos());
            this->setCurrentIndex(view()->currentIndex().row());
            this->setModelColumn(view()->currentIndex().column());
            emit(clicked(view()->currentIndex().row(), view()->currentIndex().column()));
        }
        return false;
    }

signals:
    void clicked(int row, int column);
};

// Classe ComboBox pour destination et source
class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QWidget* parent = 0) : QComboBox(parent)
    {
        this->limite = 0;
        this->view()->viewport()->installEventFilter(this);
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            int index = view()->indexAt(mouseEvent->pos()).row();
            if (index >= this->limite)
            {
                // index du modulateur pointé
                char T[20];
                strcpy(T, this->itemText(index).toStdString().c_str());
                index = this->itemText(index).split("#").last().toInt();
                //sscanf(T, ": #%i", &index);
                index += 32767; // 32768 - 1
            }
            emit(this->clicked(index));
        }
        return false;
    }

    void setLimite(int lim) {this->limite = lim;}

    void removeItemsAfterLim()
    {
        for (int i = this->count(); i >= this->limite; i--)
            this->removeItem(i);
    }

    void selectIndex(WORD index, WORD numChamp)
    {
        if (index > 99)
        {
            // On cherche le modulateur numChamp
            numChamp -= 32768;
            int iVal = this->findText(trUtf8("Modulateur") + ": #" + QString::number(numChamp + 1));
            if (iVal != -1)
                this->setCurrentIndex(iVal);
            else
                this->setCurrentIndex(0);
        }
        else
        {
            // Sélection de l'index pointé par index
            this->setCurrentIndex(index);
        }
    }

    void selectIndex(WORD index)
    {
        this->setCurrentIndex(index);
    }

private:
    int limite;

signals:
    void clicked(int index);
};

// Spécialisation de page pour inst et prst
class PageTable : public Page
{
    Q_OBJECT

public:
    PageTable(TypePage typePage, QWidget *parent = 0);
    // Méthodes publiques
    void afficher();
    void clearTable();
    void reselect();
    void updateId(EltID id);
    void paramGlobal();
    void duplication();
    void spatialisation();
    void visualize();
    void enlightColumn(int key, bool isEnlighted);

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

    void select(EltID id);
    static void remplirComboSource(ComboBox *comboBox);
    int getDestIndex(int i);
    int getDestNumber(int i);
    WORD getSrcIndex(WORD wVal, bool bVal);
    WORD getSrcNumber(WORD wVal, bool &CC);

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

    void afficheMod(EltID id, int selectedRow = -1);
    static void addAvailableReceiverMod(ComboBox *combo, EltID id);
    static void addAvailableSenderMod(ComboBox *combo, EltID id);
    int getAssociatedMod(EltID id);
    int limit(int iVal, Champ champ, EltID id);
    void duplication(EltID id);
    void duplication(EltID id, QVector<int> listeVelocite);
    void resetChamp(bool &newAction, int colonne, Champ champ1, Champ champ2);
    void setOffset(bool &newAction, int ligne, int colonne, Champ champ1, Champ champ2);
    void set(int ligne, int colonne, bool &newAction, bool allowPropagation);
    void customizeKeyboard();

    static QList<Modulator> _modulatorCopy;
    QList<int> _listKeyEnlighted;

public slots:
    void set(int ligne, int colonne);
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

private slots:
    void paramGlobal(QVector<double> dValues, QList<EltID> listElt, int typeModif, int champ, int velMin, int velMax);
    void spatialisation(QMap<int, double> mapPan);
    void duplication(QVector<int> listeVelocites, bool duplicKey, bool duplicVel);
};

#endif // PAGETABLE_H
