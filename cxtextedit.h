#ifndef CXTEXTEDIT_H
#define CXTEXTEDIT_H

#include <QTextEdit>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include "PagesTextEdit.h"
#include <QTextEdit>
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include "cxcontrolpoint.h"
#include "cxcontrolbody.h"

class ResizeImageDialog : public QDialog
{
    Q_OBJECT

double                   m_ratio;

QLabel                  *m_widthLabel;
QLabel                  *m_hightLabel;
QDoubleSpinBox          *m_widthSpinBox;
QDoubleSpinBox          *m_hightSpinBox;
QCheckBox               *m_keepRatioCheckBox;

QPushButton             *m_okButton;
QPushButton             *m_cancelButton;

QHBoxLayout             *m_widthLayout;
QHBoxLayout             *m_hightLayout;
QHBoxLayout             *m_buttonLayout;
QVBoxLayout             *m_generalLayout;

private slots:
//    void widthChanged(double width);
//    void hightChanged(double hight);

public:
    ResizeImageDialog(QWidget * parent = 0, double imageWidth = 100.0, double imageHight = 100.0):QDialog(parent)
    {
    m_widthLabel = new QLabel("Image width");
    m_hightLabel = new QLabel("Image hight");

    m_widthSpinBox = new QDoubleSpinBox;
    m_widthSpinBox->setMaximum(1500);
    m_widthSpinBox->setValue(imageWidth);
    connect(m_widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));


    m_hightSpinBox = new QDoubleSpinBox;
    m_hightSpinBox->setMaximum(1500);
    m_hightSpinBox->setValue(imageHight);
    connect(m_hightSpinBox, SIGNAL(valueChanged(double)), this, SLOT(hightChanged(double)));

    m_ratio = imageWidth/imageHight;


    m_keepRatioCheckBox = new QCheckBox("Keep ratio",this);
    m_keepRatioCheckBox->setChecked(true);


    m_widthLayout = new QHBoxLayout;
    m_widthLayout->addWidget(m_widthLabel);
    m_widthLayout->addWidget(m_widthSpinBox);

    m_hightLayout  = new QHBoxLayout;
    m_hightLayout->addWidget(m_hightLabel);
    m_hightLayout->addWidget(m_hightSpinBox);

    m_okButton = new QPushButton("OK");
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));

    m_cancelButton = new QPushButton("Cancel");
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_okButton);
    m_buttonLayout->addWidget(m_cancelButton);

    m_generalLayout = new QVBoxLayout;
    m_generalLayout->addLayout(m_widthLayout);
    m_generalLayout->addLayout(m_hightLayout);
    m_generalLayout->addWidget(m_keepRatioCheckBox);
    m_generalLayout->addLayout(m_buttonLayout);
    setLayout(m_generalLayout);

    setMaximumSize(300, 300);
    setModal(true);
    //resize(670,630);
    setWindowTitle(tr("Resize Image"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

static QPair<double, double> getNewSize(QWidget * parent = 0, double imageWidth = 100.0, double imageHight = 100.0)
{
    ResizeImageDialog dlg(parent, imageWidth, imageHight);
    dlg.exec();



    QPair<double, double> size;
    size.first = dlg.m_widthSpinBox->value();
    size.second = dlg.m_hightSpinBox->value();
    return size;
}
};


class CxTextEdit : public PagesTextEdit
{
	Q_OBJECT

public:
	CxTextEdit(QWidget *parent);
	~CxTextEdit();
    void resizeImage( double w, double h ) ;
    void setControlPos( QRect rc ) ;
    void setControlVisible(bool) ;
signals:
	void __showTextFormat() ;
    void __resizeImage() ;
public slots:
    void onImageResize( int mode, QPoint delta, bool isRelease ) ;
    void onMoveImage( QPoint delta, int mode ) ;
    void onWheel( QWheelEvent* event ) ;
protected:
	void dragEnterEvent(QDragEnterEvent *event) ;
	void dragMoveEvent(QDragMoveEvent* event) ;
	void dropEvent(QDropEvent *event) ;
	void keyPressEvent(QKeyEvent *event) ;
    void mouseDoubleClickEvent(QMouseEvent *e) ;
    void mousePressEvent(QMouseEvent *e) ;
    void mouseMoveEvent(QMouseEvent *e) ;
    void mouseReleaseEvent(QMouseEvent *e) ;
private:
    QRect imageRect(const QTextCursor &cursor) const ;
    QList<CxControlPoint*> m_controlList ;
    CxControlBody* m_bound ;
    int m_resizingState ;
    QRect m_curImageRect ;
    QPoint m_startPnt ;
    bool m_isMovingImage ;
    int m_imagePosition ;
    QString m_curImageName ;
};

#endif // CXTEXTEDIT_H
