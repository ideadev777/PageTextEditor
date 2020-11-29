#ifndef CXREDBOOKBTN_H
#define CXREDBOOKBTN_H

#include <QToolButton>
#include <QObject>

class CxRedBookBtn : public QToolButton
{
    Q_OBJECT
public:
    CxRedBookBtn(QWidget* parent=0);
signals:
    void __showRedBookPanel(bool on) ;
protected:
    void enterEvent(QEvent *e) ;
    void leaveEvent(QEvent *e) ;
};

#endif // CXREDBOOKBTN_H
