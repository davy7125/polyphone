/****************************************************************************
** Meta object code from reading C++ file 'page_inst.h'
**
** Created: Tue 5. Feb 21:45:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pages/page_inst.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_inst.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Page_Inst[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      62,   11,   10,   10, 0x08,
     130,  106,   10,   10, 0x08,
     200,  167,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Page_Inst[] = {
    "Page_Inst\0\0"
    "motif,nbDiv,etalement,occupation,offset,sens,sens2\0"
    "spatialisation(int,int,int,int,int,int,int)\0"
    "dValues,typeModif,param\0"
    "paramGlobal(QVector<double>,int,int)\0"
    "listeParam,nomInst,bouclage,freq\0"
    "mixture(QList<QList<int> >,QString,bool,int)\0"
};

void Page_Inst::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Page_Inst *_t = static_cast<Page_Inst *>(_o);
        switch (_id) {
        case 0: _t->spatialisation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: _t->paramGlobal((*reinterpret_cast< QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->mixture((*reinterpret_cast< QList<QList<int> >(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Page_Inst::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Page_Inst::staticMetaObject = {
    { &PageTable::staticMetaObject, qt_meta_stringdata_Page_Inst,
      qt_meta_data_Page_Inst, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Page_Inst::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Page_Inst::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Page_Inst::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Page_Inst))
        return static_cast<void*>(const_cast< Page_Inst*>(this));
    return PageTable::qt_metacast(_clname);
}

int Page_Inst::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PageTable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_TableWidgetInst[] = {

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

static const char qt_meta_stringdata_TableWidgetInst[] = {
    "TableWidgetInst\0"
};

void TableWidgetInst::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TableWidgetInst::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TableWidgetInst::staticMetaObject = {
    { &TableWidget::staticMetaObject, qt_meta_stringdata_TableWidgetInst,
      qt_meta_data_TableWidgetInst, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TableWidgetInst::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TableWidgetInst::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TableWidgetInst::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableWidgetInst))
        return static_cast<void*>(const_cast< TableWidgetInst*>(this));
    return TableWidget::qt_metacast(_clname);
}

int TableWidgetInst::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
