#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "mydb.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();
	
	void reload();
	
private slots:
	void on_pbCategories_clicked();
	void on_pbNewTransaction_clicked();
	void on_pbTransactions_clicked();
	void on_pbStats_clicked();
	void on_lvTransactions_doubleClicked(const QModelIndex &index);
	
private:
	Ui::MainWindow *ui;
	
	TDBTransactionsCollection m_trans;
};

#endif // MAINWINDOW_H
