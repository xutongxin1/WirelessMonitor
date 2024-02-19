//
// Created by xtx on 2022/9/15.
//

#include "RepeaterWidget.h"


void RepeaterWidget::GetObjectFromUi(QPushButtonMap &result) {
}

RepeaterWidget::RepeaterWidget(QWidget *parent): QWidget(parent) {
}

void RepeaterWidget::GetConstructConfig() {
}

RepeaterWidget::~RepeaterWidget() = default;

void RepeaterWidget::SaveConstructConfig() {
}

void RepeaterWidget::StopAllInfoTCP() {
	if (tcp_info_handler_[1] == nullptr) { return; }
	if (tcp_info_handler_[2] == nullptr) { return; }
	if (tcp_info_handler_[3] == nullptr) { return; }
	disconnect(tcp_info_handler_[1], &TCPInfoHandle::RecNewData, 0, 0);
	disconnect(tcp_info_handler_[2], &TCPInfoHandle::RecNewData, 0, 0);
	disconnect(tcp_info_handler_[3], &TCPInfoHandle::RecNewData, 0, 0);
	if (tcp_info_handler_[1]->is_connected_)
	{
		tcp_info_handler_[1]->disconnectFromHost();
	}
	if (tcp_info_handler_[2]->is_connected_)
	{
		tcp_info_handler_[2]->disconnectFromHost();
	}
	if (tcp_info_handler_[3]->is_connected_)
	{
		tcp_info_handler_[3]->disconnectFromHost();
	}
}

void RepeaterWidget::SizeSplitterWithFactor(QSplitter *splitter, bool isHorizontal, float a, float b) {
	if (a + b > 1.01) //非法输入
		return;
	QList<int> SplitterSize; // 保存宽度（如果是水平分割器）或高度（如果是垂直分割器）
	static int tmp;
	// 我这里是水平分割器。如果是垂直分割器，请把下面的width()改为height()
	if (isHorizontal)
	{
		tmp = splitter->height() - splitter->handleWidth();
		// tmp = splitter->sizeHint().height();
		SplitterSize << static_cast<int>(static_cast<float>(tmp) * a);
		SplitterSize << tmp - SplitterSize[0] - 1;
	} else
	{
		tmp = splitter->width() - splitter->handleWidth();
		SplitterSize << static_cast<int>(static_cast<float>(tmp) * a);
		SplitterSize << tmp - SplitterSize[0] - 1;
	}
	qDebug() << splitter->sizes() << " " << SplitterSize;


	splitter->setSizes(SplitterSize);
	qDebug() << splitter->sizeHint().height() << " " << splitter->sizes();

}

/*Ui::Charts *RepeaterWidget::GetChartUi()
{

}

QList<QPair<QString,Datanode>> RepeaterWidget::GetChartDataPair()
{

}*/
