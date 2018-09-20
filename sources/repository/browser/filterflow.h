#ifndef FILTERFLOW_H
#define FILTERFLOW_H

#include <QWidget>
class FlowLayout;

class FilterFlow : public QWidget
{
    Q_OBJECT

public:
    explicit FilterFlow(QWidget *parent = 0);

    // Clear the list
    void initialize(bool singleSelection = false);

    // Add an element in the list, with the possibility to link it with an id (for the categories)
    void addElement(QString text, int id = -1, bool selected = false);

    // Change the selection
    void select(QStringList elements);
    void select(QList<int> elements);
    void select(int element);

    // Get all selected elements (as text)
    QStringList getSelectedElements();

    // Get all selected elements (as int)
    QList<int> getSelectedIds();

    // Get the unique selected id (if singleselection is true)
    int getSelectedId();

signals:
    // Emitted when the selection changed
    void selectionChanged();

private slots:
    void buttonClicked();

private:
    QWidget * createItem(QString text, int id, bool selected);

    bool _singleSelection;
    FlowLayout * _layout;
};

#endif // FILTERFLOW_H
