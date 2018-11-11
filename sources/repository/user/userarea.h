#ifndef USERAREA_H
#define USERAREA_H

#include <QWidget>

namespace Ui {
class UserArea;
}

class UserArea : public QWidget
{
    Q_OBJECT

public:
    explicit UserArea(QWidget *parent = 0);
    ~UserArea();

    void initializeInterface();

private:
    Ui::UserArea *ui;
};

#endif // USERAREA_H
