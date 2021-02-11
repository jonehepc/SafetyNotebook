#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);

    initSignalAndSLots();
}

void PasswordDialog::initSignalAndSLots() {
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &PasswordDialog::accept);
}

QString PasswordDialog::getPassword() const {
    return ui->lineEditPasswd->text();
}
