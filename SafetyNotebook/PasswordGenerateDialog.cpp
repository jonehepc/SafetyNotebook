//
// Created by jone on 2020/11/10.
//

#include <QtWidgets/QMessageBox>
#include <QtGui/QClipboard>
#include <QtCore/QRandomGenerator>
#include "PasswordGenerateDialog.h"
#include "ui_passwordgeneratedialog.h"

PasswordGenerateDialog::PasswordGenerateDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::DialogGeneratePassword) {
    ui->setupUi(this);

    initUi();
    initSignalAndSlot();
}

void PasswordGenerateDialog::initSignalAndSlot() {
    bool success = false;
    success = connect(ui->pushButtonCopy, qOverload<bool>(&QPushButton::clicked), this,
                      &PasswordGenerateDialog::onPushButtonCopyClicked);
    Q_ASSERT(success);
    success = connect(ui->pushButtonGenerate, qOverload<bool>(&QPushButton::clicked), this,
                      &PasswordGenerateDialog::onPushButtonGenerateClicked);
    Q_ASSERT(success);
    success = connect(ui->spinBoxPassLen, qOverload<int>(&QSpinBox::valueChanged), this,
                      &PasswordGenerateDialog::onSpinBoxPassLenValueChanged);
    Q_ASSERT(success);
    success = connect(ui->spinBoxLowerSize, qOverload<int>(&QSpinBox::valueChanged), this,
                      &PasswordGenerateDialog::onSpinBoxLowerSizeValueChanged);
    Q_ASSERT(success);
    success = connect(ui->spinBoxUpperSize, qOverload<int>(&QSpinBox::valueChanged), this,
                      &PasswordGenerateDialog::onSpinBoxUpperSizeValueChanged);
    Q_ASSERT(success);
    success = connect(ui->spinBoxNumberSize, qOverload<int>(&QSpinBox::valueChanged), this,
                      &PasswordGenerateDialog::onSpinBoxNumberSizeValueChanged);
    Q_ASSERT(success);
    success = connect(ui->spinBoxSpecialSize, qOverload<int>(&QSpinBox::valueChanged), this,
                      &PasswordGenerateDialog::onSpinBoxSpecialSizeValueChanged);
    Q_ASSERT(success);
}

void PasswordGenerateDialog::initUi() {

}

QString PasswordGenerateDialog::password() const {
    return ui->lineEditPasswd->text();
}

/**
 * 生成密码。
 */
void PasswordGenerateDialog::onPushButtonGenerateClicked() {
    auto len = ui->spinBoxPassLen->value();
    if (len <= 0) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("密码长度必须大于0，建议大于8位！"));
        return;
    }
    if (!ui->checkBoxLow->isChecked() && !ui->checkBoxUpper->isChecked() &&
        !ui->checkBoxNumber->isChecked() && !ui->checkBoxSpecial->isChecked()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("至少需要选择一种字符类型，建议选择3种或以上！"));
        return;
    }

    auto lower = QString::fromLocal8Bit("abcdefghijklmnopqrstuvwxyz");
    auto upper = QString::fromLocal8Bit("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    auto number = QString::fromLocal8Bit("0123456789");
    auto special = QString::fromLocal8Bit("~!@#$%^&*()-_=+\\|]}[{;:/?.>,<");
    auto pass_lower = QString::fromLocal8Bit("");
    auto pass_upper = QString::fromLocal8Bit("");
    auto pass_special = QString::fromLocal8Bit("");
    auto pass_number = QString::fromLocal8Bit("");
    auto all_type = QString::fromLocal8Bit("");
    auto pass_temp = QString::fromLocal8Bit("");
    auto min_len = getMinPassLen();

    // 生成各个类型的最小长度需求。
    if (ui->checkBoxLow->isChecked() && ui->spinBoxLowerSize->value() > 0) {
        all_type += lower;
        for (int i = 0; i < ui->spinBoxLowerSize->value(); i++) {
            pass_lower.append(lower[QRandomGenerator::global()->generate() % lower.length()]);
        }
    }
    if (ui->checkBoxUpper->isChecked() && ui->spinBoxUpperSize->value() > 0) {
        all_type += upper;
        for (int i = 0; i < ui->spinBoxUpperSize->value(); i++) {
            pass_upper.append(upper[QRandomGenerator::global()->generate() % upper.length()]);
        }
    }
    if (ui->checkBoxNumber->isChecked() && ui->spinBoxUpperSize->value() > 0) {
        all_type += number;
        for (int i = 0; i < ui->spinBoxUpperSize->value(); i++) {
            pass_number.append(number[QRandomGenerator::global()->generate() % number.length()]);
        }
    }
    if (ui->checkBoxSpecial->isChecked() && ui->spinBoxUpperSize->value() > 0) {
        all_type += special;
        for (int i = 0; i < ui->spinBoxUpperSize->value(); i++) {
            pass_special.append(special[QRandomGenerator::global()->generate() % special.length()]);
        }
    }

    // 判断是否还需要生成密码字符。
    if (len > min_len) {
        for (int i = 0; i < len - min_len; i++) {
            pass_temp.append(all_type[QRandomGenerator::global()->generate() % all_type.length()]);
        }
    }

    //将密码字符乱序列生成最终密码。
    pass_temp = pass_temp + pass_upper + pass_lower + pass_number + pass_special;
    auto result = QString::fromLocal8Bit("");
    QList<unsigned int> moved_list;
    for (int i = 0; i < pass_temp.length(); i++) {
        unsigned int idx;
        do {
            idx = QRandomGenerator::global()->generate() % pass_temp.length();
        } while (moved_list.contains(idx));
        moved_list.append(idx);
        result.append(pass_temp[idx]);
    }

    ui->lineEditPasswd->setText(result);
}

/**
 * 复制密码到剪贴板。
 */
void PasswordGenerateDialog::onPushButtonCopyClicked() {
    auto clipboard = QGuiApplication::clipboard();
    clipboard->setText(ui->lineEditPasswd->text());
}

/**
 * 密码长度值改变。
 * @param i 密码长度
 */
void PasswordGenerateDialog::onSpinBoxPassLenValueChanged(int i) {
    // 检查密码最小长度要求。
    if (i < getMinPassLen()) {
        ui->spinBoxPassLen->setValue(getMinPassLen());
    }
}

/**
 * 小写字符长度改变。
 * @param i 小写字符长度。
 */
void PasswordGenerateDialog::onSpinBoxLowerSizeValueChanged(int i) {
    ui->spinBoxPassLen->setMinimum(getMinPassLen());
}

/**
 * 大写字符长度改变。
 * @param i 大写字符长度。
 */
void PasswordGenerateDialog::onSpinBoxUpperSizeValueChanged(int i) {
    ui->spinBoxPassLen->setMinimum(getMinPassLen());
}

/**
 * 数字长度改变。
 * @param i 数字长度。
 */
void PasswordGenerateDialog::onSpinBoxNumberSizeValueChanged(int i) {
    ui->spinBoxPassLen->setMinimum(getMinPassLen());
}

/**
 * 特殊字符长度改变。
 * @param i 特殊字符长度。
 */
void PasswordGenerateDialog::onSpinBoxSpecialSizeValueChanged(int i) {
    ui->spinBoxPassLen->setMinimum(getMinPassLen());
}

/**
 * 获取最小密码长度需求。
 * @return 密码最小长度。
 */
int PasswordGenerateDialog::getMinPassLen() const {
    int minLen = 0;
    if (ui->checkBoxLow->isChecked()) {
        minLen += ui->spinBoxLowerSize->value();
    }
    if (ui->checkBoxUpper->isChecked()) {
        minLen += ui->spinBoxUpperSize->value();
    }
    if (ui->checkBoxNumber->isChecked()) {
        minLen += ui->spinBoxNumberSize->value();
    }
    if (ui->checkBoxSpecial->isChecked()) {
        minLen += ui->spinBoxSpecialSize->value();
    }

    return minLen;
}
