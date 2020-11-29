#ifndef CXCONTROLBODY_H
#define CXCONTROLBODY_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>

class CxControlBody : public QLabel
{
    Q_OBJECT
public:
    CxControlBody(QWidget* parent);
signals:
    void __moved( QPoint delta, int mouseState ) ;
    void __wheel(QWheelEvent* event) ;
protected:
    void mousePressEvent(QMouseEvent* event) ;
    void mouseMoveEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent* event );
    void wheelEvent(QWheelEvent *event) ;
private:
    bool m_isPressing ;
    QPoint m_startPnt ;
};

#endif // CXCONTROLBODY_H
