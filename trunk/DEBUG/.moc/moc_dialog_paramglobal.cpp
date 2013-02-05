/****************************************************************************
** Meta object code from reading C++ file 'dialog_paramglobal.h'
**
** Created: Tue Feb 5 22:38:27 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tools/dialog_paramglobal.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_paramglobal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DialogParamGlobal[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      43,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   18,   18,   18, 0x08,
      92,   86,   18,   18, 0x08,
     121,  115,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DialogParamGlobal[] = {
    "DialogParamGlobal\0\0dValues,typeModif,param\0"
    "accepted(QVector<double>,int,int)\0"
    "accept()\0index\0indexMotifChanged(int)\0"
    "value\0raideurChanged(double)\0"
};

void DialogParamGlobal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DialogParamGlobal *_t = static_cast<DialogParamGlobal *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->accept(); break;
        case 2: _t->indexMotifChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->raideurChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DialogParamGlobal::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DialogParamGlobal::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DialogParamGlobal,
      qt_meta_data_DialogParamGlobal, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DialogParamGlobal::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DialogParamGlobal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DialogParamGlobal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogParamGlobal))
        return static_cast<void*>(const_cast< DialogParamGlobal*>(this));
    return QDialog::qt_metacast(_clname);
}

int DialogParamGlobal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DialogParamGlobal::accepted(QVector<double> _t1, int _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_GraphParamGlobal[] = {

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

static const char qt_meta_stringdata_GraphParamGlobal[] = {
    "GraphParamGlobal\0"
};

void GraphParamGlobal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GraphParamGlobal::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GraphParamGlobal::staticMetaObject = {
    { &QCustomPlot::staticMetaObject, qt_meta_stringdata_GraphParamGlobal,
      qt_meta_data_GraphParamGlobal, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GraphParamGlobal::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GraphParamGlobal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GraphParamGlobal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GraphParamGlobal))
        return static_cast<void*>(const_cast< GraphParamGlobal*>(this));
    return QCustomPlot::qt_metacast(_clname);
}

int GraphParamGlobal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCustomPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
