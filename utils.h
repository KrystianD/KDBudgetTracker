#ifndef UTILS_H
#define UTILS_H

#include "mydb.h"

#include <QStandardItemModel>

class Utils
{
public:
	static void appendCategoriesToStandardItemModel(const TDBCategoriesCollection& cats, QStandardItemModel* model);
	
private:
	static void insertCategory(const TDBCategoriesCollection& cats, QStandardItemModel* model, const TDBCategory &cat, int level);
};

#endif // UTILS_H
