//
// Created by xtx on 2023/2/27.
//

#include "RepeaterDialog.h"

#include <qdialogbuttonbox.h>

RepeaterDialog::RepeaterDialog(QWidget *parent): QDialog(parent) {
	setModal(true);
	setAttribute(Qt::WA_DeleteOnClose);
	// connectResultSingal();
	show();
}

void RepeaterDialog::connectResultSingal() {

}

void RepeaterDialog::GetData() {
}
