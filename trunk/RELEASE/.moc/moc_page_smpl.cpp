/****************************************************************************
** Meta object code from reading C++ file 'page_smpl.h'
**
** Created: Tue 5. Feb 21:45:24 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pages/page_smpl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_smpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Page_Smpl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      43,   10,   10,   10, 0x08,
      53,   10,   10,   10, 0x08,
      71,   10,   10,   10, 0x08,
      81,   10,   10,   10, 0x08,
     110,   90,   10,   10, 0x08,
     138,   10,   10,   10, 0x08,
     157,  153,   10,   10, 0x08,
     175,   10,   10,   10, 0x08,
     188,  153,   10,   10, 0x08,
     204,   10,   10,   10, 0x08,
     217,  153,   10,   10, 0x08,
     233,   10,   10,   10, 0x08,
     243,  153,   10,   10, 0x08,
     262,  256,   10,   10, 0x08,
     275,  256,   10,   10, 0x08,
     294,  256,   10,   10, 0x08,
     307,  153,   10,   10, 0x08,
     327,  153,   10,   10, 0x08,
     348,  153,   10,   10, 0x08,
     367,  153,   10,   10, 0x08,
     382,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Page_Smpl[] = {
    "Page_Smpl\0\0unused,vel\0noteChanged(int,int)\0"
    "lecture()\0lecteurFinished()\0applyEQ()\0"
    "initEQ()\0freq1,freq2,raideur\0"
    "sifflements(int,int,double)\0setStartLoop()\0"
    "val\0setStartLoop(int)\0setEndLoop()\0"
    "setEndLoop(int)\0setRootKey()\0"
    "setRootKey(int)\0setTune()\0setTune(int)\0"
    "index\0setType(int)\0setLinkedSmpl(int)\0"
    "setRate(int)\0setLoopEnabled(int)\0"
    "setSinusEnabled(int)\0setGainSample(int)\0"
    "setStereo(int)\0selectionChanged()\0"
};

void Page_Smpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Page_Smpl *_t = static_cast<Page_Smpl *>(_o);
        switch (_id) {
        case 0: _t->noteChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->lecture(); break;
        case 2: _t->lecteurFinished(); break;
        case 3: _t->applyEQ(); break;
        case 4: _t->initEQ(); break;
        case 5: _t->sifflements((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 6: _t->setStartLoop(); break;
        case 7: _t->setStartLoop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setEndLoop(); break;
        case 9: _t->setEndLoop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setRootKey(); break;
        case 11: _t->setRootKey((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->setTune(); break;
        case 13: _t->setTune((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->setType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->setLinkedSmpl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->setRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->setLoopEnabled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->setSinusEnabled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->setGainSample((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->setStereo((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->selectionChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Page_Smpl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Page_Smpl::staticMetaObject = {
    { &Page::staticMetaObject, qt_meta_stringdata_Page_Smpl,
      qt_meta_data_Page_Smpl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Page_Smpl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Page_Smpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Page_Smpl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Page_Smpl))
        return static_cast<void*>(const_cast< Page_Smpl*>(this));
    return Page::qt_metacast(_clname);
}

int Page_Smpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Page::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void Page_Smpl::noteChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_Graphique[] = {

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
      11,   10,   10,   10, 0x05,
      30,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   47,   10,   10, 0x0a,
      69,   65,   10,   10, 0x0a,
      87,   65,   10,   10, 0x0a,
     103,   65,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Graphique[] = {
    "Graphique\0\0startLoopChanged()\0"
    "endLoopChanged()\0posX\0setPosX(int)\0"
    "pos\0setStartLoop(int)\0setEndLoop(int)\0"
    "setCurrentSample(int)\0"
};

void Graphique::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Graphique *_t = static_cast<Graphique *>(_o);
        switch (_id) {
        case 0: _t->startLoopChanged(); break;
        case 1: _t->endLoopChanged(); break;
        case 2: _t->setPosX((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setStartLoop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setEndLoop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setCurrentSample((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Graphique::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Graphique::staticMetaObject = {
    { &QCustomPlot::staticMetaObject, qt_meta_stringdata_Graphique,
      qt_meta_data_Graphique, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Graphique::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Graphique::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Graphique::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Graphique))
        return static_cast<void*>(const_cast< Graphique*>(this));
    return QCustomPlot::qt_metacast(_clname);
}

int Graphique::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCustomPlot::qt_metacall(_c, _id, _a);
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
void Graphique::startLoopChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Graphique::endLoopChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
static const uint qt_meta_data_GraphiqueFourier[] = {

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

static const char qt_meta_stringdata_GraphiqueFourier[] = {
    "GraphiqueFourier\0"
};

void GraphiqueFourier::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GraphiqueFourier::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GraphiqueFourier::staticMetaObject = {
    { &QCustomPlot::staticMetaObject, qt_meta_stringdata_GraphiqueFourier,
      qt_meta_data_GraphiqueFourier, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GraphiqueFourier::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GraphiqueFourier::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GraphiqueFourier::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GraphiqueFourier))
        return static_cast<void*>(const_cast< GraphiqueFourier*>(this));
    return QCustomPlot::qt_metacast(_clname);
}

int GraphiqueFourier::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCustomPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
