//
// Created by xtx on 2022/9/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ComTool.h" resolved

#include <QWidget>
#include <Qt>
#include <QSet>
#include <QtConcurrent/qtconcurrentrun.h>
#include "Comtool.h"

#include "ui_comtool.h"
#include "quihelper.h"
#include "quihelperdata.h"
#include "QTextEditWithKey.h"
// #include "USBInfo.h"
#include "enumser.h"

/*
 * TODO:shell语法高亮，使用正则表达式https://c.runoob.com_/front-end/
 * TODO:以回车分隔
 * TODO:ui修改
 */
ComTool::ComTool(int device_num, int win_num, QSettings *cfg, ToNewWidget *parent_info,
                 QWidget *parent) : RepeaterWidget(parent), ui_(new Ui::ComTool) {
	this->cfg_ = cfg;
	this->config_file_path_ = "./config/Device" + QString::number(device_num) + ".ini";

	this->group_name_ = "Win" + QString::number(win_num);

	if (parent_info != nullptr) //防止XCOM程序由外部启动而导致崩溃
	{
		(*(parent_info->devices_info))[device_num].com_tool = this;
	}

	timer_refresh_cnt_ = new QTimer(this);
	timer_for_port_ = new QTimer(this);
	timer_line_max_ = new QTimer(this);
	timer_for_highlight_ = new QTimer(this);

	my_serialport_ = new QSerialPort(this);

	QuiHelper::InitAll();


	AppData::ReadSendData();
	AppData::ReadDeviceData();

	ui_->setupUi(this);
	ui_->HEXEdit->hide();

	receive_count_ = 0;
	send_count_ = 0;
	//    this->InitConfig();
	QuiHelper::SetFormInCenter(this);

	connect(ui_->btnSend, &QPushButton::clicked, this, [&] {
		this->SendData();
	});

	connect(ui_->btnPaste, &QPushButton::clicked, this, [&] {
		QClipboard *clipboard = QApplication::clipboard();
		QString text = clipboard->text();
		ui_->SendDataEdit->setPlainText(text);
		this->SendData();
		ui_->SendDataEdit->setTextColor("black");
	});

	// 清空发送区
	connect(ui_->ClearSendDataEdit, &QPushButton::clicked, this, [&] {
		ui_->SendDataEdit->clear();
	});

	// 重新渲染高亮
	disconnect(ui_->refreshLog, 0, 0, 0);
	connect(ui_->refreshLog, &QPushButton::clicked, this, [&] {
		QString textstr = ui_->txtMain->toPlainText();
		ui_->txtMain->clear();
		QStringList textlist = textstr.split('\n');
		std::string recive = "<<";
		std::string send = ">>";
		int type = 0; // 0 默认 1 接收（蓝） 2 发送（黑）
		for (QStringList::iterator i = textlist.begin(); i != textlist.end(); i++)
		{
			QString test = *i;
			std::string text = test.toStdString();
			if (text.find(recive) != std::string::npos)
			{
				type = 1;
			} else if (text.find(send) != std::string::npos)
			{
				type = 2;
			}
			if (type == 1)
			{
				ui_->txtMain->setTextColor(QColor("dodgerblue"));
			} else if (type == 2)
			{
				ui_->txtMain->setTextColor(QColor("black"));
			}
			ui_->txtMain->append(QString::fromStdString(text));
		}
	});


	QStringList baud_list;
	baud_list <<
			QString::number(baud_rate_)
			<< "600"
			<< "1200"
			<< "1800"
			<< "2400"
			<< "4800"
			<< "9600"
			<< "14400"
			<< "19200"
			<< "38400"
			<< "56000"
			<< "57600"
			<< "76800"
			<< "115200"
			<< "128000"
			<< "256000";

	ui_->BandCombo->
			addItems(baud_list);

	void (QComboBox::*fp)(int) = &QComboBox::currentIndexChanged;

	connect(ui_->BandCombo, fp, this, [&](
        int num
    ) {
		        baud_rate_ = ui_->BandCombo->currentText().toInt();
		        my_serialport_->setBaudRate(baud_rate_);

		        UpdateComSetting();
	        });

	QStringList data_bits_list;
	data_bits_list << "8"
			<< "7"
			<< "6"
			<< "5";

	ui_->DataBitCombo->
			addItems(data_bits_list);
	connect(ui_
	        ->DataBitCombo, fp, this, [&](
        int num
    ) {
		        data_bit_ = ui_->DataBitCombo->currentText().toInt();
		        my_serialport_->
				        setDataBits(QSerialPort::DataBits(data_bit_)
				        );

		        UpdateComSetting();
	        });

	QStringList parity_list;
	parity_list << "无"
			<< "奇"
			<< "偶";
	ui_->ParityBitCombo->
			addItems(parity_list);
	connect(ui_
	        ->ParityBitCombo, fp, this, [&](
        int num
    ) {
		        parity_ = ui_->ParityBitCombo->currentIndex();
		        my_serialport_->
				        setParity(QSerialPort::Parity(parity_)
				        );

		        UpdateComSetting();
	        });

	// 初始化停止位表
	QStringList stop_bits_list;
	stop_bits_list << "1";
#ifdef Q_OS_WIN
	stop_bits_list << "1.5";
#endif
	stop_bits_list << "2";

	ui_->StopBitCombo->
			addItems(stop_bits_list);
	connect(ui_
	        ->StopBitCombo, fp, this, [&](
        int num
    ) {
		        stop_bit_ = ui_->StopBitCombo->currentText().toDouble();
		        if (stop_bit_ == 1.5)
		        {
			        my_serialport_->
					        setStopBits(QSerialPort::OneAndHalfStop);
		        } else
		        {
			        my_serialport_->
					        setStopBits(QSerialPort::StopBits(stop_bit_)
					        );
		        }

		        UpdateComSetting();
	        });

	//要等选项加载完才可以加载配置文件
	ComTool::GetConstructConfig();

	ui_->SendDataEdit->
			setLineWrapMode(QTextEdit::NoWrap);

	//扫描有效的端口
	timer_for_port_->start(500);

	connect(timer_for_port_, &QTimer::timeout,
	        this, &ComTool::ReflashComCombo);
	//    ui_->COMCombo->addItem("COM39");
	connect(ui_
	        ->StartTool, &QPushButton::clicked, this, &ComTool::ToolSwitch);

	connect(my_serialport_, &QSerialPort::readyRead,
	        this, &ComTool::GetData);

	connect(ui_
	        ->COMButton, &QRadioButton::toggled, this, &ComTool::ChangeMode);
	connect(ui_
	        ->TCPClientButton, &QRadioButton::toggled, this, &ComTool::ChangeMode);
	connect(ui_
	        ->TCPServerButton, &QRadioButton::toggled, this, &ComTool::ChangeMode);

	//高亮转义字符
	highlighter_send_ = new Highlighter(ui_->SendDataEdit->document());
	highlighter_rec_ = new Highlighter(ui_->txtMain->document());

	//ui美化项
	ui_->StartTool->setBackgroundColor(QColor(155, 199, 250, 100));
	ui_->StartTool->setFontSize(15);

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

	// 切换发送区控件
	connect(ui_->ckHexSend, &QRadioButton::toggled, this, [&] {
		if (ui_->ckHexSend->isChecked())
		{
			connect(ui_->HEXEdit, &QTextEditWithKey::released, this, [&] {
				HEXCollation();
			});
			HEXCollation();

			connect(ui_->SendDataEdit, &QTextEditWithKey::released, this, [&] {
				StringEditToHEX();
			});
			ui_->SendDataEdit->setPlaceholderText("String区");
			ui_->HEXEdit->show();
		} else
		{
			disconnect(ui_->SendDataEdit, &QTextEditWithKey::released, nullptr, nullptr);
			disconnect(ui_->HEXEdit, &QTextEditWithKey::released, nullptr, nullptr);
			ui_->SendDataEdit->setPlainText(ui_->HEXEdit->toPlainText());
			ui_->HEXEdit->hide();
			ui_->HEXEdit->clear();
			ui_->SendDataEdit->setPlaceholderText("发送区");
		}
	});

	//绑定由于选择不同端口响应配置文件的修改
	connect(ui_->COMCombo, fp, this, &ComTool::ComboChange);

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
	ui_->tabWidget->setCurrentIndex(0);

	//预设上下分割比例
	ui_->splitter->setStretchFactor(0, 3);
	ui_->splitter->setStretchFactor(1, 1);

	TimerRefreshCntConncet(); //绑定计数器刷新函数

	connect(this, &ComTool::AddText, this, [&](const QString &text, const char type) {
		//      if (type == 1) {
		//          ui_->txtMain->setTextColor(QColor("dodgerblue"));
		//      } else if (type == 2) {
		//          ui_->txtMain->setTextColor(QColor("black"));
		//      }
		if (ui_->txtMain->verticalScrollBar()->sliderPosition() == ui_->txtMain->verticalScrollBar()->maximum())
		{
			if (!recieve_tmp_pool_.isEmpty())
			{
				ui_->txtMain->append(recieve_tmp_pool_); //加上一点优化逻辑更好
				recieve_tmp_pool_.clear();
			}
			ui_->txtMain->append(text);
			is_under_ = true;
		} else //现在不在底层
		{
			if (is_under_)
			{
				highlighter_rec_->is_work_ = true;
				//              highlighter_rec_->rehighlight();//不能用该方法开启高亮，一定要文本的改变才能触发
				ui_->txtMain->append(text);
				is_under_ = false;
				return;
			}
			recieve_tmp_pool_.append(text);
			is_under_ = false;


			//            ui_->txtMain->append(text);
			//            ui_->txtMain->verticalScrollBar()->setSliderPosition(ui_->txtMain->verticalScrollBar()->maximum());
		}


		//      qDebug() << (ui_->txtMain->verticalScrollBar()->sliderPosition() == ui_->txtMain->verticalScrollBar()->maximum());
	});

	//行数限制逻辑
	connect(timer_line_max_, &QTimer::timeout, this, [&] {
		//      qDebug() << ui_->txtMain->document()->lineCount();
		if (is_under_)
		{
			ui_->txtMain->document()->setMaximumBlockCount(10000);
			ui_->txtMain->document()->setMaximumBlockCount(0);
		}
	});

	//高亮适配器绑定
	connect(timer_line_max_, &QTimer::timeout, this, &ComTool::TimerForHightLight);

	//    connect(this, &ComTool::UpdateCntTimer, this, &ComTool::TimerRefreshCntConncet);//绑定计数器界面刷新程序

	ui_->btnLoadData->hide();

	connect(ui_->btnSave, &QPushButton::clicked, this, &ComTool::SaveData);
}

/// TODO: 对显示行数进行限制
void ComTool::LineLimit(const QString &text, const char type) {
	ui_->txtMain->setTextColor(QColor("black"));
	qDebug() << ui_->txtMain->verticalScrollBar()->sliderPosition();
	// 目前的策略是当滚动条处于最底下，才进行渲染
	if (ui_->txtMain->verticalScrollBar()->sliderPosition() == ui_->txtMain->verticalScrollBar()->maximum())
	{
		if (!recieve_tmp_pool_.isEmpty())
		{
			ui_->txtMain->append(recieve_tmp_pool_); //加上一点优化逻辑更好
			recieve_tmp_pool_.clear();
		}
		ui_->txtMain->append(text);
	} else
	{
		recieve_tmp_pool_.append(text);
		//            ui_->txtMain->append(text);
		//            ui_->txtMain->verticalScrollBar()->setSliderPosition(ui_->txtMain->verticalScrollBar()->maximum());
	}
	qDebug() << text;
	//   qDebug() << (ui_->txtMain->verticalScrollBar()->sliderPosition() == ui_->txtMain->verticalScrollBar()->maximum());
}

void ComTool::TimerForHightLight() {
	int tmp = ui_->txtMain->document()->lineCount();
	if ((tmp - last_line_cnt_ > 1000 || tmp == 10000) && is_under_)
	{
		timer_for_highlight_->start(5000);
		highlighter_rec_->is_work_ = false;
	} else
	{
		timer_for_highlight_->start(1000);
		highlighter_rec_->is_work_ = true;
	}
	last_line_cnt_ = tmp;
}

void ComTool::TimerRefreshCntConncet() {
	disconnect(timer_refresh_cnt_, &QTimer::timeout, nullptr, nullptr);

	connect(timer_refresh_cnt_, &QTimer::timeout, this, [&] {
		ui_->rec_cnt->setText(QString("%1").arg(receive_count_));
		ui_->send_cnt->setText(QString("%1").arg(send_count_));
		if (receive_count_ > 10000 || send_count_ > 10000)
		{
			disconnect(timer_refresh_cnt_, &QTimer::timeout, nullptr, nullptr);
			connect(timer_refresh_cnt_, &QTimer::timeout, this, [&] {
				ui_->rec_cnt->setText(rec_cnt_str_);
				ui_->send_cnt->setText(send_cnt_str_);
			});
		}
	});
}

void ComTool::UpdateComSetting() {
	if (my_serialport_->isOpen())
	{
	}

	SaveConstructConfig();
}

ComTool::~ComTool() {
	delete ui_;
}


bool ComTool::OpenSerial() {
	if (GetNowTrueComName() == "")
	{
		emit(OrderShowSnackbar("没有有效的串口"));
		return false;
	}
	qDebug() << GetNowTrueComName();
	my_serialport_->setPortName(GetNowTrueComName());
	my_serialport_->setBaudRate(baud_rate_);
	my_serialport_->setParity(QSerialPort::Parity(parity_));
	my_serialport_->setDataBits(QSerialPort::DataBits(data_bit_));
	if (stop_bit_ == 1.5)
	{
		my_serialport_->setStopBits(QSerialPort::OneAndHalfStop);
	} else
	{
		my_serialport_->setStopBits(QSerialPort::StopBits(int(stop_bit_)));
	}
	my_serialport_->setFlowControl(QSerialPort::NoFlowControl); //不使用流控制
	my_serialport_->setReadBufferSize(500);
	if (my_serialport_->open(QIODevice::ReadWrite))
	{
		timer_refresh_cnt_->start(200);
		timer_line_max_->start(10000);
		timer_for_highlight_->start(1000);
		ui_->txtMain->document()->setMaximumBlockCount(10000);
		ui_->txtMain->document()->setMaximumBlockCount(0);

		return true;
	} else
	{
		qDebug() << my_serialport_->error();
		my_serialport_->close();
		emit(OrderShowSnackbar("串口打开失败，请检查是否被占用或权限不足"));
		return false;
	}
}

///启动串口/tcp工具
void ComTool::ToolSwitch() {
	//    qDebug() << (ui_->txtMain->document()->lineCount());

	//串口关闭行为
	if (ui_->StartTool->text() == "停止")
	{
		if (ui_->COMButton->isChecked())
		{
			timer_refresh_cnt_->stop();
			timer_line_max_->stop();
			timer_for_highlight_->stop();
			highlighter_rec_->is_work_ = true;

			my_serialport_->close();
		}

		ui_->COMButton->setEnabled(true);
		ui_->TCPClientButton->setEnabled(true);
		ui_->TCPServerButton->setEnabled(true);
		ui_->StartTool->setText("启动");
		is_start_ = false;
		ui_->StartTool->setStyleSheet("background-color: rgba(170, 255, 0, 125);");
	} else //串口打开行为
	{
		if (ui_->COMButton->isChecked())
		{
			if (!OpenSerial())
			{
				return;
			}
		}

		ui_->COMButton->setEnabled(false);
		ui_->TCPClientButton->setEnabled(false);
		ui_->TCPServerButton->setEnabled(false);
		ui_->StartTool->setText("停止");
		is_start_ = true;
		ui_->StartTool->setStyleSheet("background-color: rgba(255, 0, 0, 125);");
	}
}

/// 往日志区添加数据
/// \param type 数据类型
/// \param data 数据
/// \param clear 是否清空
void ComTool::Append(char type, const QString &data) {
	//    static int current_count = 0;
	//    static int max_count = 81920;


	//    if (current_count >= max_count) {
	//        ui_->txtMain->clear();
	//        current_count = 0;
	//    }

	QString str_type;
	if (type == 1)
	{
		str_type = "接收 <<";
		//        ui_->txtMain->setTextColor(QColor("dodgerblue"));
	} else if (type == 2)
	{
		str_type = "发送 >>";
		//        ui_->txtMain->setTextColor(QColor("black"));
	}

	//过滤回车换行符
	QString str_data = data;
	str_data = str_data.replace("\a", "\\a");
	str_data = str_data.replace("\b", "\\b");
	str_data = str_data.replace("\f", "\\f");
	str_data = str_data.replace("\t", "\\t");
	str_data = str_data.replace("\v", "\\v");
	str_data = str_data.replace("\\", "\\\\");
	str_data = str_data.replace("\'", "\\'");
	str_data = str_data.replace("\"", R"RX(\\")RX");
	str_data = str_data.replace("\r", "\\r");
	str_data = str_data.replace("\n", "\\n\n");

	//不同类型不同颜色显示


	if (str_data.at(str_data.length() - 1) != '\n')
	{
		str_data = QString("时间[%1] %2 %3\n").arg(TIMEMS, str_type, str_data);
	} else
	{
		str_data = QString("时间[%1] %2 %3").arg(TIMEMS, str_type, str_data);
	}

	//    ui_->txtMain->append(str_data);
	emit(AddText(str_data, type));
	receive_count_ = receive_count_ + (int) data.size();
	rec_cnt_str_ = QString("%1").arg((float) receive_count_, 0, 'E', 2);
	send_cnt_str_ = QString("%1").arg((float) send_count_, 0, 'E', 2);
}

/// 处理收到的数据
void ComTool::ProcessData(QByteArray main_serial_recv_data) {
	QString buffer;
	if (ui_->ckHexReceive->isChecked())
	{
		buffer = QUIHelperData::byteArrayToHexStr(main_serial_recv_data);
	} else
	{
		buffer = QString::fromUtf8(main_serial_recv_data); // 修复接收数据打印乱码问题
	}
	if (buffer.length() == 0) { return; }
	Append(1, buffer); // 往接收窗口添加数据

	emit(RecNewData(main_serial_recv_data, QDateTime::currentDateTime()));
}

/// 数据收入处理
/// \param data 数据
/// \param port 端口
void ComTool::GetData() {
	if (my_serialport_->bytesAvailable() > 0) //判断等到读取的数据大小
	{
		QByteArray main_serial_recv_data = my_serialport_->readAll();
		//        QtConcurrent::run(this, &ComTool::ProcessData, main_serial_recv_data);//未知原因无法运行
		(void) QtConcurrent::run([&, main_serial_recv_data] {
			ProcessData(main_serial_recv_data);
		});
	}
}

///发送发送栏里的数据
void ComTool::SendData() {
	if (!is_start_)
	{
		emit(OrderShowSnackbar("请先打开串口再发送"));
		return;
	}

	QString data = ui_->SendDataEdit->toPlainText();
	if (data.isEmpty())
	{
		ui_->SendDataEdit->setFocus();
		return;
	}

	if (history_send_list_.contains(data))
	{
		if (ui_->ckHexSend->isChecked() != history_send_list_[data].is_Hex)
		{
			history_send_list_[data].is_Hex = ui_->ckHexSend->isChecked();
			history_send_list_[data].send_num = 1;
		} else
		{
			history_send_list_[data].send_num++;
		}

		history_send_list_[data].time = QDateTime::currentDateTime();
	} else
	{
		HistorySend tmp;
		tmp.is_Hex = ui_->ckHexSend->isChecked();
		tmp.data = data;
		tmp.time = QDateTime::currentDateTime();
		history_send_list_.insert(data, tmp);
	}
	UpdateSendHistory();

	data = data.replace("\\n", "\n");
	data = data.replace("\\a", "\a");
	data = data.replace("\\b", "\b");
	data = data.replace("\\f", "\f");
	data = data.replace("\\r", "\r");
	data = data.replace("\\t", "\t");
	data = data.replace("\\v", "\v");
	data = data.replace("\\\\", "\\");
	data = data.replace("\\'", "\'");
	data = data.replace(R"RX(\\")RX", "\"");

	if (ui_->checkBox->isChecked())
	{
		data = data.append("\r\n");
	}

	QByteArray buffer;
	if (ui_->ckHexSend->isChecked())
	{
		buffer = QUIHelperData::hexStrToByteArray(data);
	} else
	{
		buffer = QUIHelperData::asciiStrToByteArray(data);
	}

	Append(2, data);
	my_serialport_->write(buffer);

	send_count_ = send_count_ + (int) buffer.size();

	//    ui_->SendCount->setText(QString("发送 : %1 字节").arg(send_count_));
}

void ComTool::SaveData() {
	// QString temp_data = ui_->txtMain->toPlainText();
	// if (temp_data.isEmpty()) {
	//     return;
	// }
	//
	// QDateTime now = QDateTime::currentDateTime();
	// QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
	// QString file_name = QString("%1/%2.txt").arg(QuiHelper::AppPath(), name);
	//
	// QFile file(file_name);
	// file.open(QFile::WriteOnly | QIODevice::Text);
	// QTextStream out(&file);
	// out << temp_data;
	// file.close();
	//
	// on_btnClear_clicked();
}

void ComTool::on_btnClear_clicked() {
	ui_->txtMain->clear();
	send_count_ = receive_count_ = 0;
	send_cnt_str_ = "0";
	rec_cnt_str_ = "0";
	TimerRefreshCntConncet();
	ui_->rec_cnt->setText(rec_cnt_str_);
	ui_->send_cnt->setText(send_cnt_str_);
}


void ComTool::GetConstructConfig() {
	isGetingConfig = true;
	cfg_->beginGroup(group_name_);

	cfg_->endGroup();

	cfg_->beginGroup(ComUUID);

	if (ui_->BandCombo->findText(cfg_->value("baud_rate", "115200").toString()) == -1)
		ui_->BandCombo->addItem(cfg_->value("baud_rate", "115200").toString());
	else
		ui_->BandCombo->setCurrentText(cfg_->value("baud_rate", "115200").toString());
	ui_->DataBitCombo->setCurrentText(cfg_->value("data_bit", "8").toString());
	ui_->StopBitCombo->setCurrentText(cfg_->value("stop_bit", "1").toString());
	ui_->ParityBitCombo->setCurrentIndex(cfg_->value("parity", "0").toInt());
	cfg_->endGroup();
	isGetingConfig = false;
}

void ComTool::SaveConstructConfig() {
	if (isGetingConfig)
		return;
	cfg_->beginGroup(group_name_);

	cfg_->endGroup();

	cfg_->beginGroup(ComUUID);
	cfg_->setValue("baud_rate", baud_rate_);
	cfg_->setValue("data_bit", data_bit_);
	cfg_->setValue("parity", parity_);
	cfg_->setValue("stop_bit", stop_bit_);
	cfg_->endGroup();
}

void ComTool::ReadErrorNet() {
}

///模式改变所导致的视觉效果变化的更新
void ComTool::ChangeMode() {
	bool tmp;
	if (ui_->COMButton->isChecked())
	{
		ui_->label_6->setVisible(false);
		ui_->IPLine->setVisible(false);
		tmp = true;
	} else if (ui_->TCPClientButton->isChecked())
	{
		ui_->label_6->setVisible(true);
		ui_->IPLine->setVisible(true);
		ui_->label_6->setText("IP:Port");
		tmp = false;
	} else
	{
		ui_->label_6->setVisible(true);
		ui_->IPLine->setVisible(true);
		ui_->label_6->setText("HearingIP");
		tmp = false;
	}

	ui_->COMLayout->setEnabled(tmp);
	ui_->BandLayout->setEnabled(tmp);
	ui_->DataBitLayout->setEnabled(tmp);
	ui_->ParityBitLayout->setEnabled(tmp);
	ui_->StopBitLayout->setEnabled(tmp);
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


/// TODO: 1. 当错误输入的时候，下方出现提示框（超出F）
///       2. 将有的小写转换为大写
///       3. 如果输入有0x，自动去除

//通过HEX区的KeyReleased触发
void ComTool::HEXCollation() {
	// qDebug() << "Process";
	//转化为大写，并将内容复制到HEX区
	QString trans_line = ui_->HEXEdit->toPlainText();
	trans_line = trans_line.toUpper();
	const QRegExp rule1("0X");
	const QRegExp rule2(" ");
	QString afterRule1 = rule1.replaceIn(trans_line, "");

	//    ui_->SendDataEdit->append()
	QString afterRule2 = rule2.replaceIn(afterRule1, "");

	//    QRegExp rule3("\\s\\s");
	//    QString afterRule3 = rule3.replaceIn(afterRule2 , "$$ ");

	QTextCursor tc = ui_->HEXEdit->textCursor(); //获取当前光标
	int cursorPo = tc.position(); //保存光标位置

	long long n = afterRule2.length();

	if (n % 2 == 0)
	{
		while (n - 2 > 0)
		{
			n = n - 2;
			afterRule2.insert(n, " ");
			cursorPo++;
		}
	} else if (n % 2 != 0)
	{
		n++;
		while ((n - 2 > 0))
		{
			n = n - 2;
			afterRule2.insert(n, " ");
			cursorPo++;
		}
	}


	ui_->HEXEdit->setPlainText(afterRule2); //修改文本

	tc.setPosition(cursorPo); //设置光标坐标为上一坐标
	ui_->HEXEdit->setTextCursor(tc);

	QString afterFix = ui_->HEXEdit->toPlainText();
	if (!((afterFix.length() + 1) % 3))
	{
		QByteArray hexData = QByteArray::fromHex(afterFix.toUtf8()); //转换一堆十六进制字符串为十六进制HEX

		QString str_data = QString::fromUtf8(hexData);

		str_data = str_data.replace("\a", "\\a");
		str_data = str_data.replace("\b", "\\b");
		str_data = str_data.replace("\f", "\\f");
		str_data = str_data.replace("\t", "\\t");
		str_data = str_data.replace("\v", "\\v");
		str_data = str_data.replace("\\", "\\\\");
		str_data = str_data.replace("\'", "\\'");
		str_data = str_data.replace("\"", R"RX(\\")RX");
		str_data = str_data.replace("\r", "\\r");
		str_data = str_data.replace("\n", "\\n");

		ui_->SendDataEdit->setText(str_data);
	}
}

//通过发送框的KeyReleased触发
void ComTool::StringEditToHEX() {
	// qDebug() << "Process";

	QString str_data = ui_->SendDataEdit->toPlainText();
	str_data = str_data.replace("\\n", "\n");
	str_data = str_data.replace("\\a", "\a");
	str_data = str_data.replace("\\b", "\b");
	str_data = str_data.replace("\\f", "\f");
	str_data = str_data.replace("\\r", "\r");
	str_data = str_data.replace("\\t", "\t");
	str_data = str_data.replace("\\v", "\v");
	str_data = str_data.replace("\\\\", "\\");
	str_data = str_data.replace("\\'", "\'");
	str_data = str_data.replace(R"RX(\\")RX", "\"");


	QString hex = QString::fromUtf8(str_data.toUtf8().toHex()).toUpper();


	long long n = hex.length();

	if (n % 2 == 0)
	{
		while (n - 2 > 0)
		{
			n = n - 2;
			hex.insert(n, " ");
		}
	} else if (n % 2 != 0)
	{
		n++;
		while ((n - 2 > 0))
		{
			n = n - 2;
			hex.insert(n, " ");
		}
	}

	ui_->HEXEdit->setPlainText(hex);
	ui_->HEXEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}
