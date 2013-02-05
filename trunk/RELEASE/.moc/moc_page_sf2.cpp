/****************************************************************************
** Meta object code from reading C++ file 'page_sf2.h'
**
** Created: Tue 5. Feb 21:45:22 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pages/page_sf2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'page_sf2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextEdit[] = {

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
      10,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TextEdit[] = {
    "TextEdit\0\0editingFinished()\0"
};

void TextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEdit *_t = static_cast<TextEdit *>(_o);
        switch (_id) {
        case 0: _t->editingFinished(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEdit::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_TextEdit,
      qt_meta_data_TextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEdit))
        return static_cast<void*>(const_cast< TextEdit*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int TextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
void TextEdit::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_Page_Sf2[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   10,    9,    9, 0x0a,
      33,    9,    9,    9, 0x0a,
      43,    9,    9,    9, 0x0a,
      58,    9,    9,    9, 0x0a,
      70,    9,    9,    9, 0x0a,
      80,    9,    9,    9, 0x0a,
      93,    9,    9,    9, 0x0a,
     110,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Page_Sf2[] = {
    "Page_Sf2\0\0checked\0set24bits(int)\0"
    "setName()\0setCopyright()\0setAuthor()\0"
    "setDate()\0setProduct()\0setCommentaire()\0"
    "setNow()\0"
};

void Page_Sf2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Page_Sf2 *_t = static_cast<Page_Sf2 *>(_o);
        switch (_id) {
        case 0: _t->set24bits((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setName(); break;
        case 2: _t->setCopyright(); break;
        case 3: _t->setAuthor(); break;
        case 4: _t->setDate(); break;
        case 5: _t->setProduct(); break;
        case 6: _t->setCommentaire(); break;
        case 7: _t->setNow(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Page_Sf2::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Page_Sf2::staticMetaObject = {
    { &Page::staticMetaObject, qt_meta_stringdata_Page_Sf2,
      qt_meta_data_Page_Sf2, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Page_Sf2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Page_Sf2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Page_Sf2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Page_Sf2))
        return static_cast<void*>(const_cast< Page_Sf2*>(this));
    return Page::qt_metacast(_clname);
}

int Page_Sf2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Page::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
