#ifndef FILTERTAGCELL_H
#define FILTERTAGCELL_H

#include <QWidget>

namespace Ui {
class FilterTagCell;
}

class FilterTagCell : public QWidget
{
    Q_OBJECT

public:
    explicit FilterTagCell(QWidget *parent = 0);
    ~FilterTagCell();

    void setTagName(QString tagName);
    QString getTagName();

signals:
    void onRemoveClicked();

private slots:
    void on_pushDelete_clicked();

private:
    Ui::FilterTagCell *ui;
    QString _tagName;
};

#endif // FILTERTAGCELL_H
