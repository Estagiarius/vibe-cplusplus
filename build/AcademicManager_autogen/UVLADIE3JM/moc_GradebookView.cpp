/****************************************************************************
** Meta object code from reading C++ file 'GradebookView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/GradebookView.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GradebookView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_GradebookView_t {
    uint offsetsAndSizes[24];
    char stringdata0[14];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[14];
    char stringdata5[4];
    char stringdata6[7];
    char stringdata7[16];
    char stringdata8[14];
    char stringdata9[5];
    char stringdata10[17];
    char stringdata11[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GradebookView_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_GradebookView_t qt_meta_stringdata_GradebookView = {
    {
        QT_MOC_LITERAL(0, 13),  // "GradebookView"
        QT_MOC_LITERAL(14, 14),  // "onClassChanged"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 5),  // "index"
        QT_MOC_LITERAL(36, 13),  // "onCellChanged"
        QT_MOC_LITERAL(50, 3),  // "row"
        QT_MOC_LITERAL(54, 6),  // "column"
        QT_MOC_LITERAL(61, 15),  // "onEnrollStudent"
        QT_MOC_LITERAL(77, 13),  // "onDateChanged"
        QT_MOC_LITERAL(91, 4),  // "date"
        QT_MOC_LITERAL(96, 16),  // "onSaveAttendance"
        QT_MOC_LITERAL(113, 14)   // "onExportGrades"
    },
    "GradebookView",
    "onClassChanged",
    "",
    "index",
    "onCellChanged",
    "row",
    "column",
    "onEnrollStudent",
    "onDateChanged",
    "date",
    "onSaveAttendance",
    "onExportGrades"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GradebookView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x08,    1 /* Private */,
       4,    2,   53,    2, 0x08,    3 /* Private */,
       7,    0,   58,    2, 0x08,    6 /* Private */,
       8,    1,   59,    2, 0x08,    7 /* Private */,
      10,    0,   62,    2, 0x08,    9 /* Private */,
      11,    0,   63,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QDate,    9,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GradebookView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_GradebookView.offsetsAndSizes,
    qt_meta_data_GradebookView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GradebookView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GradebookView, std::true_type>,
        // method 'onClassChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onCellChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onEnrollStudent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QDate &, std::false_type>,
        // method 'onSaveAttendance'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onExportGrades'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GradebookView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GradebookView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onClassChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->onCellChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->onEnrollStudent(); break;
        case 3: _t->onDateChanged((*reinterpret_cast< std::add_pointer_t<QDate>>(_a[1]))); break;
        case 4: _t->onSaveAttendance(); break;
        case 5: _t->onExportGrades(); break;
        default: ;
        }
    }
}

const QMetaObject *GradebookView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GradebookView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GradebookView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GradebookView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
