/****************************************************************************
** Meta object code from reading C++ file 'tree.h'
**
** Created: Tue Feb 5 22:38:11 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../sf2_core/tree.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tree.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tree[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,    6,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,    5,    5,    5, 0x0a,
     103,   98,    5,    5, 0x0a,
     123,    5,    5,    5, 0x0a,
     134,    5,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Tree[] = {
    "Tree\0\0dest,src,temps,msg,ba1,ba2\0"
    "dropped(EltID,EltID,int,int*,QByteArray*,QByteArray*)\0"
    "collapse()\0qStr\0searchTree(QString)\0"
    "clicTree()\0clicTreeRight()\0"
};

void Tree::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Tree *_t = static_cast<Tree *>(_o);
        switch (_id) {
        case 0: _t->dropped((*reinterpret_cast< EltID(*)>(_a[1])),(*reinterpret_cast< EltID(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QByteArray*(*)>(_a[5])),(*reinterpret_cast< QByteArray*(*)>(_a[6]))); break;
        case 1: _t->collapse(); break;
        case 2: _t->searchTree((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->clicTree(); break;
        case 4: _t->clicTreeRight(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Tree::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Tree::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_Tree,
      qt_meta_data_Tree, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tree::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tree::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tree::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tree))
        return static_cast<void*>(const_cast< Tree*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int Tree::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Tree::dropped(EltID _t1, EltID _t2, int _t3, int * _t4, QByteArray * _t5, QByteArray * _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
