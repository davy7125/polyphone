/****************************************************************************
** Meta object code from reading C++ file 'page.h'
**
** Created: Tue 5. Feb 21:45:30 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pages/page.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Page[] = {

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

static const char qt_meta_stringdata_Page[] = {
    "Page\0"
};

void Page::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Page::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Page::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Page,
      qt_meta_data_Page, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Page::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Page::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Page::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Page))
        return static_cast<void*>(const_cast< Page*>(this));
    return QWidget::qt_metacast(_clname);
}

int Page::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_TableWidget[] = {

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
      37,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   13,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TableWidget[] = {
    "TableWidget\0\0ligne,colonne,newAction\0"
    "set(int,int,bool)\0emitSet(int,int,bool)\0"
};

void TableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TableWidget *_t = static_cast<TableWidget *>(_o);
        switch (_id) {
        case 0: _t->set((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->emitSet((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TableWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TableWidget::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_TableWidget,
      qt_meta_data_TableWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TableWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableWidget))
        return static_cast<void*>(const_cast< TableWidget*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int TableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
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
void TableWidget::set(int _t1, int _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_TableWidgetMod[] = {

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

static const char qt_meta_stringdata_TableWidgetMod[] = {
    "TableWidgetMod\0"
};

void TableWidgetMod::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TableWidgetMod::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TableWidgetMod::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_TableWidgetMod,
      qt_meta_data_TableWidgetMod, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TableWidgetMod::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TableWidgetMod::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TableWidgetMod::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableWidgetMod))
        return static_cast<void*>(const_cast< TableWidgetMod*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int TableWidgetMod::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_KeyPressCatcher[] = {

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
      37,   17,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_KeyPressCatcher[] = {
    "KeyPressCatcher\0\0ligne,row,newAction\0"
    "set(int,int,bool)\0"
};

void KeyPressCatcher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        KeyPressCatcher *_t = static_cast<KeyPressCatcher *>(_o);
        switch (_id) {
        case 0: _t->set((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData KeyPressCatcher::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject KeyPressCatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KeyPressCatcher,
      qt_meta_data_KeyPressCatcher, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &KeyPressCatcher::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *KeyPressCatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *KeyPressCatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KeyPressCatcher))
        return static_cast<void*>(const_cast< KeyPressCatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int KeyPressCatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KeyPressCatcher::set(int _t1, int _t2, bool _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_TableComboBox[] = {

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
      26,   15,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TableComboBox[] = {
    "TableComboBox\0\0row,column\0clicked(int,int)\0"
};

void TableComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TableComboBox *_t = static_cast<TableComboBox *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TableComboBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TableComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_TableComboBox,
      qt_meta_data_TableComboBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TableComboBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TableComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TableComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TableComboBox))
        return static_cast<void*>(const_cast< TableComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int TableComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
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
void TableComboBox::clicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ComboBox[] = {

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
      16,   10,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ComboBox[] = {
    "ComboBox\0\0index\0clicked(int)\0"
};

void ComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ComboBox *_t = static_cast<ComboBox *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ComboBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_ComboBox,
      qt_meta_data_ComboBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ComboBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ComboBox))
        return static_cast<void*>(const_cast< ComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int ComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
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
void ComboBox::clicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_PageTable[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      35,   11,   10,   10, 0x0a,
      67,   53,   10,   10, 0x2a,
      80,   10,   10,   10, 0x0a,
      92,   10,   10,   10, 0x0a,
     101,   10,   10,   10, 0x0a,
     112,   10,   10,   10, 0x0a,
     140,  129,   10,   10, 0x0a,
     163,  129,   10,   10, 0x0a,
     198,  192,   10,   10, 0x0a,
     211,  192,   10,   10, 0x0a,
     226,  192,   10,   10, 0x0a,
     242,   10,   10,   10, 0x0a,
     257,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PageTable[] = {
    "PageTable\0\0ligne,colonne,newAction\0"
    "set(int,int,bool)\0ligne,colonne\0"
    "set(int,int)\0setAmount()\0setAbs()\0"
    "selected()\0afficheEditMod()\0row,column\0"
    "setSourceType(int,int)\0"
    "setSourceAmountType(int,int)\0index\0"
    "setDest(int)\0setSource(int)\0setSource2(int)\0"
    "supprimerMod()\0nouveauMod()\0"
};

void PageTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PageTable *_t = static_cast<PageTable *>(_o);
        switch (_id) {
        case 0: _t->set((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->set((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->setAmount(); break;
        case 3: _t->setAbs(); break;
        case 4: _t->selected(); break;
        case 5: _t->afficheEditMod(); break;
        case 6: _t->setSourceType((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->setSourceAmountType((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->setDest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->setSource((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->setSource2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->supprimerMod(); break;
        case 12: _t->nouveauMod(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PageTable::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PageTable::staticMetaObject = {
    { &Page::staticMetaObject, qt_meta_stringdata_PageTable,
      qt_meta_data_PageTable, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PageTable::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PageTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PageTable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PageTable))
        return static_cast<void*>(const_cast< PageTable*>(this));
    return Page::qt_metacast(_clname);
}

int PageTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Page::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
