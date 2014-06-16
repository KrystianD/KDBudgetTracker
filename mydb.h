#ifndef DB_H
#define DB_H

#include "mydb_types.h"

#include <QDateTime>
#include <QVector>
#include <QByteArray>
#include <QMutex>
#include <QString>

const quint32 DBVersion = 1;

class DB
{
public:
	static QString getDBPath()
	{
		return DBPath;
	}
	static void setDBPath(const QString& path)
	{
		DBPath = path;
	}
	
	static void createNew();
	static void recreate();
	static bool connect();
	static void disconnect();
	
	static bool beginTransaction(QString name = QString());
	static bool commitTransaction(QString name = QString());
	static bool rollbackTransaction(QString name = QString());
	
	// Categories
	static bool getCategories(TDBCategoriesCollection& col);
	static bool getCategoriesByParentId(TDBCategoriesCollection& col, quint32 id);
	static bool insertCategory(TDBCategory& category);
	static bool deleteCategoryById(quint32 id);
	
	// Transactions
	static bool getTransactions(TDBTransactionsCollection& col, const TDBTransactionsFetchSettings& settings = TDBTransactionsFetchSettings());
	static bool insertTransaction(TDBTransaction& transaction);
	static bool deleteTransactionById(quint32 id);
	static bool getContractors(QStringList& contractors);
	
private:
	static QString DBPath;
};

#endif // DB_H
