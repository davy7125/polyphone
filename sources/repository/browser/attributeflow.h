#ifndef ATTRIBUTEFLOW_H
#define ATTRIBUTEFLOW_H

#include <QWidget>
#include "soundfontinformation.h"
class FlowLayout;
class SoundfontFilter;
class ElidedPushButton;

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

protected:
    void resizeEvent(QResizeEvent * event);

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

    ElidedPushButton * createItem(QString text);

    FlowLayout * _layout;
    QMap<ElidedPushButton *, FilterDefinition> _filterDefinitions;
};

#endif // ATTRIBUTEFLOW_H
