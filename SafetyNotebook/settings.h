#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtGui/QFont>

struct RemoteSettings {
    bool state;
    QString server;
    int port;
    QString user;
    QString password;
};

class Settings {
public:
    Settings();

    QFont font() const;

    void setFont(const QFont &);

    QString lastNotePath() const;

    void setLastNotePath(const QString &);

    RemoteSettings remoteSettings() const;

    void setRemoteSettings(const RemoteSettings &);

    void saveSettings();

    void loadSettings();

private:
    QString _font_name;
    int _font_size = 0;
    QString _last_note_path;
    QSettings _settings;
    RemoteSettings _remote_settings = {false, "", 0, "", ""};
};

#endif // SETTINGS_H
