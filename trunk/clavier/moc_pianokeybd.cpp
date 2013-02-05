/****************************************************************************
** Meta object code from reading C++ file 'pianokeybd.h'
**
** Created: Sun 3. Feb 20:31:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pianokeybd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pianokeybd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PianoKeybd[] = {

 // content:
       6,       // revision
       0,       // classname
       3,   14, // classinfo
       6,   20, // methods
       7,   50, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // classinfo: key, value
      54,   11,
      98,   61,
     106,  102,

 // signals: signature, parameters, type, tag, flags
     124,  115,  114,  114, 0x05,
     136,  115,  114,  114, 0x05,

 // slots: signature, parameters, type, tag, flags
     162,  149,  114,  114, 0x0a,
     182,  115,  114,  114, 0x2a,
     198,  149,  114,  114, 0x0a,
     219,  115,  114,  114, 0x2a,

 // properties: name, type, flags
     240,  236, 0x02095103,
     251,  236, 0x02095103,
     262,  236, 0x02095103,
     278,  271, 0x43095103,
     299,  294, 0x01095103,
     310,  294, 0x01095103,
     319,  236, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_PianoKeybd[] = {
    "PianoKeybd\0Pedro Lopez-Cabanillas <plcl@users.sf.net>\0"
    "Author\0http://sourceforge.net/projects/vmpk\0"
    "URL\0""0.9\0Version\0\0midiNote\0noteOn(int)\0"
    "noteOff(int)\0midiNote,vel\0showNoteOn(int,int)\0"
    "showNoteOn(int)\0showNoteOff(int,int)\0"
    "showNoteOff(int)\0int\0baseOctave\0"
    "numOctaves\0rotation\0QColor\0keyPressedColor\0"
    "bool\0showLabels\0useFlats\0transpose\0"
};

void PianoKeybd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PianoKeybd *_t = static_cast<PianoKeybd *>(_o);
        switch (_id) {
        case 0: _t->noteOn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->noteOff((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->showNoteOn((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->showNoteOn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->showNoteOff((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->showNoteOff((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PianoKeybd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PianoKeybd::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_PianoKeybd,
      qt_meta_data_PianoKeybd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PianoKeybd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PianoKeybd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PianoKeybd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PianoKeybd))
        return static_cast<void*>(const_cast< PianoKeybd*>(this));
    if (!strcmp(_clname, "RawKbdHandler"))
        return static_cast< RawKbdHandler*>(const_cast< PianoKeybd*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int PianoKeybd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = baseOctave(); break;
        case 1: *reinterpret_cast< int*>(_v) = numOctaves(); break;
        case 2: *reinterpret_cast< int*>(_v) = getRotation(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getKeyPressedColor(); break;
        case 4: *reinterpret_cast< bool*>(_v) = showLabels(); break;
        case 5: *reinterpret_cast< bool*>(_v) = useFlats(); break;
        case 6: *reinterpret_cast< int*>(_v) = getTranspose(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBaseOctave(*reinterpret_cast< int*>(_v)); break;
        case 1: setNumOctaves(*reinterpret_cast< int*>(_v)); break;
        case 2: setRotation(*reinterpret_cast< int*>(_v)); break;
        case 3: setKeyPressedColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setShowLabels(*reinterpret_cast< bool*>(_v)); break;
        case 5: setUseFlats(*reinterpret_cast< bool*>(_v)); break;
        case 6: setTranspose(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PianoKeybd::noteOn(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PianoKeybd::noteOff(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
