#include "EditTransactionDialog.h"
#include "ui_EditTransactionDialog.h"

#include <QStandardItemModel>
#include <QStringListModel>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QDebug>

#include "utils.h"

EditTransactionDialog::EditTransactionDialog(TDBTransaction* trans, QWidget* parent) :
	QDialog(parent), ui(new Ui::EditTransactionDialog)
{
	ui->setupUi(this);
	
	//setWindowFlags (windowFlags () & ~Qt::WindowSystemMenuHint);
	
	QStringList conts;
	DB::getContractors(conts);
	//qDebug () << conts;
	
	QStringListModel *model2 = new QStringListModel(conts);
	ui->cbContractor->setModel(model2);
	ui->cbContractor->setEditText("");
	
	QStandardItemModel *model = new QStandardItemModel();
	ui->cbCategory->setModel(model);
	
	DB::getCategories(m_cats);
	
	Utils::appendCategoriesToStandardItemModel(m_cats, model);
	
	on_cbCategory_currentIndexChanged(-1);
	
	ui->dtpDate->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	
	if (trans)
	{
		Transaction.id = trans->id;
		Transaction.contractor = trans->contractor;
		Transaction.date = trans->date;
		Transaction.type = trans->type;
		Transaction.value = trans->value;
		Transaction.catId = trans->catId;
		Transaction.desc = trans->desc;
		
		ui->dtpDate->setDate(Transaction.date);
		ui->cbContractor->setEditText(Transaction.contractor);
		ui->tbValue->setText(QString::number(qAbs(Transaction.value), 'f', 2));
		ui->tbDesc->setText(Transaction.desc);
		
		for (int i = 1; i < model->rowCount(); i++)
		{
			int id = model->item(i)->data().toInt();
			if (id == Transaction.catId)
			{
				ui->cbCategory->setCurrentIndex(i);
				break;
			}
		}
		
		setWindowTitle("Edit transaction");
	}
	else
	{
		Transaction.id = -1;
		
		ui->cbCategory->setCurrentIndex(0);
		ui->dtpDate->setDate(QDate::currentDate());
		
		setWindowTitle("New transaction");
	}
}

EditTransactionDialog::~EditTransactionDialog()
{
	delete ui;
}

void EditTransactionDialog::accept()
{
	Transaction.date = ui->dtpDate->date();
	Transaction.desc = ui->tbDesc->toPlainText();
	Transaction.contractor = ui->cbContractor->currentText();
	
	int idx = ui->cbCategory->currentIndex();
	if (idx == -1)
	{
		QMessageBox::information(this, windowTitle(), "Select category", QMessageBox::Ok);
		return;
	}
	QStandardItemModel *model = (QStandardItemModel*)ui->cbCategory->model();
	int id = model->item(idx)->data().toInt();
	TDBCategory *cat = m_cats.findById(id);
	
	QString str = ui->tbValue->text();
	str = str.replace(",", ".");
	bool ok;
	double val = str.toDouble(&ok);
	if (!ok)
	{
		QMessageBox::warning(this, windowTitle(), "Invalid value", QMessageBox::Ok);
		return;
	}
	
	if (cat->type == TDBCategory::Income)
		Transaction.value = val;
	else
		Transaction.value = -val;
		
	Transaction.catId = cat->id;
	Transaction.type = (TDBTransaction::EType)cat->type;
	
	if (Transaction.value == 0)
	{
		QMessageBox::information(this, windowTitle(), "No value", QMessageBox::Ok);
		return;
	}
	
	done(QDialog::Accepted);
}

void EditTransactionDialog::on_cbCategory_currentIndexChanged(int index)
{
	int idx = ui->cbCategory->currentIndex();
	if (idx == -1) return;
	QStandardItemModel *model = (QStandardItemModel*)ui->cbCategory->model();
	int id = model->item(idx)->data().toInt();
	TDBCategory *cat = m_cats.findById(id);
	
	if (cat->type == TDBCategory::Income)
		ui->tbValue->setStyleSheet("color: black;");
	else
		ui->tbValue->setStyleSheet("color: red;");
}
