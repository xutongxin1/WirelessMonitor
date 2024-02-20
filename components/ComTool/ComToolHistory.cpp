#include "Comtool.h"
#include <QMenu>
//
// Created by xtx on 24-2-18.
//
void ComTool::ComToolHistoryTableInit() {
	//表格自动拉宽
	ui_->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui_->clearHistorySend, &QPushButton::clicked, this, [&] {
		ui_->historyTable->setRowCount(0);
		ui_->historyTable->clearContents();
		history_send_list_.clear();
	}); //清空历史记录

	connect(ui_->historyTable, &QTableWidget::cellClicked, this, [&](int row, int col) {
		ui_->SendDataEdit->setText(ui_->historyTable->item(row, 0)->text());
	}); //单击

	connect(ui_->historyTable, &QTableWidget::cellDoubleClicked, this, [&](int row, int col) {
		ui_->SendDataEdit->setText(ui_->historyTable->item(row, 0)->text());
		this->SendData();
	}); //双击
	//列宽
	ui_->historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
	ui_->historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui_->historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);

	//历史发送框的右键菜单
	ui_->historyTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui_->historyTable, &QTableWidget::customContextMenuRequested, this, [&](const QPoint pos) {
		//获得鼠标点击的x，y坐标点
		int x = pos.x();
		int y = pos.y();
		QModelIndex index = ui_->historyTable->indexAt(QPoint(x, y));
		if (index.isValid())
		{
			int row = index.row(); //获得QTableWidget列表点击的行数
			QMenu *menu = new QMenu(ui_->historyTable);
			//          QAction *pnew_1 = new QAction("复制并修改该项", ui_->historyTable);
			QAction *pnew_2 = new QAction("删除该项", ui_->historyTable);
			connect(pnew_2, &QAction::triggered, this, [&, row] {
				history_send_list_.remove(ui_->historyTable->item(row, 0)->text());
				ui_->historyTable->removeRow(row);
			});

			//          menu->addAction(pnew_1);
			menu->addAction(pnew_2);
			menu->move(cursor().pos());
			menu->show();
		}
	});
}



void ComTool::UpdateSendHistory() {
	ui_->historyTable->setRowCount(0);
	ui_->historyTable->clearContents();
	ui_->historyTable->setSortingEnabled(false);
	int j = 0;
	for (QHash<QString, HistorySend>::iterator i = history_send_list_.begin(); i != history_send_list_.end(); ++i)
	{
		ui_->historyTable->insertRow(j);
		HistorySend tmp = i.value();
		ui_->historyTable->setItem(j, 0, new QTableWidgetItem(tmp.data));
		ui_->historyTable->setItem(j, 1, new QTableWidgetItem(tmp.time.toString("yyyy-MM-dd HH:mm:ss")));
		ui_->historyTable->setItem(j, 2, new QTableWidgetItem(QString::number(tmp.send_num)));
		if (tmp.is_Hex)
		{
			ui_->historyTable->setItem(j, 3, new QTableWidgetItem("hex"));
		} else
		{
			ui_->historyTable->setItem(j, 3, new QTableWidgetItem("str"));
		}
		j++;
	}
	ui_->historyTable->setSortingEnabled(true);
}
