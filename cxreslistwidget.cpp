#include "cxreslistwidget.h"
#include <QTextEdit>
#include <QScrollBar>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QCursor>
#include <QDrag>
#include <QCursor>

#include "cxsmalltext.h"
#define TEXTITEMH 60

CxResListWidget::CxResListWidget(QWidget *parent)
	: QListWidget(parent)
{
	m_isRemoteMode = false ;
    verticalScrollBar()->setStyleSheet("{ background:#c3c3c3; }");
    setViewMode(QListView::IconMode) ;
    setSpacing(5) ;
    setUniformItemSizes(true) ;
    setFlow(QListView::LeftToRight) ;
//	setFlow(QListView::TopToBottom) ;
    setDragDropMode(QAbstractItemView::DragDrop) ;


    setAcceptDrops(true) ;
    setDragEnabled(true) ;
    setDefaultDropAction(Qt::MoveAction) ;
    setWrapping(true);
    showDropIndicator() ;
    setResizeMode(QListView::Adjust);



    connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDoubleClickItem(QListWidgetItem*))) ;
	connect( this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged())) ;
	m_preView = new CxPreviewLabel(this) ;
//	m_preView->setGeometry(0,0,40,40) ;
    m_preView->hide() ;
}

CxResListWidget::~CxResListWidget()
{

}

void CxResListWidget::addImage(QString fileName)
{
	addImage(count(),fileName) ;
}

void CxResListWidget::addImage( int pos, QString fileName )
{
	int w = (viewport()->width()-spacing()*3)/2-10;
	setIconSize(QSize(w,TEXTITEMH)) ;
	QListWidgetItem* item = new QListWidgetItem ;
	CxContentLabel* lb = new CxContentLabel(item) ;
    connect( lb, SIGNAL(__showPreview(QListWidgetItem*, bool, QPoint)), this, SLOT(onShowContentHover(QListWidgetItem*, bool, QPoint))) ;
//    connect( lb, SIGNAL(__showPreview(QListWidgetItem*, bool, QPoint)), this, SIGNAL(__showContentHover(QListWidgetItem*, bool, QPoint))) ;
    lb->setPixmap(QPixmap(fileName).scaled(w,TEXTITEMH,Qt::KeepAspectRatio,Qt::SmoothTransformation)) ;
    lb->resize(w,TEXTITEMH) ;
	lb->setAlignment(Qt::AlignCenter) ;
	item->setSizeHint(QSize(w,TEXTITEMH)) ;
	insertItem(pos,item) ;
	setItemWidget(item,lb) ;
	m_contentList << fileName ;
	m_typeList << IMAGETYPE ;
	item->setData(Qt::EditRole,fileName) ;
	item->setData(Qt::EditRole+1,IMAGETYPE) ;
	refresh() ;
}

void CxResListWidget::addText(QString txt)
{
	addText(count(),txt) ;
}

void CxResListWidget::addText( int pos, QString txt )
{
    int w = (viewport()->width()-spacing()*3)/2-10;
    setIconSize(QSize(w,TEXTITEMH)) ;
    QListWidgetItem* item = new QListWidgetItem ;
    CxContentLabel* lb = new CxContentLabel(item) ;
    connect( lb, SIGNAL(__showPreview(QListWidgetItem*, bool, QPoint)), this, SLOT(onShowContentHover(QListWidgetItem*, bool, QPoint))) ;
    //	QLabel* lb = new QLabel() ;
    lb->setText("<b> "+txt+"</b>") ;
	lb->setTextInteractionFlags(Qt::NoTextInteraction) ;
    //	lb->setText(txt) ;
    //	item->setBackground(Qt::white) ;
    lb->resize(w,TEXTITEMH) ;
    item->setSizeHint(QSize(w,TEXTITEMH)) ;
    insertItem(pos,item) ;
	setItemWidget(item,lb) ;
    m_contentList << txt ;
	m_typeList << TEXTTYPE ;
	item->setData(Qt::EditRole,txt) ;
    item->setData(Qt::EditRole+1,TEXTTYPE) ;
    refresh() ;
    repaint() ;
}

void CxResListWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-item")) {
        QListWidgetItem* item = itemAt(event->pos()) ;
        int tar = row(item) ;
        if( m_lastDropIndex != -1 ) takeItem(m_lastDropIndex) ;
        m_lastDropIndex = -1 ;
        refresh() ;
        if( !item )
        {
            event->ignore() ;
            return ;
        }
        /*
        QListWidgetItem *newItem = new QListWidgetItem;
        QString name = event->mimeData()->data("application/x-item");
        newItem->setText(name);
        */
        QString str = event->mimeData()->data("tom_content") ;
        int contentType = event->mimeData()->data("tom_content_type").toInt() ;
        if( contentType == 0 ) addText(tar,str) ;
        else addImage(tar,str) ;
//        QMessageBox::information(NULL,"",QString("%1").arg(tar)) ;
        event->accept();
        event->setDropAction(Qt::MoveAction);
        refresh() ;
    } else
        event->ignore();
// 	QListWidget::dropEvent(event) ;
// 	refresh() ;
}


void CxResListWidget::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasFormat("application/x-item")) {
//        QMessageBox::information(NULL,"","A") ;
        e->setDropAction(Qt::MoveAction);
		e->accept();
        QListWidgetItem* var = itemAt(e->pos()) ;
        if( var )
        {
            int id = row(var) ;
            if( id == m_lastDropIndex ) return ;
            QListWidgetItem* curDrag = NULL ;
            if( m_lastDropIndex != -1 ) curDrag = item(m_lastDropIndex) ;
            QString str = e->mimeData()->data("tom_content") ;
            QListWidgetItem* tmp = new QListWidgetItem ;
            int contentType = e->mimeData()->data("tom_content_type").toInt() ;
            insertItem(id,tmp) ;
            if( curDrag ) takeItem(row(curDrag)) ;
            m_lastDropIndex = row(tmp) ;
            /*
            if( id == m_lastDropIndex ) return ;
            if( m_lastDropIndex != -1 ) takeItem(m_lastDropIndex) ;
            m_lastDropIndex = id ;
            QString str = e->mimeData()->data("tom_content") ;
            QListWidgetItem* tmp = new QListWidgetItem ;
            int contentType = e->mimeData()->data("tom_content_type").toInt() ;
//            if( contentType == 0 ) addText(id,str) ;
//            else addImage(id,str) ;
            insertItem(id,tmp) ;
            */
        }
        else{
            if( m_lastDropIndex != -1 ) takeItem(m_lastDropIndex) ;
            m_lastDropIndex = -1 ;
        }
    } else
		e->ignore();
}

void CxResListWidget::onDoubleClickItem(QListWidgetItem *item)
{
    if( item->data(Qt::EditRole+1).toInt() == IMAGETYPE ) return ;
    CxContentLabel* lb = getLabel(item) ;
    emit __showContentHover(this,item,true,mapToGlobal(lb->geometry().topLeft()));
}

void CxResListWidget::onChangeItem( QListWidgetItem* item )
{
	if( item->data(Qt::EditRole+1).toInt() == IMAGETYPE ) return ;
	CxSmallText dlg ;
	dlg.setText(item->data(Qt::EditRole).toString()) ;
	if( dlg.exec() == QDialog::Accepted )
	{
		item->setData(Qt::EditRole,dlg.content()) ;
		CxContentLabel* lb = getLabel(item) ;
		if( lb ) lb->setText(dlg.content()) ;
		refresh() ;
	}
}

void CxResListWidget::contextMenuEvent(QContextMenuEvent *event)
{

	QMargins var = contentsMargins() ;
//	QMessageBox::information(NULL,"",QString("%1 %2 %3 %4").arg(var.left()).arg(var.right()).arg(var.top()).arg(var.bottom())) ;

    QListWidgetItem*item = itemAt(event->pos()) ;
	if(item)
	{
        showContextMenu(item) ;
	}
}

void CxResListWidget::showContextMenu(QListWidgetItem* item)
{
    QMenu menu ;
    int itemType = item->data(Qt::EditRole+1).toInt() ;
    menu.setFont(QFont("Impact",16)) ;
    QAction* changeAction = NULL ;

    if( itemType == TEXTTYPE ) changeAction = menu.addAction("Change") ;
    QAction* deleteAction = menu.addAction("Delete") ;
    QAction* moveContent = menu.addAction(QString("Move to Content%1").arg(m_index?"+":"-")) ;
    QAction* ret = menu.exec(QCursor::pos()) ;
    if( ret == changeAction && itemType == TEXTTYPE )
    {
        onChangeItem(item) ;
    }
    if( ret == deleteAction )
    {
        takeItem(row(item)) ;
        refresh() ;
    }
    if( ret == moveContent )
    {
        emit __moveContent(m_index,item) ;
        takeItem(row(item)) ;
        refresh() ;
    }
}

CxContentLabel* CxResListWidget::getLabel(QListWidgetItem* item)
{
	return (CxContentLabel*)itemWidget(item) ;
}


void CxResListWidget::refresh()
{
	m_contentList.clear() ;
	m_typeList.clear() ;
	for( int i = 0; i < count(); i++ )
	{
		QListWidgetItem* var = item(i) ;
		m_contentList << var->data(Qt::EditRole).toString() ;
		m_typeList << var->data(Qt::EditRole+1).toInt() ;
//		CxContentLabel* lb = getLabel(var) ;
//		lb->setStyleSheet(getStyleSheet(var->isSelected(),i)) ;
    }
//    refit() ;
    int ww = width() ;
    resize(ww-15,height()) ;
    resize(ww,height()) ;
    refit() ;
    emit __changed(m_index) ;
}

QString CxResListWidget::getStyleSheet( bool isSelected, int row )
{
	QString border = (isSelected?"border-style:solid;border-color:yellow;border-width:3px;":"") ;
	QString ret = QString("QLabel{background:%1;%2}QLabel::hover{background:#ababab;%3}").arg(row%2?"#545454":"#f3f3f3").arg(border).arg(border) ;
	return ret ;
}

void CxResListWidget::onSelectionChanged()
{
	for( int i = 0; i < count(); i++ )
	{
		QListWidgetItem* var = item(i) ;
//		CxContentLabel* lb = getLabel(var) ;
//		if( !lb ) continue ;
//		lb->setStyleSheet(getStyleSheet(var->isSelected(),i)) ;
    }
}

void CxResListWidget::setData( QStringList contentList, QList<int> typeList )
{
	m_contentList = contentList ;
	m_typeList = typeList ;
	clear() ;
	int cnt = m_contentList.count() ;
	m_isRemoteMode = true ;
	for( int i = 0; i < cnt; i++ )
	{
		if( typeList[i] == TEXTTYPE ) addText(contentList[i]) ;
		if( typeList[i] == IMAGETYPE ) addImage(contentList[i]) ;
	}
	m_isRemoteMode = false ;
}
QMimeData* CxResListWidget::mimeData(const QList<QListWidgetItem *> items) const
{
	QMimeData *md = QListWidget::mimeData(items);
	return md ;
	QStringList texts;
	for(QListWidgetItem *item : selectedItems())
	{
		texts << item->text();
		QByteArray ba;
		ba = item->text().toLatin1().data();
		md->setData("application/x-item", ba);
		md->setText(texts.join(QStringLiteral("\n")));
		md->setData("tom_content",item->data(Qt::EditRole).toString().toLatin1()) ;
		md->setData("tom_content_type",item->data(Qt::EditRole+1).toString().toLatin1()) ;
		break ;
	}
	return md;
}

void CxResListWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat("application/x-item"))
		event->accept();
	else
		event->ignore();
}

Qt::DropAction CxResListWidget::supportedDropActions()
{
	return Qt::MoveAction;
}

void CxResListWidget::startDrag(Qt::DropActions supportedActions)
{
    m_lastDropIndex = -1 ;
	QListWidgetItem* item = currentItem();
//    getLabel(item)->emitSignal(false) ;
    m_dragIndex = row(item) ;
	QList<QListWidgetItem*> itemList ;
	itemList<<item ;
//	QMimeData *md = mimeData(itemList);
    QMimeData* md = new QMimeData;

	QByteArray ba;
	ba = item->text().toLatin1().data();
	md->setText("SSS") ;
	md->setData("application/x-item", ba);
	md->setData("tom_content",item->data(Qt::EditRole).toString().toLatin1()) ;
	md->setData("tom_content_type",item->data(Qt::EditRole+1).toString().toLatin1()) ;
	
	QDrag* drag = new QDrag(this);
	drag->setMimeData(md);
//	int sz = getLabel(item)->size() ;
    CxContentLabel* lb = getLabel(item) ;
	if(!lb) return ;
	drag->setPixmap(lb->grab() ) ;
    drag->setHotSpot(lb->mapFromGlobal(QCursor::pos()));
    takeItem(m_dragIndex) ;
	int ret = drag->exec(Qt::MoveAction) ;
	if ( ret == Qt::MoveAction) {
	}
	else
	{
		QString str=  item->data(Qt::EditRole).toString() ;
		if( item->data(Qt::EditRole+1).toInt() == 0 ) addText(m_dragIndex,str) ;
		else addImage(m_dragIndex,str) ;
	}
    if( m_lastDropIndex != -1 ) takeItem(m_lastDropIndex) ;
//	QListWidget::startDrag(supportedActions) ;
//	return ;
    /*
    QListWidgetItem* item = currentItem();
    QMimeData* mimeData = new QMimeData;
    QByteArray ba;
    ba = item->text().toLatin1().data();
    mimeData->setData("application/x-item", ba);
    mimeData->setData("tom_content",item->data(Qt::EditRole).toString().toLatin1()) ;
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
//		delete takeItem(row(item));
//		emit itemDroped();
    }
    */
}

void CxResListWidget::showEvent(QShowEvent* event)
{
	QListWidget::showEvent(event) ;
    refit() ;
}

void CxResListWidget::resizeEvent(QResizeEvent* event)
{
	QListWidget::resizeEvent(event) ;
	refit() ;
}

void CxResListWidget::refit()
{
    int cnt = count() ;
	int w = (viewport()->width()-spacing()*3)/2-10;
//	setGridSize(QSize(w, TEXTITEMH));
// 	return ;
//	setIconSize(QSize(w,TEXTITEMH)) ;
    int ww = width() ;
    for( int i = 0; i < cnt; i++ )
	{
		QListWidgetItem* var = item(i) ;
        var->setSizeHint(QSize(w,TEXTITEMH)) ;
        CxContentLabel* lb = getLabel(var) ;
		if( lb ) {
            lb->resize(w,TEXTITEMH) ;
			if(m_typeList[i] == IMAGETYPE)
			{
                lb->setPixmap(QPixmap(m_contentList[i]).scaled(w,TEXTITEMH,Qt::KeepAspectRatio,Qt::SmoothTransformation)) ;
			}
		}
    }
}

void CxResListWidget::onShowPreview( QListWidgetItem* item, bool on, QPoint pnt )
{
	if( on )
	{
	}
	else
	{
		m_preView->hide() ;
	}
}

void CxResListWidget::onShowContentHover(QListWidgetItem* src, bool on, QPoint pos )
{
    QPoint tpLeft ;
    if( !src->listWidget() ) return ;
    CxContentLabel* lb = getLabel(src) ;
    if( !lb ) return ;
    int id = row(src) ;
    QPoint pnt = lb->geometry().topLeft() ;
    if( id % 2 )
    {
        CxContentLabel* prev = getLabel(item(id-1)) ;
        if( prev ) pnt = prev->geometry().topLeft() ;
    }
    if( lb )emit __showContentHover(this,src,on,mapToGlobal(pnt));
}
