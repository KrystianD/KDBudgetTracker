#include "StatisticsDialog.h"
#include "ui_StatisticsDialog.h"

#include <QCalendarWidget>
#include <QStandardItemModel>
#include <QDebug>

struct TStatsCatEntry
{
	double sum;
	int cnt;
	QDate minDate, maxDate;
};

StatisticsDialog::StatisticsDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::StatisticsDialog)
{
	ui->setupUi(this);
	
	QStandardItemModel *model = new QStandardItemModel(this);
	ui->lvCats->setModel(model);
	ui->lvCats->setRootIsDecorated(false);
	
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
	
	//model->setColumnCount (1);
	model->setHorizontalHeaderLabels(QStringList() << "Category" << "Amount" << "Per month" << "Total");
	
	ui->lvCats->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->lvCats->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->lvCats->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->lvCats->header()->setResizeMode(3, QHeaderView::Stretch);
	ui->lvCats->header()->setStretchLastSection(false);
	
	model->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignRight);
	model->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignRight);
	model->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignRight);
	
	ui->lvCats->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	QDate startDate = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	connect(ui->dtpStartDate, SIGNAL(dateChanged(QDate)), this, SLOT(reload()));
	connect(ui->dtpEndDate, SIGNAL(dateChanged(QDate)), this, SLOT(reload()));
	//connect (ui->lvTransactions, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pbEditTransactoin_clicked()));
	
	ui->dtpStartDate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	ui->dtpEndDate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	
	reload();
}

StatisticsDialog::~StatisticsDialog()
{
	delete ui;
}

void StatisticsDialog::reload()
{
	QStandardItemModel *model = (QStandardItemModel*)ui->lvCats->model();
	
	TDBTransactionsFetchSettings set;
	set.from = ui->dtpStartDate->date();
	set.to = ui->dtpEndDate->date();
	
	DB::getTransactions(m_trans, set);
	
	DB::getCategories(m_cats);
	
	QMap<int, TStatsCatEntry> cats;
	for (int i = 0; i < m_trans.size(); i++)
	{
		TDBTransaction &trans = m_trans[i];
		TStatsCatEntry &entry = cats[trans.catId];
		entry.sum += trans.value;
		entry.cnt++;
		if (entry.minDate.isNull() || entry.minDate > trans.date) entry.minDate = trans.date;
		if (entry.maxDate.isNull() || entry.maxDate < trans.date) entry.maxDate = trans.date;
	}
	
	model->setRowCount(0);
	int idx = 0;
	for (int i = 0; i < m_cats.size(); i++)
	{
		TDBCategory cat = m_cats[i];
		
		//if (!cats.contains (cat.id))
		//	continue;
		TStatsCatEntry entry = cats[cat.id];
		
		double diffMonth = (double)entry.minDate.daysTo(entry.maxDate) / 30.0;
		
		QStandardItem *item = new QStandardItem(cat.name);
		if (cat.type == 0)
			item->setBackground(QBrush(QColor(220, 255, 220)));
		else if (cat.type == 1)
			item->setBackground(QBrush(QColor(255, 220, 220)));
		item->setCheckable(true);
		item->setData(cat.id, Qt::UserRole + 1);
		item->setData(QString("%1%2").arg(cat.type == 0 ? "I" : "O").arg(cat.name), Qt::UserRole + 2);
		item->setCheckState(m_catSels[cat.id] ? Qt::Checked : Qt::Unchecked);
		model->setItem(idx, 0, item);
		
		item = new QStandardItem("");
		if (entry.cnt > 0)
		{
			item->setText(QString::number(entry.cnt));
		}
		if (cat.type == 0)
			item->setBackground(QBrush(QColor(220, 255, 220)));
		else if (cat.type == 1)
			item->setBackground(QBrush(QColor(255, 220, 220)));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		model->setItem(idx, 1, item);
		
		item = new QStandardItem("");
		if (diffMonth > 1)
		{
			item->setText(QString::number(entry.sum / diffMonth, 'f', 2) + " zł");
		}
		if (cat.type == 0)
			item->setBackground(QBrush(QColor(220, 255, 220)));
		else if (cat.type == 1)
			item->setBackground(QBrush(QColor(255, 220, 220)));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		model->setItem(idx, 2, item);
		
		item = new QStandardItem("");
		if (entry.cnt > 0)
		{
			item->setText(QString::number(entry.sum, 'f', 2) + " zł");
		}
		if (cat.type == 0)
			item->setBackground(QBrush(QColor(220, 255, 220)));
		else if (cat.type == 1)
			item->setBackground(QBrush(QColor(255, 220, 220)));
		item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		model->setItem(idx, 3, item);
		
		idx++;
	}
	
	model->setSortRole(Qt::UserRole + 2);
	model->sort(0);
	
	/*
	
	DB::getCategories (m_cats);
	
	QStandardItemModel *model = (QStandardItemModel*)ui->lvTransactions->model ();
	
	model->setRowCount (0);
	for (int i = 0; i < m_trans.size (); i++)
	{
		TDBTransaction& tr = m_trans[i];
	
		TDBCategory *cat = m_cats.findById (tr.catId);
	
		QStandardItem *item;
	
		item = new QStandardItem (QString::number (tr.id));
		item->setData (tr.id);
		model->setItem (i, 0, item);
		item = new QStandardItem (tr.date.toString ("yyyy-MM-dd"));
		model->setItem (i, 1, item);
		item = new QStandardItem (cat->name);
		model->setItem (i, 2, item);
		item = new QStandardItem (tr.contractor);
		model->setItem (i, 3, item);
		item = new QStandardItem (QString::number (tr.value, 'f', 2) + " zł");
		item->setTextAlignment (Qt::AlignRight);
		if (tr.value < 0)
			item->setForeground (QBrush (Qt::red));
		model->setItem (i, 4, item);
	}
	model->sort (1);
	ui->widgetInfo->repaint ();*/
}
void StatisticsDialog::onItemChanged(QStandardItem* item)
{
	int id = item->data().toInt();
	m_catSels[id] = item->checkState() == Qt::Checked;
}

void StatisticsDialog::on_pbPrevMonth_clicked()
{
	QDate curDate = ui->dtpStartDate->date();
	
	QDate startDate = QDate(curDate.year(), curDate.month(), 1);
	startDate = startDate.addMonths(-1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	reload();
}
void StatisticsDialog::on_pbNextMonth_clicked()
{
	QDate curDate = ui->dtpStartDate->date();
	
	QDate startDate = QDate(curDate.year(), curDate.month(), 1);
	startDate = startDate.addMonths(1);
	ui->dtpStartDate->setDate(startDate);
	ui->dtpEndDate->setDate(startDate.addMonths(1).addDays(-1));
	
	reload();
}
