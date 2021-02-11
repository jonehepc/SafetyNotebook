#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class SettingsDialog;
}
QT_END_NAMESPACE

struct RemoteSettings;

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    ~SettingsDialog();

    void initUi();

    void initSignalAndSlot();

    RemoteSettings getRemoteSettings() const;

    void setRemoteSettings(const RemoteSettings &);

public slots:

    void onPushButtonApplyClicked();

    void onPushButtonCancelClicked();

    void onRadioButtonRemoteEnableClicked();

    void onRadioButtonRemoteDisableClicked();


private:
    Ui::SettingsDialog *ui{};
    RemoteSettings *_remoteSettings{};
};

#endif // SETTINGSDIALOG_H
