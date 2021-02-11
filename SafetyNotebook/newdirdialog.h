#ifndef NEWDIRDIALOG_H
#define NEWDIRDIALOG_H

#include <QtWidgets/QDialog>

namespace Ui {
class DirNameDialog;
}

class NewDirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDirDialog(QWidget *parent = nullptr);

    void initSignalAndSlot();

    QString getDirName();

private:
    Ui::DirNameDialog *ui;
};

#endif // NEWDIRDIALOG_H
