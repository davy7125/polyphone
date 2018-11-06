#include "treeitem.h"
#include "treemodel.h"

TreeItem::TreeItem(EltID id, TreeItem * parent) :
    _model(nullptr),
    _id(id),
    _parent(parent),
    _isHidden(false)
{
    if (parent != nullptr)
        _model = parent->_model;
    if (_model != nullptr)
        _model->elementBeingAdded(_id);
}

void TreeItem::notifyCreated()
{
    if (_model)
        _model->endOfAddition();
}

void TreeItem::notifyRename()
{
    if (_model)
        _model->elementUpdated(_id);
}

void TreeItem::notifyDeletion(bool storeExpandedState)
{
    if (_model)
        _model->elementBeingDeleted(_id, storeExpandedState);
}

TreeItem::~TreeItem()
{
    if (_model)
        _model->endOfDeletion();
}

void TreeItem::setHidden(bool isHidden)
{
    _isHidden = isHidden;
    if (_model)
        _model->visibilityChanged(_id);
}
