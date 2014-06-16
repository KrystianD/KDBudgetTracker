#include <QtGui/QApplication>
#include "Forms/MainWindow.h"
#include "Forms/EditTransactionDialog.h"

#include "mydb.h"

#include <QFile>
#include <QTextCodec>
#include <QDebug>
#include <QMap>

#include <getopt.h>

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	
	QApplication a(argc, argv);
	
	bool newTransactionOnly = false;
	
	int c;
	while ((c = getopt(argc, argv, "c:n")) != -1)
	{
		QString arg = optarg;
		switch (c)
		{
		case 'c':
			DB::setDBPath(arg);
			break;
		case 'n':
			newTransactionOnly = true;
			break;
		}
	}
	
	QString path = DB::getDBPath();
	if (!QFile(path).exists())
	{
		DB::createNew();
	}
	DB::connect();
	
	if (newTransactionOnly)
	{
		EditTransactionDialog trans(0, 0);
		if (trans.exec() == QDialog::Accepted)
		{
			DB::insertTransaction(trans.Transaction);
		}
		return 0;
	}
	else
	{
		MainWindow w;
		w.show();
		return a.exec();
	}
}
