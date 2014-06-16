#include "utils.h"

void Utils::appendCategoriesToStandardItemModel(const TDBCategoriesCollection& cats, QStandardItemModel* model)
{
	for (int i = 0; i < cats.size(); i++)
		if (cats[i].parentId == -1)
			insertCategory(cats, model, cats[i], 0);
}
void Utils::insertCategory(const TDBCategoriesCollection& cats, QStandardItemModel* model, const TDBCategory &cat, int level)
{
	QString str = QString(level * 2, ' ') + cat.name;
	
	QStandardItem *item = new QStandardItem(str);
	item->setData(cat.id);
	if (cat.type == TDBCategory::Outcome)
		item->setForeground(QBrush(QColor(Qt::red)));
	model->appendRow(item);
	
	for (int i = 0; i < cats.size(); i++)
		if (cats[i].parentId == cat.id)
			insertCategory(cats, model, cats[i], level + 1);
}
