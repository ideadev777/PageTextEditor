#ifndef CXUPLOADWIDGET_H
#define CXUPLOADWIDGET_H

#include <QWidget>
#include "ui_w_upload.h"

class CxUploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CxUploadWidget(QWidget *parent = 0);
    Ui::Form_Upload form(){ return ui ; }
signals:
    void __addFolder() ;
    void __addFile() ;
private:
    Ui::Form_Upload ui ;
};

#endif // CXUPLOADWIDGET_H
