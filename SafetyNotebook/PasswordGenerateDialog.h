//
// Created by jone on 2020/11/10.
//

#ifndef SECNOTE_PASSWORDGENERATEDIALOG_H
#define SECNOTE_PASSWORDGENERATEDIALOG_H

#include <QtWidgets/QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class DialogGeneratePassword; }
QT_END_NAMESPACE


class PasswordGenerateDialog : public QDialog {
Q_OBJECT

public:
    explicit PasswordGenerateDialog(QWidget *parent = nullptr);

    void initSignalAndSlot();

    void initUi();

    QString password() const;

public slots:

    void onPushButtonGenerateClicked();

    void onPushButtonCopyClicked();

    void onSpinBoxSpecialSizeValueChanged(int i);

    void onSpinBoxNumberSizeValueChanged(int i);

    void onSpinBoxPassLenValueChanged(int i);

    void onSpinBoxLowerSizeValueChanged(int i);

    void onSpinBoxUpperSizeValueChanged(int i);

private:
    Ui::DialogGeneratePassword *ui;

    int getMinPassLen() const;

};


#endif //SECNOTE_PASSWORDGENERATEDIALOG_H
