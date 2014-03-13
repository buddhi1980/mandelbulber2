/****************************************************************************
** Meta object code from reading C++ file 'qt_interface.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/qt_interface.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qt_interface.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_InterfaceSlots[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      27,   15,   15,   15, 0x0a,
      40,   34,   15,   15, 0x0a,
      61,   34,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_InterfaceSlots[] = {
    "InterfaceSlots\0\0testSlot()\0load()\0"
    "value\0slotSliderMoved(int)\0"
    "slotDoubleSpinBoxChanged(double)\0"
};

void InterfaceSlots::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        InterfaceSlots *_t = static_cast<InterfaceSlots *>(_o);
        switch (_id) {
        case 0: _t->testSlot(); break;
        case 1: _t->load(); break;
        case 2: _t->slotSliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->slotDoubleSpinBoxChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData InterfaceSlots::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject InterfaceSlots::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_InterfaceSlots,
      qt_meta_data_InterfaceSlots, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &InterfaceSlots::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *InterfaceSlots::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *InterfaceSlots::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_InterfaceSlots))
        return static_cast<void*>(const_cast< InterfaceSlots*>(this));
    return QObject::qt_metacast(_clname);
}

int InterfaceSlots::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_RenderedImage[] = {

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

static const char qt_meta_stringdata_RenderedImage[] = {
    "RenderedImage\0"
};

void RenderedImage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData RenderedImage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RenderedImage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RenderedImage,
      qt_meta_data_RenderedImage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RenderedImage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RenderedImage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RenderedImage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RenderedImage))
        return static_cast<void*>(const_cast< RenderedImage*>(this));
    return QWidget::qt_metacast(_clname);
}

int RenderedImage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
