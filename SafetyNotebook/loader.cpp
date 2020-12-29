#include "loader.h"
#include "snoteitem.h"
#include <QtCore/QCryptographicHash>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <qaesencryption.h>
#include <QtCore/QDebug>

QByteArray md5_sum(const QByteArray &data) {
    QByteArray sum = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    return sum;
}

QByteArray aes_encrypt(const QByteArray &key, const QByteArray &data) {
    QByteArray iv("0123456789ABCDEF");
    QAESEncryption::MODE mode = QAESEncryption::CBC;
    QAESEncryption::AES level = QAESEncryption::AES_128;
    QAESEncryption::PADDING padding = QAESEncryption::X923;
    switch (key.size()) {
        case 16:
            level = QAESEncryption::AES_128;
            break;
        case 32:
            level = QAESEncryption::AES_192;
            break;
        case 64:
            level = QAESEncryption::AES_256;
            break;
    }
    return QAESEncryption::Crypt(level, mode, data, key, iv, padding);
}


QByteArray aes_decrypt(const QByteArray &key, const QByteArray &data) {
    QByteArray iv("0123456789ABCDEF");
    QAESEncryption::MODE mode = QAESEncryption::CBC;
    QAESEncryption::AES level = QAESEncryption::AES_128;
    QAESEncryption::PADDING padding = QAESEncryption::X923;
    switch (key.size()) {
        case 16:
            level = QAESEncryption::AES_128;
            break;
        case 32:
            level = QAESEncryption::AES_192;
            break;
        case 64:
            level = QAESEncryption::AES_256;
            break;
    }
    return QAESEncryption::Decrypt(level, mode, data, key, iv, padding);
}

SNoteItem* load_item_from_path(const QString &path, const QString &key) {
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    auto b_key = md5_sum(key.toUtf8());
    auto jsonArray = aes_decrypt(b_key, data);
    auto s_len = jsonArray.length();
    for(int i = 0; i < s_len; i++) {
        if (jsonArray[s_len - i - 1] == '\0') {
            jsonArray.remove(s_len - i - 1, 1);
        } else {
            break;
        }
    }
    auto ret = SNoteItem::fromJson(jsonArray);
    return ret;
}

void save_items_to_path(const QString &path, const QString &key, const SNoteItem &item) {
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    auto b_key = md5_sum(key.toUtf8());
    auto jsonArray = SNoteItem::toJson(item);
    auto data = aes_encrypt(b_key, jsonArray);
    file.write(data);
    file.close();
}

RemoteSettings str_to_remote_settings(const QString &json) {
    QJsonDocument document;
    QJsonParseError parseError;
    RemoteSettings remoteSettings;
    document = QJsonDocument::fromJson(json.toUtf8(), &parseError);
    if (!document.isNull() && parseError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            QJsonObject jsonObject = document.object();
            if (jsonObject.contains("state")) {
                auto state = jsonObject.value("state");
                if (state.isBool()) {
                    remoteSettings.state = state.toBool();
                }
            }
            if (jsonObject.contains("server")) {
                auto server = jsonObject.value("server");
                if (server.isString())
                    remoteSettings.server = server.toString();
            }
            if (jsonObject.contains("port")) {
                auto port = jsonObject.value("port");
                if (port.isDouble())
                    remoteSettings.port = port.toInt();
            }
            if (jsonObject.contains("user")) {
                auto user = jsonObject.value("user");
                if (user.isString())
                    remoteSettings.user = user.toString();
            }
            if (jsonObject.contains("password")) {
                auto password = jsonObject.value("password");
                if (password.isString())
                    remoteSettings.password = password.toString();
            }
        }
    }

    return remoteSettings;
}

QString remoteSettings_to_str(const RemoteSettings &remoteSettings) {
    QJsonObject jsonObject;
    jsonObject.insert("state", remoteSettings.state);
    jsonObject.insert("server", remoteSettings.server);
    jsonObject.insert("port", remoteSettings.port);
    jsonObject.insert("user", remoteSettings.user);
    jsonObject.insert("password", remoteSettings.password);
    QJsonDocument jsonDocument;
    jsonDocument.setObject(jsonObject);
    auto jsonArray = jsonDocument.toJson(QJsonDocument::Compact);
    return QString::fromUtf8(jsonArray);
}

