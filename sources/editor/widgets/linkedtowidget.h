#ifndef LINKEDTOWIDGET_H
#define LINKEDTOWIDGET_H

#include <QWidget>
#include <QMap>
#include "basetypes.h"
class FlowLayout;

class LinkedToWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LinkedToWidget(QWidget *parent = 0);
    ~LinkedToWidget();

    /// Clear all links
    void clear();

    /// Initialization
    void initialize(EltID id);

    /// Get the number of elements linked
    int getLinkNumber();

signals:
    /// Emitted when an item is clicked
    void itemClicked(EltID id);

private slots:
    void onClick(bool isClicked);

private:
    void addLink(EltID id, QString text);

    FlowLayout * _layout;
    QMap<QWidget *, EltID> _buttonIds;
};

#endif // LINKEDTOWIDGET_H
