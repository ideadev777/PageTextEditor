#ifndef CXCONTENTHOVER_H
#define CXCONTENTHOVER_H

#include <QWidget>
#include "ui_w_text_hover.h"
#include "cxreslistwidget.h"

class CxContentHover : public QWidget
{
    Q_OBJECT
public:
    explicit CxContentHover(QWidget *parent = 0);
    void init( CxResListWidget* lw, QListWidgetItem* item ) ;
signals:

public slots:
    void onSave() ;
    void onClose() ;
    void onContext() ;
private:
    Ui::Form_Content_Hover ui ;
    CxResListWidget* m_lw;
    QListWidgetItem* m_item ;
};

#endif // CXCONTENTHOVER_H
