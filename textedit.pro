QT += widgets xml axcontainer
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit

HEADERS         = textedit.h \
                  PagesTextEdit.h\
                  PageMetrics.h \
    centeredtoolbuttonstyle.h \
    cxallchapterdlg.h \
    cxcenterbutton.h \
    cxchapterlist.h \
    cxcolorbutton.h \
    cxcolormenu.h \
    cxcolorwidget.h \
    cxcontentlabel.h \
    cxpagebutton.h \
    cxpagelabel.h \
    cxpagemenu.h \
    cxpreviewlabel.h \
    cxreslistwidget.h \
    cxsmalltext.h \
    cxtextedit.h \
    cxwordhandler.h \
    cxcontenthover.h \
    cxredbookbtn.h \
    cxuploadwidget.h \
    cxcontrolpoint.h \
    cxcontrolbody.h
SOURCES         = textedit.cpp \
                  main.cpp \
                  PageMetrics.cpp \
                  PagesTextEdit.cpp \
    centeredtoolbuttonstyle.cpp \
    cxallchapterdlg.cpp \
    cxcenterbutton.cpp \
    cxchapterlist.cpp \
    cxcolorbutton.cpp \
    cxcolormenu.cpp \
    cxcolorwidget.cpp \
    cxcontentlabel.cpp \
    cxpagebutton.cpp \
    cxpagelabel.cpp \
    cxpagemenu.cpp \
    cxpreviewlabel.cpp \
    cxreslistwidget.cpp \
    cxsmalltext.cpp \
    cxtextedit.cpp \
    cxwordhandler.cpp \
    cxcontenthover.cpp \
    cxredbookbtn.cpp \
    cxuploadwidget.cpp \
    cxcontrolpoint.cpp \
    cxcontrolbody.cpp

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
CONFIG += axcontainer axwidget
}

EXAMPLE_FILES = textedit.qdoc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/textedit
INSTALLS += target

RC_ICONS = icon.ico

FORMS += \
    dlg_all_chapter.ui \
    dlg_color_menu.ui \
    dlg_small_text.ui \
    mainwindow.ui \
    w_color_button.ui \
    w_page.ui \
    w_text_hover.ui \
    w_upload.ui
