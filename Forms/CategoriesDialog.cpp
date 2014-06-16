#include "CategoriesDialog.h"
#include "ui_CategoriesDialog.h"

#include <QStandardItemModel>
#include <QList>
#include <QDebug>
#include <QBrush>
#include <QMessageBox>

#include "EditCategoryDialog.h"

CategoriesDialog::CategoriesDialog(QWidget* parent) :
	QDialog(parent), ui(new Ui::CategoriesDialog)
{
	ui->setupUi(this);
	
	QStandardItemModel *model = new QStandardItemModel(this);
	ui->tvCats->setModel(model);
	
	model->setColumnCount(1);
	model->setHorizontalHeaderLabels(QStringList() << "Name");
	
	ui->tvCats->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	reload();
}

CategoriesDialog::~CategoriesDialog()
{
	delete ui;
}

void CategoriesDialog::reload()
{
	DB::getCategories(m_cats);
	
	QStandardItemModel *model = (QStandardItemModel*)ui->tvCats->model();
	
	model->setRowCount(0);
	for (int i = 0; i < m_cats.size(); i++)
	{
		if (m_cats[i].parentId == -1)
			appendSubcategories(model->invisibleRootItem(), m_cats[i]);
	}
	
	ui->tvCats->expandAll();
}
void CategoriesDialog::appendSubcategories(QStandardItem* parentItem, const TDBCategory& cat)
{
	QList<QStandardItem*> cols;
	QStandardItem *item;
	
	qDebug() << cat.name;
	item = new QStandardItem(cat.name);
	item->setData(cat.id);
	cols.append(item);
	
	if (cat.type == 0)
		item->setBackground(QBrush(QColor(220, 255, 220)));
	else if (cat.type == 1)
		item->setBackground(QBrush(QColor(255, 220, 220)));
		
	parentItem->appendRow(cols);
	
	for (int i = 0; i < m_cats.size(); i++)
	{
		if (m_cats[i].parentId == cat.id)
		{
			appendSubcategories(item, m_cats[i]);
		}
	}
}

void CategoriesDialog::on_pbAdd_clicked()
{
	int parentId = -1, type = -1;
	QItemSelectionModel *selmodel = ui->tvCats->selectionModel();
	if (selmodel->selectedRows().count() > 0)
	{
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		
		for (int i = 0; i < m_cats.size(); i++)
		{
			if (m_cats[i].id == id)
			{
				parentId = m_cats[i].id;
				type = m_cats[i].type;
				break;
			}
		}
		
		qDebug() << id;
	}
	
	EditCategoryDialog editDialog(0, parentId, type, this);
	if (editDialog.exec() == QDialog::Accepted)
	{
		DB::insertCategory(editDialog.Category);
		reload();
	}
}
void CategoriesDialog::on_pbEdit_clicked()
{
	QItemSelectionModel *selmodel = ui->tvCats->selectionModel();
	TDBCategory *cat;
	if (selmodel->selectedRows().count() > 0)
	{
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		
		for (int i = 0; i < m_cats.size(); i++)
		{
			if (m_cats[i].id == id)
			{
				cat = &m_cats[i];
				break;
			}
		}
		
		qDebug() << id;
	}
	
	EditCategoryDialog editDialog(cat, -1, -1, this);
	if (editDialog.exec() == QDialog::Accepted)
	{
		DB::insertCategory(editDialog.Category);
		reload();
	}
}
void CategoriesDialog::on_pbDelete_clicked()
{
	QItemSelectionModel *selmodel = ui->tvCats->selectionModel();
	if (selmodel->selectedRows().size() > 0)
	{
		if (QMessageBox::question(this, "Deleting category", "Are you sure you want to delete this category and all it's subcategories?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
			return;
			
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		qDebug() << id;
		
		DB::deleteCategoryById(id);
		
		reload();
	}
}
