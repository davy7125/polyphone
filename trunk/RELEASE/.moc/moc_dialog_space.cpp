/****************************************************************************
** Meta object code from reading C++ file 'dialog_space.h'
**
** Created: Tue 5. Feb 21:45:33 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tools/dialog_space.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_space.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DialogSpace[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      64,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     102,   12,   12,   12, 0x08,
     117,  111,   12,   12, 0x08,
     135,  111,   12,   12, 0x08,
     153,  111,   12,   12, 0x08,
     175,  111,   12,   12, 0x08,
     198,  111,   12,   12, 0x08,
     217,  111,   12,   12, 0x08,
     234,  111,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DialogSpace[] = {
    "DialogSpace\0\0"
    "motif,nbDiv,etalement,occupation,offset,sens,sens2\0"
    "accepted(int,int,int,int,int,int,int)\0"
    "accept()\0value\0motifChanged(int)\0"
    "nbDivChanged(int)\0etalementChanged(int)\0"
    "occupationChanged(int)\0offsetChanged(int)\0"
    "sensChanged(int)\0sens2Changed(int)\0"
};

void DialogSpace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DialogSpace *_t = static_cast<DialogSpace *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: _t->accept(); break;
        case 2: _t->motifChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->nbDivChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->etalementChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->occupationChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->offsetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->sensChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->sens2Changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DialogSpace::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DialogSpace::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DialogSpace,
      qt_meta_data_DialogSpace, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DialogSpace::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DialogSpace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DialogSpace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogSpace))
        return static_cast<void*>(const_cast< DialogSpace*>(this));
    return QDialog::qt_metacast(_clname);
}

int DialogSpace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DialogSpace::accepted(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_GraphSpace[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_GraphSpace[] = {
    "GraphSpace\0"
};

void GraphSpace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GraphSpace::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GraphSpace::staticMetaObject = {
    { &QCustomPlot::staticMetaObject, qt_meta_stringdata_GraphSpace,
      qt_meta_data_GraphSpace, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GraphSpace::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GraphSpace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GraphSpace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GraphSpace))
        return static_cast<void*>(const_cast< GraphSpace*>(this));
    return QCustomPlot::qt_metacast(_clname);
}

int GraphSpace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCustomPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
