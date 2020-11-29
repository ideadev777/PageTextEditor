#include "cxuploadwidget.h"

CxUploadWidget::CxUploadWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect( ui.tb_add_file, SIGNAL(clicked()), this, SIGNAL(__addFile())) ;
    connect( ui.tb_add_folder, SIGNAL(clicked()), this, SIGNAL(__addFolder())) ;
}
