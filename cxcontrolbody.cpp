#include "cxcontrolbody.h"
#include <QMessageBox>

CxControlBody::CxControlBody(QWidget* parent)
    :QLabel(parent)
{
    m_isPressing = false ;
}

void CxControlBody::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event) ;
    if( event->button() & Qt::RightButton ){
        return ;
    }
    m_isPressing = true ;
    m_startPnt = event->globalPos() ;
    emit __moved(QPoint(0,0),0);
}

void CxControlBody::mouseMoveEvent(QMouseEvent *event)
{
    QLabel::mouseMoveEvent(event) ;
    if( !m_isPressing ) return ;
    emit __moved(event->globalPos()-m_startPnt,1);
}

void CxControlBody::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event) ;
    if( event->button() & Qt::RightButton ){
        return ;
    }
    if( !m_isPressing ) return ;
    m_isPressing = false ;
    emit __moved(event->globalPos()-m_startPnt,2);
}

void CxControlBody::wheelEvent(QWheelEvent *event)
{
    emit __wheel(event);
}
