#ifndef LOADER_H
#define LOADER_H

#include "snoteitem.h"
#include "settings.h"
#include <QString>

QByteArray md5_sum(const QByteArray &data);

QByteArray sha512_sum(const QByteArray &data);

QByteArray aes_encrypt(const QByteArray &key, const QByteArray &data);

QByteArray aes_decrypt(const QByteArray &key, const QByteArray &data);

SNoteItem *load_item_from_path(const QString &path, const QString &key);

void save_items_to_path(const QString &path, const QString &key, const SNoteItem &item);

RemoteSettings str_to_remote_settings(const QString &);

QString remoteSettings_to_str(const RemoteSettings &);

#endif // LOADER_H
