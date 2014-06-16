#ifndef TRANSACTIONSDIALOG_H
#define TRANSACTIONSDIALOG_H

#include <QDialog>

#include "mydb.h"

namespace Ui
{
class TransactionsDialog;
}

class TransactionsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit TransactionsDialog(QWidget* parent = 0);
	~TransactionsDialog();
	
private slots:
	void reload();
	
private slots:
	void on_pbPrevMonth_clicked();
	void on_pbNextMonth_clicked();
	
	void on_pbEditTransactoin_clicked();
	void on_pbDeleteTransatcion_clicked();
	
private:
	Ui::TransactionsDialog *ui;
	
	TDBTransactionsCollection m_trans;
	TDBCategoriesCollection m_cats;
	
	bool eventFilter(QObject* sender, QEvent* event);
	void redrawInfo();
};

#endif // TRANSACTIONSDIALOG_H
