#ifndef PAGESTEXTEDIT_H
#define PAGESTEXTEDIT_H

#include <QTextEdit>

#include "PageMetrics.h"


class PagesTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit PagesTextEdit(QWidget* parent = 0);

	void setPageFormat(QPageSize::PageSizeId _pageFormat);

	void setPageMargins(const QMarginsF& _margins);

	bool usePageMode() const;

public slots:
	void setUsePageMode(bool _use);

	void setAddSpaceToBottom(bool _addSpace);

	void setShowPageNumbers(bool _show);

	void setPageNumbersAlignment(Qt::Alignment _align);

protected:
	void paintEvent(QPaintEvent* _event);
	void resizeEvent(QResizeEvent* _event);

private:
	void updateViewportMargins();

	void updateVerticalScrollRange();

	void paintPagesView();
	void paintPageNumbers();

	void paintPageNumber(QPainter* _painter, const QRectF& _rect, bool _isHeader, int _number);

private slots:
	void aboutVerticalScrollRangeChanged(int _minimum, int _maximum);

	void aboutDocumentChanged();
	void aboutUpdateDocumentGeometry();

private:
	QTextDocument* m_document;

	bool m_usePageMode;

	bool m_addBottomSpace;

	bool m_showPageNumbers;

	Qt::Alignment m_pageNumbersAlignment;
	PageMetrics m_pageMetrics;
};

#endif // PAGESTEXTEDIT_H
