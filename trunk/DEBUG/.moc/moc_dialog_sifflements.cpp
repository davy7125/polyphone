/****************************************************************************
** Meta object code from reading C++ file 'dialog_sifflements.h'
**
** Created: Tue Feb 5 22:38:28 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tools/dialog_sifflements.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_sifflements.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DialogSifflements[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      39,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DialogSifflements[] = {
    "DialogSifflements\0\0freq1,freq2,raideur\0"
    "accepted(int,int,double)\0accept()\0"
};

void DialogSifflements::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DialogSifflements *_t = static_cast<DialogSifflements *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 1: _t->accept(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DialogSifflements::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DialogSifflements::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DialogSifflements,
      qt_meta_data_DialogSifflements, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DialogSifflements::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DialogSifflements::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DialogSifflements::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogSifflements))
        return static_cast<void*>(const_cast< DialogSifflements*>(this));
    return QDialog::qt_metacast(_clname);
}

int DialogSifflements::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DialogSifflements::accepted(int _t1, int _t2, double _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
