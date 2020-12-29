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
    PasswordGenerateDialog(QWidget *parent = nullptr);

    ~PasswordGenerateDialog();

    void initSignalAndSlot();

    void initUi();

    QString password() const;

public slots:

    void onPushButtonGenerateClicked();

    void onPushButtonCopyClicked();

private:
    Ui::DialogGeneratePassword *ui;
};


#endif //SECNOTE_PASSWORDGENERATEDIALOG_H
