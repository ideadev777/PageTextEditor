#include "cxtextedit.h"
#include <QMimeData>
#include <QMessageBox>
#include <QDropEvent>
#include <QKeyEvent>
#include <QImageReader>
#include <QUrl>
#include <QMouseEvent>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QScrollBar>
#include <QCursor>

#define SZ 8

CxTextEdit::CxTextEdit(QWidget *parent)
    : PagesTextEdit(parent)
{
	setAcceptDrops(true) ;

    setPageFormat(QPageSize::B5);
    setPageMargins(QMarginsF(10,10,10,10));
    setUsePageMode(true);
    setPageNumbersAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_bound = new CxControlBody(this) ;
//    m_bound->setStyleSheet("QLabel{background:transparent;border:3px solid #2a4657;border:none;}");
    m_bound->setStyleSheet("QLabel{background:transparent;border:3px solid #2a4657;}");
    m_bound->hide() ;
    m_bound->setCursor(Qt::SizeAllCursor);
    connect( m_bound, SIGNAL(__moved(QPoint,int)), this, SLOT(onMoveImage(QPoint,int))) ;
    connect( m_bound, SIGNAL(__wheel(QWheelEvent*)), this, SLOT(onWheel(QWheelEvent*))) ;
    for( int i = 0; i < 8; i++ )
    {
        CxControlPoint* lb = new CxControlPoint(i,this) ;
        lb->hide() ;
        connect( lb, SIGNAL(__resize(int,QPoint,bool)), this, SLOT(onImageResize(int,QPoint,bool))) ;
        m_controlList << lb ;
    }
    m_resizingState = -1 ;
    m_isMovingImage = false;
    setMouseTracking(true);
}

CxTextEdit::~CxTextEdit()
{

}



void CxTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
//	event->accept() ;
	QTextEdit::dragEnterEvent(event) ;
	return ;
	QString str = event->mimeData()->data("tom_content") ;
	if( !str.length() ) event->ignore() ;
	else {
		event->acceptProposedAction() ;
		event->accept() ;
	}
}

void CxTextEdit::dragMoveEvent(QDragMoveEvent* event)
{
/*
event->acceptProposedAction();
	QDragMoveEvent move(event->pos(),event->dropAction(), event->mimeData(), event->mouseButtons(), event->keyboardModifiers(), event->type());
	QTextEdit::dragMoveEvent(&move); // Call the parent function (show cursor and keep selection)

	QTextCursor t = cursorForPosition(event->pos()) ;
	setTextCursor(t) ;

	event->accept() ;
	*/
	QTextEdit::dragMoveEvent(event) ;
	return ;
	QString str = event->mimeData()->data("tom_content") ;
	if( !str.length() ) event->ignore() ;
	else {
		QTextCursor t = cursorForPosition(event->pos());
		setTextCursor(t) ;
		event->acceptProposedAction() ;
		event->accept() ;
	}
}



void CxTextEdit::dropEvent(QDropEvent *event)
{
	if( event->source() == viewport() )
	{
		QTextEdit::dropEvent(event) ;
		return ;
	}
	QString str = event->mimeData()->data("tom_content") ;
	int contentType = event->mimeData()->data("tom_content_type").toInt() ;
	QTextCursor t = cursorForPosition(event->pos()) ;
	setTextCursor(t) ;

	if( contentType == 0 ) {//Text 
		insertPlainText(str) ;
	}
	if( contentType == 1 )//Image
	{
//		t.insertImage(QImage(str)) ;
		QUrl Uri = QUrl(str);
//		QUrl Uri = QUrl::fromLocalFile(str);
		QImage image = QImageReader(str).read();
		QTextDocument * textDocument = document();
		textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant (image) );
		QTextImageFormat imageFormat;
		imageFormat.setWidth( image.width() );
		imageFormat.setHeight( image.height() );
		imageFormat.setName( Uri.toString() );
		textCursor().insertImage(imageFormat);
	}

	QMimeData* mimeData = new QMimeData ;
	mimeData->setText("") ;
	event->setDropAction(Qt::MoveAction) ;
	event->accept() ;
	QDropEvent* dummy = new QDropEvent(event->posF(), Qt::MoveAction,mimeData, event->mouseButtons(), event->keyboardModifiers()) ;
	QTextEdit::dropEvent(dummy) ;
}

void CxTextEdit::keyPressEvent(QKeyEvent *event)
{
	QTextEdit::keyPressEvent(event) ;
    setControlVisible(false);
    emit __showTextFormat() ;
}

void CxTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextEdit::mouseDoubleClickEvent(e) ;
//    emit __resizeImage();
}


void CxTextEdit::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e) ;
    QPoint pnt = e->pos() ;
    for( int i = 0; i < 8; i++ ){
        if( m_controlList[i]->isVisible() == false ) break ;
        if( !i )
        {
//            QMessageBox::information(NULL,"",QString("%1 %2").arg(pnt.x()).arg(pnt.y())) ;
//            QMessageBox::information(NULL,"",QString("%1 %2").arg(m_controlList[i]->geometry().topLeft().x()).arg(m_controlList[i]->geometry().topLeft().y())) ;
        }
        if( m_controlList[i]->geometry().contains(pnt) )
        {
            m_resizingState = i ;
            m_startPnt = pnt ;
            return ;
        }
    }
    QTextCursor cur = cursorForPosition(pnt) ;

    QRect ret ;
    QPoint delta(10,20) ;
    bool vis = false ;
    if( cur.movePosition(QTextCursor::NextCharacter) )
    {
        QTextCharFormat fmt = cur.charFormat();
        if (fmt.objectType() == QTextFormat::NoObject) {
                  // workaround, sometimes the cursor will point one object to the left of the image
                  cur.movePosition(QTextCursor::NextCharacter);
                  fmt = cur.charFormat();
              }
        if (fmt.isImageFormat()) {
            m_curImageName = fmt.toImageFormat().name() ;
            ret = cursorRect(cursorForPosition(pnt)) ;
            vis = true ;
            m_imagePosition = cursorForPosition(pnt).position() ;
            ret = QRect(ret.topLeft()+delta,QSize(fmt.toImageFormat().width(),fmt.toImageFormat().height()));
        }
    }
    if( !vis )
    {
        cur = cursorForPosition(pnt) ;
        QTextCharFormat fmt = cur.charFormat();
        if (fmt.objectType() == QTextFormat::NoObject) {
                  cur.movePosition(QTextCursor::NextCharacter);
                  fmt = cur.charFormat();
              }
        if (fmt.isImageFormat()) {
            m_curImageName = fmt.toImageFormat().name() ;
            cur.movePosition(QTextCursor::PreviousCharacter) ;
            ret = cursorRect(cur) ;
            vis = true ;
            m_imagePosition = cur.position() ;
            ret = QRect(ret.topLeft()+delta,QSize(fmt.toImageFormat().width(),fmt.toImageFormat().height()));
//            ret = QRect(ret.topLeft()-QPoint(fmt.toImageFormat().width(),0),QSize(fmt.toImageFormat().width(),fmt.toImageFormat().height()));
        }
    }


    if( vis ) {
        m_bound->setGeometry(ret) ;
        setControlPos(ret);
        m_curImageRect = ret ;
    }
    setControlVisible(vis);
    return ;
}

void CxTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e) ;
    if( m_resizingState == -1 ) return ;
    QPoint pnt = e->pos() ;
    QRect ret = m_curImageRect ;
    if( m_resizingState == 0 )
    {
        ret.setTopLeft(ret.topLeft()+pnt-m_startPnt) ;
    }
    setControlPos(ret);
}

void CxTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e) ;
    m_resizingState = -1 ;
    QPoint pnt = e->pos() ;
}

QRect CxTextEdit::imageRect(const QTextCursor &cursor) const
{
    QTextImageFormat fmt = textCursor().charFormat().toImageFormat();
    if (!fmt.isValid()) return QRect();
    QRect rc = cursorRect(textCursor()) ;
//    QPoint delta =
    return QRect( rc.topLeft() , QSize(fmt.width(),fmt.height())) ;
    /*
    if (!cursor.block().layout())
    return QRect();

    QRect curRect = cursorRect(cursor);
    QRect cursorFrameRect(curRect.topLeft(), QSize(fmt.width(), fmt.height()));

    QTextLayout *blockLayout = cursor.block().layout();
    QTextLine curLine = blockLayout->lineForTextPosition(cursor.positionInBlock());
    QRectF curLineRect = curLine.rect();
    curLineRect.moveTopLeft(blockLayout->position() + curLineRect.topLeft());

    int x = curRect.topLeft().x() - fmt.width();
    int y = curLineRect.y() + curLine.ascent() + (curLine.leadingIncluded() ? qMax(curLine.leading(), 0.0) : 0); //baseline
    y -= verticalScrollBar()->value();

    return QRect(x, y, fmt.width(), fmt.height());
    */
}

void CxTextEdit::resizeImage( double w, double h )
{
    if( w < 0 ) w *= -1 ;
    if( h < 0 ) h *= -1 ;
    QTextBlock currentBlock = textCursor().block();
    QTextBlock::iterator it;

    for (it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
             QTextFragment fragment = it.fragment();
             if (fragment.isValid())
             {

                 if(fragment.charFormat().isImageFormat ())
                 {
                      QTextImageFormat newImageFormat = fragment.charFormat().toImageFormat();

                      QPair<double, double> size = QPair<double,double>(w,h) ;

                      newImageFormat.setWidth(size.first);
                      newImageFormat.setHeight(size.second);

                      if (newImageFormat.isValid())
                      {
                          QTextCursor helper = textCursor();
                          helper.setPosition(fragment.position());
                          helper.setPosition(fragment.position() + fragment.length(),
                                              QTextCursor::KeepAnchor);
                          helper.setCharFormat(newImageFormat);
                      }
                  }
              }
       }
}

void CxTextEdit::setControlPos(QRect rc)
{
    m_bound->setGeometry(rc) ;
    m_controlList[0]->setGeometry(QRect(rc.topLeft()-QPoint(SZ,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[1]->setGeometry(QRect(rc.topLeft()-QPoint(SZ-rc.width()/2,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[2]->setGeometry(QRect(rc.topRight()-QPoint(SZ,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[3]->setGeometry(QRect(rc.topRight()-QPoint(SZ,SZ-rc.height()/2),QSize(SZ*2,SZ*2)));
    m_controlList[4]->setGeometry(QRect(rc.bottomRight()-QPoint(SZ,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[5]->setGeometry(QRect(rc.bottomRight()-QPoint(SZ+rc.width()/2,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[6]->setGeometry(QRect(rc.bottomLeft()-QPoint(SZ,SZ),QSize(SZ*2,SZ*2)));
    m_controlList[7]->setGeometry(QRect(rc.bottomLeft()-QPoint(SZ,SZ+rc.height()/2),QSize(SZ*2,SZ*2)));
}

void CxTextEdit::setControlVisible(bool vis)
{
    for( int i = m_controlList.count()-1; i >= 0; i-- )
    {
        m_controlList[i]->setVisible(vis);
    }
//    if( !vis && !m_isMovingImage ) m_bound->setVisible(vis);
//    if( !vis && !m_isMovingImage ) m_bound->setVisible(vis);
    if( !m_isMovingImage ) m_bound->setVisible(vis);
}


void CxTextEdit::onImageResize(int mode, QPoint delta, bool isRelease)
{
    QRect ret = m_curImageRect ;
    if( mode == 0 )
    {
        ret.setTopLeft(ret.topLeft()+delta) ;
    }
    if( mode == 1 )
    {
        ret.setTop(ret.y()+delta.y());
    }
    if( mode == 2 )
    {
        ret.setTopRight(ret.topRight()+delta);
    }
    if( mode == 3 )
    {
        ret.setRight(ret.right()+delta.x());
    }
    if( mode == 4 )
    {
        ret.setBottomRight(ret.bottomRight()+delta);
    }
    if( mode == 5 )
    {
        ret.setBottom(ret.bottom()+delta.y());
    }
    if( mode == 6 )
    {
        ret.setBottomLeft(ret.bottomLeft()+delta);
    }
    if( mode == 7 )
    {
        ret.setLeft(ret.left()+delta.x());
    }

    QPoint tp = ret.topLeft() ;
    tp.setX(qMax(tp.x(),30));
    tp.setY(qMax(tp.y(),30));
    QPoint bt = ret.bottomRight() ;
    bt.setX(qMin(bt.x(),viewport()->width()-20)) ;
    bt.setY(qMin(bt.y(),viewport()->height()-20)) ;
    ret.setTopLeft(tp);
    ret.setBottomRight(bt);
    setControlPos(ret);
    m_bound->setVisible(true);
    if( isRelease )
    {
        resizeImage(ret.width(),ret.height()) ;
        setControlVisible(false);
//        QMessageBox::information(NULL,"",QString("%1 %2").arg(tp.x()).arg(tp.y())) ;
    }
}

void CxTextEdit::onMoveImage(QPoint delta, int mode)
{
    if( mode == 0 ) {
        m_bound->setVisible(true);
        m_isMovingImage = true ;
    }
    QPoint pnt = mapFromGlobal(QCursor::pos()) ;
    if( mode == 1 )
    {
        QTextCursor nxt = cursorForPosition(pnt) ;
        setTextCursor(nxt) ;
    }
    if( mode == 2 ) {
        m_isMovingImage = false ;
        m_bound->setVisible(false);

        QTextCursor nxt = cursorForPosition(pnt) ;
//        nxt.insertImage() ;

        QTextCursor cur = textCursor() ;
        cur.setPosition(m_imagePosition);
        cur.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor) ;
        cur.removeSelectedText();
        QTextImageFormat imageFormat;
        imageFormat.setWidth( m_curImageRect.width() );
        imageFormat.setHeight( m_curImageRect.height() );
        imageFormat.setName( m_curImageName );
        nxt.insertImage(imageFormat);
        setControlVisible(false);
    }
    QRect ret = m_curImageRect ;
    ret.translate(delta.x(),delta.y());
    m_bound->setGeometry(ret);
}

void CxTextEdit::onWheel(QWheelEvent *event)
{
    QTextEdit::wheelEvent(event) ;
}
