#include "cxcontrolpoint.h"
#include <QMouseEvent>

CxControlPoint::CxControlPoint(int id, QWidget* parent)
    :QLabel(parent)
{
    m_mode = id ;
    setStyleSheet("QLabel{border:1px solid black;background:#3FEF36;}");
    m_isPressing = false ;
    if( id == 0 ) setCursor(Qt::SizeFDiagCursor) ;
    if( id == 1 ) setCursor(Qt::SizeVerCursor) ;
    if( id == 2 ) setCursor(Qt::SizeBDiagCursor) ;
    if( id == 3 ) setCursor(Qt::SizeHorCursor) ;
    if( id == 4 ) setCursor(Qt::SizeFDiagCursor) ;
    if( id == 5 ) setCursor(Qt::SizeVerCursor) ;
    if( id == 6 ) setCursor(Qt::SizeBDiagCursor) ;
    if( id == 7 ) setCursor(Qt::SizeHorCursor) ;
}

void CxControlPoint::mousePressEvent(QMouseEvent *event)
{
    m_isPressing = true ;
    m_startPoint = event->globalPos() ;
}

void CxControlPoint::mouseMoveEvent(QMouseEvent *event)
{
    if( !m_isPressing ) return ;
    emit __resize(m_mode, event->globalPos()-m_startPoint, false ) ;
}

void CxControlPoint::mouseReleaseEvent(QMouseEvent *event)
{
    if( !m_isPressing ) return ;
    m_isPressing = false ;
    emit __resize(m_mode, event->globalPos()-m_startPoint, true ) ;
}
