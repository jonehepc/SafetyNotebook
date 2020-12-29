#include "newdirdialog.h"
#include "ui_newdirdialog.h"

NewDirDialog::NewDirDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirNameDialog)
{
    ui->setupUi(this);

    initSignalAndSlot();
}

NewDirDialog::~NewDirDialog()
{
    delete ui;
}

void NewDirDialog::initSignalAndSlot()
{
    QObject::connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
}

QString NewDirDialog::getDirName()
{
    return ui->lineEditName->text();
}
