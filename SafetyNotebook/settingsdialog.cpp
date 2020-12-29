#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    initUi();
    initSignalAndSlot();
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::initUi() {
    ui->radioButtonRemoteDisable->setChecked(true);
}

void SettingsDialog::initSignalAndSlot() {
    connect(ui->pushButtonApply, SIGNAL(clicked()), this, SLOT(onPushButtonApplyClicked()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(onPushButtonCancelClicked()));
    connect(ui->radioButtonRemoteDisable, SIGNAL(clicked()), this, SLOT(onRadioButtonRemoteDisableClicked()));
    connect(ui->radioButtonRemoteEnable, SIGNAL(clicked()), this, SLOT(onRadioButtonRemoteEnableClicked()));
}

void SettingsDialog::onPushButtonApplyClicked() {
    _remoteSettings.state = ui->radioButtonRemoteEnable->isChecked();
    _remoteSettings.server = ui->lineEditServer->text();
    _remoteSettings.port = ui->lineEditPort->text().toInt();
    _remoteSettings.user = ui->lineEditUser->text();
    _remoteSettings.password = ui->lineEditPassword->text();
}

void SettingsDialog::onPushButtonCancelClicked() {
    reject();
}

void SettingsDialog::onRadioButtonRemoteEnableClicked() {
    ui->lineEditServer->setEnabled(true);
    ui->lineEditPort->setEnabled(true);
    ui->lineEditUser->setEnabled(true);
    ui->lineEditPassword->setEnabled(true);
    ui->labelServer->setEnabled(true);
    ui->labelPort->setEnabled(true);
    ui->labelUser->setEnabled(true);
    ui->labelPassword->setEnabled(true);
}

void SettingsDialog::onRadioButtonRemoteDisableClicked() {
    ui->lineEditServer->setEnabled(false);
    ui->lineEditPort->setEnabled(false);
    ui->lineEditUser->setEnabled(false);
    ui->lineEditPassword->setEnabled(false);
    ui->labelServer->setEnabled(false);
    ui->labelPort->setEnabled(false);
    ui->labelUser->setEnabled(false);
    ui->labelPassword->setEnabled(false);
}

RemoteSettings SettingsDialog::getRemoteSettings() const {
    return _remoteSettings;
}

void SettingsDialog::setRemoteSettings(const RemoteSettings &v) {
    _remoteSettings = v;
    if(_remoteSettings.state)
        ui->radioButtonRemoteEnable->click();
    else
        ui->radioButtonRemoteDisable->click();
    ui->lineEditServer->setText(_remoteSettings.server);
    ui->lineEditPort->setText(QString(_remoteSettings.port));
    ui->lineEditUser->setText(_remoteSettings.user);
    ui->lineEditPassword->setText(_remoteSettings.password);
}
