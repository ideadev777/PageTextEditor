#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>
#include "ui_mainwindow.h"
#include "cxsmalltext.h"
#include "cxcolormenu.h"

#include <QMenu>
#include <QAction>
#include <QBasicTimer>
#include <QUuid>
#include <QCloseEvent>
#include <QTimeLine>

#include "cxpagemenu.h"
#include "cxallchapterdlg.h"
#include "cxwordhandler.h"
#include "cxcontenthover.h"
#include "cxuploadwidget.h"

#define PAGEMENUTIMER 0
#define CONTENTMENUTIMER 1
#define ADDCHAPTIMER 2
#include "cxtextedit.h"

class TextEdit : public QWidget
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);
	QString createSimpleUuid() ;
    void setLoadingState(bool on){ m_isLoadingState = on ; }
    Ui::Form_MainWindow form(){ return ui ; }
private slots:
    void resizeImage() ;
    void onStartNewBook() ;
	void onAddChapter() ;
    void onAddAsChapter() ;
	void onSelectChapter(int) ;
	void onTextChanged() ;
	void onSliderValueChanged(int) ;
	void onPdf() ;
	void onODT() ;
	void onPrint() ;
	void onMovePage(int page) ;
	void onPrevPage() ;
	void onNextPage() ;
	void onUploadFile() ;
    void onUploadFolder() ;
    void onShowUploadPanel(bool on=true) ;
	void onCreateText() ;
	void onContentPlus() ;
	void onContentMinus() ;
	void onUndo() ;
	void onRedo() ;
	void onChapterChanged() ;
	void onPreChanged( int id, bool isAdd ) ;
	void onContentChanged(int) ;
	void onSwapChapter( int s, int e ) ;
	void onMoveContent( int index, QListWidgetItem* item ) ;
	void onSave() ;
    void onOpen() ;
    void onNew() ;
	void onRequireColorMenu(CxColorWidget* w) ;
	void onCollectAll() ;
	void onAutoSave() ;
	void onShowPageMenu(bool on) ;
	void saveContent() ;
	void saveText() ;
	void saveText( int id ) ;
	void onMaximize() ;
	void onCopyAll() ;
    void onInsertImage() ;
	//////////////////////////////////////////////////////////////////////////
	void currentCharFormatChanged(const QTextCharFormat &format);
	void textBold();
	void textUnderline();
	void textItalic();
	void textFamily(const QString &f);
	void textSize(const QString &p);
	void textColor();
	void onTextLeft() ;
	void onTextRight() ;
	void onTextCenter() ;
	void onShowTextFormat() ;
	void onFontSizeIncrease() ;
	void onFontSizeDecrease() ;
	void onTextColorChanged(QColor col) ;
	void onHightlightColorChanged(QColor col) ;
	void onTextBackgroundColorChanged(QColor col) ;

	void onDocumentChanged( bool on ) ;

    void onShowContentHover( CxResListWidget* pa,QListWidgetItem* item, bool isShow, QPoint pnt ) ;
    void onShowAddChapLabel( bool on ) ;
    void onUndoAvailable(bool on) ;
    void onRedoAvailable(bool on) ;
protected:
	void timerEvent(QTimerEvent* event) ;
	void closeEvent(QCloseEvent* event) ;
	void showEvent(QShowEvent *event) ;
	void resizeEvent(QResizeEvent *event) ;
	void mousePressEvent(QMouseEvent *event) ;
	void mouseMoveEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void mouseDoubleClickEvent(QMouseEvent *event) ;
private:
	void initUI() ;
	void initConnection() ;
	void init() ;
	void load() ;
	void startBook(QString str, bool isOpenMode=false) ;
	void addChapter(QString text) ;
	void setEditable(bool on);
	void switchContent(int id) ;
	int getPageCount() ; 

	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
	void fontChanged(const QFont &f);
	void colorChanged(const QColor &c);
	void alignmentChanged(Qt::Alignment a);
    void loadDir(QString src) ;


	Ui::Form_MainWindow ui ;
	QString m_curBookName, m_curDirPath ;
	QList<QString> m_chapterList ;
	QList<QTextDocument*> m_docList ;
	int m_curChapter ;
	int m_curPage, m_curLWTab ;
	QSize m_pageSize ;
	CxSmallText* m_textDlg ;
	QList<QStringList> m_contentList[2] ;
	QList<QList<int>> m_contentTypeList[2] ;
	bool m_isRemoteTextChange ;
	bool m_isAutoSave ;
	QAction* m_autoSaveAction ;
	QBasicTimer m_timer ;
	CxPageMenu* m_pageMenu ;
	CxAllChapterDlg* m_collectDlg ;
	CxWordHandler* m_wordHandler ;
	QTimeLine* m_autoSaveTimer ;
	int m_version ;

	QRect m_oriRect;
	QPoint m_oriPnt ;
	bool m_isPressing ;
	bool m_isMaximized ;
	QRect m_restoreRect ;
    QPageSize m_b5Size ;
    CxContentHover* m_contentHoverW ;
    QToolButton* m_addChapLb;
    int m_timerMode ;
    bool m_isLoadingState ;
    CxUploadWidget* m_uploadPanel ;
    bool m_fixContentList ;
};

#endif // TEXTEDIT_H
