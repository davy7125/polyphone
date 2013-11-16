/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
#include <string>
using std::string;

class MainWindow;

class Tree : public QTreeWidget
{
    Q_OBJECT
public:
    class menuClicDroit
    {
    public :
        // constructeur
        menuClicDroit(MainWindow *mainWindow);

        // attributs
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
    // Méthodes publiques
    void init(MainWindow *mainWindow);
    void trier(int forme);
    void updateAtNextSelectionRequest();
    unsigned int getSelectedItemsNumber();
    bool isSelectedItemsTypeUnique();
    bool isSelectedItemsSf2Unique();
    bool isSelectedItemsFamilyUnique();
    EltID getID(unsigned int pos);
    void selectNone(bool refresh = false);
    void select(EltID id, bool refresh = false);
    void desactiveSuppression();
    void activeSuppression();
    void clearPastedID();

public slots:
    void collapse() {this->trier(1);}       // Clic sur "enrouler"
    void searchTree(QString qStr);          // Lors d'une modification du champ recherche
    void clicTree();                        // Modification de la sélection dans l'arborescence
    void clicTreeRight();                   // Clic droit dans l'arborescence

signals:
    void dropped(EltID dest, EltID src, int temps, int *msg, QByteArray *ba1, QByteArray *ba2);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent *event);

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
    void updateSelectionInfo();
    unsigned int infoSelectedItemsNumber;
    bool infoIsSelectedItemsTypeUnique;
    bool infoIsSelectedItemsSf2Unique;
    bool infoIsSelectedItemsFamilyUnique;

    // Méthodes privées
    QTreeWidgetItem * selectedItem(unsigned int pos);
    static EltID getItemID(QTreeWidgetItem *elt);
    void supprimerElt();
};

#endif // TREE_H
