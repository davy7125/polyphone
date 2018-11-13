#include "attributeflow.h"
#include "flowlayout.h"
#include "repositorymanager.h"
#include "windowmanager.h"
#include "elidedpushbutton.h"

AttributeFlow::AttributeFlow(QWidget *parent) : QWidget(parent)
{
    _layout = new FlowLayout(this);
    _layout->setMargin(0);
    this->setLayout(_layout);
}

AttributeFlow::~AttributeFlow()
{
    while (!_layout->isEmpty())
        delete _layout->takeAt(0);
    delete _layout;
}

void AttributeFlow::addCategory(int id)
{
    ElidedPushButton * widget = createItem(RepositoryManager::getInstance()->getCategoryName(id));
    _filterDefinitions[widget]._type = FilterType::CATEGORY;
    _filterDefinitions[widget]._id = id;
    _layout->addWidget(widget);
}

void AttributeFlow::addProperty(SoundfontInformation::Property property, QString propertyValue)
{
    ElidedPushButton * widget = createItem(propertyValue);
    _filterDefinitions[widget]._type = FilterType::PROPERTY;
    _filterDefinitions[widget]._property = property;
    _filterDefinitions[widget]._propertyValue = propertyValue;
    _layout->addWidget(widget);
}

void AttributeFlow::addTag(QString tagName)
{
    ElidedPushButton * widget = createItem(tagName);
    _filterDefinitions[widget]._type = FilterType::TAG;
    _filterDefinitions[widget]._tag = tagName;
    _layout->addWidget(widget);
}

ElidedPushButton * AttributeFlow::createItem(QString text)
{
    ElidedPushButton * button = new ElidedPushButton(this);
    button->setTextToElide(text, this->width() - 12);
    button->setCursor(Qt::PointingHandCursor);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    return button;
}

void AttributeFlow::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event)

    foreach (ElidedPushButton * button, _filterDefinitions.keys())
        button->setAvailableWidth(this->width() - 12);

    QWidget::resizeEvent(event);
}

void AttributeFlow::onClick(bool checked)
{
    Q_UNUSED(checked)

    // Create a soundfont filter
    ElidedPushButton * sender = (ElidedPushButton*)QObject::sender();
    if (_filterDefinitions.contains(sender))
    {
        SoundfontFilter * filter = new SoundfontFilter();
        switch (_filterDefinitions[sender]._type)
        {
        case FilterType::CATEGORY:
            filter->setCategory(_filterDefinitions[sender]._id);
            break;
        case FilterType::PROPERTY:
            filter->setProperties(_filterDefinitions[sender]._property, QStringList(_filterDefinitions[sender]._propertyValue));
            break;
        case FilterType::TAG:
            filter->setTags(QStringList(_filterDefinitions[sender]._tag));
            break;
        }

        // Notify the click with the filter
        emit(itemClicked(filter));
    }
}
