/****************************************************************************
** Meta object code from reading C++ file 'dialog_mixture.h'
**
** Created: Tue 5. Feb 21:45:35 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tools/dialog_mixture.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog_mixture.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DialogMixture[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      44,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      90,   14,   14,   14, 0x08,
     110,   99,   14,   14, 0x08,
     146,   99,   14,   14, 0x08,
     186,   14,   14,   14, 0x08,
     210,   14,   14,   14, 0x08,
     242,  237,   14,   14, 0x08,
     276,  237,   14,   14, 0x08,
     308,   14,   14,   14, 0x08,
     333,   14,   14,   14, 0x08,
     367,  361,   14,   14, 0x08,
     406,  361,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DialogMixture[] = {
    "DialogMixture\0\0listeParam,nom,bouclage,freq\0"
    "accepted(QList<QList<int> >,QString,bool,int)\0"
    "accept()\0currentRow\0"
    "on_listRangs_currentRowChanged(int)\0"
    "on_listDivisions_currentRowChanged(int)\0"
    "on_pushAddDiv_pressed()\0"
    "on_pushRemoveDiv_pressed()\0arg1\0"
    "on_spinDivStart_valueChanged(int)\0"
    "on_spinDivEnd_valueChanged(int)\0"
    "on_pushAddRank_clicked()\0"
    "on_pushRemoveRank_clicked()\0index\0"
    "on_comboType1_currentIndexChanged(int)\0"
    "on_comboType2_currentIndexChanged(int)\0"
};

void DialogMixture::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DialogMixture *_t = static_cast<DialogMixture *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< QList<QList<int> >(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->accept(); break;
        case 2: _t->on_listRangs_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_listDivisions_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_pushAddDiv_pressed(); break;
        case 5: _t->on_pushRemoveDiv_pressed(); break;
        case 6: _t->on_spinDivStart_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_spinDivEnd_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_pushAddRank_clicked(); break;
        case 9: _t->on_pushRemoveRank_clicked(); break;
        case 10: _t->on_comboType1_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->on_comboType2_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DialogMixture::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DialogMixture::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DialogMixture,
      qt_meta_data_DialogMixture, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DialogMixture::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DialogMixture::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DialogMixture::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DialogMixture))
        return static_cast<void*>(const_cast< DialogMixture*>(this));
    return QDialog::qt_metacast(_clname);
}

int DialogMixture::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DialogMixture::accepted(QList<QList<int> > _t1, QString _t2, bool _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
