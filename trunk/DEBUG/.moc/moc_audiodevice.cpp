/****************************************************************************
** Meta object code from reading C++ file 'audiodevice.h'
**
** Created: Tue Feb 5 22:38:34 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../synthetiseur/audiodevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audiodevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AudioDevice[] = {

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
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   21,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AudioDevice[] = {
    "AudioDevice\0\0start()\0numDevice\0"
    "initAudio(int)\0"
};

void AudioDevice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AudioDevice *_t = static_cast<AudioDevice *>(_o);
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->initAudio((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AudioDevice::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AudioDevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AudioDevice,
      qt_meta_data_AudioDevice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AudioDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AudioDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AudioDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AudioDevice))
        return static_cast<void*>(const_cast< AudioDevice*>(this));
    return QObject::qt_metacast(_clname);
}

int AudioDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void AudioDevice::start()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
