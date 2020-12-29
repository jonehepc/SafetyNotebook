//
// Created by jone on 2020/11/9.
//

#include "snoteitem.h"
#include <QtGui/QStandardItem>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>


SNoteItem::SNoteItem() {
    setEditable(false);
}

QVariant SNoteItem::data(int role) const {
    if (role == Qt::DisplayRole) {
        return _title;
    }
    return QStandardItem::data(role);
}

SNoteItem::SNoteItem(const QUuid &id, const QString &title, const QString &content, SNoteItemType type) {
    _id = id;
    _title = title;
    _content = content;
    _type = type;
    setEditable(false);
}

QUuid SNoteItem::id() const {
    return _id;
}

QString SNoteItem::title() const {
    return _title;
}

QString SNoteItem::content() const {
    return _content;
}

void SNoteItem::setId(const QUuid &id) {
    _id = id;
}

void SNoteItem::setTitle(const QString &title) {
    _title = title;
}

void SNoteItem::setContent(const QString &content) {
    _content = content;
}

QByteArray SNoteItem::toJson(const SNoteItem &item) {
    QJsonDocument document;
    document.setObject(SNoteItemToJsonObject(item));
    return document.toJson(QJsonDocument::Compact);
}

SNoteItem *SNoteItem::fromJson(const QByteArray &byteArray) {
    QJsonParseError parseError;
    auto document = QJsonDocument::fromJson(byteArray, &parseError);
    if (!document.isNull() && parseError.error == QJsonParseError::NoError) {
        auto obj = document.object();
        return JsonObjectToSNoteItem(obj);
    }
    return nullptr;
}

SNoteItemType SNoteItem::sNoteItemType() const {
    return _type;
}

void SNoteItem::setSNoteItemType(SNoteItemType type) {
    _type = type;
}

SNoteItem *SNoteItem::makeRootItem() {
    auto result = new SNoteItem(QUuid::createUuid(), QStringLiteral("安全笔记本"), "", Partition);
    return result;
}

QJsonObject SNoteItem::SNoteItemToJsonObject(const SNoteItem &item) {
    QJsonObject obj;
    obj.insert("id", item.id().toString(QUuid::WithBraces));
    obj.insert("title", item.title());
    obj.insert("type", item.sNoteItemType() == Partition ? "Partition" : "Note");
    obj.insert("content", item.content());
    QJsonArray array;
    if (item.hasChildren()) {
        for (int i = 0; i < item.rowCount(); i++) {
            array.append(SNoteItemToJsonObject(*item.child(i)));
        }
    }
    obj.insert("children", array);
    return obj;
}

SNoteItem *SNoteItem::JsonObjectToSNoteItem(const QJsonObject &obj) {
    if (obj.contains("id") && obj.contains("title") && obj.contains("type") && obj.contains("content") &&
        obj.contains("children")) {
        QJsonValue id_v = obj.value("id");
        QJsonValue title_v = obj.value("title");
        QJsonValue type_v = obj.value("type");
        QJsonValue content_v = obj.value("content");
        QJsonValue children_v = obj.value("children");
        auto a = id_v.toString().toStdWString();
        auto b = title_v.toString().toStdWString();
        auto c = type_v.toString().toStdWString();
        auto d = content_v.toString().toStdWString();
        if (!id_v.isString() || !title_v.isString() || !type_v.isString() ||
            (!content_v.isNull() && !content_v.isString()) || (!children_v.isNull() && !children_v.isArray()))
            return nullptr;
        QUuid uuid = QUuid::fromString(id_v.toString());
        if (uuid.isNull())
            return nullptr;
        auto title = title_v.toString();
        auto type = type_v.toString() == "Partition" ? Partition : Note;
        auto content = content_v.toString();
        auto ret = new SNoteItem(uuid, title, content, type);
        if (!children_v.isNull() && children_v.isArray()) {
            auto children_array = children_v.toArray();
            for (auto child: children_array) {
                if (child.isObject())
                    ret->appendRow(JsonObjectToSNoteItem(child.toObject()));
            }
        }
        return ret;
    }
    return nullptr;
}

SNoteItem *SNoteItem::child(int row) const {
    return (SNoteItem *) QStandardItem::child(row, 0);
}
