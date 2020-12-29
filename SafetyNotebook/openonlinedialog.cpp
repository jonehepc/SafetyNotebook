#include "openonlinedialog.h"
#include "ui_openonlinedialog.h"

OpenOnlineDialog::OpenOnlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenOnlineDialog)
{
    ui->setupUi(this);
}

OpenOnlineDialog::~OpenOnlineDialog()
{
    delete ui;
}
