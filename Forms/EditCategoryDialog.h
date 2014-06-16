#ifndef EDITCATEGORYDIALOG_H
#define EDITCATEGORYDIALOG_H

#include <QDialog>

#include "mydb.h"

namespace Ui
{
class EditCategoryDialog;
}

class EditCategoryDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit EditCategoryDialog(TDBCategory* cat = 0, int parentId = -1, int type = -1, QWidget* parent = 0);
	~EditCategoryDialog();
	
	TDBCategory Category;
	
private slots:
	void accept();
	
private:
	Ui::EditCategoryDialog *ui;
	
	TDBCategoriesCollection m_cats;
	TDBCategory m_cat;
};

#endif // EDITCATEGORYDIALOG_H
