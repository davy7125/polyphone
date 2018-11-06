#ifndef ATTRIBUTEFLOW_H
#define ATTRIBUTEFLOW_H

#include <QWidget>
#include "soundfontinformation.h"
class FlowLayout;
class SoundfontFilter;

class AttributeFlow : public QWidget
{
    Q_OBJECT

public:
    explicit AttributeFlow(QWidget *parent = 0);
    ~AttributeFlow();
    void addCategory(int id);
    void addProperty(SoundfontInformation::Property property, QString propertyValue);
    void addTag(QString tagName);

signals:
    // Signal emitted when an item is clicked
    // The filter must be deleted then
    void itemClicked(SoundfontFilter * filter);

private slots:
    void onClick(bool checked);

private:
    enum FilterType
    {
        CATEGORY,
        PROPERTY,
        TAG
    };

    class FilterDefinition
    {
    public:
        FilterType _type;
        int _id;
        SoundfontInformation::Property _property;
        QString _propertyValue;
        QString _tag;
    };

    QWidget * createItem(QString text);

    FlowLayout * _layout;
    QMap<QWidget *, FilterDefinition> _filterDefinitions;
};

#endif // ATTRIBUTEFLOW_H
