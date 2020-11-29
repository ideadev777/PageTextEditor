#include "PagesTextEdit.h"

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QScrollBar>
#include <QTextFrame>

PagesTextEdit::PagesTextEdit(QWidget *parent) :
	QTextEdit(parent),
	m_document(0),
	m_usePageMode(false),
	m_addBottomSpace(true),
	m_showPageNumbers(true),
	m_pageNumbersAlignment(Qt::AlignTop | Qt::AlignRight)
{
    m_showPageNumbers = false ;
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	aboutDocumentChanged();
    connect(this, SIGNAL(textChanged()), this, SLOT(aboutDocumentChanged()));

	connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)),
        this, SLOT(aboutVerticalScrollRangeChanged(int,int)));
}

void PagesTextEdit::setPageFormat(QPageSize::PageSizeId _pageFormat)
{
	m_pageMetrics.update(_pageFormat);
	repaint();
}

void PagesTextEdit::setPageMargins(const QMarginsF& _margins)
{
	m_pageMetrics.update(m_pageMetrics.pageFormat(), _margins);
	repaint();
}

bool PagesTextEdit::usePageMode() const
{
	return m_usePageMode;
}

void PagesTextEdit::setUsePageMode(bool _use)
{
	if (m_usePageMode != _use) {
		m_usePageMode = _use;
		repaint();
	}
}

void PagesTextEdit::setAddSpaceToBottom(bool _addSpace)
{
	if (m_addBottomSpace != _addSpace) {
		m_addBottomSpace = _addSpace;

        //
        // 让我们自己重画
        //
        repaint();
	}
}

void PagesTextEdit::setShowPageNumbers(bool _show)
{
	if (m_showPageNumbers != _show) {
		m_showPageNumbers = _show;
		repaint();
	}
}

void PagesTextEdit::setPageNumbersAlignment(Qt::Alignment _align)
{
	if (m_pageNumbersAlignment != _align) {
		m_pageNumbersAlignment = _align;

        //
        // 让我们自己重画
        //
        repaint();
	}
}

void PagesTextEdit::paintEvent(QPaintEvent* _event)
{
	updateVerticalScrollRange();

	paintPagesView();

	paintPageNumbers();

	QTextEdit::paintEvent(_event);
}

void PagesTextEdit::resizeEvent(QResizeEvent* _event)
{
	updateViewportMargins();

	updateVerticalScrollRange();

	QTextEdit::resizeEvent(_event);
}

void PagesTextEdit::updateViewportMargins()
{
    //
    // 我们形成显示参数
    //
    QMargins viewportMargins;

	if (m_usePageMode) {

        int pageWidth = m_pageMetrics.pxPageSize().width();
		int pageHeight = m_pageMetrics.pxPageSize().height();

        const int DEFAULT_TOP_MARGIN = 20;
		const int DEFAULT_BOTTOM_MARGIN = 20;
		{
			int leftMargin = 0;
			int rightMargin = 0;

            //
            // Если ширина редактора больше ширины страницы документа, расширим боковые отступы
            //
            if (width() > pageWidth) {
				const int BORDERS_WIDTH = 4;
				const int VERTICAL_SCROLLBAR_WIDTH =
						verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
                // ... ширина рамки вьюпорта и самого редактора
                leftMargin = rightMargin =
						(width() - pageWidth - VERTICAL_SCROLLBAR_WIDTH - BORDERS_WIDTH) / 2;
			}

			int topMargin = DEFAULT_TOP_MARGIN;

            int bottomMargin = DEFAULT_BOTTOM_MARGIN;
			int documentHeight = pageHeight * document()->pageCount();
			if ((height() - documentHeight) > (DEFAULT_TOP_MARGIN + DEFAULT_BOTTOM_MARGIN)) {
				const int BORDERS_HEIGHT = 2;
				const int HORIZONTAL_SCROLLBAR_HEIGHT =
						horizontalScrollBar()->isVisible() ? horizontalScrollBar()->height() : 0;
				bottomMargin =
					height() - documentHeight - HORIZONTAL_SCROLLBAR_HEIGHT - DEFAULT_TOP_MARGIN - BORDERS_HEIGHT;
			}

			viewportMargins = QMargins(leftMargin, topMargin, rightMargin, bottomMargin);
		}
	}

	setViewportMargins(viewportMargins);

	aboutUpdateDocumentGeometry();
}

void PagesTextEdit::updateVerticalScrollRange()
{
	//
	// В постраничном режиме показываем страницу целиком
	//
	if (m_usePageMode) {

		const int pageHeight = m_pageMetrics.pxPageSize().height();
		const int documentHeight = pageHeight * document()->pageCount();
		const int maximumValue = documentHeight - viewport()->height();
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
	else {
		const int SCROLL_DELTA = 800;
		int maximumValue =
				document()->size().height() - viewport()->size().height()
				+ (m_addBottomSpace ? SCROLL_DELTA : 0);
		if (verticalScrollBar()->maximum() != maximumValue) {
			verticalScrollBar()->setMaximum(maximumValue);
		}
	}
}

void PagesTextEdit::paintPagesView()
{
    if (m_usePageMode) {
        qreal pageWidth = m_pageMetrics.pxPageSize().width();
		qreal pageHeight = m_pageMetrics.pxPageSize().height();

		QPainter p(viewport());
		QPen spacePen(palette().window(), 9);
        QPen borderPen(palette().dark(), 1);
//        QBrush bgBrush(Qt::yellow) ;

		qreal curHeight = pageHeight - (verticalScrollBar()->value() % (int)pageHeight);
        const int x = pageWidth + (width() % 2 == 0 ? 2 : 1);
        const int horizontalDelta = horizontalScrollBar()->value();
        if (curHeight - pageHeight >= 0) {
			p.setPen(borderPen);
            // ... верхняя
            p.drawLine(0, curHeight - pageHeight, x, curHeight - pageHeight);
		}

        while (curHeight <= height()) {
            p.setPen(spacePen);
			p.drawLine(0, curHeight-4, width(), curHeight-4);

            p.setPen(borderPen);
            p.drawLine(0, curHeight-8, x, curHeight-8);
            p.drawLine(0, curHeight, x, curHeight);
            p.drawLine(0 - horizontalDelta, curHeight - pageHeight, 0 - horizontalDelta, curHeight - 8);
            p.drawLine(x - horizontalDelta-1, curHeight - pageHeight, x - horizontalDelta-1, curHeight - 8);

			curHeight += pageHeight;
		}

        if (curHeight >= height()) {
            p.setPen(borderPen);
            p.drawLine(0 - horizontalDelta, curHeight-pageHeight, 0 - horizontalDelta, height());
            p.drawLine(x - horizontalDelta-1, curHeight-pageHeight, x - horizontalDelta-1, height());
        }
	}
}

void PagesTextEdit::paintPageNumbers()
{
    if (m_usePageMode && !m_pageMetrics.pxPageMargins().isNull() && m_showPageNumbers) {

        QSizeF pageSize(m_pageMetrics.pxPageSize());
		QMarginsF pageMargins(m_pageMetrics.pxPageMargins());

		QPainter p(viewport());
		p.setFont(document()->defaultFont());
		p.setPen(QPen(palette().text(), 1));

        qreal curHeight = pageSize.height() - (verticalScrollBar()->value() % (int)pageSize.height());

        qreal leftMarginPosition = pageMargins.left() - horizontalScrollBar()->value();
        qreal marginWidth = pageSize.width() - pageMargins.left() - pageMargins.right();

        int pageNumber = verticalScrollBar()->value() / pageSize.height() + 1;

        if (curHeight - pageMargins.top() >= 0) {
			QRectF topMarginRect(leftMarginPosition, curHeight - pageSize.height(), marginWidth, pageMargins.top());
			paintPageNumber(&p, topMarginRect, true, pageNumber);
		}

		while (curHeight < height()) {
			QRect bottomMarginRect(leftMarginPosition, curHeight - pageMargins.bottom(), marginWidth, pageMargins.bottom());
			paintPageNumber(&p, bottomMarginRect, false, pageNumber);
			++pageNumber;
			QRect topMarginRect(leftMarginPosition, curHeight, marginWidth, pageMargins.top());
			paintPageNumber(&p, topMarginRect, true, pageNumber);

			curHeight += pageSize.height();
		}
	}
}

void PagesTextEdit::paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number)
{
	if (_isHeader) {
		if (m_pageNumbersAlignment.testFlag(Qt::AlignTop)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignTop),
				QString::number(_number));
		}
	}
	else {
		if (m_pageNumbersAlignment.testFlag(Qt::AlignBottom)) {
			_painter->drawText(_rect, Qt::AlignVCenter | (m_pageNumbersAlignment ^ Qt::AlignBottom),
				QString::number(_number));
		}
	}
}

void PagesTextEdit::aboutVerticalScrollRangeChanged(int _minimum, int _maximum)
{
	Q_UNUSED(_minimum);
	updateViewportMargins();


	int scrollValue = verticalScrollBar()->value();
	if (scrollValue > _maximum) {
		updateVerticalScrollRange();
	}
}

void PagesTextEdit::aboutDocumentChanged()
{
	if (m_document != document()) {
		m_document = document();
		connect(document()->documentLayout(), SIGNAL(update()), this, SLOT(aboutUpdateDocumentGeometry()));
	}
}

void PagesTextEdit::aboutUpdateDocumentGeometry()
{
	QSizeF documentSize(width() - verticalScrollBar()->width(), -1);
	if (m_usePageMode) {
		int pageWidth = m_pageMetrics.pxPageSize().width();
		int pageHeight = m_pageMetrics.pxPageSize().height();
		documentSize = QSizeF(pageWidth, pageHeight);
	}
	if (document()->pageSize() != documentSize) {
		document()->setPageSize(documentSize);
	}
	if (document()->documentMargin() != 0) {
		document()->setDocumentMargin(0);
	}
	QMarginsF rootFrameMargins = m_pageMetrics.pxPageMargins();
	QTextFrameFormat rootFrameFormat = document()->rootFrame()->frameFormat();
	if (rootFrameFormat.leftMargin() != rootFrameMargins.left()
		|| rootFrameFormat.topMargin() != rootFrameMargins.top()
		|| rootFrameFormat.rightMargin() != rootFrameMargins.right()
		|| rootFrameFormat.bottomMargin() != rootFrameMargins.bottom()) {
		rootFrameFormat.setLeftMargin(rootFrameMargins.left());
		rootFrameFormat.setTopMargin(rootFrameMargins.top());
		rootFrameFormat.setRightMargin(rootFrameMargins.right());
		rootFrameFormat.setBottomMargin(rootFrameMargins.bottom());
		document()->rootFrame()->setFrameFormat(rootFrameFormat);
	}
}

