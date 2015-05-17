/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#ifndef TREE_H
#define TREE_H

#include "qtreewidget.h"
#include "sf2_types.h"

class Pile_sf2;
class MainWindow;
class QComboBox;

class Tree : public QTreeWidget
{
    Q_OBJECT
public:
    class menuClicDroit
    {
    public :
        menuClicDroit(MainWindow *mainWindow);
        ~menuClicDroit();

        QAction *nouveauSample;
        QAction *nouvelInstrument;
        QAction *nouveauPreset;
        QAction *remplacer;
        QAction *associer;
        QAction *copier;
        QAction *coller;
        QAction *supprimer;
        QAction *renommer;
        QAction *fermer;
        QMenu *menu;
    };
    explicit Tree(QWidget *parent = 0);
    ~Tree();

    void init(MainWindow *mainWindow, Pile_sf2 *sf2, QComboBox * comboSf2);
    void trier(int forme);
    void updateAtNextSelectionRequest();
    unsigned int getSelectedItemsNumber();
    bool isSelectedItemsTypeUnique();
    bool isSelectedItemsSf2Unique();
    bool isSelectedItemsFamilyUnique();
    EltID getFirstID();
    QList<EltID> getAllIDs();
    EltID getNextID(bool closeFile);
    void selectNone(bool refresh = false);
    void select(EltID id, bool refresh = false);
    void desactiveSuppression();
    void activeSuppression();
    void clearPastedID();
    EltID getElementToSelectAfterDeletion();
    void addSf2InComboBox(int numSf2);
    void removeSf2FromComboBox(int numSf2);
    void renameSf2InComboBox(int numSf2, QString name);

public slots:
    void collapse() {this->trier(1);}       // Clic sur "enrouler"
    void searchTree(QString qStr);          // Lors d'une modification du champ recherche
    void clicTree();                        // Modification de la sélection dans l'arborescence
    void clicTreeRight();                   // Clic droit dans l'arborescence
    void comboSf2IndexChanged(int index);

signals:
    void dropped(EltID dest, EltID src, int temps, int *msg, QByteArray *ba1, QByteArray *ba2);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    QStringList mimeTypes() const
    {
        QStringList qstrList;
        qstrList.append("text/uri-list");
        return qstrList;
    }

private:
    // Attributs privés
    MainWindow *mainWindow;
    menuClicDroit *menuArborescence;
    bool refresh;
    bool updateNext;
    QList<EltID> idList;
    unsigned int infoSelectedItemsNumber;
    bool infoIsSelectedItemsTypeUnique;
    bool infoIsSelectedItemsSf2Unique;
    bool infoIsSelectedItemsFamilyUnique;
    Pile_sf2 * _sf2;
    QComboBox * _comboSf2;
    QList<EltID> _displayedElements;
    QString _searchedText;
    int _displayedSf2;

    // Méthodes privées
    void updateSelectionInfo();
    QTreeWidgetItem * selectedItem(unsigned int pos);
    static EltID getItemID(QTreeWidgetItem *elt);
    void supprimerElt();
    void displaySample(int idSf2, int index, bool repercute = true);
    void displayInstrument(int idSf2, int index, bool repercuteSmpl = true, bool repercutePrst = true);
    void displayPreset(int idSf2, int index, bool repercute = true);
    void displayElement(EltID id);
    EltID getNextSample();
    EltID getNextInst();
    EltID getNextInstSmpl(int numInst);
    EltID getNextPrst();
    EltID getNextPrstInst(int numPrst);
    void searchTree(QString qStr, int displayedSf2);
};

#endif // TREE_H
