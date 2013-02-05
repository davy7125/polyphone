/****************************************************************************
** Meta object code from reading C++ file 'page_prst.h'
**
** Created: Tue Feb 5 22:38:22 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pages/page_prst.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_prst.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Page_Prst[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      21,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Page_Prst[] = {
    "Page_Prst\0\0setBank()\0setPreset()\0"
};

void Page_Prst::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Page_Prst *_t = static_cast<Page_Prst *>(_o);
        switch (_id) {
        case 0: _t->setBank(); break;
        case 1: _t->setPreset(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Page_Prst::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Page_Prst::staticMetaObject = {
    { &PageTable::staticMetaObject, qt_meta_stringdata_Page_Prst,
      qt_meta_data_Page_Prst, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Page_Prst::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Page_Prst::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Page_Prst::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Page_Prst))
        return static_cast<void*>(const_cast< Page_Prst*>(this));
    return PageTable::qt_metacast(_clname);
}

int Page_Prst::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PageTable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_TableWidgetPrst[] = {

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

static const char qt_meta_stringdata_TableWidgetPrst[] = {
    "TableWidgetPrst\0"
};

void TableWidgetPrst::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TableWidgetPrst::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TableWidgetPrst::staticMetaObject = {
    { &TableWidget::staticMetaObject, qt_meta_stringdata_TableWidgetPrst,
      qt_meta_data_TableWidgetPrst, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TableWidgetPrst::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TableWidgetPrst::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TableWidgetPrst::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableWidgetPrst))
        return static_cast<void*>(const_cast< TableWidgetPrst*>(this));
    return TableWidget::qt_metacast(_clname);
}

int TableWidgetPrst::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_SpinBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpinBox[] = {
    "SpinBox\0\0steps\0stepBy(int)\0"
};

void SpinBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpinBox *_t = static_cast<SpinBox *>(_o);
        switch (_id) {
        case 0: _t->stepBy((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpinBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpinBox::staticMetaObject = {
    { &QSpinBox::staticMetaObject, qt_meta_stringdata_SpinBox,
      qt_meta_data_SpinBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpinBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpinBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpinBox))
        return static_cast<void*>(const_cast< SpinBox*>(this));
    return QSpinBox::qt_metacast(_clname);
}

int SpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
