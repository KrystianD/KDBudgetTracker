#include "TransactionsDialog.h"
#include "ui_TransactionsDialog.h"

#include <QStandardItemModel>
#include <QCalendarWidget>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>

#include "mydb.h"

#include "Forms/EditTransactionDialog.h"

TransactionsDialog::TransactionsDialog(QWidget* parent) :
	QDialog(parent), ui(new Ui::TransactionsDialog)
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
	
	QDate startDate = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	connect(ui->dtpStartDate, SIGNAL(dateChanged(QDate)), this, SLOT(reload()));
	connect(ui->dtpEndDate, SIGNAL(dateChanged(QDate)), this, SLOT(reload()));
	connect(ui->lvTransactions, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pbEditTransactoin_clicked()));
	
	ui->dtpStartDate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	ui->dtpEndDate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	
	reload();
	
	ui->widgetInfo->installEventFilter(this);
}

TransactionsDialog::~TransactionsDialog()
{
	delete ui;
}

void TransactionsDialog::reload()
{
	TDBTransactionsFetchSettings set;
	set.from = ui->dtpStartDate->date();
	set.to = ui->dtpEndDate->date();
	
	DB::getTransactions(m_trans, set);
	
	DB::getCategories(m_cats);
	
	QStandardItemModel *model = (QStandardItemModel*)ui->lvTransactions->model();
	
	model->setRowCount(0);
	for (int i = 0; i < m_trans.size(); i++)
	{
		TDBTransaction& tr = m_trans[i];
		
		TDBCategory *cat = m_cats.findById(tr.catId);
		
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
	ui->widgetInfo->repaint();
}

void TransactionsDialog::on_pbPrevMonth_clicked()
{
	QDate curDate = ui->dtpStartDate->date();
	
	QDate startDate = QDate(curDate.year(), curDate.month(), 1);
	startDate = startDate.addMonths(-1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	reload();
}
void TransactionsDialog::on_pbNextMonth_clicked()
{
	QDate curDate = ui->dtpStartDate->date();
	
	QDate startDate = QDate(curDate.year(), curDate.month(), 1);
	startDate = startDate.addMonths(1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	reload();
}
void TransactionsDialog::on_pbEditTransactoin_clicked()
{
	QItemSelectionModel *selmodel = ui->lvTransactions->selectionModel();
	if (selmodel->selectedRows().count() > 0)
	{
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		qDebug() << id;
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
void TransactionsDialog::on_pbDeleteTransatcion_clicked()
{
	QItemSelectionModel *selmodel = ui->lvTransactions->selectionModel();
	if (selmodel->selectedRows().count() > 0)
	{
		if (QMessageBox::question(this, "Deleting transaction", "Are you sure you want to delete this transaction?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
			return;
			
		int id = selmodel->selectedRows().at(0).data(Qt::UserRole + 1).toInt();
		TDBTransaction *trans = m_trans.findById(id);
		if (!trans) return;
		DB::deleteTransactionById(trans->id);
		reload();
	}
}

bool TransactionsDialog::eventFilter(QObject* sender, QEvent* event)
{
	if (sender == ui->widgetInfo)
	{
		if (event->type() == QEvent::Paint)
		{
			redrawInfo();
			return true;
		}
	}
	return false;
}
void TransactionsDialog::redrawInfo()
{
	QPainter painter(ui->widgetInfo);
	
	QPixmap pixmap(ui->widgetInfo->rect().size());
	pixmap.fill(Qt::transparent);
	
	QPainter p;
	p.begin(&pixmap);
	
	double income = 0, outcome = 0;
	for (int i = 0; i < m_trans.size(); i++)
	{
		TDBTransaction& tr = m_trans[i];
		
		if (tr.value < 0) outcome += -tr.value;
		if (tr.value > 0) income += tr.value;
	}
	
	QRect br;
	int y = 0;
	p.setPen(QColor(Qt::black));
	p.drawText(QRect(0, y, 110, 100), Qt::AlignRight | Qt::AlignTop, "Total income:", &br);
	p.drawText(QRect(120, y, 80, 100), Qt::AlignRight | Qt::AlignTop, QString::number(income, 'f', 2) + " zł", &br);
	
	y += br.height();
	p.setPen(QColor(Qt::red));
	p.drawText(QRect(0, y, 110, 100), Qt::AlignLeft | Qt::AlignTop, "-", &br);
	p.drawText(QRect(0, y, 110, 100), Qt::AlignRight | Qt::AlignTop, "Total outcome:", &br);
	p.drawText(QRect(120, y, 80, 100), Qt::AlignRight | Qt::AlignTop, QString::number(outcome, 'f', 2) + " zł", &br);
	
	y += br.height();
	p.setPen(QColor(Qt::black));
	p.drawLine(0, y, 200, y);
	
	p.drawText(QRect(0, y, 110, 100), Qt::AlignLeft | Qt::AlignTop, "=", &br);
	p.drawText(QRect(0, y, 110, 100), Qt::AlignRight | Qt::AlignTop, "Balance:", &br);
	if (income - outcome < 0)
		p.setPen(QColor(Qt::red));
	p.drawText(QRect(120, y, 80, 100), Qt::AlignRight | Qt::AlignTop, QString::number(income - outcome, 'f', 2) + " zł", &br);
	
	p.end();
	
	painter.drawPixmap(0, 0, pixmap);
}
