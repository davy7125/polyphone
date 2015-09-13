#ifndef TABLEPAGEOVERVIEW_H
#define TABLEPAGEOVERVIEW_H

#include <QTableWidget>

class TablePageOverview : public QTableWidget
{
    Q_OBJECT
public:
    explicit TablePageOverview(QWidget *parent = 0);

    // Colors every other row in yellow
    void colorRows();

private slots:
    void onSort(int column);
};

#endif // TABLEPAGEOVERVIEW_H
