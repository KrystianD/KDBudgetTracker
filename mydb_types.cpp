#include "mydb.h"
#include "mydb_types.h"

QDataStream& operator << (QDataStream& out, const TDBCategory& v)
{
	return out << (quint32)1 << v.id << v.parentId << (int)v.type << v.name;
}
QDataStream& operator >> (QDataStream& out, TDBCategory& v)
{
	quint32 ver;
	out >> ver;
	switch (ver)
	{
	case 1:
		return out >> v.id >> v.parentId >> (int&)v.type >> v.name;
	default:
		return out;
	}
}

QDataStream& operator << (QDataStream& out, const TDBTransaction& v)
{
	return out << (quint32)1 << v.id << v.date << (int)v.type << v.catId << v.value << v.desc << v.contractor;
}
QDataStream& operator >> (QDataStream& out, TDBTransaction& v)
{
	quint32 ver;
	out >> ver;
	switch (ver)
	{
	case 1:
		return out >> v.id >> v.date >> (int&)v.type >> v.catId >> v.value >> v.desc >> v.contractor;
	default:
		return out;
	}
}

TDBCategory* TDBCategoriesCollection::findById(int id)
{
	for (int i = 0; i < size(); i++)
		if (at(i).id == id)
			return &operator [](i);
	return 0;
}
TDBTransaction* TDBTransactionsCollection::findById(int id)
{
	for (int i = 0; i < size(); i++)
		if (at(i).id == id)
			return &operator [](i);
	return 0;
}
