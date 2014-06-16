#include "mydb.h"

#include <QDate>
#include <QVariant>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QStringList>
#include <QFileInfo>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QFile>
#include <QDir>
#include <QVector>

#include "schema.h"

QString DB::DBPath = "data.sqlite";

/*QString DB::getDBPath ()
{
	QString exeDir = QFileInfo (QCoreApplication::argv ()[0]).absolutePath ();
	return QDir::toNativeSeparators (exeDir + "/data.sqlite");
}*/
void DB::createNew()
{
	QFile fileObj(getDBPath());
	if (fileObj.exists()) fileObj.remove();
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(getDBPath());
		db.open();
		foreach(QString query, QString(sqlschema).split(";"))
		{
			query = query.trimmed();
			if (query.size() == 0)
				continue;
			QSqlQuery q(db);
			if (!q.exec(query))
			{
				qCritical() << "Error while executing query:" << q.lastError();
				qCritical() << query;
				return;
			}
		}
		db.close();
	}
	QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}
void DB::recreate()
{
	DB::disconnect();
	DB::createNew();
	DB::connect();
}
bool DB::connect()
{
	qDebug() << getDBPath();
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(getDBPath());
	if (!db.open())
	{
		qCritical() << "Cannot open database:" << db.databaseName();
		return false;
	}
	qDebug() << "DB ok";
	return true;
}
void DB::disconnect()
{
	QSqlDatabase::removeDatabase(QLatin1String(QSqlDatabase::defaultConnection));
}

bool DB::beginTransaction(QString name)
{
	if (name == QString())
	{
		qDebug() << "trans" << QSqlDatabase::database().transaction();
	}
	else
	{
		QSqlQuery q;
		if (!q.exec("SAVEPOINT " + name))
		{
			qCritical() << "Error while saving point:" << q.lastError();
			return false;
		}
	}
	return true;
}
bool DB::commitTransaction(QString name)
{
	if (name == QString())
	{
		QSqlDatabase::database().commit();
	}
	else
	{
		QSqlQuery q;
		if (!q.exec("RELEASE SAVEPOINT " + name))
		{
			qCritical() << "Error while saving point:" << q.lastError();
			return false;
		}
	}
	return true;
}
bool DB::rollbackTransaction(QString name)
{
	if (name == QString())
	{
		QSqlDatabase::database().rollback();
	}
	else
	{
		QSqlQuery q;
		if (!q.exec("ROLLBACK TRANSACTION TO SAVEPOINT " + name))
		{
			qCritical() << "Error while saving point:" << q.lastError();
			return false;
		}
		if (!q.exec("RELEASE SAVEPOINT " + name))
		{
			qCritical() << "Error while saving point:" << q.lastError();
			return false;
		}
	}
	return true;
}

// Categories
bool DB::getCategories(TDBCategoriesCollection &col)
{
	QSqlQuery q;
	if (!q.exec("SELECT id,parentId,type,name FROM categories"))
	{
		qCritical() << "Error while fetching categories:" << q.lastError();
		return false;
	}
	
	col.clear();
	while (q.next())
	{
		TDBCategory cat;
		cat.id = q.value(0).toInt();
		cat.parentId = q.value(1).toInt();
		cat.type = (TDBCategory::EType)q.value(2).toInt();
		cat.name = q.value(3).toString();
		col.append(cat);
	}
	
	return true;
}
bool DB::insertCategory(TDBCategory& category)
{
	QSqlQuery q;
	q.prepare("INSERT OR REPLACE INTO categories (id,parentId,type,name) VALUES (?,?,?,?)");
	q.addBindValue(category.id <= 0 ? QVariant(QVariant::Int) : category.id);
	q.addBindValue(category.parentId);
	q.addBindValue(category.type);
	q.addBindValue(category.name);
	if (!q.exec())
	{
		qCritical() << "Error while inserting category:" << q.lastError();
		return false;
	}
	category.id = q.lastInsertId().toInt();
	
	return true;
}
bool DB::deleteCategoryById(quint32 catId)
{
	TDBCategoriesCollection col;
	if (!DB::getCategories(col))
		return false;
		
	TDBCategory *cat = col.findById(catId);
	
	QVector<int> ids;
	ids.append(catId);
	bool added;
	do
	{
		added = false;
		for (int i = 0; i < col.size(); i++)
		{
			if (ids.contains(col[i].parentId) && !ids.contains(col[i].id))
			{
				ids.append(col[i].id);
				added = true;
			}
		}
	}
	while (added);
	
	QStringList idsStrL;
	for (int i = 0; i < ids.size(); i++)
		idsStrL << QString::number(ids[i]);
	QString idsStr = idsStrL.join(",");
	
	QSqlDatabase().transaction();
	
	QSqlQuery q;
	q.prepare("UPDATE transactions SET catId=? WHERE catId IN (" + idsStr + ")");
	q.addBindValue(cat->parentId);
	if (!q.exec())
	{
		qCritical() << "Error while updating categories:" << q.lastError();
		return false;
	}
	
	q.prepare("DELETE FROM categories WHERE id IN (" + idsStr + ")");
	if (!q.exec())
	{
		qCritical() << "Error while deleting categories:" << q.lastError();
		return false;
	}
	
	QSqlDatabase().commit();
	
	return true;
}

// Transactions
bool DB::getTransactions(TDBTransactionsCollection& col, const TDBTransactionsFetchSettings& settings)
{
	QStringList whereParts;
	if (settings.from != QDate())
		whereParts << "date >= \"" + settings.from.toString("yyyy-MM-dd") + "\"";
	if (settings.to != QDate())
		whereParts << "date <= \"" + settings.to.toString("yyyy-MM-dd") + "\"";
		
	QString whereStr = "";
	if (whereParts.size() > 0)
		whereStr = "WHERE " + whereParts.join(" AND ");
		
	QSqlQuery q;
	if (!q.exec("SELECT id,date,type,catId,value,desc,contractor FROM transactions " + whereStr))
	{
		qCritical() << "Error while fetching transactions:" << q.lastError();
		return false;
	}
	
	col.clear();
	while (q.next())
	{
		TDBTransaction trans;
		trans.id = q.value(0).toInt();
		trans.date = q.value(1).toDate();
		trans.type = (TDBTransaction::EType)q.value(2).toInt();
		trans.catId = q.value(3).toInt();
		trans.value = q.value(4).toDouble();
		trans.desc = q.value(5).toString();
		trans.contractor = q.value(6).toString();
		col.append(trans);
	}
	
	return true;
}
bool DB::insertTransaction(TDBTransaction& transaction)
{
	QSqlQuery q;
	q.prepare("INSERT OR REPLACE INTO transactions (id,date,type,catId,value,desc,contractor) VALUES (?,?,?,?,?,?,?)");
	q.addBindValue(transaction.id <= 0 ? QVariant(QVariant::Int) : transaction.id);
	q.addBindValue(transaction.date);
	q.addBindValue(transaction.type);
	q.addBindValue(transaction.catId);
	q.addBindValue(transaction.value);
	q.addBindValue(transaction.desc);
	q.addBindValue(transaction.contractor);
	if (!q.exec())
	{
		qCritical() << "Error while inserting transaction:" << q.lastError();
		return false;
	}
	transaction.id = q.lastInsertId().toInt();
	
	return true;
}
bool DB::deleteTransactionById(quint32 id)
{
	QSqlQuery q;
	q.prepare("DELETE FROM transactions WHERE id=?");
	q.addBindValue(id);
	if (!q.exec())
	{
		qCritical() << "Error while deleting transaction:" << q.lastError();
		return false;
	}
	
	return true;
}
bool DB::getContractors(QStringList& contractors)
{
	QSqlQuery q;
	if (!q.exec("SELECT DISTINCT(contractor) FROM transactions WHERE LENGTH(contractor)>0 ORDER BY contractor"))
	{
		qCritical() << "Error while fetching contractors:" << q.lastError();
		return false;
	}
	
	contractors.clear();
	while (q.next())
	{
		contractors.append(q.value(0).toString());
	}
	return true;
}

