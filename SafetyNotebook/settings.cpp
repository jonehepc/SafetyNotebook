#include "settings.h"
#include "loader.h"

Settings::Settings() : _settings("jone.org", "secnote") {
}

QFont Settings::font() const {
    return QFont(_font_name, _font_size);
}

void Settings::setFont(const QFont &v) {
    _font_name = v.family();
    _font_size = v.pointSize();
}

QString Settings::lastNotePath() const {
    return _last_note_path;
}

void Settings::setLastNotePath(const QString &v) {
    _last_note_path = v;
}

RemoteSettings Settings::remoteSettings() const {
    return _remote_settings;
}

void Settings::setRemoteSettings(const RemoteSettings &v) {
    _remote_settings = v;
}

void Settings::saveSettings() {
    _settings.setValue("font_name", _font_name);
    _settings.setValue("font_size", _font_size);
    _settings.setValue("last_note_path", _last_note_path);
    auto remote_settings_str = remoteSettings_to_str(_remote_settings);
    _settings.setValue("remote_settings", remote_settings_str);
}

void Settings::loadSettings() {
    _font_name = _settings.value("font_name", "simsun").toString();
    _font_size = _settings.value("font_size", 9).toInt();
    _last_note_path = _settings.value("last_note_path", ".").toString();
    auto remote_settings_str = _settings.value("remote_settings", "{\"state\": false}").toString();
    _remote_settings = str_to_remote_settings(remote_settings_str);
}
