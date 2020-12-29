#ifndef OPENONLINEDIALOG_H
#define OPENONLINEDIALOG_H

#include <QDialog>

namespace Ui {
class OpenOnlineDialog;
}

class OpenOnlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenOnlineDialog(QWidget *parent = nullptr);
    ~OpenOnlineDialog();

private:
    Ui::OpenOnlineDialog *ui;
};

#endif // OPENONLINEDIALOG_H
