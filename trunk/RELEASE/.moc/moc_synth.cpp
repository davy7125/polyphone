/****************************************************************************
** Meta object code from reading C++ file 'synth.h'
**
** Created: Tue 5. Feb 21:45:38 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../synthetiseur/synth.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'synth.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Synth[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,    7,    6,    6, 0x05,
      34,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,    6,    6,    6, 0x0a,
      64,   57,    6,    6, 0x09,
      85,    6,    6,    6, 0x29,
     100,    7,    6,    6, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Synth[] = {
    "Synth\0\0pos\0currentPosChanged(int)\0"
    "readFinished()\0start()\0nbData\0"
    "generateData(qint64)\0generateData()\0"
    "emitCurrentPosChanged(int)\0"
};

void Synth::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Synth *_t = static_cast<Synth *>(_o);
        switch (_id) {
        case 0: _t->currentPosChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->readFinished(); break;
        case 2: _t->start(); break;
        case 3: _t->generateData((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 4: _t->generateData(); break;
        case 5: _t->emitCurrentPosChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Synth::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Synth::staticMetaObject = {
    { &CircularBuffer::staticMetaObject, qt_meta_stringdata_Synth,
      qt_meta_data_Synth, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Synth::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Synth::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Synth::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Synth))
        return static_cast<void*>(const_cast< Synth*>(this));
    return CircularBuffer::qt_metacast(_clname);
}

int Synth::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CircularBuffer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Synth::currentPosChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Synth::readFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
