#ifndef NEWDIRDIALOG_H
#define NEWDIRDIALOG_H

#include <QDialog>

namespace Ui {
class DirNameDialog;
}

class NewDirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDirDialog(QWidget *parent = nullptr);
    ~NewDirDialog();

    void initSignalAndSlot(void);

    QString getDirName(void);

private:
    Ui::DirNameDialog *ui;
};

#endif // NEWDIRDIALOG_H
