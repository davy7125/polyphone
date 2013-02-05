/****************************************************************************
** Meta object code from reading C++ file 'config.h'
**
** Created: Tue Feb 5 22:38:14 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gui_divers/config.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'config.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Config[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,    8,    7,    7, 0x0a,
      24,    8,    7,    7, 0x0a,
      43,    8,    7,    7, 0x0a,
      72,   66,    7,    7, 0x0a,
     100,   92,    7,    7, 0x0a,
     121,   92,    7,    7, 0x0a,
     145,    8,    7,    7, 0x0a,
     166,    8,    7,    7, 0x0a,
     191,    8,    7,    7, 0x0a,
     211,    8,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Config[] = {
    "Config\0\0val\0setRam(int)\0setAfficheMod(int)\0"
    "setAfficheToolBar(int)\0index\0"
    "setAudioOutput(int)\0checked\0"
    "setWavAutoLoop(bool)\0setWavRemoveBlank(bool)\0"
    "setKeyboardType(int)\0setKeyboardVelocity(int)\0"
    "setNumPortMidi(int)\0setSynthGain(int)\0"
};

void Config::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Config *_t = static_cast<Config *>(_o);
        switch (_id) {
        case 0: _t->setRam((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setAfficheMod((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setAfficheToolBar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setAudioOutput((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setWavAutoLoop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->setWavRemoveBlank((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->setKeyboardType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->setKeyboardVelocity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setNumPortMidi((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->setSynthGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Config::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Config::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Config,
      qt_meta_data_Config, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Config::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Config::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Config::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Config))
        return static_cast<void*>(const_cast< Config*>(this));
    return QDialog::qt_metacast(_clname);
}

int Config::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
