#ifndef TOOLMENU_H
#define TOOLMENU_H

#include <QMenu>
#include "idlist.h"
class ToolFactory;
class AbstractTool;

class ToolMenu : public QMenu
{
    Q_OBJECT

public:
    ToolMenu(QWidget *parent = NULL);
    ~ToolMenu();

    /// Notify that the selection changed
    void selectionChanged(IdList ids);

private slots:
    /// When a QAction is clicked
    void onTriggered(QAction * action);

private:
    void addCategory(QString categoryName);
    static bool lessThan(const AbstractTool * tool1, const AbstractTool * tool2);
    QString _separatorBackgroundColor;
    QString _separatorTextColor;

    ToolFactory * _toolFactory;
    QMap<QAction *, AbstractTool *> _currentActions;
};

#endif // TOOLMENU_H
