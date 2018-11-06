#ifndef FILTERTAG_H
#define FILTERTAG_H

#include <QWidget>
class QCompleter;
class FlowLayout;
class QVBoxLayout;

namespace Ui {
class FilterTag;
}

class FilterTag : public QWidget
{
    Q_OBJECT

public:
    explicit FilterTag(QWidget *parent = 0);
    ~FilterTag();

    // Initialize the different tags that can be selected
    void setPossibleTags(QStringList tags);

    void select(QStringList tags);
    QStringList getSelection();

signals:
    void selectionChanged();

private slots:
    void onCompletionSelected(QString completion);
    void on_lineEdit_returnPressed();
    void onCellDelete();

private:
    Ui::FilterTag *ui;
    QStringList _possibleTags;
    QStringList _tags;
    QCompleter * _completer;
    QVBoxLayout * _layout;
};

#endif // FILTERTAG_H
