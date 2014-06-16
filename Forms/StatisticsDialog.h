#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QMap>

#include "mydb.h"

class QStandardItem;

namespace Ui
{
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit StatisticsDialog(QWidget* parent = 0);
	~StatisticsDialog();
	
private slots:
	void reload();
	void onItemChanged(QStandardItem* item);
	
private slots:
	void on_pbPrevMonth_clicked();
	void on_pbNextMonth_clicked();
	
private:
	Ui::StatisticsDialog *ui;
	
	TDBTransactionsCollection m_trans;
	TDBCategoriesCollection m_cats;
	QMap<int, bool> m_catSels;
};

#endif // STATISTICSDIALOG_H
