#include "treeitem.h"
#include "treemodel.h"

TreeItem::TreeItem(EltID id, TreeItem * parent) :
    _model(NULL),
    _id(id),
    _parent(parent),
    _isHidden(false)
{
    if (parent != NULL)
        _model = parent->_model;
}

void TreeItem::notifyCreation()
{
    if (_model)
        _model->elementAdded(_id);
}

void TreeItem::notifyRename()
{
    if (_model)
        _model->elementUpdated(_id);
}

void TreeItem::notifyDeletion()
{
    if (_model)
        _model->elementBeingDeleted(_id);
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
