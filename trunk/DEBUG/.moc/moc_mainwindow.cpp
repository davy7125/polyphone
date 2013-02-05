/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Tue Feb 5 22:38:09 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      55,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   12,   11,   11, 0x05,
      68,   37,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,   11,   11,   11, 0x0a,
     127,  109,   11,   11, 0x08,
     156,   11,   11,   11, 0x08,
     166,   11,   11,   11, 0x08,
     175,   11,   11,   11, 0x08,
     192,   11,   11,   11, 0x08,
     209,   11,   11,   11, 0x08,
     226,   11,   11,   11, 0x08,
     243,   11,   11,   11, 0x08,
     260,   11,   11,   11, 0x08,
     269,   11,   11,   11, 0x08,
     311,  280,   11,   11, 0x08,
     369,   11,   11,   11, 0x08,
     384,   11,   11,   11, 0x08,
     399,   11,   11,   11, 0x08,
     418,   11,   11,   11, 0x08,
     434,   11,   11,   11, 0x08,
     445,   11,   11,   11, 0x08,
     454,   11,   11,   11, 0x08,
     463,   11,   11,   11, 0x08,
     475,   11,   11,   11, 0x08,
     488,   11,   11,   11, 0x08,
     500,   11,   11,   11, 0x08,
     511,   11,   11,   11, 0x08,
     533,   11,   11,   11, 0x08,
     562,   11,   11,   11, 0x08,
     581,   11,   11,   11, 0x08,
     600,   11,   11,   11, 0x08,
     619,   11,   11,   11, 0x08,
     642,  638,   11,   11, 0x08,
     659,   11,   11,   11, 0x08,
     666,   11,   11,   11, 0x08,
     673,   11,   11,   11, 0x08,
     687,   11,   11,   11, 0x08,
     705,   11,   11,   11, 0x08,
     719,   11,   11,   11, 0x08,
     731,   11,   11,   11, 0x08,
     747,   11,   11,   11, 0x08,
     758,   11,   11,   11, 0x08,
     770,   11,   11,   11, 0x08,
     786,   11,   11,   11, 0x08,
     799,   11,   11,   11, 0x08,
     813,   11,   11,   11, 0x08,
     827,   11,   11,   11, 0x08,
     841,   11,   11,   11, 0x08,
     859,   11,   11,   11, 0x08,
     876,   11,   11,   11, 0x08,
     886,   11,   11,   11, 0x08,
     904,   11,   11,   11, 0x08,
     913,   11,   11,   11, 0x08,
     931,  927,   11,   11, 0x08,
     943,  927,   11,   11, 0x08,
     964,  956,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0numDevice\0initAudio(int)\0"
    "type,idSf2,idElt,note,velocity\0"
    "play(int,int,int,int,int)\0supprimerElt()\0"
    "type,sf2,elt,elt2\0updateTable(int,int,int,int)\0"
    "nouveau()\0ouvrir()\0ouvrirFichier1()\0"
    "ouvrirFichier2()\0ouvrirFichier3()\0"
    "ouvrirFichier4()\0ouvrirFichier5()\0"
    "Fermer()\0renommer()\0idDest,idSrc,temps,msg,ba1,ba2\0"
    "dragAndDrop(EltID,EltID,int,int*,QByteArray*,QByteArray*)\0"
    "importerSmpl()\0exporterSmpl()\0"
    "nouvelInstrument()\0nouveauPreset()\0"
    "associer()\0copier()\0coller()\0supprimer()\0"
    "showConfig()\0showAbout()\0showHelp()\0"
    "AfficherBarreOutils()\0"
    "afficherSectionModulateurs()\0"
    "setKeyboardType0()\0setKeyboardType1()\0"
    "setKeyboardType2()\0setKeyboardType3()\0"
    "val\0setVelocity(int)\0undo()\0redo()\0"
    "sauvegarder()\0sauvegarderSous()\0"
    "enleveBlanc()\0enleveFin()\0normalisation()\0"
    "bouclage()\0filtreMur()\0reglerBalance()\0"
    "transposer()\0desaccorder()\0duplication()\0"
    "paramGlobal()\0repartitionAuto()\0"
    "spatialisation()\0mixture()\0attenuationMini()\0"
    "purger()\0sifflements()\0key\0noteOn(int)\0"
    "noteOff(int)\0key,vel\0noteChanged(int,int)\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->initAudio((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->play((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 2: _t->supprimerElt(); break;
        case 3: _t->updateTable((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 4: _t->nouveau(); break;
        case 5: _t->ouvrir(); break;
        case 6: _t->ouvrirFichier1(); break;
        case 7: _t->ouvrirFichier2(); break;
        case 8: _t->ouvrirFichier3(); break;
        case 9: _t->ouvrirFichier4(); break;
        case 10: _t->ouvrirFichier5(); break;
        case 11: _t->Fermer(); break;
        case 12: _t->renommer(); break;
        case 13: _t->dragAndDrop((*reinterpret_cast< EltID(*)>(_a[1])),(*reinterpret_cast< EltID(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QByteArray*(*)>(_a[5])),(*reinterpret_cast< QByteArray*(*)>(_a[6]))); break;
        case 14: _t->importerSmpl(); break;
        case 15: _t->exporterSmpl(); break;
        case 16: _t->nouvelInstrument(); break;
        case 17: _t->nouveauPreset(); break;
        case 18: _t->associer(); break;
        case 19: _t->copier(); break;
        case 20: _t->coller(); break;
        case 21: _t->supprimer(); break;
        case 22: _t->showConfig(); break;
        case 23: _t->showAbout(); break;
        case 24: _t->showHelp(); break;
        case 25: _t->AfficherBarreOutils(); break;
        case 26: _t->afficherSectionModulateurs(); break;
        case 27: _t->setKeyboardType0(); break;
        case 28: _t->setKeyboardType1(); break;
        case 29: _t->setKeyboardType2(); break;
        case 30: _t->setKeyboardType3(); break;
        case 31: _t->setVelocity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: _t->undo(); break;
        case 33: _t->redo(); break;
        case 34: _t->sauvegarder(); break;
        case 35: _t->sauvegarderSous(); break;
        case 36: _t->enleveBlanc(); break;
        case 37: _t->enleveFin(); break;
        case 38: _t->normalisation(); break;
        case 39: _t->bouclage(); break;
        case 40: _t->filtreMur(); break;
        case 41: _t->reglerBalance(); break;
        case 42: _t->transposer(); break;
        case 43: _t->desaccorder(); break;
        case 44: _t->duplication(); break;
        case 45: _t->paramGlobal(); break;
        case 46: _t->repartitionAuto(); break;
        case 47: _t->spatialisation(); break;
        case 48: _t->mixture(); break;
        case 49: _t->attenuationMini(); break;
        case 50: _t->purger(); break;
        case 51: _t->sifflements(); break;
        case 52: _t->noteOn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 53: _t->noteOff((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 54: _t->noteChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 55)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 55;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::initAudio(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::play(int _t1, int _t2, int _t3, int _t4, int _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
