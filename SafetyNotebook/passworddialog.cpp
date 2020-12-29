#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);

    initSignalAndSLots();
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void PasswordDialog::initSignalAndSLots() {
    connect(ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
}

QString PasswordDialog::getPassword() const {
    return ui->lineEditPasswd->text();
}
