#include "cxcontenthover.h"

CxContentHover::CxContentHover(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect( ui.tb_close, SIGNAL(clicked()), this, SLOT(onClose())) ;
    connect( ui.tb_context, SIGNAL(clicked()), this, SLOT(onContext())) ;
    connect( ui.tb_save, SIGNAL(clicked()), this, SLOT(onSave())) ;
    ui.textEdit->document()->setDefaultFont(QFont("arial",13));
}


void CxContentHover::init(CxResListWidget *lw, QListWidgetItem *item)
{
    m_lw = lw ;
    m_item = item ;
    ui.textEdit->setText(item->data(Qt::EditRole).toString());
}

void CxContentHover::onSave()
{
    QString txt = ui.textEdit->toPlainText() ;
    m_item->setData(Qt::EditRole,txt) ;
    CxContentLabel* lb = m_lw->getLabel(m_item) ;
    if( lb ) lb->setText(txt) ;
    m_lw->refresh() ;
    onClose() ;
}

void CxContentHover::onContext()
{
    m_lw->showContextMenu(m_item);
}

void CxContentHover::onClose()
{
    close() ;
}
