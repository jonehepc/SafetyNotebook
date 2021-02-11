//
// Created by jone on 2020/11/10.
//

#include <QtWidgets/QMessageBox>
#include <QtGui/QClipboard>
#include "PasswordGenerateDialog.h"
#include "ui_passwordgeneratedialog.h"

PasswordGenerateDialog::PasswordGenerateDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::DialogGeneratePassword) {
    ui->setupUi(this);

    initUi();
    initSignalAndSlot();
}

void PasswordGenerateDialog::initSignalAndSlot() {
    connect(ui->pushButtonCopy, &QPushButton::clicked, this, &PasswordGenerateDialog::onPushButtonCopyClicked);
    connect(ui->pushButtonGenerate, &QPushButton::clicked, this, &PasswordGenerateDialog::onPushButtonGenerateClicked);
}

void PasswordGenerateDialog::initUi() {

}

QString PasswordGenerateDialog::password() const {
    return ui->lineEditPasswd->text();
}

void PasswordGenerateDialog::onPushButtonGenerateClicked() {
    auto lower = QString::fromLocal8Bit("abcdefghijklmnopqrstuvwxyz");
    auto upper = QString::fromLocal8Bit("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    auto number = QString::fromLocal8Bit("0123456789");
    auto special = QString::fromLocal8Bit("~!@#$%^&*()-_=+\\|]}[{;:/?.>,<");
    auto len = ui->spinBoxPassLen->value();
    auto str_src = QString::fromLocal8Bit("");
    if (ui->checkBoxLow->isChecked())
        str_src += lower;
    if (ui->checkBoxUpper->isChecked())
        str_src += upper;
    if (ui->checkBoxNumber->isChecked())
        str_src += number;
    if (ui->checkBoxSpecial->isChecked())
        str_src += special;
    if (len <= 0) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("密码长度必须大于0，建议大于8位！"));
        return;
    }
    if (str_src.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("至少需要选择一种字符类型，建议选择3种或以上！"));
        return;
    }
    auto src_max = str_src.length() - 1;
    QString result;
    for (int i = 0; i < len; i++) {
        auto index = qrand() % src_max;
        result.append(str_src[index]);
    }
    ui->lineEditPasswd->setText(result);
}

void PasswordGenerateDialog::onPushButtonCopyClicked() {
    auto clipboard = QGuiApplication::clipboard();
    clipboard->setText(ui->lineEditPasswd->text());
}
