#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Forms/CategoriesDialog.h"
#include "Forms/EditTransactionDialog.h"
#include "Forms/TransactionsDialog.h"
#include "Forms/StatisticsDialog.h"

#include <QStandardItemModel>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	QStandardItemModel *model = new QStandardItemModel(this);
	ui->lvTransactions->setModel(model);
	ui->lvTransactions->setRootIsDecorated(false);
	
	model->setColumnCount(5);
	model->setHorizontalHeaderLabels(QStringList() << "ID" << "Date" << "Category" << "Contractor" << "Value");
	
	ui->lvTransactions->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->lvTransactions->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->lvTransactions->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->lvTransactions->header()->setResizeMode(3, QHeaderView::Stretch);
	ui->lvTransactions->header()->setResizeMode(4, QHeaderView::ResizeToContents);
	ui->lvTransactions->header()->setStretchLastSection(false);
	
	model->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignRight);
	
	ui->lvTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	reload();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reload()
{
	TDBTransactionsFetchSettings set;
	set.from = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
	set.to = set.from.addMonths(1).addDays(-1);
	
	DB::getTransactions(m_trans, set);
	
	TDBCategoriesCollection cats;
	DB::getCategories(cats);
	
	QStandardItemModel *model = (QStandardItemModel*)ui->lvTransactions->model();
	
	model->setRowCount(0);
	for (int i = 0; i < m_trans.size(); i++)
	{
		TDBTransaction& tr = m_trans[i];
		
		TDBCategory *cat = cats.findById(tr.catId);
		
		QStandardItem *item;
		
		item = new QStandardItem(QString::number(tr.id));
		item->setData(tr.id);
		model->setItem(i, 0, item);
		item = new QStandardItem(tr.date.toString("yyyy-MM-dd"));
		model->setItem(i, 1, item);
		item = new QStandardItem(cat->name);
		model->setItem(i, 2, item);
		item = new QStandardItem(tr.contractor);
		model->setItem(i, 3, item);
		item = new QStandardItem(QString::number(tr.value, 'f', 2) + " zł");
		item->setTextAlignment(Qt::AlignRight);
		if (tr.value < 0)
			item->setForeground(QBrush(Qt::red));
		model->setItem(i, 4, item);
	}
	model->sort(1);
}

void MainWindow::on_pbCategories_clicked()
{
	DB::beginTransaction("cats");
	CategoriesDialog cats(this);
	if (cats.exec() == QDialog::Accepted)
	{
		DB::commitTransaction("cats");
	}
	else
	{
		DB::rollbackTransaction("cats");
	}
}
void MainWindow::on_pbTransactions_clicked()
{
	TransactionsDialog trans(this);
	trans.exec();
	reload();
}
void MainWindow::on_pbStats_clicked()
{
	StatisticsDialog stats(this);
	stats.exec();
	reload();
}
void MainWindow::on_pbNewTransaction_clicked()
{
	EditTransactionDialog trans(0, this);
	if (trans.exec() == QDialog::Accepted)
	{
		DB::insertTransaction(trans.Transaction);
		reload();
	}
}
void MainWindow::on_lvTransactions_doubleClicked(const QModelIndex &index)
{
	QItemSelectionModel *selmodel = ui->lvTransactions->selectionModel();
	if (selmodel->selectedRows().count() > 0)
	{
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		TDBTransaction *trans = m_trans.findById(id);
		if (!trans) return;
		EditTransactionDialog editTrans(trans, this);
		if (editTrans.exec() == QDialog::Accepted)
		{
			DB::insertTransaction(editTrans.Transaction);
			reload();
		}
	}
}
