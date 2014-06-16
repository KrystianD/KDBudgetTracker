#ifndef EDITTRANSACTIONDIALOG_H
#define EDITTRANSACTIONDIALOG_H

#include <QDialog>

#include "mydb.h"

namespace Ui
{
class EditTransactionDialog;
}

class EditTransactionDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit EditTransactionDialog(TDBTransaction* trans = 0, QWidget* parent = 0);
	~EditTransactionDialog();
	
	TDBTransaction Transaction;
	
private slots:
	void accept();
	
	void on_cbCategory_currentIndexChanged(int index);
	
private:
	Ui::EditTransactionDialog *ui;
	
	TDBCategoriesCollection m_cats;
};

#endif // EDITTRANSACTIONDIALOG_H
