#ifndef TABLEHEADERVIEW_H
#define TABLEHEADERVIEW_H

#include <QHeaderView>
#include "basetypes.h"

class TableHeaderView: public QHeaderView
{
    Q_OBJECT

public:
    TableHeaderView(QWidget *parent = nullptr);
    ~TableHeaderView();

signals:
    void rightClick(QPoint pos);

protected:
    void mousePressEvent(QMouseEvent * e);

private slots:
    void mute(bool isMute);
    void unmuteAll(bool unused);

private:
    QPixmap _muteIcon;
    QMenu * _menu;
    QAction * _muteAction;
    int _currentSection;
    EltID _currentId;
};

#endif // TABLEHEADERVIEW_H
