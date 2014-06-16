#include "EditCategoryDialog.h"
#include "ui_EditCategoryDialog.h"

#include <QDebug>
#include <QStandardItemModel>
#include <QMessageBox>

#include "utils.h"

EditCategoryDialog::EditCategoryDialog(TDBCategory* cat, int parentId, int type, QWidget* parent) :
	QDialog(parent), ui(new Ui::EditCategoryDialog)
{
	ui->setupUi(this);
	
	QStandardItemModel *model = new QStandardItemModel();
	ui->cbParent->setModel(model);
	
	DB::getCategories(m_cats);
	
	QStandardItem *item = new QStandardItem("--- none ---");
	model->appendRow(item);
	ui->cbParent->setCurrentIndex(0);
	
	Utils::appendCategoriesToStandardItemModel(m_cats, model);
	
	Category.id = 0;
	if (cat)
	{
		Category.id = cat->id;
		Category.name = cat->name;
		Category.parentId = cat->parentId;
		Category.type = cat->type;
		
		ui->tbName->setText(Category.name);
		ui->cbType->setCurrentIndex(Category.type);
		
		for (int i = 1; i < model->rowCount(); i++)
		{
			int id = model->item(i)->data().toInt();
			if (id == Category.parentId)
			{
				ui->cbParent->setCurrentIndex(i);
				break;
			}
		}
		
		setWindowTitle("Edit category");
	}
	else
	{
		setWindowTitle("New category");
		
		if (type != -1)
			ui->cbType->setCurrentIndex(type);
			
		if (parentId != -1)
		{
			for (int i = 1; i < model->rowCount(); i++)
			{
				int id = model->item(i)->data().toInt();
				if (id == parentId)
				{
					ui->cbParent->setCurrentIndex(i);
					break;
				}
			}
		}
	}
}

EditCategoryDialog::~EditCategoryDialog()
{
	delete ui;
}

void EditCategoryDialog::accept()
{
	Category.name = ui->tbName->text();
	
	if (Category.name.size() == 0)
	{
		QMessageBox::information(this, windowTitle(), "Category name cannot be empty", QMessageBox::Ok);
		return;
	}
	
	int idx = ui->cbParent->currentIndex();
	if (idx == 0)
	{
		Category.parentId = -1;
	}
	else
	{
		QStandardItemModel *model = (QStandardItemModel*)ui->cbParent->model();
		Category.parentId = model->item(idx)->data().toInt();
	}
	
	if (Category.parentId == Category.id)
	{
		QMessageBox::information(this, windowTitle(), "Category cannot be parent for itself", QMessageBox::Ok);
		return;
	}
	
	Category.type = (TDBCategory::EType)ui->cbType->currentIndex();
	
	done(QDialog::Accepted);
}
