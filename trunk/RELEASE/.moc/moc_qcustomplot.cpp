/****************************************************************************
** Meta object code from reading C++ file 'qcustomplot.h'
**
** Created: Tue 5. Feb 21:45:32 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qcustomplot/qcustomplot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qcustomplot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCPLayerable[] = {

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

static const char qt_meta_stringdata_QCPLayerable[] = {
    "QCPLayerable\0"
};

void QCPLayerable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPLayerable::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPLayerable::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QCPLayerable,
      qt_meta_data_QCPLayerable, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPLayerable::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPLayerable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPLayerable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPLayerable))
        return static_cast<void*>(const_cast< QCPLayerable*>(this));
    return QObject::qt_metacast(_clname);
}

int QCPLayerable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPAbstractPlottable[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   22,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QCPAbstractPlottable[] = {
    "QCPAbstractPlottable\0\0selected\0"
    "selectionChanged(bool)\0"
};

void QCPAbstractPlottable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCPAbstractPlottable *_t = static_cast<QCPAbstractPlottable *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCPAbstractPlottable::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPAbstractPlottable::staticMetaObject = {
    { &QCPLayerable::staticMetaObject, qt_meta_stringdata_QCPAbstractPlottable,
      qt_meta_data_QCPAbstractPlottable, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPAbstractPlottable::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPAbstractPlottable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPAbstractPlottable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPAbstractPlottable))
        return static_cast<void*>(const_cast< QCPAbstractPlottable*>(this));
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAbstractPlottable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractPlottable::selectionChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QCPGraph[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       2,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
       9, 0x0,    6,   22,
      19, 0x0,    4,   34,

 // enum data: key, value
      29, uint(QCPGraph::lsNone),
      36, uint(QCPGraph::lsLine),
      43, uint(QCPGraph::lsStepLeft),
      54, uint(QCPGraph::lsStepRight),
      66, uint(QCPGraph::lsStepCenter),
      79, uint(QCPGraph::lsImpulse),
      89, uint(QCPGraph::etNone),
      96, uint(QCPGraph::etKey),
     102, uint(QCPGraph::etValue),
     110, uint(QCPGraph::etBoth),

       0        // eod
};

static const char qt_meta_stringdata_QCPGraph[] = {
    "QCPGraph\0LineStyle\0ErrorType\0lsNone\0"
    "lsLine\0lsStepLeft\0lsStepRight\0"
    "lsStepCenter\0lsImpulse\0etNone\0etKey\0"
    "etValue\0etBoth\0"
};

void QCPGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPGraph::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPGraph::staticMetaObject = {
    { &QCPAbstractPlottable::staticMetaObject, qt_meta_stringdata_QCPGraph,
      qt_meta_data_QCPGraph, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPGraph::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPGraph))
        return static_cast<void*>(const_cast< QCPGraph*>(this));
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPCurve[] = {

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

static const char qt_meta_stringdata_QCPCurve[] = {
    "QCPCurve\0"
};

void QCPCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPCurve::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPCurve::staticMetaObject = {
    { &QCPAbstractPlottable::staticMetaObject, qt_meta_stringdata_QCPCurve,
      qt_meta_data_QCPCurve, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPCurve::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPCurve))
        return static_cast<void*>(const_cast< QCPCurve*>(this));
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPBars[] = {

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

static const char qt_meta_stringdata_QCPBars[] = {
    "QCPBars\0"
};

void QCPBars::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPBars::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPBars::staticMetaObject = {
    { &QCPAbstractPlottable::staticMetaObject, qt_meta_stringdata_QCPBars,
      qt_meta_data_QCPBars, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPBars::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPBars::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPBars::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPBars))
        return static_cast<void*>(const_cast< QCPBars*>(this));
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPBars::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPStatisticalBox[] = {

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

static const char qt_meta_stringdata_QCPStatisticalBox[] = {
    "QCPStatisticalBox\0"
};

void QCPStatisticalBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPStatisticalBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPStatisticalBox::staticMetaObject = {
    { &QCPAbstractPlottable::staticMetaObject, qt_meta_stringdata_QCPStatisticalBox,
      qt_meta_data_QCPStatisticalBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPStatisticalBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPStatisticalBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPStatisticalBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPStatisticalBox))
        return static_cast<void*>(const_cast< QCPStatisticalBox*>(this));
    return QCPAbstractPlottable::qt_metacast(_clname);
}

int QCPStatisticalBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractPlottable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPAbstractItem[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QCPAbstractItem[] = {
    "QCPAbstractItem\0\0selected\0"
    "selectionChanged(bool)\0"
};

void QCPAbstractItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCPAbstractItem *_t = static_cast<QCPAbstractItem *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCPAbstractItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPAbstractItem::staticMetaObject = {
    { &QCPLayerable::staticMetaObject, qt_meta_stringdata_QCPAbstractItem,
      qt_meta_data_QCPAbstractItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPAbstractItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPAbstractItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPAbstractItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPAbstractItem))
        return static_cast<void*>(const_cast< QCPAbstractItem*>(this));
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAbstractItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractItem::selectionChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QCPItemStraightLine[] = {

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

static const char qt_meta_stringdata_QCPItemStraightLine[] = {
    "QCPItemStraightLine\0"
};

void QCPItemStraightLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemStraightLine::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemStraightLine::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemStraightLine,
      qt_meta_data_QCPItemStraightLine, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemStraightLine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemStraightLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemStraightLine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemStraightLine))
        return static_cast<void*>(const_cast< QCPItemStraightLine*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemStraightLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemLine[] = {

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

static const char qt_meta_stringdata_QCPItemLine[] = {
    "QCPItemLine\0"
};

void QCPItemLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemLine::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemLine::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemLine,
      qt_meta_data_QCPItemLine, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemLine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemLine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemLine))
        return static_cast<void*>(const_cast< QCPItemLine*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemEllipse[] = {

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

static const char qt_meta_stringdata_QCPItemEllipse[] = {
    "QCPItemEllipse\0"
};

void QCPItemEllipse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemEllipse::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemEllipse::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemEllipse,
      qt_meta_data_QCPItemEllipse, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemEllipse::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemEllipse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemEllipse::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemEllipse))
        return static_cast<void*>(const_cast< QCPItemEllipse*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemEllipse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemRect[] = {

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

static const char qt_meta_stringdata_QCPItemRect[] = {
    "QCPItemRect\0"
};

void QCPItemRect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemRect::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemRect::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemRect,
      qt_meta_data_QCPItemRect, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemRect::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemRect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemRect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemRect))
        return static_cast<void*>(const_cast< QCPItemRect*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemRect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemPixmap[] = {

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

static const char qt_meta_stringdata_QCPItemPixmap[] = {
    "QCPItemPixmap\0"
};

void QCPItemPixmap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemPixmap::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemPixmap::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemPixmap,
      qt_meta_data_QCPItemPixmap, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemPixmap::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemPixmap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemPixmap::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemPixmap))
        return static_cast<void*>(const_cast< QCPItemPixmap*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemPixmap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemText[] = {

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

static const char qt_meta_stringdata_QCPItemText[] = {
    "QCPItemText\0"
};

void QCPItemText::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemText::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemText::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemText,
      qt_meta_data_QCPItemText, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemText::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemText::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemText::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemText))
        return static_cast<void*>(const_cast< QCPItemText*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemText::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemCurve[] = {

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

static const char qt_meta_stringdata_QCPItemCurve[] = {
    "QCPItemCurve\0"
};

void QCPItemCurve::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemCurve::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemCurve::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemCurve,
      qt_meta_data_QCPItemCurve, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemCurve::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemCurve::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemCurve::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemCurve))
        return static_cast<void*>(const_cast< QCPItemCurve*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemCurve::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemBracket[] = {

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

static const char qt_meta_stringdata_QCPItemBracket[] = {
    "QCPItemBracket\0"
};

void QCPItemBracket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemBracket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemBracket::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemBracket,
      qt_meta_data_QCPItemBracket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemBracket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemBracket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemBracket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemBracket))
        return static_cast<void*>(const_cast< QCPItemBracket*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemBracket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPItemTracer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
      14, 0x0,    5,   18,

 // enum data: key, value
      26, uint(QCPItemTracer::tsNone),
      33, uint(QCPItemTracer::tsPlus),
      40, uint(QCPItemTracer::tsCrosshair),
      52, uint(QCPItemTracer::tsCircle),
      61, uint(QCPItemTracer::tsSquare),

       0        // eod
};

static const char qt_meta_stringdata_QCPItemTracer[] = {
    "QCPItemTracer\0TracerStyle\0tsNone\0"
    "tsPlus\0tsCrosshair\0tsCircle\0tsSquare\0"
};

void QCPItemTracer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPItemTracer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPItemTracer::staticMetaObject = {
    { &QCPAbstractItem::staticMetaObject, qt_meta_stringdata_QCPItemTracer,
      qt_meta_data_QCPItemTracer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPItemTracer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPItemTracer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPItemTracer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPItemTracer))
        return static_cast<void*>(const_cast< QCPItemTracer*>(this));
    return QCPAbstractItem::qt_metacast(_clname);
}

int QCPItemTracer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPAbstractLegendItem[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      32,   23,   22,   22, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QCPAbstractLegendItem[] = {
    "QCPAbstractLegendItem\0\0selected\0"
    "selectionChanged(bool)\0"
};

void QCPAbstractLegendItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCPAbstractLegendItem *_t = static_cast<QCPAbstractLegendItem *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCPAbstractLegendItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPAbstractLegendItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QCPAbstractLegendItem,
      qt_meta_data_QCPAbstractLegendItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPAbstractLegendItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPAbstractLegendItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPAbstractLegendItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPAbstractLegendItem))
        return static_cast<void*>(const_cast< QCPAbstractLegendItem*>(this));
    return QObject::qt_metacast(_clname);
}

int QCPAbstractLegendItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPAbstractLegendItem::selectionChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QCPPlottableLegendItem[] = {

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

static const char qt_meta_stringdata_QCPPlottableLegendItem[] = {
    "QCPPlottableLegendItem\0"
};

void QCPPlottableLegendItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPPlottableLegendItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPPlottableLegendItem::staticMetaObject = {
    { &QCPAbstractLegendItem::staticMetaObject, qt_meta_stringdata_QCPPlottableLegendItem,
      qt_meta_data_QCPPlottableLegendItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPPlottableLegendItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPPlottableLegendItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPPlottableLegendItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPPlottableLegendItem))
        return static_cast<void*>(const_cast< QCPPlottableLegendItem*>(this));
    return QCPAbstractLegendItem::qt_metacast(_clname);
}

int QCPPlottableLegendItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPAbstractLegendItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPLegend[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       2,   19, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   11,   10,   10, 0x05,

 // enums: name, flags, count, data
      66, 0x0,    9,   27,
      80, 0x0,    3,   45,

 // enum data: key, value
      95, uint(QCPLegend::psManual),
     104, uint(QCPLegend::psTopLeft),
     114, uint(QCPLegend::psTop),
     120, uint(QCPLegend::psTopRight),
     131, uint(QCPLegend::psRight),
     139, uint(QCPLegend::psBottomRight),
     153, uint(QCPLegend::psBottom),
     162, uint(QCPLegend::psBottomLeft),
     175, uint(QCPLegend::psLeft),
     182, uint(QCPLegend::spNone),
     189, uint(QCPLegend::spLegendBox),
     201, uint(QCPLegend::spItems),

       0        // eod
};

static const char qt_meta_stringdata_QCPLegend[] = {
    "QCPLegend\0\0selection\0"
    "selectionChanged(QCPLegend::SelectableParts)\0"
    "PositionStyle\0SelectablePart\0psManual\0"
    "psTopLeft\0psTop\0psTopRight\0psRight\0"
    "psBottomRight\0psBottom\0psBottomLeft\0"
    "psLeft\0spNone\0spLegendBox\0spItems\0"
};

void QCPLegend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCPLegend *_t = static_cast<QCPLegend *>(_o);
        switch (_id) {
        case 0: _t->selectionChanged((*reinterpret_cast< QCPLegend::SelectableParts(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCPLegend::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPLegend::staticMetaObject = {
    { &QCPLayerable::staticMetaObject, qt_meta_stringdata_QCPLegend,
      qt_meta_data_QCPLegend, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPLegend::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPLegend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPLegend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPLegend))
        return static_cast<void*>(const_cast< QCPLegend*>(this));
    return QCPLayerable::qt_metacast(_clname);
}

int QCPLegend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QCPLegend::selectionChanged(QCPLegend::SelectableParts _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_QCPGrid[] = {

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

static const char qt_meta_stringdata_QCPGrid[] = {
    "QCPGrid\0"
};

void QCPGrid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QCPGrid::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPGrid::staticMetaObject = {
    { &QCPLayerable::staticMetaObject, qt_meta_stringdata_QCPGrid,
      qt_meta_data_QCPGrid, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPGrid::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPGrid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPGrid::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPGrid))
        return static_cast<void*>(const_cast< QCPGrid*>(this));
    return QCPLayerable::qt_metacast(_clname);
}

int QCPGrid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_QCPAxis[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
      32,   44, // properties
       4,  140, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,
      33,   24,    8,    8, 0x05,
      66,   56,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     115,  109,    8,    8, 0x0a,
     145,  134,    8,    8, 0x0a,
     194,  185,    8,    8, 0x0a,

 // properties: name, type, flags
     241,  232, 0x0009510b,
     260,  250, 0x0009510b,
     277,  270, 0x06095103,
     296,  290, 0x13095103,
     109,  305, 0x0009510b,
     319,  314, 0x01095103,
     324,  314, 0x01095103,
     332,  314, 0x01095103,
     346,  342, 0x02095103,
     360,  314, 0x01095103,
     375,  314, 0x01095103,
     388,  314, 0x01095103,
     401,  314, 0x01095103,
     407,  314, 0x01095103,
     418,  342, 0x02095103,
     445,  435, 0x0009510b,
     465,  459, 0x40095103,
     479,  270, 0x06095103,
     505,  497, 0x0a095103,
     520,  497, 0x0a095103,
     533,  270, 0x06095103,
     558,  542, 0x0009510b,
     586,  569, 0x0009510b,
     603,  342, 0x02095103,
     621,  616, 0x4d095103,
     629,  616, 0x4d095103,
     637,  616, 0x4d095103,
     648,  616, 0x4d095103,
     656,  616, 0x4d095103,
     667,  459, 0x40095103,
     677,  497, 0x0a095103,
     683,  342, 0x02095103,

 // enums: name, flags, count, data
     232, 0x0,    4,  156,
     435, 0x0,    2,  164,
     250, 0x0,    2,  168,
     696, 0x0,    4,  172,

 // enum data: key, value
     711, uint(QCPAxis::atLeft),
     718, uint(QCPAxis::atRight),
     726, uint(QCPAxis::atTop),
     732, uint(QCPAxis::atBottom),
     741, uint(QCPAxis::ltNumber),
     750, uint(QCPAxis::ltDateTime),
     761, uint(QCPAxis::stLinear),
     770, uint(QCPAxis::stLogarithmic),
     784, uint(QCPAxis::spNone),
     791, uint(QCPAxis::spAxis),
     798, uint(QCPAxis::spTickLabels),
     811, uint(QCPAxis::spAxisLabel),

       0        // eod
};

static const char qt_meta_stringdata_QCPAxis[] = {
    "QCPAxis\0\0ticksRequest()\0newRange\0"
    "rangeChanged(QCPRange)\0selection\0"
    "selectionChanged(QCPAxis::SelectableParts)\0"
    "range\0setRange(QCPRange)\0selectable\0"
    "setSelectable(QCPAxis::SelectableParts)\0"
    "selected\0setSelected(QCPAxis::SelectableParts)\0"
    "AxisType\0axisType\0ScaleType\0scaleType\0"
    "double\0scaleLogBase\0QRect\0axisRect\0"
    "QCPRange\0bool\0grid\0subGrid\0autoTicks\0"
    "int\0autoTickCount\0autoTickLabels\0"
    "autoTickStep\0autoSubTicks\0ticks\0"
    "tickLabels\0tickLabelPadding\0LabelType\0"
    "tickLabelType\0QFont\0tickLabelFont\0"
    "tickLabelRotation\0QString\0dateTimeFormat\0"
    "numberFormat\0tickStep\0QVector<double>\0"
    "tickVector\0QVector<QString>\0"
    "tickVectorLabels\0subTickCount\0QPen\0"
    "basePen\0gridPen\0subGridPen\0tickPen\0"
    "subTickPen\0labelFont\0label\0labelPadding\0"
    "SelectablePart\0atLeft\0atRight\0atTop\0"
    "atBottom\0ltNumber\0ltDateTime\0stLinear\0"
    "stLogarithmic\0spNone\0spAxis\0spTickLabels\0"
    "spAxisLabel\0"
};

void QCPAxis::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCPAxis *_t = static_cast<QCPAxis *>(_o);
        switch (_id) {
        case 0: _t->ticksRequest(); break;
        case 1: _t->rangeChanged((*reinterpret_cast< const QCPRange(*)>(_a[1]))); break;
        case 2: _t->selectionChanged((*reinterpret_cast< QCPAxis::SelectableParts(*)>(_a[1]))); break;
        case 3: _t->setRange((*reinterpret_cast< const QCPRange(*)>(_a[1]))); break;
        case 4: _t->setSelectable((*reinterpret_cast< const QCPAxis::SelectableParts(*)>(_a[1]))); break;
        case 5: _t->setSelected((*reinterpret_cast< const QCPAxis::SelectableParts(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCPAxis::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCPAxis::staticMetaObject = {
    { &QCPLayerable::staticMetaObject, qt_meta_stringdata_QCPAxis,
      qt_meta_data_QCPAxis, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCPAxis::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCPAxis::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCPAxis::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCPAxis))
        return static_cast<void*>(const_cast< QCPAxis*>(this));
    return QCPLayerable::qt_metacast(_clname);
}

int QCPAxis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCPLayerable::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< AxisType*>(_v) = axisType(); break;
        case 1: *reinterpret_cast< ScaleType*>(_v) = scaleType(); break;
        case 2: *reinterpret_cast< double*>(_v) = scaleLogBase(); break;
        case 3: *reinterpret_cast< QRect*>(_v) = axisRect(); break;
        case 4: *reinterpret_cast< QCPRange*>(_v) = range(); break;
        case 5: *reinterpret_cast< bool*>(_v) = grid(); break;
        case 6: *reinterpret_cast< bool*>(_v) = subGrid(); break;
        case 7: *reinterpret_cast< bool*>(_v) = autoTicks(); break;
        case 8: *reinterpret_cast< int*>(_v) = autoTickCount(); break;
        case 9: *reinterpret_cast< bool*>(_v) = autoTickLabels(); break;
        case 10: *reinterpret_cast< bool*>(_v) = autoTickStep(); break;
        case 11: *reinterpret_cast< bool*>(_v) = autoSubTicks(); break;
        case 12: *reinterpret_cast< bool*>(_v) = ticks(); break;
        case 13: *reinterpret_cast< bool*>(_v) = tickLabels(); break;
        case 14: *reinterpret_cast< int*>(_v) = tickLabelPadding(); break;
        case 15: *reinterpret_cast< LabelType*>(_v) = tickLabelType(); break;
        case 16: *reinterpret_cast< QFont*>(_v) = tickLabelFont(); break;
        case 17: *reinterpret_cast< double*>(_v) = tickLabelRotation(); break;
        case 18: *reinterpret_cast< QString*>(_v) = dateTimeFormat(); break;
        case 19: *reinterpret_cast< QString*>(_v) = numberFormat(); break;
        case 20: *reinterpret_cast< double*>(_v) = tickStep(); break;
        case 21: *reinterpret_cast< QVector<double>*>(_v) = tickVector(); break;
        case 22: *reinterpret_cast< QVector<QString>*>(_v) = tickVectorLabels(); break;
        case 23: *reinterpret_cast< int*>(_v) = subTickCount(); break;
        case 24: *reinterpret_cast< QPen*>(_v) = basePen(); break;
        case 25: *reinterpret_cast< QPen*>(_v) = gridPen(); break;
        case 26: *reinterpret_cast< QPen*>(_v) = subGridPen(); break;
        case 27: *reinterpret_cast< QPen*>(_v) = tickPen(); break;
        case 28: *reinterpret_cast< QPen*>(_v) = subTickPen(); break;
        case 29: *reinterpret_cast< QFont*>(_v) = labelFont(); break;
        case 30: *reinterpret_cast< QString*>(_v) = label(); break;
        case 31: *reinterpret_cast< int*>(_v) = labelPadding(); break;
        }
        _id -= 32;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAxisType(*reinterpret_cast< AxisType*>(_v)); break;
        case 1: setScaleType(*reinterpret_cast< ScaleType*>(_v)); break;
        case 2: setScaleLogBase(*reinterpret_cast< double*>(_v)); break;
        case 3: setAxisRect(*reinterpret_cast< QRect*>(_v)); break;
        case 4: setRange(*reinterpret_cast< QCPRange*>(_v)); break;
        case 5: setGrid(*reinterpret_cast< bool*>(_v)); break;
        case 6: setSubGrid(*reinterpret_cast< bool*>(_v)); break;
        case 7: setAutoTicks(*reinterpret_cast< bool*>(_v)); break;
        case 8: setAutoTickCount(*reinterpret_cast< int*>(_v)); break;
        case 9: setAutoTickLabels(*reinterpret_cast< bool*>(_v)); break;
        case 10: setAutoTickStep(*reinterpret_cast< bool*>(_v)); break;
        case 11: setAutoSubTicks(*reinterpret_cast< bool*>(_v)); break;
        case 12: setTicks(*reinterpret_cast< bool*>(_v)); break;
        case 13: setTickLabels(*reinterpret_cast< bool*>(_v)); break;
        case 14: setTickLabelPadding(*reinterpret_cast< int*>(_v)); break;
        case 15: setTickLabelType(*reinterpret_cast< LabelType*>(_v)); break;
        case 16: setTickLabelFont(*reinterpret_cast< QFont*>(_v)); break;
        case 17: setTickLabelRotation(*reinterpret_cast< double*>(_v)); break;
        case 18: setDateTimeFormat(*reinterpret_cast< QString*>(_v)); break;
        case 19: setNumberFormat(*reinterpret_cast< QString*>(_v)); break;
        case 20: setTickStep(*reinterpret_cast< double*>(_v)); break;
        case 21: setTickVector(*reinterpret_cast< QVector<double>*>(_v)); break;
        case 22: setTickVectorLabels(*reinterpret_cast< QVector<QString>*>(_v)); break;
        case 23: setSubTickCount(*reinterpret_cast< int*>(_v)); break;
        case 24: setBasePen(*reinterpret_cast< QPen*>(_v)); break;
        case 25: setGridPen(*reinterpret_cast< QPen*>(_v)); break;
        case 26: setSubGridPen(*reinterpret_cast< QPen*>(_v)); break;
        case 27: setTickPen(*reinterpret_cast< QPen*>(_v)); break;
        case 28: setSubTickPen(*reinterpret_cast< QPen*>(_v)); break;
        case 29: setLabelFont(*reinterpret_cast< QFont*>(_v)); break;
        case 30: setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 31: setLabelPadding(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 32;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 32;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 32;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 32;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 32;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 32;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 32;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QCPAxis::ticksRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QCPAxis::rangeChanged(const QCPRange & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QCPAxis::selectionChanged(QCPAxis::SelectableParts _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
static const uint qt_meta_data_QCustomPlot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
      10,  119, // properties
       2,  149, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   13,   12,   12, 0x05,
      50,   13,   12,   12, 0x05,
      75,   13,   12,   12, 0x05,
      99,   13,   12,   12, 0x05,
     126,   13,   12,   12, 0x05,
     167,  151,   12,   12, 0x05,
     218,  151,   12,   12, 0x05,
     286,  275,   12,   12, 0x05,
     327,  275,   12,   12, 0x05,
     390,  374,   12,   12, 0x05,
     447,  374,   12,   12, 0x05,
     528,  510,   12,   12, 0x05,
     588,  510,   12,   12, 0x05,
     654,   13,   12,   12, 0x05,
     679,   13,   12,   12, 0x05,
     710,   12,   12,   12, 0x05,
     735,   12,   12,   12, 0x05,
     750,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     764,   12,   12,   12, 0x0a,
     778,   12,   12,   12, 0x0a,
     787,   12,   12,   12, 0x0a,

 // properties: name, type, flags
     809,  801, 0x0a095103,
     821,  815, 0x13095103,
     834,  830, 0x02095103,
     845,  830, 0x02095103,
     857,  830, 0x02095103,
     867,  830, 0x02095103,
     880,  830, 0x02095103,
     898,  891, 0x43095103,
     921,  904, 0x0009510b,
     931,  904, 0x0009510b,

 // enums: name, flags, count, data
     941, 0x0,    8,  157,
     953, 0x0,    2,  173,

 // enum data: key, value
     969, uint(QCustomPlot::iRangeDrag),
     980, uint(QCustomPlot::iRangeZoom),
     991, uint(QCustomPlot::iMultiSelect),
    1004, uint(QCustomPlot::iSelectTitle),
    1017, uint(QCustomPlot::iSelectPlottables),
    1035, uint(QCustomPlot::iSelectAxes),
    1047, uint(QCustomPlot::iSelectLegend),
    1061, uint(QCustomPlot::iSelectItems),
    1074, uint(QCustomPlot::limBelow),
    1083, uint(QCustomPlot::limAbove),

       0        // eod
};

static const char qt_meta_stringdata_QCustomPlot[] = {
    "QCustomPlot\0\0event\0mouseDoubleClick(QMouseEvent*)\0"
    "mousePress(QMouseEvent*)\0"
    "mouseMove(QMouseEvent*)\0"
    "mouseRelease(QMouseEvent*)\0"
    "mouseWheel(QWheelEvent*)\0plottable,event\0"
    "plottableClick(QCPAbstractPlottable*,QMouseEvent*)\0"
    "plottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*)\0"
    "item,event\0itemClick(QCPAbstractItem*,QMouseEvent*)\0"
    "itemDoubleClick(QCPAbstractItem*,QMouseEvent*)\0"
    "axis,part,event\0"
    "axisClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)\0"
    "axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)\0"
    "legend,item,event\0"
    "legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)\0"
    "legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)\0"
    "titleClick(QMouseEvent*)\0"
    "titleDoubleClick(QMouseEvent*)\0"
    "selectionChangedByUser()\0beforeReplot()\0"
    "afterReplot()\0deselectAll()\0replot()\0"
    "rescaleAxes()\0QString\0title\0QRect\0"
    "axisRect\0int\0marginLeft\0marginRight\0"
    "marginTop\0marginBottom\0autoMargin\0"
    "QColor\0color\0Qt::Orientations\0rangeDrag\0"
    "rangeZoom\0Interaction\0LayerInsertMode\0"
    "iRangeDrag\0iRangeZoom\0iMultiSelect\0"
    "iSelectTitle\0iSelectPlottables\0"
    "iSelectAxes\0iSelectLegend\0iSelectItems\0"
    "limBelow\0limAbove\0"
};

void QCustomPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCustomPlot *_t = static_cast<QCustomPlot *>(_o);
        switch (_id) {
        case 0: _t->mouseDoubleClick((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 1: _t->mousePress((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 2: _t->mouseMove((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: _t->mouseRelease((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: _t->mouseWheel((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 5: _t->plottableClick((*reinterpret_cast< QCPAbstractPlottable*(*)>(_a[1])),(*reinterpret_cast< QMouseEvent*(*)>(_a[2]))); break;
        case 6: _t->plottableDoubleClick((*reinterpret_cast< QCPAbstractPlottable*(*)>(_a[1])),(*reinterpret_cast< QMouseEvent*(*)>(_a[2]))); break;
        case 7: _t->itemClick((*reinterpret_cast< QCPAbstractItem*(*)>(_a[1])),(*reinterpret_cast< QMouseEvent*(*)>(_a[2]))); break;
        case 8: _t->itemDoubleClick((*reinterpret_cast< QCPAbstractItem*(*)>(_a[1])),(*reinterpret_cast< QMouseEvent*(*)>(_a[2]))); break;
        case 9: _t->axisClick((*reinterpret_cast< QCPAxis*(*)>(_a[1])),(*reinterpret_cast< QCPAxis::SelectablePart(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 10: _t->axisDoubleClick((*reinterpret_cast< QCPAxis*(*)>(_a[1])),(*reinterpret_cast< QCPAxis::SelectablePart(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 11: _t->legendClick((*reinterpret_cast< QCPLegend*(*)>(_a[1])),(*reinterpret_cast< QCPAbstractLegendItem*(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 12: _t->legendDoubleClick((*reinterpret_cast< QCPLegend*(*)>(_a[1])),(*reinterpret_cast< QCPAbstractLegendItem*(*)>(_a[2])),(*reinterpret_cast< QMouseEvent*(*)>(_a[3]))); break;
        case 13: _t->titleClick((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 14: _t->titleDoubleClick((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 15: _t->selectionChangedByUser(); break;
        case 16: _t->beforeReplot(); break;
        case 17: _t->afterReplot(); break;
        case 18: _t->deselectAll(); break;
        case 19: _t->replot(); break;
        case 20: _t->rescaleAxes(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCustomPlot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCustomPlot::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QCustomPlot,
      qt_meta_data_QCustomPlot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCustomPlot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCustomPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCustomPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCustomPlot))
        return static_cast<void*>(const_cast< QCustomPlot*>(this));
    return QWidget::qt_metacast(_clname);
}

int QCustomPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = title(); break;
        case 1: *reinterpret_cast< QRect*>(_v) = axisRect(); break;
        case 2: *reinterpret_cast< int*>(_v) = marginLeft(); break;
        case 3: *reinterpret_cast< int*>(_v) = marginRight(); break;
        case 4: *reinterpret_cast< int*>(_v) = marginTop(); break;
        case 5: *reinterpret_cast< int*>(_v) = marginBottom(); break;
        case 6: *reinterpret_cast< int*>(_v) = autoMargin(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = color(); break;
        case 8: *reinterpret_cast< Qt::Orientations*>(_v) = rangeDrag(); break;
        case 9: *reinterpret_cast< Qt::Orientations*>(_v) = rangeZoom(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 1: setAxisRect(*reinterpret_cast< QRect*>(_v)); break;
        case 2: setMarginLeft(*reinterpret_cast< int*>(_v)); break;
        case 3: setMarginRight(*reinterpret_cast< int*>(_v)); break;
        case 4: setMarginTop(*reinterpret_cast< int*>(_v)); break;
        case 5: setMarginBottom(*reinterpret_cast< int*>(_v)); break;
        case 6: setAutoMargin(*reinterpret_cast< int*>(_v)); break;
        case 7: setColor(*reinterpret_cast< QColor*>(_v)); break;
        case 8: setRangeDrag(*reinterpret_cast< Qt::Orientations*>(_v)); break;
        case 9: setRangeZoom(*reinterpret_cast< Qt::Orientations*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QCustomPlot::mouseDoubleClick(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCustomPlot::mousePress(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QCustomPlot::mouseMove(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QCustomPlot::mouseRelease(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QCustomPlot::mouseWheel(QWheelEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QCustomPlot::plottableClick(QCPAbstractPlottable * _t1, QMouseEvent * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QCustomPlot::plottableDoubleClick(QCPAbstractPlottable * _t1, QMouseEvent * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QCustomPlot::itemClick(QCPAbstractItem * _t1, QMouseEvent * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QCustomPlot::itemDoubleClick(QCPAbstractItem * _t1, QMouseEvent * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QCustomPlot::axisClick(QCPAxis * _t1, QCPAxis::SelectablePart _t2, QMouseEvent * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QCustomPlot::axisDoubleClick(QCPAxis * _t1, QCPAxis::SelectablePart _t2, QMouseEvent * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QCustomPlot::legendClick(QCPLegend * _t1, QCPAbstractLegendItem * _t2, QMouseEvent * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QCustomPlot::legendDoubleClick(QCPLegend * _t1, QCPAbstractLegendItem * _t2, QMouseEvent * _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QCustomPlot::titleClick(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void QCustomPlot::titleDoubleClick(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void QCustomPlot::selectionChangedByUser()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void QCustomPlot::beforeReplot()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void QCustomPlot::afterReplot()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}
QT_END_MOC_NAMESPACE
