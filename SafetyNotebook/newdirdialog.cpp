#include "newdirdialog.h"
#include "ui_newdirdialog.h"

NewDirDialog::NewDirDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DirNameDialog) {
    ui->setupUi(this);

    initSignalAndSlot();
}

void NewDirDialog::initSignalAndSlot() {
    connect(ui->pushButtonOk, &QPushButton::clicked, this, &NewDirDialog::accept);
}

QString NewDirDialog::getDirName() {
    return ui->lineEditName->text();
}
