#include "textedit.h"
#include <QInputDialog>
#include <QDir>
#include <QScreen>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QObject>
#include <QScrollBar>
#include <QColorDialog>
#include <QFontDatabase>
#include <QPrinter>
#include <QFileDialog>
#include <QPalette>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QTextDocumentWriter>
#include <QTimerEvent>
#include <QStandardPaths>
#include <QDataStream>
#include <QPrintDialog>
#include "centeredtoolbuttonstyle.h"
#include <QTextCodec>
#include <QDomDocument>
#include <QProcess>
#include <QTextFrame>
#include <QImageReader>

#define TITLEHEIGHT 30

TextEdit::TextEdit(QWidget *parent /* = 0 */)
{
    ui.setupUi(this) ;
    m_curDirPath = "" ;
    ui.textEdit->setUndoRedoEnabled(false);
    QDir().mkpath("res/image") ;
    QDir().mkpath("res/tmp") ;

    m_version = 100 ;
    m_fixContentList = true ;

    m_pageMenu = new CxPageMenu(this) ;
	m_pageMenu->hide() ;
	m_autoSaveTimer = new QTimeLine(120000,this) ;
	m_collectDlg = new CxAllChapterDlg ;
	m_wordHandler = new CxWordHandler ;
    m_contentHoverW = new CxContentHover(this) ;
    m_contentHoverW->hide();
    m_addChapLb = new QToolButton(this) ;
    m_addChapLb->hide() ;
    m_addChapLb->setText("ADD AS CHAP");
    m_addChapLb->setFont(QFont("impact",20)) ;

    initUI() ;
	initConnection() ;
    init() ;

	m_textDlg = new CxSmallText ;
	ui.lw_res_1->setIndex(0) ;
	ui.lw_res_2->setIndex(1) ;
	m_isRemoteTextChange = false ;
    m_isAutoSave = false ;

//    load() ;
    onDocumentChanged(false) ;

    m_timerMode = PAGEMENUTIMER ;

    ui.tb_undo->setEnabled(false) ;
    ui.tb_redo->setEnabled(false) ;

    m_uploadPanel = new CxUploadWidget(this) ;
    m_uploadPanel->hide() ;
    connect( m_uploadPanel->form().tb_add_file, SIGNAL(clicked()), this, SLOT(onUploadFile())) ;
    connect( m_uploadPanel->form().tb_add_folder, SIGNAL(clicked()), this, SLOT(onUploadFolder())) ;
    m_uploadPanel->setWindowFlags(Qt::Widget|Qt::Popup) ;

//    QString str = "" ;
//    for( int i  = 0; i < 1000; i++ ) str.append(" ") ;
//    ui.textEdit->setPlainText(str) ;
//    ui.tb_insert_image->hide() ;
}


void TextEdit::initUI()
{	

    ui.w_backgroundcolor->hide() ;
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint );

	QDesktopWidget* myWidget = new QDesktopWidget;
	int w = 6.93*myWidget->logicalDpiX() ;
	int h = 9.84*myWidget->logicalDpiY() ;
	m_pageSize = QSize(w,h) ;
    m_b5Size = QPageSize(QPageSize::B5) ;
    ui.w_textEdit->setFixedWidth(m_b5Size.sizePixels(myWidget->logicalDpiX()).width()+40) ;

	ui.cb_fontsize->setEditable(true);
	ui.w_textcolor->setIndex(TEXTCOLOR) ;
	ui.w_highlightcolor->setIndex(HIGHLIGHTCOLOR) ;
	ui.w_backgroundcolor->setIndex(FOREGROUNDCOLOR) ;

	QMenu* fileMenu = new QMenu ;
	QMenu* textMenu = new QMenu;
	fileMenu->setFont(QFont("impact",16)) ;
	QAction* newAction = fileMenu->addAction("New") ;
    QAction* openAction = fileMenu->addAction("Open") ;
    QAction* saveAction = fileMenu->addAction("Save") ;
	QMenu* saveMenu = new QMenu;
	saveMenu->setTitle("Save As") ;
	fileMenu->addMenu(saveMenu) ;
	saveMenu->setFont(QFont("impact",16)) ;
	QAction* saveToPdf = saveMenu->addAction("Save to PDF") ;
	QAction* saveToODT = saveMenu->addAction("Save to ODT") ;
	ui.tb_menu_file->setMenu(fileMenu) ;

	textMenu->setFont(QFont("impact",16)) ;
	QAction* collectAction = textMenu->addAction("Collect All") ;
	m_autoSaveAction = textMenu->addAction("AutoSave") ;
	m_autoSaveAction->setCheckable(true) ;
    m_autoSaveAction->setChecked(false) ;
	ui.tb_menu_text->setMenu(textMenu) ;

	connect( saveAction, SIGNAL(triggered()), this, SLOT(onSave())) ;
    connect( openAction, SIGNAL(triggered()), this, SLOT(onOpen())) ;
    connect( newAction, SIGNAL(triggered()), this, SLOT(onNew())) ;
	connect( saveToODT, SIGNAL(triggered()), this, SLOT(onODT())) ;
	connect( saveToPdf, SIGNAL(triggered()), this, SLOT(onPdf())) ;

	connect( collectAction, SIGNAL(triggered()), this, SLOT(onCollectAll())) ;
	connect( m_autoSaveAction, SIGNAL(triggered()), this, SLOT(onAutoSave())) ;
	connect( ui.tb_menu_text, SIGNAL(clicked()),ui.tb_menu_text, SLOT(showMenu())) ;
	connect( ui.tb_menu_file, SIGNAL(clicked()),ui.tb_menu_file, SLOT(showMenu())) ;

    m_isMaximized = true ;
	m_isPressing = false ;

}

void TextEdit::initConnection()
{
    connect( m_addChapLb, SIGNAL(clicked()), this, SLOT(onAddAsChapter())) ;
    connect( ui.tb_red_icon, SIGNAL(__showRedBookPanel(bool)), this, SLOT(onShowAddChapLabel(bool))) ;
    connect( ui.tb_insert_image, SIGNAL(clicked()), this, SLOT(onInsertImage())) ;
    connect( ui.lw_res_1, SIGNAL(__showContentHover(CxResListWidget*,QListWidgetItem*,bool,QPoint)), this, SLOT(onShowContentHover(CxResListWidget* , QListWidgetItem*, bool, QPoint))) ;
    connect( ui.lw_res_2, SIGNAL(__showContentHover(CxResListWidget*,QListWidgetItem*,bool,QPoint)), this, SLOT(onShowContentHover(CxResListWidget* , QListWidgetItem*, bool, QPoint))) ;

    connect( ui.textEdit, SIGNAL(__resizeImage()), this, SLOT(resizeImage())) ;
    connect( ui.tb_collect, SIGNAL(clicked()), this, SLOT(onCollectAll())) ;
//	connect( ui.tb_copy_all, SIGNAL(clicked()), this, SLOT(onCopyAll())) ;
	connect( ui.tb_minimize, SIGNAL(clicked()), this, SLOT(showMinimized())) ;
	connect( ui.tb_maximize, SIGNAL(clicked()), this, SLOT(onMaximize())) ;
	connect( ui.tb_close, SIGNAL(clicked()), this, SLOT(close())) ;


    connect( ui.textEdit, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool))) ;
    connect( ui.textEdit, SIGNAL(redoAvailable(bool)), this, SLOT(onRedoAvailable(bool))) ;

	connect( m_autoSaveTimer, SIGNAL(finished()), this, SLOT(onSave())) ;
	connect( ui.lw_chapter_list, SIGNAL(__changed()), this, SLOT(onChapterChanged())) ;
	connect( ui.lw_chapter_list, SIGNAL(__preChanged(int,bool)), this, SLOT(onPreChanged(int,bool))) ;
	connect( ui.lw_chapter_list, SIGNAL(__swap(int,int)), this, SLOT(onSwapChapter(int,int)) );
	connect( ui.lw_res_1, SIGNAL(__changed(int)), this, SLOT(onContentChanged(int))) ;
	connect( ui.lw_res_2, SIGNAL(__changed(int)), this, SLOT(onContentChanged(int))) ;
	connect( ui.lw_res_1, SIGNAL(__moveContent(int,QListWidgetItem*)), this, SLOT(onMoveContent(int,QListWidgetItem*))) ;
	connect( ui.lw_res_2, SIGNAL(__moveContent(int,QListWidgetItem*)), this, SLOT(onMoveContent(int,QListWidgetItem*))) ;
    connect( ui.tb_start, SIGNAL(clicked()), this, SLOT(onSave()));
	connect( ui.tb_add_chapter, SIGNAL(clicked()), this, SLOT(onAddChapter())) ;
	connect( ui.lw_chapter_list, SIGNAL(__selectChapter(int)), this, SLOT(onSelectChapter(int))) ;
	connect( ui.textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged())) ;
	connect( ui.textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int))) ;
	connect( ui.tb_prev_page, SIGNAL(clicked()), this, SLOT(onPrevPage())) ;
	connect( ui.tb_next_page, SIGNAL(clicked()), this, SLOT(onNextPage())) ;
    connect( ui.tb_upload, SIGNAL(clicked()),  this, SLOT(onShowUploadPanel())) ;
	connect( ui.tb_create_text, SIGNAL(clicked()),  this, SLOT(onCreateText())) ;
	connect( ui.tb_res_1, SIGNAL(clicked()),  this, SLOT(onContentPlus())) ;
	connect( ui.tb_res_2, SIGNAL(clicked()),  this, SLOT(onContentMinus())) ;
	connect( ui.tb_undo, SIGNAL(clicked()), this, SLOT(onUndo())) ;
	connect( ui.tb_redo, SIGNAL(clicked()), this, SLOT(onRedo())) ;
	connect( ui.tb_next_chapter, SIGNAL(clicked()), ui.lw_chapter_list, SLOT(onNext())) ;
	connect( ui.tb_prev_chapter, SIGNAL(clicked()), ui.lw_chapter_list, SLOT(onPrevious())) ;
	connect(ui.tb_pdf, SIGNAL(clicked()), this, SLOT(onPdf())) ;
	connect(ui.tb_print, SIGNAL(clicked()), this, SLOT(onPrint())) ;
	connect( ui.tb_save, SIGNAL(clicked()), this, SLOT(onSave())) ;
	connect( ui.tb_show_page, SIGNAL(__showPageMenu(bool)), this, SLOT(onShowPageMenu(bool))) ;
	connect( m_pageMenu, SIGNAL(__selectPage(int)), this, SLOT(onMovePage(int))) ;
	for( int i = 1; i <= 5; i++ )
	{
		CxPageLabel* lb = (CxPageLabel*)(findChild<QLabel*>(QString("lb_page_%1").arg(i)));
		lb->setIndex(i) ;
		connect( lb, SIGNAL(__movePage(int)), this, SLOT(onMovePage(int))) ;
	}

	QFontDatabase db;
	foreach(int size, db.standardSizes())
		ui.cb_fontsize->addItem(QString::number(size));

	connect(ui.textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
		this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    connect(ui.textEdit, SIGNAL(__showTextFormat()), this, SLOT(onShowTextFormat())) ;
	connect(ui.textEdit, SIGNAL(cursorPositionChanged()),this, SLOT(onShowTextFormat()));
	connect(ui.cb_font, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));
	connect(ui.cb_fontsize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
	connect(ui.tb_bold, SIGNAL(clicked()), this, SLOT(textBold()));
	connect(ui.tb_italic, SIGNAL(clicked()), this, SLOT(textItalic()));
	connect(ui.tb_underline, SIGNAL(clicked()), this, SLOT(textUnderline()));
	connect(ui.tb_left, SIGNAL(clicked()), this, SLOT(onTextLeft()));
	connect(ui.tb_center, SIGNAL(clicked()), this, SLOT(onTextCenter()));
	connect(ui.tb_right, SIGNAL(clicked()), this, SLOT(onTextRight()));
	connect(ui.tb_font_inc, SIGNAL(clicked()), this, SLOT(onFontSizeIncrease())) ;
	connect(ui.tb_font_dec, SIGNAL(clicked()), this, SLOT(onFontSizeDecrease())) ;
	connect(ui.w_textcolor, SIGNAL(__requireMenu(CxColorWidget*)),this, SLOT(onRequireColorMenu(CxColorWidget*))) ;
	connect(ui.w_textcolor,SIGNAL(__colorChanged(QColor)),this,SLOT(onTextColorChanged(QColor))); 

	connect(ui.w_highlightcolor, SIGNAL(__requireMenu(CxColorWidget*)),this, SLOT(onRequireColorMenu(CxColorWidget*))) ;
	connect(ui.w_highlightcolor,SIGNAL(__colorChanged(QColor)),this,SLOT(onHightlightColorChanged(QColor))); 

	connect(ui.w_backgroundcolor, SIGNAL(__requireMenu(CxColorWidget*)),this, SLOT(onRequireColorMenu(CxColorWidget*))) ;
	connect(ui.w_backgroundcolor,SIGNAL(__colorChanged(QColor)),this,SLOT(onTextBackgroundColorChanged(QColor))); 

}

void TextEdit::onAddChapter()
{
	bool ok;
	QString text = QInputDialog::getText(NULL, tr("New  "),
		tr("Chapter Name:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty())
	{
		addChapter(text) ;
	}
}

void TextEdit::onAddAsChapter()
{
    bool ok;
    QString content = ui.textEdit->toHtml() ;
    QString text = QInputDialog::getText(NULL, tr("New  "),
        tr("Chapter Name:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        int cnt = m_chapterList.count() ;
        QStringList contentList[2] ;
        QList<int> contentTypeList[2] ;
        contentList[0] = ui.lw_res_1->contentList() ;
        contentList[1] = ui.lw_res_2->contentList() ;
        contentTypeList[0] = ui.lw_res_1->contentTypeList() ;
        contentTypeList[1] = ui.lw_res_2->contentTypeList() ;
        addChapter(text) ;
        ui.textEdit->setHtml(content);
        if( !cnt )
        {
            if( !m_fixContentList || 1 )
            {
                ui.lw_res_1->setData(contentList[0],contentTypeList[0]) ;
                ui.lw_res_2->setData(contentList[1],contentTypeList[1]) ;
            }
        }
    }
}

void TextEdit::addChapter(QString text)
{
	setEditable(true) ;
	m_chapterList << "" ;
	QTextDocument* doc = new QTextDocument ;
//	doc->setDefaultStyleSheet("QTextDocument{background:red;}") ;
    m_docList << doc ;
    //		doc->setTextWidth(m_pageSize.width()) ;
    doc->setDefaultFont(QFont("Arial",11)) ;
//    doc->setPageSize(m_pageSize) ;
    ui.lw_chapter_list->addChapter(text) ;

    //	doc->setDefaultStyleSheet("body { color : green; background-color : black; }");
}

void TextEdit::onStartNewBook()
{
    onNew() ;
    return ;
    /*
	if( m_curBookName.length() )//means SAVE button
	{
		onSave() ;
		return ;
	}
	bool ok;
	QString text = QInputDialog::getText(NULL, tr("Create New Book"),
		tr("Book Name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
	if (ok && !text.isEmpty())
	{
		startBook(text) ; 
        onAddChapter();
	}
    */
}

void TextEdit::startBook(QString src, bool isOpenMode)
{
    QString dirPath = QFileInfo(src).absolutePath() ;
	if( isOpenMode )
	{
		if( !QDir(dirPath).exists() )
		{
            QMessageBox::information(NULL,"","Book does n't exists.");
			return ;
		}
	}
	else
	{
		if( QDir(dirPath).exists() )
		{
			QMessageBox::information(NULL,"","Same name exists already."); 
			return ;
		}
		QDir().mkpath(dirPath) ;
		if( !QDir(dirPath).exists() )
		{
			QMessageBox::information(NULL,"","Please try again with another name"); 
			return ;
		}
	}

	m_curDirPath = dirPath ;
    m_curBookName = QFileInfo(dirPath).fileName() ;

    init() ;
    ui.tb_add_chapter->setEnabled(true) ;
	ui.tb_menu_file->setEnabled(true) ;
    ui.tb_menu_text->setEnabled(true) ;
    ui.w_cfmt_bar->setEnabled(true) ;
    ui.tb_start->setEnabled(true) ;
    setWindowTitle(m_curBookName) ;
//	ui.tb_start->setText("Save") ;
    if( m_isAutoSave ) m_autoSaveTimer->start() ;
}

void TextEdit::init()
{
    ui.lw_chapter_list->clear() ;
    ui.lw_res_1->clear() ;
    ui.lw_res_2->clear() ;

    m_chapterList.clear() ;
    for( int i = 0; i < 2; i++ )
    {
        m_contentList[i].clear() ;
        m_contentTypeList[i].clear() ;
    }
    m_docList.clear() ;
    /*
    setEditable(false) ;
    ui.tb_add_chapter->setEnabled(false) ;
	ui.tb_menu_file->setEnabled(false) ;
	ui.tb_menu_text->setEnabled(false) ;
	ui.w_cfmt_bar->setEnabled(false) ;
	ui.tb_start->setEnabled(true) ;
    */

	m_curChapter = -1 ;
	for( int i = 1; i <= 5; i++ )
	{
		QLabel* lb = findChild<QLabel*>(QString("lb_page_%1").arg(i));
		lb->setVisible(false) ;
	}
	ui.tb_prev_page->setVisible(false) ;
	ui.tb_next_page->setVisible(false) ;
	switchContent(0) ;
}

void TextEdit::setEditable(bool on)
{
	ui.w_pagination->setEnabled(on) ;
//	ui.textEdit->setEnabled(on) ;
    ui.w_text_control->setEnabled(on) ;
//	ui.w_content->setEnabled(on) ;
}

void TextEdit::onSelectChapter(int id)
{
	if( !m_docList.count() ) return ;
	m_curChapter = id ;
	ui.textEdit->setDocument(m_docList[id]) ;
	onTextChanged() ;

    if( !m_fixContentList )
    {
        ui.lw_res_1->setData(m_contentList[0][id],m_contentTypeList[0][id]) ;
        ui.lw_res_2->setData(m_contentList[1][id],m_contentTypeList[1][id]) ;
    }

    ui.tb_undo->setEnabled(false);
    ui.tb_redo->setEnabled(false);
    /*
    QPalette p = ui.textEdit->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::red); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::green); // set text color which is selected from color pallete
    ui.textEdit->setPalette(p);
    ui.textEdit->setStyleSheet("QTextEdit{background:green;}") ;
    ui.textEdit->setTextBackgroundColor(Qt::red) ;
    */
}

int TextEdit::getPageCount()
{
	if( !m_docList.count() ) return 1 ;
	int h = m_docList[m_curChapter]->size().height() ;
	int pg = h/m_pageSize.height() ;
	if( pg*m_pageSize.height() < h ) pg++ ;
	return pg ;
}

void TextEdit::onTextChanged()
{
	onDocumentChanged(true);
	int pg = getPageCount() ;
	m_pageMenu->setCount(pg) ;
	for( int i = 1; i <= 5; i++ )
	{
		QLabel* lb = findChild<QLabel*>(QString("lb_page_%1").arg(i));
		lb->setVisible((pg>=2&&i<=pg)?1:0) ;
	}
	ui.tb_prev_page->setVisible(pg>=2) ;
	ui.tb_next_page->setVisible(pg>=2) ;
}

void TextEdit::onSliderValueChanged(int val)
{
	//944
    ui.textEdit->setControlVisible(false);
	val = val + ui.textEdit->verticalScrollBar()->pageStep() ;
	m_curPage = val/m_pageSize.height() ;
	if( m_curPage*m_pageSize.height() <= val ) m_curPage++ ;
	for( int i = 1; i <= 5; i++ )
	{
		CxPageLabel* lb = (CxPageLabel*)(findChild<QLabel*>(QString("lb_page_%1").arg(i)));
		lb->setSelected(i==m_curPage) ;
	}
	m_pageMenu->setCurrentPage(m_curPage) ;
}

void TextEdit::onMovePage(int page)
{
	int totPage = getPageCount() ;
	if( page < 0 || page > totPage ) return ;
	int val = (page-1)*m_pageSize.height() ;
	ui.textEdit->verticalScrollBar()->setValue(val) ;
	ui.tb_prev_page->setEnabled(page>1) ;
	ui.tb_next_page->setEnabled(page<totPage) ;
}

void TextEdit::onPrevPage()
{
	onMovePage(m_curPage-1) ;
}

void TextEdit::onNextPage()
{
	onMovePage(m_curPage+1) ;
}

void TextEdit::onShowUploadPanel( bool on )
{
    QPoint pnt = ui.tb_upload->mapToGlobal(QPoint(0,0)) ;
    pnt = mapFromGlobal(QCursor::pos()) ;
    int w = 230 ;
    int h = 50 ;
//    QMessageBox::information(NULL,"",QString("%1").arg(on)) ;
    m_uploadPanel->setVisible(on);
    m_uploadPanel->setGeometry(QRect(pnt.x()-w,pnt.y()-h,w,h));
}

void TextEdit::onUploadFolder()
{
    QString src = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    if( src.length() == 0 ) return ;
    loadDir(src) ;
}

void TextEdit::loadDir(QString src)
{
    QDir dir(src);
    if (! dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
//        loadDir(src+ QDir::separator() + d);
    }

    foreach (QString f, dir.entryList(QDir::Files)) {
        QString fileName = src + QDir::separator() + f ;
        QString suf = QFileInfo(fileName).suffix().toLower() ;
        CxResListWidget* w = ui.lw_res_1 ;
        if( m_curLWTab ) w = ui.lw_res_2 ;
        if(suf.contains("txt"))
        {
            QString str;
            QFile file(fileName) ;
            file.open(QIODevice::ReadOnly) ;
            QTextStream in(&file);
            str = in.readAll() ;
            file.close() ;
            w->addText(str) ;
            continue ;
        }
        if(suf.contains("png") || suf.contains("jpg"))
        {
            QPixmap pix(fileName) ;
            if( pix.isNull() )
            {
                continue ;
            }
            QString filePath = QString("res/image/%1.%2").arg(createSimpleUuid()).arg(QFileInfo(fileName).suffix()) ;
            pix = pix.scaled(ui.textEdit->viewport()->width()-80,ui.textEdit->viewport()->height()-80,Qt::KeepAspectRatio) ;
            pix.save(filePath) ;
            w->addImage(filePath) ;
            continue ;
        }
        if( suf.contains("odt") )
        {
            QStringList arg ;
            QString dirPath = QString("res/tmp/%1").arg(createSimpleUuid()) ;
            QDir().mkpath(dirPath) ;
            QFile newFile(dirPath+"/"+"content.xml") ;
            arg << "x" << fileName << QString("-o%1").arg(dirPath) << "content.xml" << "-r" ;
            QProcess::execute( "7z",arg ) ;
            QTextEdit tmp ;
            QTextStream in(&newFile) ;
            if( newFile.open(QIODevice::ReadOnly) )
            {
                QString str = in.readAll();
                newFile.close() ;
                newFile.remove() ;
                tmp.setHtml(str) ;
                w->addText(tmp.toPlainText()) ;
            }
            QDir(dirPath).removeRecursively() ;
            continue ;
        }
        if( suf.contains("doc") ){
            QFile file(fileName) ;
            QString filePath = QString("%1/%2.%3").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(createSimpleUuid()).arg(QFileInfo(fileName).suffix()) ;
            file.copy(filePath) ;
            QApplication::setOverrideCursor(Qt::WaitCursor) ;
            QString content =m_wordHandler->readDoc(filePath) ;
            QApplication::restoreOverrideCursor() ;
            w->addText(content) ;
            QFile(filePath).remove() ;
        }

    }
}

void TextEdit::onUploadFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Upload File"),
		"",
        tr("Files (*.txt *.doc *.docx *.odt *.png *.jpg)")) ;
    if( fileName.length() == 0 ) return ;
	QString suf = QFileInfo(fileName).suffix().toLower() ;
	CxResListWidget* w = ui.lw_res_1 ;
	if( m_curLWTab ) w = ui.lw_res_2 ;
	if(suf.contains("txt"))
	{
		QString str;
		QFile file(fileName) ;
		file.open(QIODevice::ReadOnly) ;
		QTextStream in(&file);
		str = in.readAll() ;
		file.close() ;
		w->addText(str) ;
		return ;
	}
	if(suf.contains("png") || suf.contains("jpg"))
	{
		QPixmap pix(fileName) ;
		if( pix.isNull() )
		{
			QMessageBox::information(NULL,"","Invalid image format") ;
			return ;
		}
		QString filePath = QString("res/image/%1.%2").arg(createSimpleUuid()).arg(QFileInfo(fileName).suffix()) ;
        pix = pix.scaled(ui.textEdit->viewport()->width()-80,ui.textEdit->viewport()->height()-80,Qt::KeepAspectRatio) ;
        pix.save(filePath) ;
		w->addImage(filePath) ;
		return ;
	}
	if( suf.contains("odt") )
	{
		QStringList arg ;
		QString dirPath = QString("res/tmp/%1").arg(createSimpleUuid()) ;
		QDir().mkpath(dirPath) ;
		QFile newFile(dirPath+"/"+"content.xml") ;
 		arg << "x" << fileName << QString("-o%1").arg(dirPath) << "content.xml" << "-r" ;
		QProcess::execute( "7z",arg ) ;
		QTextEdit tmp ;
		QTextStream in(&newFile) ;
		if( newFile.open(QIODevice::ReadOnly) )
		{
			QString str = in.readAll();
			newFile.close() ;
			newFile.remove() ;
			tmp.setHtml(str) ;
			w->addText(tmp.toPlainText()) ;
		}
		QDir(dirPath).removeRecursively() ;
		return ;
	}
	QFile file(fileName) ;
	QString filePath = QString("%1/%2.%3").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(createSimpleUuid()).arg(QFileInfo(fileName).suffix()) ;
	file.copy(filePath) ;
	QApplication::setOverrideCursor(Qt::WaitCursor) ;
	QString content =m_wordHandler->readDoc(filePath) ;
	QApplication::restoreOverrideCursor() ;
	w->addText(content) ;
	QFile(filePath).remove() ;
	
}

void TextEdit::onCreateText()
{
	m_textDlg->setText("") ;
	if(m_textDlg->exec() == QDialog::Accepted)
	{
		QString txt = m_textDlg->form().textEdit->toPlainText() ;
		if( txt.length() == 0 ) return ;
		if( m_curLWTab == 0 )
		{
			ui.lw_res_1->addText(txt) ;
		}
		else{
			ui.lw_res_2->addText(txt) ;
		}
	}
}

void TextEdit::onContentPlus()
{
	switchContent(0) ;
}

void TextEdit::onContentMinus()
{
	switchContent(1) ;
}

void TextEdit::switchContent(int id)
{
	m_curLWTab = id ;
	QStringList wStyle, btnStyle ;
	wStyle << "QWidget{background:#fff200;}" << "QWidget{background:#c3c3c3;}" ;
	btnStyle << "QToolButton{background:#fff200;border:1px solid #c3c3c3;border-radius:15px;}" 
		<< "QToolButton{background:#ebebeb; border:1px solid #c3c3c3;border-radius:15px;}"  ;
//	ui.w_res_1->setStyleSheet(wStyle[id]) ;
//	ui.w_res_2->setStyleSheet(wStyle[!id]) ;
	ui.tb_res_1->setStyleSheet(btnStyle[id]) ;
	ui.tb_res_2->setStyleSheet(btnStyle[!id]) ;
	ui.lw_res_1->setVisible(id==0) ;
	ui.lw_res_2->setVisible(id==1) ;
}

void TextEdit::onUndo()
{
	ui.textEdit->undo() ;
	onShowTextFormat() ;
}

void TextEdit::onRedo()
{
	ui.textEdit->redo() ;
	onShowTextFormat() ;
}

//////////////////////////////////////////////////////////////////////////

void TextEdit::onContentChanged(int id)
{
    if( !m_chapterList.count() ) return ;
	CxResListWidget* w = ui.lw_res_1 ;
	if( id == 1 ) w = ui.lw_res_2 ;
	m_contentList[id][m_curChapter] = w->contentList() ;
	m_contentTypeList[id][m_curChapter] = w->contentTypeList() ;
	onDocumentChanged(true) ;
}

void TextEdit::onDocumentChanged(bool on)
{
	ui.tb_save->setEnabled(on) ;
}

void TextEdit::onPreChanged( int id, bool isAdd )
{
//    QMessageBox::information(NULL,"",QString("%1").arg(m_chapterList.count())) ;
    if( m_chapterList.count() == 0 ) return ;
	onDocumentChanged(true);
	if( isAdd ) {
		QStringList contentList;
		QList<int> typeList ;
		m_contentList[0] << contentList ;
		m_contentTypeList[0] << typeList ;
		m_contentList[1] << contentList ;
		m_contentTypeList[1] << typeList ;
	}
	else
	{
		m_contentList[0].takeAt(id) ;
		m_contentTypeList[0].takeAt(id) ;
		m_contentList[1].takeAt(id) ;
		m_contentTypeList[1].takeAt(id) ;
		m_docList.takeAt(id) ;
	}
}

void TextEdit::onSwapChapter(int s, int e)
{
	onDocumentChanged(true);
	m_contentTypeList[0].swap(s,e) ;
	m_contentTypeList[1].swap(s,e) ;
	m_contentList[0].swap(s,e) ;
	m_contentList[1].swap(s,e) ;
	m_docList.swap(s,e) ;
}

void TextEdit::onChapterChanged()
{
	m_chapterList = ui.lw_chapter_list->chapterList() ;
	int cnt = m_chapterList.count() ;
//	setEnabled(cnt) ;
//    ui.w_content->setEnabled(cnt) ;
	ui.w_chapter_control->setEnabled(cnt) ;
}

void TextEdit::onMoveContent( int index, QListWidgetItem* item )
{
	CxResListWidget* w = ui.lw_res_1 ;

	if( index == 0 ) w = ui.lw_res_2 ; 
	int t = item->data(Qt::EditRole+1).toInt() ;
	if( t == TEXTTYPE ) w->addText(item->data(Qt::EditRole).toString()) ;
	else w->addImage(item->data(Qt::EditRole).toString()) ;
}

//////////////////////////////////////////////////////////////////////////

void TextEdit::fontChanged(const QFont &f)
{
	ui.cb_font->setCurrentIndex(ui.cb_font->findText(QFontInfo(f).family()));
	ui.cb_fontsize->setCurrentIndex(ui.cb_fontsize->findText(QString::number(f.pointSize())));
	ui.tb_bold->setChecked(f.bold());
	ui.tb_italic->setChecked(f.italic());
	ui.tb_underline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
	QPixmap pix(16, 16);
	pix.fill(c);
//	ui.tb_textcolor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
	if (a & Qt::AlignLeft)
		ui.tb_left->setChecked(true);
	else if (a & Qt::AlignHCenter)
		ui.tb_center->setChecked(true);
	else if (a & Qt::AlignRight)
		ui.tb_right->setChecked(true);
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = ui.textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	ui.textEdit->mergeCurrentCharFormat(format);
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
//	onDocumentChanged(true);
//	fontChanged(format.font());
//	colorChanged(format.foreground().color());
}

void TextEdit::onShowTextFormat()
{
	QTextCursor cursor = ui.textEdit->textCursor() ;
	m_isRemoteTextChange = true ;
	QTextCharFormat cfmt = cursor.charFormat() ;
	QFont f = cfmt.font() ;
	QTextBlockFormat bfmt = cursor.blockFormat() ; 
	ui.tb_bold->setChecked(cfmt.fontWeight()==QFont::Bold) ;
	ui.tb_italic->setChecked(cfmt.fontItalic()) ;
	ui.tb_underline->setChecked(cfmt.fontUnderline()) ;
	ui.tb_left->setChecked(bfmt.alignment()==Qt::AlignLeft) ;
	ui.tb_right->setChecked(bfmt.alignment()==Qt::AlignRight) ;
	ui.tb_center->setChecked(bfmt.alignment()==Qt::AlignHCenter) ;
	ui.cb_font->setCurrentIndex(ui.cb_font->findText(QFontInfo(f).family()));
	ui.cb_fontsize->setCurrentIndex(ui.cb_fontsize->findText(QString::number(f.pointSize())));
//	ui.w_textcolor->setColor(cfmt.foreground().color()) ;
	m_isRemoteTextChange = false ;
//	alignmentChanged(ui.textEdit->alignment());
}

void TextEdit::textBold()
{
	if( m_isRemoteTextChange ) return ;
	QTextCharFormat fmt;
	fmt.setFontWeight(ui.tb_bold->isChecked() ? QFont::Bold : QFont::Normal);
	QTextCursor cursor = ui.textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(fmt);

//	mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
	if( m_isRemoteTextChange ) return ;
	QTextCharFormat fmt;
	fmt.setFontUnderline(ui.tb_underline->isChecked());
	QTextCursor cursor = ui.textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(fmt);
}

void TextEdit::textItalic()
{
	if( m_isRemoteTextChange ) return ;
	QTextCharFormat fmt;
	fmt.setFontItalic(ui.tb_italic->isChecked());
	QTextCursor cursor = ui.textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(fmt);
}

void TextEdit::textFamily(const QString &f)
{
	if( m_isRemoteTextChange ) return ;
	QTextCharFormat fmt;
	fmt.setFontFamily(f);
	QTextCursor cursor = ui.textEdit->textCursor();
	cursor.mergeCharFormat(fmt);
//	mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
	if( m_isRemoteTextChange ) return ;
	qreal pointSize = p.toFloat();
	if (p.toFloat() > 0) {
		QTextCharFormat fmt;
		fmt.setFontPointSize(pointSize);
		QTextCursor cursor = ui.textEdit->textCursor();
		cursor.mergeCharFormat(fmt);
//		mergeFormatOnWordOrSelection(fmt);
	}
}

void TextEdit::onTextColorChanged(QColor col)
{
	QTextCharFormat fmt;
	fmt.setForeground(col);
	QTextCursor cursor = ui.textEdit->textCursor();
	cursor.mergeCharFormat(fmt);
}

void TextEdit::textColor()
{
	QColor col = QColorDialog::getColor(ui.textEdit->textColor(), this);
	if (!col.isValid())
		return;
	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
}

void TextEdit::onTextLeft()
{
	if( m_isRemoteTextChange ) return ;
	ui.textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
}

void TextEdit::onTextRight()
{
	if( m_isRemoteTextChange ) return ;
	ui.textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
}

void TextEdit::onTextCenter()
{
	if( m_isRemoteTextChange ) return ;
	ui.textEdit->setAlignment(Qt::AlignHCenter);
}

void TextEdit::onFontSizeIncrease()
{
	QTextCursor cursor = ui.textEdit->textCursor() ;
	int cur = cursor.charFormat().font().pointSize() ;
	cur += 2 ;
	QTextCharFormat fmt;
	fmt.setFontPointSize(cur);
	cursor.mergeCharFormat(fmt);
	int id = ui.cb_fontsize->findText(QString::number(cur)) ;
	if( id != -1 ) ui.cb_fontsize->setCurrentIndex(id);
	else ui.cb_fontsize->setEditText(QString::number(cur));
}

void TextEdit::onFontSizeDecrease()
{
	QTextCursor cursor = ui.textEdit->textCursor() ;
	int cur = cursor.charFormat().font().pointSize() ;
	if( cur < 4 ) return ;
	cur -= 2 ;
	QTextCharFormat fmt;
	fmt.setFontPointSize(cur);
	cursor.mergeCharFormat(fmt);
	int id = ui.cb_fontsize->findText(QString::number(cur)) ;
	if( id != -1 ) ui.cb_fontsize->setCurrentIndex(id);
	else ui.cb_fontsize->setEditText(QString::number(cur));
}

void TextEdit::onNew()
{
    if( m_chapterList.count() )
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(NULL, tr("Confirm"),
            tr("The document has been modified.\n"
            "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            onSave();
        }
    }

    init() ;
    m_curBookName = "" ;
    m_curDirPath = "" ;
    ui.textEdit->setUndoRedoEnabled(false);
    ui.textEdit->setHtml("");
    ui.textEdit->setUndoRedoEnabled(true);
    ui.tb_undo->setEnabled(false);
    ui.tb_redo->setEnabled(false);
    /*
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Create New Book"),
        tr("Book Name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
    {
        startBook(text) ;
        onAddChapter();
    }
    */
}

void TextEdit::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Book"),"",tr("Files (*.config)")) ;
    if( !fileName.length() ) return ;
//    QString fileName = "D://imagetest1/content.config" ;
    startBook(fileName,true) ;
    QFile file(m_curDirPath+"/content.config") ;
    QDataStream in(&file) ;
    int cnt = 0, sz, contentType ;
    QString content ;
    if( file.open(QIODevice::ReadOnly) )
    {
        in >> cnt ;
        QString chapter ;
        if( !cnt )
        {
            QString txt ;
            in >> txt ;
            ui.textEdit->setUndoRedoEnabled(false);
            ui.textEdit->setHtml(txt);
            int k ;
            in >> k ;
            QStringList contentList ;
            QList<int> typeList ;
            for( int i = 0; i < k; i++ )
            {
                in >> contentType >> content  ;
                contentList << content ;
                typeList << contentType ;
            }
            ui.lw_res_1->setData(contentList,typeList) ;
            in >> k ;
            contentList.clear() ;
            typeList.clear() ;
            for( int i = 0; i < k; i++ )
            {
                in >> contentType >> content  ;
                contentList << content ;
                typeList << contentType ;
            }
            ui.lw_res_2->setData(contentList,typeList) ;

            ui.textEdit->setUndoRedoEnabled(true);
        }
        for( int i = 0; i < cnt; i++ )
        {
            in >> chapter ;
            addChapter(chapter) ;
            in >> sz ;
            QStringList contentList ;
            QList<int> typeList ;
            for( int j = 0; j < sz; j++ )
            {
                in >> contentType >> content  ;
                contentList << content ;
                typeList << contentType ;
            }
            if( !m_fixContentList || !i ) ui.lw_res_1->setData(contentList,typeList) ;

            in >> sz ;
            contentList.clear() ;
            typeList.clear() ;
            for( int j = 0; j < sz; j++ )
            {
                in >> contentType >> content  ;
                contentList << content ;
                typeList << contentType ;
            }
            if( !m_fixContentList || !i ) ui.lw_res_2->setData(contentList,typeList) ;
        }
    }
    for( int i = 0; i < cnt; i++ )
    {
        QFile file(QString("%1/%2.dat").arg(m_curDirPath).arg(i+1)) ;
        if( !file.exists() ) continue ;
        QDataStream in(&file) ;
        if( file.open(QIODevice::ReadOnly) )
        {
            QString str ;
            in >> str ;
            m_docList[i]->setHtml(str) ;
        }
    }
    if( m_chapterList.count() ) ui.lw_chapter_list->setCurrentRow(0) ;
}

void TextEdit::onSave()
{
    if( m_chapterList.count() == 0 )
    {
    }
	if( m_autoSaveTimer->state() == QTimeLine::Running )
	{
		m_autoSaveTimer->stop() ;
	}
    if( m_curDirPath.length() == 0 )
    {
        QString path = QFileDialog::getSaveFileName(this, tr("Save Directory"));
        if( path.length() == 0 ) return ;
        QDir().mkpath(path) ;
        m_curDirPath = path ;
        m_curBookName = QFileInfo(path).fileName() ;
    }
	QApplication::setOverrideCursor(Qt::WaitCursor) ;
	saveContent() ;
	saveText() ;
	onDocumentChanged(false) ;
	if( m_isAutoSave )
	{
		m_autoSaveTimer->setCurrentTime(0) ;
		m_autoSaveTimer->start() ;
	}
	QApplication::restoreOverrideCursor() ;
}

void TextEdit::onRequireColorMenu(CxColorWidget* w)
{
	CxColorMenu menu(this) ;
	QRect rc = w->geometry() ;
	QPoint st = rc.topLeft() ;
	st = w->mapToGlobal(QPoint(0,0)) ;
	menu.setGeometry(st.x(),st.y()+w->height()+5,COLORMENUW,COLORMENUH) ;
	if ( menu.exec() == QDialog::Accepted )
	{
		w->setColor(menu.myColor()) ;
	}
}

void TextEdit::onTextBackgroundColorChanged(QColor col)
{
	ui.textEdit->setStyleSheet(QString("QTextEdit{background:%1;}").arg(col.name())) ;
}

void TextEdit::onHightlightColorChanged(QColor col)
{
	QTextCharFormat fmt;
	fmt.setBackground(col);
	QTextCursor cursor = ui.textEdit->textCursor();
	cursor.mergeCharFormat(fmt);
}

void TextEdit::onAutoSave()
{
	m_isAutoSave = !m_isAutoSave ;
	m_autoSaveAction->setChecked(m_isAutoSave) ;
	if( m_isAutoSave )
	{
		m_autoSaveTimer->start() ;
	}
	else
	{
		m_autoSaveTimer->stop() ;
	}
}

void TextEdit::onCollectAll()
{
	m_collectDlg->setData(m_docList, ui.lw_chapter_list->chapterList()) ;
	m_collectDlg->exec() ;
}

//////////////////////////////////////////////////////////////////////////

void TextEdit::onPdf()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save to PDF"),"",tr("Pdf files (*.pdf)"));	
	if( fileName.isEmpty() ) return ;
	QPrinter printer(QPrinter::ScreenResolution);
	printer.setPaperSize(QPrinter::B5);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName( fileName );
	printer.setPageMargins(0.3, 0.3, 0.3, 0.3, QPrinter::Inch);

	QSizeF paperSize;
	paperSize.setWidth(printer.width());
	paperSize.setHeight(printer.height());
    ui.textEdit->document()->setPageSize(paperSize); // the document needs a valid PageSize
	ui.textEdit->document()->print(&printer);
	QMessageBox::information(NULL,"","Saved to PDF") ;
}

void TextEdit::onPrint()
{
	QPrinter printer(QPrinter::ScreenResolution);
	printer.setPageSize(QPagedPaintDevice::B5) ;
	printer.setPageMargins(0.3, 0.3, 0.3, 0.3, QPrinter::Inch);
	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (ui.textEdit->textCursor().hasSelection()) dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
	dlg->setWindowTitle(tr("Print Document"));

	QApplication::setOverrideCursor(Qt::WaitCursor) ;
	QSizeF paperSize;
	paperSize.setWidth(printer.width());
	paperSize.setHeight(printer.height());
    ui.textEdit->document()->setPageSize(paperSize); // the document needs a valid PageSize
	if (dlg->exec() == QDialog::Accepted)
		ui.textEdit->print(&printer);
	QApplication::restoreOverrideCursor() ;
	delete dlg;
}

void TextEdit::onODT()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save to ODT file"),"",tr("ODT files (*.odf)"));	
	if( !fileName.length() ) return ;
	QTextDocumentWriter writer(fileName) ;
	writer.setFormat("odf") ;
	writer.write(ui.textEdit->document()) ;
}


void TextEdit::onShowPageMenu( bool on )
{
	if( on )
	{
		int pg = getPageCount() ;
		m_pageMenu->setCount(pg) ;
		if( pg <= 5 ) return ;

		QPoint pnt = mapFromGlobal(ui.tb_show_page->mapToGlobal(QPoint(0,0))) ;
		m_pageMenu->setGeometry(pnt.x()+40,pnt.y(), m_pageMenu->prefWidth(), m_pageMenu->prefHeight() ) ;
		m_pageMenu->setVisible(true) ;
	}
	else
	{
        m_timerMode = PAGEMENUTIMER ;
		m_timer.start(100,this) ;
	}
}

void TextEdit::timerEvent(QTimerEvent* event)
{
    if( event->timerId() != m_timer.timerId() ) return ;
    if( m_timerMode == PAGEMENUTIMER )
    {
        if( m_pageMenu->isVisible() )
        {
            QRect rc = m_pageMenu->geometry() ;
            if( rc.contains(mapFromGlobal(QCursor::pos()))) return ;
            m_pageMenu->hide();
        }
        m_timer.stop() ;
    }
    if( m_timerMode == CONTENTMENUTIMER )
    {
        if( m_contentHoverW->isVisible() )
        {
            QRect rc = m_contentHoverW->geometry() ;
            if( rc.contains(mapFromGlobal(QCursor::pos()))) return ;
            m_contentHoverW->hide();
        }
        m_timer.stop() ;
    }
    if( m_timerMode == ADDCHAPTIMER )
    {
        if( m_addChapLb->isVisible() )
        {
            QRect rc = m_addChapLb->geometry() ;
            if( rc.contains(mapFromGlobal(QCursor::pos()))) return ;
            m_addChapLb->hide();
        }
        m_timer.stop() ;
    }
}

QString TextEdit::createSimpleUuid()
{
	QString ret = QUuid::createUuid().toString() ;
	ret.chop(1) ;
	ret = ret.right(ret.length()-1) ;
	return ret ;
}

void TextEdit::closeEvent(QCloseEvent* event)
{
    if( ui.tb_save->isEnabled() )
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(NULL, tr("Confirm"),
			tr("The document has been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save) {
            onSave();
//			saveContent() ;
//			saveText() ;
		}
		if (ret == QMessageBox::Cancel) {
			event->ignore() ;
			return ;
		}
	}
	event->accept() ;
}

void TextEdit::saveContent()
{
	if( !m_curBookName.length() ) return ; 
	int cnt = m_contentList[0].count() ;
	QFile file(m_curDirPath+"/"+"content.config") ;
	QDataStream out(&file) ;
	file.open(QIODevice::WriteOnly) ;
	out << cnt ;
	int sz, j ;
	for( int i = 0; i < cnt; i++ ) {

		out << m_chapterList[i] ;

		sz = m_contentList[0][i].count() ;

		out << sz ;
		for( int j = 0; j < sz; j++ ) out << m_contentTypeList[0][i][j] << m_contentList[0][i][j] ;

		sz = m_contentList[1][i].count() ;

		out << sz ;
		for( int j = 0; j < sz; j++ ) out << m_contentTypeList[1][i][j] << m_contentList[1][i][j] ;
	}
    if( !cnt )
    {
        QStringList contentList = ui.lw_res_1->contentList() ;
        QList<int> contentTypeList = ui.lw_res_1->contentTypeList() ;
        int k = contentList.count() ;
        out << ui.textEdit->toHtml() ;
        out << k ;
        for( int i = 0; i < k; i++ ) out << contentTypeList[i] << contentList[i] ;
        contentList = ui.lw_res_2->contentList() ;
        contentTypeList = ui.lw_res_2->contentTypeList() ;
        k = contentList.count() ;
        out << k ;
        for( int i = 0; i < k; i++ ) out << contentTypeList[i] << contentList[i] ;
    }
	file.close() ;

	QFile settingFile("last.data") ;
	QDataStream settingOut(&settingFile) ;
	settingFile.open(QIODevice::WriteOnly) ;
	settingOut << m_version << m_curBookName ;
	settingFile.close() ;
}

void TextEdit::saveText()
{
	int cnt = m_docList.count() ;
	for( int i = 0; i < cnt; i++ )
	{
		saveText(i) ;
	}
}

void TextEdit::saveText(int id)
{
	QFile file(QString("%1/%2.dat").arg(m_curDirPath).arg(id+1)) ;
	QDataStream out(&file) ;
	if( file.open(QIODevice::WriteOnly) )
	{
		out << m_docList[id]->toHtml() ;
	}
}

void TextEdit::load()
{
    /*
    QFile settingFile("last.data") ;
	QDataStream settingIn(&settingFile) ;
	int version ;
	QString book ;
    if( settingFile.open(QIODevice::ReadOnly) || 1 )
	{
        startBook("content",true) ;
    }
    else return ;

    QFile file(m_curDirPath+"/content.config") ;
	QDataStream in(&file) ;
	int cnt = 0, sz, contentType ;
	QString content ;
	if( file.open(QIODevice::ReadOnly) )
	{
		in >> cnt ;
		QString chapter ;
		for( int i = 0; i < cnt; i++ )
		{
			in >> chapter ;
			addChapter(chapter) ;
			in >> sz ;
			QStringList contentList ;
			QList<int> typeList ;
			for( int j = 0; j < sz; j++ )
			{
				in >> contentType >> content  ;
				contentList << content ;
				typeList << contentType ;
			}
            if( !m_fixContentList || !i ) ui.lw_res_1->setData(contentList,typeList) ;

			in >> sz ;
			contentList.clear() ;
			typeList.clear() ;
			for( int j = 0; j < sz; j++ )
			{
				in >> contentType >> content  ;
				contentList << content ;
				typeList << contentType ;
			}
            if( !m_fixContentList || !i ) ui.lw_res_2->setData(contentList,typeList) ;
		}
	}
	for( int i = 0; i < cnt; i++ )
	{
		QFile file(QString("%1/%2.dat").arg(m_curDirPath).arg(i+1)) ;
		if( !file.exists() ) continue ;
		QDataStream in(&file) ;
		if( file.open(QIODevice::ReadOnly) )
		{
			QString str ;
			in >> str ;
			m_docList[i]->setHtml(str) ;
		}
	}
    if( m_chapterList.count() ) ui.lw_chapter_list->setCurrentRow(0) ;
    */
}

void TextEdit::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event) ;
	QDesktopWidget* w = new QDesktopWidget ;
	int h = w->availableGeometry().height() ;
    if( h < 800 ) h = qMin(h,600) ;
    else h = h*0.9 ;
    m_restoreRect = QRect(0,0,width(),h) ;
    resize(w->availableGeometry().size()) ;
//	resize(width(),h) ;
//	showMaximized() ;
//	setMinimumHeight(700) ;
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event) ;
    m_uploadPanel->hide();
}


void TextEdit::mousePressEvent(QMouseEvent *event)
{
	if( m_isMaximized ) return ;
	m_oriRect = geometry() ;
	m_oriPnt = event->globalPos() ;
	if( event->pos().y() < 0 || event->pos().y() > TITLEHEIGHT ) return ;
	m_isPressing = true ;
}

void TextEdit::mouseMoveEvent(QMouseEvent *event)
{
	if( !m_isPressing ) return ;
	QPoint tmp = event->globalPos()-m_oriPnt ;
	QRect geo = m_oriRect.translated(tmp.x(),tmp.y()) ;
	setGeometry(geo);
}

void TextEdit::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPressing = false ;
}

void TextEdit::mouseDoubleClickEvent(QMouseEvent *event)
{
	if( event->pos().y() < 0 || event->pos().y() > TITLEHEIGHT ) return ;
//	if( !ui.w_black->geometry().contains(event->pos())) return ;
//	onMaximize() ;
}

void TextEdit::onMaximize()
{
	if( m_isMaximized ){
//		showNormal() ;
		setGeometry(m_restoreRect) ;
	}
	else
	{
		m_restoreRect = geometry() ;
		setGeometry(QApplication::desktop()->availableGeometry()) ;
//		showMaximized() ;
	}
	m_isMaximized = !m_isMaximized ; 
}

void TextEdit::onCopyAll()
{
	QApplication::setOverrideCursor(Qt::WaitCursor) ;
	ui.textEdit->selectAll() ;
	ui.textEdit->copy() ;
	QTextCursor cursor = ui.textEdit->textCursor() ;
	cursor.clearSelection() ;
	ui.textEdit->setTextCursor(cursor) ;
//	ui.textEdit->clearSe
	QApplication::restoreOverrideCursor() ;
}

void TextEdit::onShowContentHover(CxResListWidget *pa, QListWidgetItem *item, bool isShow, QPoint pnt)
{
    pnt = mapFromGlobal(pnt) ;
    int w = 500 ;
    int h = 300 ;
    m_contentHoverW->init(pa,item) ;
    m_contentHoverW->setGeometry(pnt.x()-w,pnt.y(),w,h);
    if( isShow ) m_timer.stop(), m_contentHoverW->show() ;
    else
    {
        m_timerMode = CONTENTMENUTIMER ;
        m_timer.start(500,this) ;
    }
}

void TextEdit::onShowAddChapLabel(bool on)
{
    QPoint pnt = ui.tb_red_icon->mapToGlobal(QPoint(0,0)) ;
    pnt = mapFromGlobal(pnt) ;
    int w = 180 ;
    int h = 40 ;
    m_addChapLb->setGeometry(pnt.x()+40,pnt.y(),w,h);
    if( on )
    {
        m_timer.stop();
        m_addChapLb->show();
    }
    else
    {
        m_timerMode = ADDCHAPTIMER ;
        m_timer.start(100,this) ;
    }
}

void TextEdit::onInsertImage()
{

    /*
    QTextFrameFormat frameFormat2;
    frameFormat2.setBackground(Qt::lightGray); //Set the background color
    frameFormat2.setLeftMargin(50); //Set margins
    frameFormat2.setPadding(55); //Set padding
    frameFormat2.setBorder(2);
    frameFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_Dotted); //Set the border style

    QTextCursor cursor = ui.textEdit->textCursor(); //Get the cursor
    cursor.insertFrame(frameFormat2);
    return ;
    */

    QString fileName = QFileDialog::getOpenFileName(this, tr("Insert Image File"),
        "",
        tr("Files (*.png *.jpg *.bmp *.jpeg)")) ;
    if( !fileName.length() ) return ;
    QPixmap pix(fileName) ;
    if( pix.isNull() )
    {
        QMessageBox::information(NULL,"","Invalid image format") ;
        return ;
    }
    QString filePath = QString("res/image/%1.%2").arg(createSimpleUuid()).arg(QFileInfo(fileName).suffix()) ;
    pix = pix.scaled(ui.textEdit->viewport()->width()-80,ui.textEdit->viewport()->height()-80,Qt::KeepAspectRatio) ;
    pix.save(filePath) ;

    QUrl Uri = QUrl(filePath);
//		QUrl Uri = QUrl::fromLocalFile(str);
    QImage image = QImageReader(filePath).read();
    QTextDocument * textDocument = ui.textEdit->document();
    textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant (image) );
    QTextImageFormat imageFormat;
    imageFormat.setWidth( image.width() );
    imageFormat.setHeight( image.height() );
    imageFormat.setName( Uri.toString() );
    ui.textEdit->textCursor().insertImage(imageFormat);
    return ;
}

void TextEdit::onUndoAvailable(bool on)
{
    ui.tb_undo->setEnabled(on);
}

void TextEdit::onRedoAvailable(bool on)
{
    ui.tb_redo->setEnabled(on);
}


void TextEdit::resizeImage()
{

    QTextBlock currentBlock = ui.textEdit->textCursor().block();
    QTextBlock::iterator it;

    for (it = currentBlock.begin(); !(it.atEnd()); ++it)
    {
             QTextFragment fragment = it.fragment();
             if (fragment.isValid())
             {

                 if(fragment.charFormat().isImageFormat ())
                 {
                      QTextImageFormat newImageFormat = fragment.charFormat().toImageFormat();

                      QPair<double, double> size = ResizeImageDialog::getNewSize(this, newImageFormat.width(), newImageFormat.height());

                      newImageFormat.setWidth(size.first);
                      newImageFormat.setHeight(size.second);

                      if (newImageFormat.isValid())
                      {
                          QTextCursor helper = ui.textEdit->textCursor();

                          helper.setPosition(fragment.position());
                          helper.setPosition(fragment.position() + fragment.length(),
                                              QTextCursor::KeepAnchor);
                          helper.setCharFormat(newImageFormat);
                      }
                  }
              }
       }
}
