/****************************************************************************
** Meta object code from reading C++ file 'sniffdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../NetworkHandler/sniffdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sniffdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SniffDialog_t {
    QByteArrayData data[19];
    char stringdata0[278];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SniffDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SniffDialog_t qt_meta_stringdata_SniffDialog = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SniffDialog"
QT_MOC_LITERAL(1, 12, 21), // "on_push_start_clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 20), // "on_push_stop_clicked"
QT_MOC_LITERAL(4, 56, 7), // "capture"
QT_MOC_LITERAL(5, 64, 14), // "unsigned char*"
QT_MOC_LITERAL(6, 79, 3), // "buf"
QT_MOC_LITERAL(7, 83, 3), // "len"
QT_MOC_LITERAL(8, 87, 3), // "arg"
QT_MOC_LITERAL(9, 91, 30), // "on_edit_filter_editingFinished"
QT_MOC_LITERAL(10, 122, 23), // "on_SniffDialog_finished"
QT_MOC_LITERAL(11, 146, 6), // "result"
QT_MOC_LITERAL(12, 153, 22), // "on_list_packet_clicked"
QT_MOC_LITERAL(13, 176, 11), // "QModelIndex"
QT_MOC_LITERAL(14, 188, 5), // "index"
QT_MOC_LITERAL(15, 194, 32), // "on_list_packet_currentRowChanged"
QT_MOC_LITERAL(16, 227, 10), // "currentRow"
QT_MOC_LITERAL(17, 238, 19), // "on_push_bot_clicked"
QT_MOC_LITERAL(18, 258, 19) // "on_push_top_clicked"

    },
    "SniffDialog\0on_push_start_clicked\0\0"
    "on_push_stop_clicked\0capture\0"
    "unsigned char*\0buf\0len\0arg\0"
    "on_edit_filter_editingFinished\0"
    "on_SniffDialog_finished\0result\0"
    "on_list_packet_clicked\0QModelIndex\0"
    "index\0on_list_packet_currentRowChanged\0"
    "currentRow\0on_push_bot_clicked\0"
    "on_push_top_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SniffDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    3,   61,    2, 0x08 /* Private */,
       9,    0,   68,    2, 0x08 /* Private */,
      10,    1,   69,    2, 0x08 /* Private */,
      12,    1,   72,    2, 0x08 /* Private */,
      15,    1,   75,    2, 0x08 /* Private */,
      17,    0,   78,    2, 0x08 /* Private */,
      18,    0,   79,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int, QMetaType::VoidStar,    6,    7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SniffDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SniffDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_push_start_clicked(); break;
        case 1: _t->on_push_stop_clicked(); break;
        case 2: _t->capture((*reinterpret_cast< unsigned char*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< void*(*)>(_a[3]))); break;
        case 3: _t->on_edit_filter_editingFinished(); break;
        case 4: _t->on_SniffDialog_finished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_list_packet_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: _t->on_list_packet_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_push_bot_clicked(); break;
        case 8: _t->on_push_top_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SniffDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_SniffDialog.data,
    qt_meta_data_SniffDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SniffDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SniffDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SniffDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SniffDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
