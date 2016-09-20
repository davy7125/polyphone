#ifndef COMBOBOXLOOPMODE_H
#define COMBOBOXLOOPMODE_H

#include <QComboBox>
#include <QListView>

class ComboBoxLoopMode : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBoxLoopMode(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent * event);
    void hidePopup();

private slots:
    void onActivated(int index);

private:
    bool _ignoreFirstHide;
};

class ComboView : public QListView
{
    Q_OBJECT

protected:
    QStyleOptionViewItem viewOptions() const
    {
        // Set icon on the top and center of combo box item.
        QStyleOptionViewItem option = QListView::viewOptions();
        option.decorationAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        option.decorationSize = QSize(37, 14);
        return option;
    }
};

#endif // COMBOBOXLOOPMODE_H
