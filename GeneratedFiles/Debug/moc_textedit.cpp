/****************************************************************************
** Meta object code from reading C++ file 'textedit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../textedit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TextEdit_t {
    QByteArrayData data[30];
    char stringdata[318];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TextEdit_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TextEdit_t qt_meta_stringdata_TextEdit = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 7),
QT_MOC_LITERAL(2, 17, 0),
QT_MOC_LITERAL(3, 18, 8),
QT_MOC_LITERAL(4, 27, 8),
QT_MOC_LITERAL(5, 36, 10),
QT_MOC_LITERAL(6, 47, 9),
QT_MOC_LITERAL(7, 57, 16),
QT_MOC_LITERAL(8, 74, 12),
QT_MOC_LITERAL(9, 87, 8),
QT_MOC_LITERAL(10, 96, 13),
QT_MOC_LITERAL(11, 110, 10),
QT_MOC_LITERAL(12, 121, 10),
QT_MOC_LITERAL(13, 132, 1),
QT_MOC_LITERAL(14, 134, 8),
QT_MOC_LITERAL(15, 143, 1),
QT_MOC_LITERAL(16, 145, 9),
QT_MOC_LITERAL(17, 155, 10),
QT_MOC_LITERAL(18, 166, 9),
QT_MOC_LITERAL(19, 176, 9),
QT_MOC_LITERAL(20, 186, 8),
QT_MOC_LITERAL(21, 195, 1),
QT_MOC_LITERAL(22, 197, 24),
QT_MOC_LITERAL(23, 222, 15),
QT_MOC_LITERAL(24, 238, 6),
QT_MOC_LITERAL(25, 245, 21),
QT_MOC_LITERAL(26, 267, 20),
QT_MOC_LITERAL(27, 288, 5),
QT_MOC_LITERAL(28, 294, 12),
QT_MOC_LITERAL(29, 307, 9)
    },
    "TextEdit\0fileNew\0\0fileOpen\0fileSave\0"
    "fileSaveAs\0filePrint\0filePrintPreview\0"
    "filePrintPdf\0textBold\0textUnderline\0"
    "textItalic\0textFamily\0f\0textSize\0p\0"
    "textStyle\0styleIndex\0textColor\0textAlign\0"
    "QAction*\0a\0currentCharFormatChanged\0"
    "QTextCharFormat\0format\0cursorPositionChanged\0"
    "clipboardDataChanged\0about\0printPreview\0"
    "QPrinter*\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x0a,
       3,    0,  115,    2, 0x08,
       4,    0,  116,    2, 0x08,
       5,    0,  117,    2, 0x08,
       6,    0,  118,    2, 0x08,
       7,    0,  119,    2, 0x08,
       8,    0,  120,    2, 0x08,
       9,    0,  121,    2, 0x08,
      10,    0,  122,    2, 0x08,
      11,    0,  123,    2, 0x08,
      12,    1,  124,    2, 0x08,
      14,    1,  127,    2, 0x08,
      16,    1,  130,    2, 0x08,
      18,    0,  133,    2, 0x08,
      19,    1,  134,    2, 0x08,
      22,    1,  137,    2, 0x08,
      25,    0,  140,    2, 0x08,
      26,    0,  141,    2, 0x08,
      27,    0,  142,    2, 0x08,
      28,    1,  143,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 29,    2,

       0        // eod
};

void TextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextEdit *_t = static_cast<TextEdit *>(_o);
        switch (_id) {
        case 0: _t->fileNew(); break;
        case 1: _t->fileOpen(); break;
        case 2: { bool _r = _t->fileSave();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->fileSaveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->filePrint(); break;
        case 5: _t->filePrintPreview(); break;
        case 6: _t->filePrintPdf(); break;
        case 7: _t->textBold(); break;
        case 8: _t->textUnderline(); break;
        case 9: _t->textItalic(); break;
        case 10: _t->textFamily((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->textSize((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->textStyle((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->textColor(); break;
        case 14: _t->textAlign((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 15: _t->currentCharFormatChanged((*reinterpret_cast< const QTextCharFormat(*)>(_a[1]))); break;
        case 16: _t->cursorPositionChanged(); break;
        case 17: _t->clipboardDataChanged(); break;
        case 18: _t->about(); break;
        case 19: _t->printPreview((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject TextEdit::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TextEdit.data,
      qt_meta_data_TextEdit,  qt_static_metacall, 0, 0}
};


const QMetaObject *TextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEdit.stringdata))
        return static_cast<void*>(const_cast< TextEdit*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int TextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
