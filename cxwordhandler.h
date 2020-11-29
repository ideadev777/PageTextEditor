#ifndef CXWORDHANDLER_H
#define CXWORDHANDLER_H

#include <QObject>
#include <QStyleFactory>
#include <ActiveQt/QAxWidget>
#include <ActiveQt/QAxObject>

class CxWordHandler : public QWidget
{
	Q_OBJECT

public:
	CxWordHandler(QWidget *parent=0);
	~CxWordHandler();
	QString readDoc(QString fileName) ;

private:
	
};

#endif // CXWORDHANDLER_H
