#ifndef CXCONTROLPOINT_H
#define CXCONTROLPOINT_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>

class CxControlPoint : public QLabel
{
    Q_OBJECT
public:
    CxControlPoint( int id, QWidget* parent);
protected:
    void mousePressEvent(QMouseEvent *event) ;
    void mouseMoveEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *event) ;
signals:
    void __resize( int mode, QPoint delta, bool isRelease ) ;
private:
    bool m_isPressing ;
    int m_mode ;
    QPoint m_startPoint ;
};

#endif // CXCONTROLPOINT_H
