#ifndef CATEGORIESDIALOG_H
#define CATEGORIESDIALOG_H

#include <QDialog>
#include <QStandardItem>

#include "mydb.h"

namespace Ui
{
class CategoriesDialog;
}

class CategoriesDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit CategoriesDialog(QWidget* parent = 0);
	~CategoriesDialog();
	
private slots:
	void on_pbAdd_clicked();
	void on_pbEdit_clicked();
	void on_pbDelete_clicked();
	
private:
	Ui::CategoriesDialog *ui;
	
	TDBCategoriesCollection m_cats;
	
	void reload();
	void appendSubcategories(QStandardItem* parentItem, const TDBCategory& cat);
};

#endif // CATEGORIESDIALOG_H
