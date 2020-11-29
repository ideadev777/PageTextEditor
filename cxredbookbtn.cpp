#include "cxredbookbtn.h"

CxRedBookBtn::CxRedBookBtn(QWidget* parent)
    :QToolButton(parent)
{

}

void CxRedBookBtn::enterEvent(QEvent *e)
{
    QToolButton::enterEvent(e) ;
    emit __showRedBookPanel(true);
}

void CxRedBookBtn::leaveEvent(QEvent *e)
{
    QToolButton::leaveEvent(e) ;
    emit __showRedBookPanel(false);
}
