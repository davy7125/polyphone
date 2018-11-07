#ifndef TREEVIEWMENU_H
#define TREEVIEWMENU_H

#include <QMenu>
#include "basetypes.h"
class DialogList;

class TreeViewMenu : public QMenu
{
    Q_OBJECT

public:
    TreeViewMenu(QWidget * parent);
    ~TreeViewMenu();

    /// Initialize the menu with the selected IDs
    void initialize(IdList ids);

public slots:
    void associate();
    void replace();
    void itemSelectedFromList(EltID id, bool isAssociation);
    void copy();
    void paste();
    void duplicate();
    void remove();
    void rename();

private slots:
    void bulkRename(int renameType, QString text1, QString text2, int val1, int val2);

private:
    void associate(IdList ids, EltID idDest);
    void replace(EltID idSrc, EltID idDest);

    IdList _currentIds;
    QAction * _replaceAction;
    QAction * _associateAction;
    QAction * _copyAction;
    QAction * _pasteAction;
    QAction * _duplicateAction;
    QAction * _removeAction;
    QAction * _renameAction;
    DialogList * _dialogList;

    // Same copy for all instances
    static IdList s_copy;
};

#endif // TREEVIEWMENU_H
