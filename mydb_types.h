#ifndef DB_TYPES_H
#define DB_TYPES_H

#include <QDateTime>
#include <QDataStream>
#include <QVector>

class TDBCategory
{
public:
	enum EType { Income, Outcome };
	
	int id;
	int parentId;
	EType type;
	QString name;
	
	TDBCategory() : id(0), parentId(0), type(Income), name("") { }
};

QDataStream& operator << (QDataStream& out, const TDBCategory& v);
QDataStream& operator >> (QDataStream& out, TDBCategory& v);

class TDBTransaction
{
public:
	enum EType { Income, Outcome };
	
	int id;
	QDate date;
	EType type;
	int catId;
	double value;
	QString desc, contractor;
	
	TDBTransaction() : id(0), type(Income), catId(0), value(0), desc(""), contractor("") { }
};

QDataStream& operator << (QDataStream& out, const TDBTransaction& v);
QDataStream& operator >> (QDataStream& out, TDBTransaction& v);

class TDBCategoriesCollection : public QVector<TDBCategory>
{
public:
	TDBCategory* findById(int id);
};
class TDBTransactionsCollection : public QVector<TDBTransaction>
{
public:
	TDBTransaction* findById(int id);
};

class TDBTransactionsFetchSettings
{
public:
	QDate from, to;
};

#endif // DB_TYPES_H
